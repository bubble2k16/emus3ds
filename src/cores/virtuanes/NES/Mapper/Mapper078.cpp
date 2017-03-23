//////////////////////////////////////////////////////////////////////////
// Mapper078  Jaleco(Cosmo Carrier)                                     //
//////////////////////////////////////////////////////////////////////////
void	Mapper078::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper078::Write( WORD addr, BYTE data )
{
//DEBUGOUT( "MAP78 WR $%04X=$%02X L=%d\n", addr, data, nes->GetScanline() );
	SetPROM_16K_Bank( 4, data&0x0F );
	SetVROM_8K_Bank( (data&0xF0)>>4 );

	if( (addr & 0xFE00) != 0xFE00 ) {
		if( data & 0x08 ) SetVRAM_Mirror( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror( VRAM_MIRROR4L );
	}
}
