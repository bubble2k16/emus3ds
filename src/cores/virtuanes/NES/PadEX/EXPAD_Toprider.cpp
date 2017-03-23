//////////////////////////////////////////////////////////////////////////
// Toprider                                                             //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_Toprider::Reset()
{
	rider_bita = 0;
	rider_bitb = 0;

	rider_pos = 0;
	rider_accel = 0;
	rider_brake = 0;

	rider_button = 0;
}

void	EXPAD_Toprider::Write4016( BYTE data )
{
	if( data & 0x01 ) {
		rider_bita = rider_bitb = 0;
		if( rider_pos > 0 ) {
		// RIGHT
			if( rider_pos > 0x10 ) {
				rider_bita |= (1<<1)|(1<<3);
			} else if( rider_pos > 0x0A ) {
				rider_bita |= (1<<1)|(0<<3);
			} else if( rider_pos > 0x04 ) {
				rider_bita |= (0<<1)|(1<<3);
			}
		} else {
		// LEFT
			if( rider_pos < -0x10 ) {
				rider_bita |= (1<<2)|(1<<4);
			} else if( rider_pos < -0x0A ) {
				rider_bita |= (1<<2)|(0<<4);
			} else if( rider_pos < -0x04 ) {
				rider_bita |= (0<<2)|(1<<4);
			}
		}
		if( rider_accel > 0x08 || rider_brake < 0x08 ) {
			if( rider_accel > 0x10 ) {
				rider_bitb |= (1<<0);
			} else if( rider_accel > 0x0A ) {
				rider_bitb |= (1<<4);
			} else if( rider_accel > 0x04 ) {
				rider_bitb |= (1<<5);
			}
		} else {
			rider_bita |= (1<<5);
			if( rider_brake > 0x10 ) {
				rider_bitb |= (1<<1);
			} else if( rider_brake > 0x0A ) {
				rider_bitb |= (1<<2);
			} else if( rider_brake > 0x04 ) {
				rider_bitb |= (1<<3);
			}
		}
		// �E�C���[
		if( rider_button&0x01 )
			rider_bita |= 0x80;
		// SHIFT(Toggle)
		if( rider_button&0x80 )
			rider_bita |= 0x40;
		// START,SELECT
		if( rider_button&0x10 )
			rider_bitb |= 0x40;
		if( rider_button&0x20 )
			rider_bitb |= 0x80;
	}
}

BYTE	EXPAD_Toprider::Read4017()
{
BYTE	data = 0;
	data |= (rider_bita&0x01)<<4;
	data |= (rider_bitb&0x01)<<3;
	rider_bita >>= 1;
	rider_bitb >>= 1;
	return	data;
}

void	EXPAD_Toprider::Sync()
{
BYTE	bit = 0;

/*
	// Up
	if( Config.controller.nButton[0][ 0] && DirectInput.m_Sw[Config.controller.nButton[0][ 0]]
	 || Config.controller.nButton[0][16] && DirectInput.m_Sw[Config.controller.nButton[0][16]] )
		bit |= 1<<4;
	// Down
	if( Config.controller.nButton[0][ 1] && DirectInput.m_Sw[Config.controller.nButton[0][ 1]]
	 || Config.controller.nButton[0][17] && DirectInput.m_Sw[Config.controller.nButton[0][17]] )
		bit |= 1<<5;
	// Left
	if( Config.controller.nButton[0][ 2] && DirectInput.m_Sw[Config.controller.nButton[0][ 2]]
	 || Config.controller.nButton[0][18] && DirectInput.m_Sw[Config.controller.nButton[0][18]] )
		bit |= 1<<6;
	// Right
	if( Config.controller.nButton[0][ 3] && DirectInput.m_Sw[Config.controller.nButton[0][ 3]]
	 || Config.controller.nButton[0][19] && DirectInput.m_Sw[Config.controller.nButton[0][19]] )
		bit |= 1<<7;

	// ���E�������͂��֎~����
	if( (bit&((1<<6)|(1<<7))) == ((1<<6)|(1<<7)) )
		bit &= ~((1<<6)|(1<<7));

	// A,B
	if( Config.controller.nButton[0][ 4] && DirectInput.m_Sw[Config.controller.nButton[0][ 4]]
	 || Config.controller.nButton[0][20] && DirectInput.m_Sw[Config.controller.nButton[0][20]] )
		bit |= 1<<0;
	if( Config.controller.nButton[0][ 5] && DirectInput.m_Sw[Config.controller.nButton[0][ 5]]
	 || Config.controller.nButton[0][21] && DirectInput.m_Sw[Config.controller.nButton[0][21]] )
		bit |= 1<<1;

	// Start, Select
	if( Config.controller.nButton[0][ 8] && DirectInput.m_Sw[Config.controller.nButton[0][ 8]]
	 || Config.controller.nButton[0][24] && DirectInput.m_Sw[Config.controller.nButton[0][24]] )
		bit |= 1<<2;
	if( Config.controller.nButton[0][ 9] && DirectInput.m_Sw[Config.controller.nButton[0][ 9]]
	 || Config.controller.nButton[0][25] && DirectInput.m_Sw[Config.controller.nButton[0][25]] )
		bit |= 1<<3;
*/

	if( !(bit & ((1<<6)|(1<<7))) ) {
		// To center...
		if( rider_pos ) {
			rider_pos += (rider_pos>0)?-1:1;
		}
	} else if( bit & (1<<6) ) {
		// LEFT
		rider_pos -= (rider_pos>-0x18)?1:0;
	} else if( bit & (1<<7) ) {
		// RIGHT
		rider_pos += (rider_pos<0x18)?1:0;
	}

	// Brake(A)
	if( bit & (1<<0) ) {
		rider_brake += (rider_brake<0x18)?1:0;
	} else {
		rider_brake -= (rider_brake>0)?1:0;
	}
	// Accel(B)
	if( bit & (1<<1) ) {
		rider_accel += (rider_accel<0x18)?1:0;
	} else {
		rider_accel -= (rider_accel>0)?1:0;
	}

	rider_button &= 0xC0;

	// Shift(UP)(Toggle)
	if( bit & (1<<4) ) {
		if( !(rider_button&0x40) ) {
			if( rider_button&0x80 ) {
				rider_button &= ~0x80;
			} else {
				rider_button |= 0x80;
			}
		}
	}
	// old
	if( bit & (1<<4) )
		rider_button |= 0x40;
	else
		rider_button &= ~0x40;

	// Willey(DOWN)
	if( bit & (1<<5) )
		rider_button |= 0x01;
	// Start, Select
	if( bit & (1<<2) )
		rider_button |= 0x20;
	if( bit & (1<<3) )
		rider_button |= 0x10;
#if	0
DEBUGOUT( "RIDER POS=%d\n", rider_pos );
DEBUGOUT( "RIDER ACC=%d\n", rider_accel );
DEBUGOUT( "RIDER BRK=%d\n", rider_brake );
DEBUGOUT( "RIDER BTN=%02X\n", rider_button );
#endif
}

void	EXPAD_Toprider::SetSyncData( INT type, LONG data )
{
	rider_pos    = (INT)((SBYTE)( data     &0x000000FF));
	rider_accel  = (INT)((SBYTE)((data>> 8)&0x000000FF));
	rider_brake  = (INT)((SBYTE)((data>>16)&0x000000FF));
	rider_button = (BYTE)(data>>24);

#if	0
DEBUGOUT( "RIDER POS=%d\n", rider_pos );
DEBUGOUT( "RIDER ACC=%d\n", rider_accel );
DEBUGOUT( "RIDER BRK=%d\n", rider_brake );
DEBUGOUT( "RIDER BTN=%02X\n", rider_button );
#endif
}

LONG	EXPAD_Toprider::GetSyncData( INT type )
{
LONG	data = 0;

	data = (rider_pos&0xFF)|
	       ((rider_accel&0xFF)<< 8)|
	       ((rider_brake&0xFF)<<16)|
	       (rider_button<<24);

	return	data;
}

