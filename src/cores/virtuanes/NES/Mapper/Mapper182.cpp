//////////////////////////////////////////////////////////////////////////
// Mapper182  PC-SuperDonkeyKong                                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper182::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

	reg = 0;
	irq_enable = 0;
	irq_counter = 0;
}

void	Mapper182::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case	0x8001:
			if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		  SetVRAM_Mirror( VRAM_VMIRROR );
			break;
		case	0xA000:
			reg = data & 0x07;
			break;
		case	0xC000:
			switch( reg ) {
				case	0:
					SetVROM_1K_Bank( 0, (data&0xFE)+0 );
					SetVROM_1K_Bank( 1, (data&0xFE)+1 );
					break;
				case	1:
					SetVROM_1K_Bank( 5, data );
					break;
				case	2:
					SetVROM_1K_Bank( 2, (data&0xFE)+0 );
					SetVROM_1K_Bank( 3, (data&0xFE)+1 );
					break;
				case	3:
					SetVROM_1K_Bank( 7, data );
					break;
				case	4:
					SetPROM_8K_Bank( 4, data );
					break;
				case	5:
					SetPROM_8K_Bank( 5, data );
					break;
				case	6:
					SetVROM_1K_Bank( 4, data );
					break;
				case	7:
					SetVROM_1K_Bank( 6, data );
					break;
			}
			break;
		case	0xE003:
			irq_enable  = data;
			irq_counter = data;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper182::HSync( INT scanline )
{
	if( irq_enable ) {
		if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
			if( !(--irq_counter) ) {
				irq_enable  = 0;
				irq_counter = 0;
//				nes->cpu->IRQ_NotPending();
				nes->cpu->SetIRQ( IRQ_MAPPER );
			}
		}
	}
}

void	Mapper182::SaveState( LPBYTE p )
{
	p[0] = reg;
	p[1] = irq_enable;
	p[2] = irq_counter;
}

void	Mapper182::LoadState( LPBYTE p )
{
	reg         = p[0];
	irq_enable  = p[1];
	irq_counter = p[2];
}
