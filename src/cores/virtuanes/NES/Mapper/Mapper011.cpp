//////////////////////////////////////////////////////////////////////////
// Mapper011  Color Dreams                                              //
//////////////////////////////////////////////////////////////////////////
void	Mapper011::Reset()
{
	SetPROM_32K_Bank( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
//		SetVROM_8K_Bank( 1 );
	}
	SetVRAM_Mirror( VRAM_VMIRROR );
}

void	Mapper011::Write( WORD addr, BYTE data )
{
DEBUGOUT( "WR A:%04X D:%02X\n", addr, data );
	SetPROM_32K_Bank( data );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( data>>4 );
	}
}

