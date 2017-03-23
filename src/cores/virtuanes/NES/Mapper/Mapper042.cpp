//////////////////////////////////////////////////////////////////////////
// Mapper042  Mario Baby                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper042::Reset()
{
	irq_enable = 0;
	irq_counter = 0;

	SetPROM_8K_Bank( 3, 0 );
	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper042::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE003 ) {
		case	0xE000:
			SetPROM_8K_Bank( 3, data&0x0F );
			break;

		case	0xE001:
			if( data&0x08 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		SetVRAM_Mirror( VRAM_VMIRROR );
			break;

		case	0xE002:
			if( data&0x02 ) {
				irq_enable = 0xFF;
			} else {
				irq_enable = 0;
				irq_counter = 0;
			}
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper042::HSync( INT scanline )
{
	nes->cpu->ClrIRQ( IRQ_MAPPER );
	if( irq_enable ) {
		if( irq_counter < 215 ) {
			irq_counter++;
		}
		if( irq_counter == 215 ) {
			irq_enable = 0;
//			nes->cpu->IRQ();
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper042::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
}

void	Mapper042::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = p[1];
}
