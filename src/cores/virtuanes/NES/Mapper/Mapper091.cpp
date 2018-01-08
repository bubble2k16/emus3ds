//////////////////////////////////////////////////////////////////////////
// Mapper091  PC-HK-SF3                                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper091::Reset()
{
	SetPROM_32K_Bank( PROM_8K_SIZE-2, PROM_8K_SIZE-1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0, 0, 0, 0, 0, 0, 0, 0 );
	}

	irq_enable = 0;
	irq_counter = 0;

	nes->SetRenderMethod( NES::POST_ALL_RENDER );
}

void	Mapper091::WriteLow( WORD addr, BYTE data )
{
//DEBUGOUT( "$%04X:$%02X(%3d) L=%3d\n", addr, data, data, nes->GetScanline() );
	switch( addr & 0xF003 ) {
		case	0x6000:
		case	0x6001:
		case	0x6002:
		case	0x6003:
			SetVROM_2K_Bank( (addr&0x03)*2, data );
			break;

		case	0x7000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x7001:
			SetPROM_8K_Bank( 5, data );
			break;

		case	0x7002:
			irq_enable = 0;
			irq_counter = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x7003:
			irq_enable = 1;
			break;
	}
}

void	Mapper091::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline < 240) && nes->ppu->IsDispON() ) {
		if( irq_enable ) {
			irq_counter++;
		}
		if( irq_counter >= 8 ) {
//			nes->cpu->IRQ_NotPending();
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper091::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
}

void	Mapper091::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = p[1];
}
