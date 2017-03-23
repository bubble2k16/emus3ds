//////////////////////////////////////////////////////////////////////////
// Mapper087  Konami 74161/32                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper087::Reset()
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper087::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		SetVROM_8K_Bank( (data & 0x02)>>1 );
	}
}
