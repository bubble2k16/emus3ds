//////////////////////////////////////////////////////////////////////////
// Mapper077  Irem Early Mapper #0                                      //
//////////////////////////////////////////////////////////////////////////
void	Mapper077::Reset()
{
	SetPROM_32K_Bank( 0 );

	SetVROM_2K_Bank( 0, 0 );
	SetCRAM_2K_Bank( 2, 1 );
	SetCRAM_2K_Bank( 4, 2 );
	SetCRAM_2K_Bank( 6, 3 );
}

void	Mapper077::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data & 0x07 );

	SetVROM_2K_Bank( 0, (data & 0xF0)>>4 );
}
