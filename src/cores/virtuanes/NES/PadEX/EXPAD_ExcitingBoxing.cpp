//////////////////////////////////////////////////////////////////////////
// Exciting Boxing                                                      //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_ExcitingBoxing::Reset()
{
	outbits = 0;
	padbits = 0;
}

BYTE	EXPAD_ExcitingBoxing::Read4017()
{
	return	outbits;
}

void	EXPAD_ExcitingBoxing::Write4016( BYTE data )
{
	outbits = 0;
	if( data & 0x02 ) {
		outbits = (~padbits<<1)&0x1E;
	} else {
		outbits = (~padbits>>3)&0x1E;
	}
}

void	EXPAD_ExcitingBoxing::Sync()
{
	padbits = 0;

	// Straight
	if( Config.ExButtonCheck( 2,  0 ) ) padbits |= 1<<3;
	// Right Jabb
	if( Config.ExButtonCheck( 2,  1 ) ) padbits |= 1<<2;
	// Body
	if( Config.ExButtonCheck( 2,  2 ) ) padbits |= 1<<1;
	// Left Jabb
	if( Config.ExButtonCheck( 2,  3 ) ) padbits |= 1<<0;
	// Right hook
	if( Config.ExButtonCheck( 2,  4 ) ) padbits |= 1<<7;
	// Left hook
	if( Config.ExButtonCheck( 2,  5 ) ) padbits |= 1<<4;
	// Right move
	if( Config.ExButtonCheck( 2,  6 ) ) padbits |= 1<<5;
	// Left move
	if( Config.ExButtonCheck( 2,  7 ) ) padbits |= 1<<6;
}

void	EXPAD_ExcitingBoxing::SetSyncData( INT type, LONG data )
{
	padbits = (BYTE)data;
}

LONG	EXPAD_ExcitingBoxing::GetSyncData( INT type )
{
	return	(LONG)padbits;
}
