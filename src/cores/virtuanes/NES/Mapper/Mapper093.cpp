//////////////////////////////////////////////////////////////////////////
// Mapper093  SunSoft (Fantasy Zone)                                    //
//////////////////////////////////////////////////////////////////////////
void	Mapper093::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper093::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetPROM_16K_Bank( 4, data );
	}
}
