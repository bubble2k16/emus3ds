//////////////////////////////////////////////////////////////////////////
// Paddle                                                               //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_Paddle::Reset()
{
	paddle_bits = 0;
	paddle_data = 0;
	paddle_posold = 0;
}

BYTE	EXPAD_Paddle::Read4016()
{
	return	paddle_bits;
}

BYTE	EXPAD_Paddle::Read4017()
{
BYTE	data = (paddle_data&0x01)<<1;
	paddle_data >>= 1;
	return	data;
}

void	EXPAD_Paddle::Write4016( BYTE data )
{
LONG	x;

	paddle_bits = 0;
	if( paddle_button )
		paddle_bits |= 0x02;

	x = paddle_x;
	if( x < 0 ) {
		paddle_data = paddle_posold;
		return;
	}

	if( x <  32 ) x =  32;
	if( x > 223 ) x = 223;
	x = (192*(x-32))/192;

	BYTE	px = 0xFF-(BYTE)(0x52+172*x/192);

	// ���E�r�b�g���]
	paddle_data = 0;
	for( INT i = 0; i < 8; i++ ) {
		paddle_data |= (px&(1<<i))?(0x80>>i):0;
	}
	paddle_posold = paddle_data;
}

void	EXPAD_Paddle::Sync()
{
	LONG	y;
	nes->GetZapperPos( paddle_x, y );

	paddle_button = 0;
	//if( ::GetAsyncKeyState(VK_LBUTTON)&0x8000 )
	//	paddle_button = 0xFF;
}

void	EXPAD_Paddle::SetSyncData( INT type, LONG data )
{
	if( type == 0 ) {
		paddle_x = data;
	} else if( type == 2 ) {
		paddle_button = data?0xFF:0x00;
	}
}

LONG	EXPAD_Paddle::GetSyncData( INT type )
{
LONG	data = 0;

	if( type == 0 ) {
		data = paddle_x;
	} else if( type == 2 ) {
		data = paddle_button?0xFF:0x00;
	}
	return	data;
}

