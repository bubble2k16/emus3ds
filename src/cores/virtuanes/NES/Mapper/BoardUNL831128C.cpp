//////////////////////////////////////////////////////////////////////////
// Board831128C                                                         //
//////////////////////////////////////////////////////////////////////////
void	Board831128C::Reset()
{
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	bn = 1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

}

void	Board831128C::Write( WORD addr, BYTE data )
{
//	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
	
	BYTE bank = (data&((0x10*bn)|0x0F))+(0x10*bn);
	switch( addr & 0x0F ) {
		case	0x00:	case	0x01:
		case	0x02:	case	0x03:
		case	0x04:	case	0x05:
		case	0x06:	case	0x07:
			SetVROM_1K_Bank( addr&0x07, data+(0x100*bn) );
			break;

		case	0x08:
			if(!bn && !(data&0x40)) SetPROM_8K_Bank( 3, bank );
			break;
		case	0x09:
			SetPROM_8K_Bank( 4, bank );
			break;
		case	0x0A:
			SetPROM_8K_Bank( 5, bank );
			break;
		case	0x0B:
			DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
//			SetPROM_8K_Bank( 6, bank );
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

//			irq_enable = (irq_enable & 0x01) * 3;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x0E:
			bn = (addr >> 14) & 1;
			SetPROM_32K_Bank( 0, 1, 0x0E|(0x20*bn), 0x0F|(0x20*bn) );
			break;
		case	0x0F:
			irq_counter = (irq_counter & 0xFF00) | RAM[0x0E];
			BYTE VVV = (((data-0xF1)&0xFF)^0xFF)>>1;
			irq_counter = (irq_counter & 0x00FF) | (VVV << 8);

//			irq_enable = data & 0x03;
//			irq_latch = data;
//			irq_counter = data;
//			irq_clock = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}

}

void	Board831128C::HSync( INT scanline )
{
	if( irq_enable ) {
		irq_counter -= 114;
		if( irq_counter <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

void	Board831128C::Clock( INT cycles )
{
/*	if( irq_enable ) {
		irq_clock += cycles*4;
		while( irq_clock >= 455 ) {
			irq_clock -= 455;
			irq_counter++;
			if( irq_counter == 0 ) {
				irq_counter = irq_latch;
				nes->cpu->SetIRQ( IRQ_MAPPER );
			}
		}
	}*/
}

void	Board831128C::SaveState( LPBYTE p )
{
	//
}

void	Board831128C::LoadState( LPBYTE p )
{
	//
}
