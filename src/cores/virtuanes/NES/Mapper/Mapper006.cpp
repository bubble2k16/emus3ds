//////////////////////////////////////////////////////////////////////////
// Mapper006  FFE F4xxx                                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper006::Reset()
{
	SetPROM_32K_Bank( 0, 1, 14, 15 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

	irq_enable = 0;
	irq_counter = 0;
}

void	Mapper006::WriteLow( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x42FE:
			if( data&0x10 ) SetVRAM_Mirror( VRAM_MIRROR4H );
			else		SetVRAM_Mirror( VRAM_MIRROR4L );
			break;
		case	0x42FF:
			if( data&0x10 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		SetVRAM_Mirror( VRAM_VMIRROR );
			break;

		case	0x4501:
			irq_enable = 0;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x4502:
			irq_counter = (irq_counter&0xFF00)|data;
			break;
		case	0x4503:
			irq_counter = (irq_counter&0x00FF)|((INT)data<<8);
			irq_enable = 0xFF;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		default:
			Mapper::WriteLow( addr, data );
			break;
	}
}

void	Mapper006::Write( WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, (data&0x3C)>>2 );
	SetCRAM_8K_Bank( data&0x03 );
}

void	Mapper006::HSync( INT scanline )
{
	if( irq_enable ) {
		irq_counter+=133;
		if( irq_counter >= 0xFFFF ) {
//			nes->cpu->IRQ();
			irq_counter = 0;

			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper006::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	*(INT*)&p[1] = irq_counter;
}

void	Mapper006::LoadState( LPBYTE p )
{
	irq_enable = p[0];
	irq_counter = *(INT*)&p[1];
}
