//////////////////////////////////////////////////////////////////////////
// Gyromite
//////////////////////////////////////////////////////////////////////////
INT	EXPAD_Gyromite::CheckSignal()
{
	RGBQUAD palette[256];
	RGBQUAD pixel;

	// throttling detection issues
	//DirectDraw.GetPaletteData( palette );
	pixel = palette[BGPAL[0]];

	for( INT lcv=1; lcv<16; lcv++ ) {
		// reject non-solid screens
		if( memcmp( &(palette[BGPAL[lcv]]), &pixel, sizeof(RGBQUAD) ) != 0 ) {
			return 0xF00;
	}
	}

	// check for a black or green screen
	return ( pixel.rgbGreen > 0x10 ) ? 1:0;
}

void	EXPAD_Gyromite::Reset()
{
	bits = 0;
	arms = 0;
	spot = 0;

	// Initialize light on/off sequences
	code[0] = 0x0EE8;	// Open arm
	code[1] = 0x0FA8;	// Close arm
	code[2] = 0x1AE8;	// Test robot
	code[3] = 0x0AAA;	// Unused
	code[4] = 0x1BA8;	// Raise arm
	code[5] = 0x1BE8;	// Lower arm
	code[6] = 0x0BA8;	// Turn left
	code[7] = 0x0AE8;	// Turn right
}

void	EXPAD_Gyromite::Strobe()
{
	nes->pad->pad2bit &= ~0xFF;
	nes->pad->pad2bit |= arms;
}

void	EXPAD_Gyromite::Sync()
{
//	INT signal = CheckSignal( 129 );
	INT signal = CheckSignal();

	// build our message
	if( signal > 0xFF ) {
		bits = 0;
		spot = 0;
	}
	else {
		bits |= (signal << spot);
		spot++;

		// Check for a test message: BBBBBB[test]
		if( spot==6 && bits==0 ) {
			spot = 0;
		}
	}

	// decode message
	if( spot==13 ) {
		for( INT i=0; i<8; i++ ) {
			if( bits == code[i] ) {
				arms ^= (1<<i);
			}
		}
		spot = 0;
		bits = 0;
	}
}

void	EXPAD_Gyromite::SetSyncData( INT type, LONG data )
{
	bits = ((WORD)( data     &0x0000FFFF));
	arms = ((BYTE)((data>>16)&0x000000FF));
	spot = (BYTE)(data>>24);
}

LONG	EXPAD_Gyromite::GetSyncData( INT type )
{
LONG	data = 0;

	data = (bits&0xFFFF)|
	       ((arms&0xFF)<<16)|
	       (spot<<24);

	return	data;
}
