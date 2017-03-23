//////////////////////////////////////////////////////////////////////////
// Famly Trainer                                                        //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_FamlyTrainer::Reset()
{
	outbits = 0;
	padbits = 0;
}

BYTE	EXPAD_FamlyTrainer::Read4017()
{
	return	outbits;
}

void	EXPAD_FamlyTrainer::Write4016( BYTE data )
{
	outbits = 0;
	if( !(data & 0x04) ) {
		outbits = (BYTE)((~padbits<<1)&0x1E);
	}
	if( !(data & 0x02) ) {
		outbits = (BYTE)((~padbits>>3)&0x1E);
	}
	if( !(data & 0x01) ) {
		outbits = (BYTE)((~padbits>>7)&0x1E);
	}
}

void	EXPAD_FamlyTrainer::Sync()
{
	padbits = 0;

	if( nes->pad->GetExController() == PAD::EXCONTROLLER_FAMILYTRAINER_A ) {
		if( Config.ExButtonCheck( 1,  3 ) ) padbits |= 1<<3;
		if( Config.ExButtonCheck( 1,  2 ) ) padbits |= 1<<2;
		if( Config.ExButtonCheck( 1,  1 ) ) padbits |= 1<<1;
		if( Config.ExButtonCheck( 1,  0 ) ) padbits |= 1<<0;
		if( Config.ExButtonCheck( 1,  7 ) ) padbits |= 1<<7;
		if( Config.ExButtonCheck( 1,  6 ) ) padbits |= 1<<6;
		if( Config.ExButtonCheck( 1,  5 ) ) padbits |= 1<<5;
		if( Config.ExButtonCheck( 1,  4 ) ) padbits |= 1<<4;
		if( Config.ExButtonCheck( 1, 11 ) ) padbits |= 1<<11;
		if( Config.ExButtonCheck( 1, 10 ) ) padbits |= 1<<10;
		if( Config.ExButtonCheck( 1,  9 ) ) padbits |= 1<<9;
		if( Config.ExButtonCheck( 1,  8 ) ) padbits |= 1<<8;
	}
	if( nes->pad->GetExController() == PAD::EXCONTROLLER_FAMILYTRAINER_B ) {
		if( Config.ExButtonCheck( 1,  0 ) ) padbits |= 1<<3;
		if( Config.ExButtonCheck( 1,  1 ) ) padbits |= 1<<2;
		if( Config.ExButtonCheck( 1,  2 ) ) padbits |= 1<<1;
		if( Config.ExButtonCheck( 1,  3 ) ) padbits |= 1<<0;
		if( Config.ExButtonCheck( 1,  4 ) ) padbits |= 1<<7;
		if( Config.ExButtonCheck( 1,  5 ) ) padbits |= 1<<6;
		if( Config.ExButtonCheck( 1,  6 ) ) padbits |= 1<<5;
		if( Config.ExButtonCheck( 1,  7 ) ) padbits |= 1<<4;
		if( Config.ExButtonCheck( 1,  8 ) ) padbits |= 1<<11;
		if( Config.ExButtonCheck( 1,  9 ) ) padbits |= 1<<10;
		if( Config.ExButtonCheck( 1, 10 ) ) padbits |= 1<<9;
		if( Config.ExButtonCheck( 1, 11 ) ) padbits |= 1<<8;
	}
}

void	EXPAD_FamlyTrainer::SetSyncData( INT type, LONG data )
{
	padbits = (DWORD)data;
}

LONG	EXPAD_FamlyTrainer::GetSyncData( INT type )
{
	return	(LONG)padbits;
}
