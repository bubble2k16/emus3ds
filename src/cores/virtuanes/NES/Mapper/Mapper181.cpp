//////////////////////////////////////////////////////////////////////////
// Mapper181  Hacker International Type2                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper181::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper181::WriteLow( WORD addr, BYTE data )
{
//DEBUGOUT( "$%04X:$%02X\n", addr, data );
	if( addr == 0x4120 ) {
		SetPROM_32K_Bank( (data & 0x08) >> 3 );
		SetVROM_8K_Bank( data & 0x07 );
	}
}

