//////////////////////////////////////////////////////////////////////////
// Mapper241  Fon Serm Bon                                              //
//////////////////////////////////////////////////////////////////////////
void    Mapper241::Reset()
{
	SetPROM_32K_Bank( 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper241::Write( WORD addr, BYTE data )
{
	if( addr >= 0x8000 ) {
		SetPROM_32K_Bank( data );
	}
}


BYTE	Mapper241::ReadLow ( WORD A )
{
	if(A<0x6000)
	{
		return 0x50;
	}
	else
		return Mapper::ReadLow(A);
}