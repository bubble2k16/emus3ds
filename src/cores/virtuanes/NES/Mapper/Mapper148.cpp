//////////////////////////////////////////////////////////////////////////
// Mapper148  SACHEN                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper148::Reset()
{
	SetPROM_32K_Bank( 0 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper148::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data >> 3 & 0x1 );
	SetVROM_8K_Bank( data & 0x7 );
}

