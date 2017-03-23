//////////////////////////////////////////////////////////////////////////
// Mapper107  Magic Dragon Mapper                                       //
//////////////////////////////////////////////////////////////////////////
void	Mapper107::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper107::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( (data>>1)&0x03 );
	SetVROM_8K_Bank( data&0x07 );
}
