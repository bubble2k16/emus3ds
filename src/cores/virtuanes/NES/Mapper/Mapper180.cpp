//////////////////////////////////////////////////////////////////////////
// Mapper180  Nichibutsu                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper180::Reset()
{
	SetPROM_32K_Bank( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper180::Write( WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 6, data&0x07 );
}
