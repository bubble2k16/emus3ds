//////////////////////////////////////////////////////////////////////////
// Mapper079  Nina-3                                                    //
//////////////////////////////////////////////////////////////////////////
void	Mapper079::Reset()
{
	SetPROM_32K_Bank( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper079::WriteLow( WORD addr, BYTE data )
{
	if( addr&0x0100 ) {
		SetPROM_32K_Bank( (data>>3)&0x01 );
		SetVROM_8K_Bank( data&0x07 );
	}
}
