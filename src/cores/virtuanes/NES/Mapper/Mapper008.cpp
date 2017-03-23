//////////////////////////////////////////////////////////////////////////
// Mapper008  FFE F3xxx                                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper008::Reset()
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper008::Write( WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, (data&0xF8)>>3 );
	SetVROM_8K_Bank( data&0x07 );
}

