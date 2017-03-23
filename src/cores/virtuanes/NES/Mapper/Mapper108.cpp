//////////////////////////////////////////////////////////////////////////
// Mapper108                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper108::Reset()
{
	SetPROM_32K_Bank( 0xC,0xD,0xE,0xF );
	SetPROM_8K_Bank( 3, 0 );
}

void	Mapper108::Write( WORD addr, BYTE data )
{
	SetPROM_8K_Bank( 3, data );
}
