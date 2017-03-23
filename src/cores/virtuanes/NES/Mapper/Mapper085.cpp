//////////////////////////////////////////////////////////////////////////
// Mapper085  Konami VRC7                                               //
//////////////////////////////////////////////////////////////////////////
void	Mapper085::Reset()
{
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

#if	0
//	DWORD	crc = nes->rom->GetPROM_CRC();
//	if( crc == 0x1aa0479c ) {	// For Tiny Toon Adventures 2 - Montana Land he Youkoso(J)
//		nes->SetRenderMethod( NES::PRE_RENDER );
//	}
//	if( crc == 0x33ce3ff0 ) {	// For Lagrange Point(J)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//	}
#endif
	nes->apu->SelectExSound( 2 );
}

void	Mapper085::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF038 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8008:
		case	0x8010:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x9000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0x9010:
		case	0x9030:
			nes->apu->ExWrite( addr, data );
			break;

		case	0xA000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 0, data );
			} else {
				SetCRAM_1K_Bank( 0, data );
			}
			break;

		case	0xA008:
		case	0xA010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 1, data );
			} else {
				SetCRAM_1K_Bank( 1, data );
			}
			break;

		case	0xB000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 2, data );
			} else {
				SetCRAM_1K_Bank( 2, data );
			}
			break;

		case	0xB008:
		case	0xB010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 3, data );
			} else {
				SetCRAM_1K_Bank( 3, data );
			}
			break;

		case	0xC000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 4, data );
			} else {
				SetCRAM_1K_Bank( 4, data );
			}
			break;

		case	0xC008:
		case	0xC010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 5, data );
			} else {
				SetCRAM_1K_Bank( 5, data );
			}
			break;

		case	0xD000:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 6, data );
			} else {
				SetCRAM_1K_Bank( 6, data );
			}
			break;

		case	0xD008:
		case	0xD010:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( 7, data );
			} else {
				SetCRAM_1K_Bank( 7, data );
			}
			break;

		case	0xE000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0xE008:
		case	0xE010:
			irq_latch = data;
			break;

		case	0xF000:
			irq_enable = data & 0x03;
			irq_counter = irq_latch;
			irq_clock = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xF008:
		case	0xF010:
			irq_enable = (irq_enable & 0x01) * 3;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper085::Clock( INT cycles )
{
	if( irq_enable & 0x02 ) {
		irq_clock += cycles*4;
		while( irq_clock >= 455 ) {
			irq_clock -= 455;
			irq_counter++;
			if( irq_counter == 0 ) {
				irq_counter = irq_latch;
				nes->cpu->SetIRQ( IRQ_MAPPER );
			}
		}
	}
}

void	Mapper085::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
	p[2] = irq_latch;
	*((INT*)&p[4]) = irq_clock;
}

void	Mapper085::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = p[1];
	irq_latch   = p[2];
	irq_clock   = *((INT*)&p[4]);
}
