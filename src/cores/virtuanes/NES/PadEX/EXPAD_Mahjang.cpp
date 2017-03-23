//////////////////////////////////////////////////////////////////////////
// Ide Yousuke Jissen Mahjang                                           //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_Mahjang::Reset()
{
	outbits = 0;
	padbits = 0;
}

BYTE	EXPAD_Mahjang::Read4017()
{
BYTE	out = (outbits&1)<<1;

	outbits >>= 1;
	return	out;
}

void	EXPAD_Mahjang::Write4016( BYTE data )
{
	outbits = 0;

	if( (data & 0x06) == 0x02 ) {
	// H-N
		outbits = (BYTE)padbits;
	} else
	if( (data & 0x06) == 0x04 ) {
	// A-G
		outbits = (BYTE)(padbits>>8);
	} else
	if( (data & 0x06) == 0x06 ) {
	// Start,Select,Kan,Pon,Chii,Reach,Ron
		outbits = (BYTE)(padbits>>16);
	}
}

void	EXPAD_Mahjang::Sync()
{
	padbits = 0;

	// H-N
	if( Config.ExButtonCheck( 3,  8 ) ) padbits |= 1<<7;
	if( Config.ExButtonCheck( 3,  9 ) ) padbits |= 1<<6;
	if( Config.ExButtonCheck( 3, 10 ) ) padbits |= 1<<5;
	if( Config.ExButtonCheck( 3, 11 ) ) padbits |= 1<<4;
	if( Config.ExButtonCheck( 3, 12 ) ) padbits |= 1<<3;
	if( Config.ExButtonCheck( 3, 13 ) ) padbits |= 1<<2;
	// A-G
	if( Config.ExButtonCheck( 3,  0 ) ) padbits |= 1<<(7+8);
	if( Config.ExButtonCheck( 3,  1 ) ) padbits |= 1<<(6+8);
	if( Config.ExButtonCheck( 3,  2 ) ) padbits |= 1<<(5+8);
	if( Config.ExButtonCheck( 3,  3 ) ) padbits |= 1<<(4+8);
	if( Config.ExButtonCheck( 3,  4 ) ) padbits |= 1<<(3+8);
	if( Config.ExButtonCheck( 3,  5 ) ) padbits |= 1<<(2+8);
	if( Config.ExButtonCheck( 3,  6 ) ) padbits |= 1<<(1+8);
	if( Config.ExButtonCheck( 3,  7 ) ) padbits |= 1<<(0+8);
	// Select,Start,Kan,Pon,Chii,Reach,Ron
	if( Config.ExButtonCheck( 3, 14 ) ) padbits |= 1<<(6+16);
	if( Config.ExButtonCheck( 3, 15 ) ) padbits |= 1<<(7+16);
	if( Config.ExButtonCheck( 3, 16 ) ) padbits |= 1<<(5+16);
	if( Config.ExButtonCheck( 3, 17 ) ) padbits |= 1<<(4+16);
	if( Config.ExButtonCheck( 3, 18 ) ) padbits |= 1<<(3+16);
	if( Config.ExButtonCheck( 3, 19 ) ) padbits |= 1<<(2+16);
	if( Config.ExButtonCheck( 3, 20 ) ) padbits |= 1<<(1+16);
DEBUGOUT( "%08X\n", padbits );
}

void	EXPAD_Mahjang::SetSyncData( INT type, LONG data )
{
	padbits = (DWORD)data;
}

LONG	EXPAD_Mahjang::GetSyncData( INT type )
{
	return	(LONG)padbits;
}
