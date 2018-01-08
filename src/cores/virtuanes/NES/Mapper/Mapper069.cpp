//////////////////////////////////////////////////////////////////////////
// Mapper069  SunSoft FME-7                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper069::Reset()
{
	reg = 0;
	irq_enable = 0;
	irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	nes->apu->SelectExSound(32);
	nes->SetIrqType( NES::IRQ_CLOCK );

	patch = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0xfeac6916 ) {	// Honoo no Toukyuuji - Dodge Danpei 2(J)
//		nes->SetIrqType( NES::IRQ_HSYNC );
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0xad28aef6 ) {	// Dynamite Batman(J) / Dynamite Batman - Return of the Joker(U)
		patch = 1;
	}
}

void	Mapper069::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE000 ) {
		case	0x8000:
			reg = data;
			break;

		case	0xA000:
			switch( reg & 0x0F ) {
				case	0x00:	case	0x01:
				case	0x02:	case	0x03:
				case	0x04:	case	0x05:
				case	0x06:	case	0x07:
					SetVROM_1K_Bank( reg&0x07, data );
					break;
				case	0x08:
					if( !patch && !(data & 0x40) ) {
						SetPROM_8K_Bank( 3, data );
					}
					break;
				case	0x09:
					SetPROM_8K_Bank( 4, data );
					break;
				case	0x0A:
					SetPROM_8K_Bank( 5, data );
					break;
				case	0x0B:
					SetPROM_8K_Bank( 6, data );
					break;

				case	0x0C:
					data &= 0x03;
					if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
					else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
					else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
					else		     SetVRAM_Mirror( VRAM_MIRROR4H );
					break;

				case	0x0D:
					irq_enable = data;
					nes->cpu->ClrIRQ( IRQ_MAPPER );
					break;

				case	0x0E:
					irq_counter = (irq_counter & 0xFF00) | data;
					nes->cpu->ClrIRQ( IRQ_MAPPER );
					break;

				case	0x0F:
					irq_counter = (irq_counter & 0x00FF) | (data << 8);
					nes->cpu->ClrIRQ( IRQ_MAPPER );
					break;
			}
			break;

		case	0xC000:
		case	0xE000:
			nes->apu->ExWrite( addr, data );
			break;
	}
}

void	Mapper069::Clock( INT cycles )
{
	if( irq_enable && (nes->GetIrqType() == NES::IRQ_CLOCK) ) {
		irq_counter -= cycles;
		if( irq_counter <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

void	Mapper069::HSync( INT scanline )
{
	if( irq_enable && (nes->GetIrqType() == NES::IRQ_HSYNC) ) {
		irq_counter -= 114;
		if( irq_counter <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

void	Mapper069::SaveState( LPBYTE p )
{
	p[0] = reg;
	p[1] = irq_enable;
	*(INT*)&p[2] = irq_counter;
}

void	Mapper069::LoadState( LPBYTE p )
{
	reg = p[0];
	irq_enable  = p[1];
	irq_counter = *(INT*)&p[2];
}
