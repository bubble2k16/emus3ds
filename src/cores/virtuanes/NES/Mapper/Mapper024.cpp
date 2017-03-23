//////////////////////////////////////////////////////////////////////////
// Mapper024  Konami VRC6(Normal)                                       //
//////////////////////////////////////////////////////////////////////////
void	Mapper024::Reset()
{
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	nes->SetRenderMethod( NES::POST_RENDER );
//	nes->SetRenderMethod( NES::PRE_RENDER );

	nes->apu->SelectExSound( 1 );
}

void	Mapper024::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case 0x8000:
			SetPROM_16K_Bank( 4, data );
			break;

		case 0x9000: case 0x9001: case 0x9002:
		case 0xA000: case 0xA001: case 0xA002:
		case 0xB000: case 0xB001: case 0xB002:
			nes->apu->ExWrite( addr, data );
			break;

		case 0xB003:
			data = data & 0x0C;
			if( data == 0x00 )	SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 0x04 )	SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 0x08 )	SetVRAM_Mirror( VRAM_MIRROR4L );
			else if( data == 0x0C )	SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case 0xC000:
			SetPROM_8K_Bank( 6, data );
			break;

		case 0xD000:
			SetVROM_1K_Bank( 0, data );
			break;

		case 0xD001:
			SetVROM_1K_Bank( 1, data );
			break;

		case 0xD002:
			SetVROM_1K_Bank( 2, data );
			break;

		case 0xD003:
			SetVROM_1K_Bank( 3, data );
			break;

		case 0xE000:
			SetVROM_1K_Bank( 4, data );
			break;

		case 0xE001:
			SetVROM_1K_Bank( 5, data );
			break;

		case 0xE002:
			SetVROM_1K_Bank( 6, data );
			break;

		case 0xE003:
			SetVROM_1K_Bank( 7, data );
			break;

		case 0xF000:
			irq_latch = data;
			break;
		case 0xF001:
			irq_enable = data & 0x03;
			if( irq_enable & 0x02 ) {
				irq_counter = irq_latch;
				irq_clock = 0;
			}
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case 0xF002:
			irq_enable = (irq_enable & 0x01) * 3;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper024::Clock( INT cycles )
{
	if( irq_enable & 0x02 ) {
		if( (irq_clock+=cycles) >= 0x72 ) {
			irq_clock -= 0x72;
			if( irq_counter == 0xFF ) {
				irq_counter = irq_latch;
//				nes->cpu->IRQ_NotPending();
				nes->cpu->SetIRQ( IRQ_MAPPER );
			} else {
				irq_counter++;
			}
		}
	}
}

void	Mapper024::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
	p[2] = irq_latch;
	*(INT*)&p[3] = irq_clock;
}

void	Mapper024::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = p[1];
	irq_latch   = p[2];
	irq_clock   = *(INT*)&p[3];
}
