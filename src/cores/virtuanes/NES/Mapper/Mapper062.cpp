//////////////////////////////////////////////////////////////////////////
// Mapper062                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper062::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper062::Write( WORD addr, BYTE data )
{
	switch( addr & 0xFF00 ) {
		case	0x8100:
			SetPROM_8K_Bank( 4, data );
			SetPROM_8K_Bank( 5, data+1 );
			break;
		case	0x8500:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8700:
			SetPROM_8K_Bank( 5, data );
			break;
	SetVROM_1K_Bank( 0, data+0 );
	SetVROM_1K_Bank( 1, data+1 );
	SetVROM_1K_Bank( 2, data+2 );
	SetVROM_1K_Bank( 3, data+3 );
	SetVROM_1K_Bank( 4, data+4 );
	SetVROM_1K_Bank( 5, data+5 );
	SetVROM_1K_Bank( 6, data+6 );
	SetVROM_1K_Bank( 7, data+7 );
	}
}

