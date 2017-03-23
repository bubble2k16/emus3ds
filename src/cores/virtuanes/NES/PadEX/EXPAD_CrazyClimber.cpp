//////////////////////////////////////////////////////////////////////////
// Crazy climber                                                        //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_CrazyClimber::Strobe()
{
	nes->pad->pad1bit &= ~0xF0;
	nes->pad->pad1bit |=  bits & 0xF0;
	nes->pad->pad2bit &= ~0xF0;
	nes->pad->pad2bit |= (bits & 0x0F)<<4;
}

void	EXPAD_CrazyClimber::Sync()
{
	bits = 0;

	// Left
	if( Config.ExButtonCheck( 0,  0 ) ) bits |= 1<<6;
	if( Config.ExButtonCheck( 0,  1 ) ) bits |= 1<<7;
	if( Config.ExButtonCheck( 0,  2 ) ) bits |= 1<<5;
	if( Config.ExButtonCheck( 0,  3 ) ) bits |= 1<<4;

	// Right
	if( Config.ExButtonCheck( 0,  4 ) ) bits |= 1<<2;
	if( Config.ExButtonCheck( 0,  5 ) ) bits |= 1<<3;
	if( Config.ExButtonCheck( 0,  6 ) ) bits |= 1<<1;
	if( Config.ExButtonCheck( 0,  7 ) ) bits |= 1<<0;

	// “¯“ü—Í‚ğ‹Ö~‚·‚é
	if( (bits&((1<<0)|(1<<1))) == ((1<<0)|(1<<1)) )
		bits &= ~((1<<0)|(1<<1));
	if( (bits&((1<<2)|(1<<3))) == ((1<<2)|(1<<3)) )
		bits &= ~((1<<2)|(1<<3));
	if( (bits&((1<<4)|(1<<5))) == ((1<<4)|(1<<5)) )
		bits &= ~((1<<4)|(1<<5));
	if( (bits&((1<<6)|(1<<7))) == ((1<<6)|(1<<7)) )
		bits &= ~((1<<6)|(1<<7));
}

void	EXPAD_CrazyClimber::SetSyncData( INT type, LONG data )
{
	bits = (BYTE)data;
}

LONG	EXPAD_CrazyClimber::GetSyncData( INT type )
{
	return	bits;
}

