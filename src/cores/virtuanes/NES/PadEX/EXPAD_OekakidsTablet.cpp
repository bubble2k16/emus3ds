//////////////////////////////////////////////////////////////////////////
// Oekakids Tablet                                                      //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_OekakidsTablet::Reset()
{
	olddata = 0;
	outbits = 0;
	databits = 0;

	zapper_x = 0;
	zapper_y = 0;
	zapper_button = 0;
}

BYTE	EXPAD_OekakidsTablet::Read4017()
{
	return	outbits;
}

void	EXPAD_OekakidsTablet::Write4016( BYTE data )
{
	//if( zapper_y < 48 ) {
	//	DirectDraw.SetZapperDrawMode( TRUE );
	//} else {
	//	DirectDraw.SetZapperDrawMode( FALSE );
	//}

	if( !(data & 0x01) ) {
		outbits = 0;
		databits = 0;

		if( zapper_button )
			databits |= 0x0001;

		LONG	x, y;

		if( zapper_y >= 48 ) {
			databits |= 0x0002;
		} else if( zapper_button ) {
			databits |= 0x0003;
		}

		if( zapper_x < 0 ) {
			x = 0;
		} else {
			x = ((240*zapper_x)/256)+8;
		}
		if( zapper_y < 0 ) {
			y = 0;
		} else {
			y = ((256*zapper_y)/240)-12;
			if( y < 0 )
				y = 0;
		}

		databits = databits | (x << 10) | (y << 2);
	} else {
		// L->H
		if( ((~olddata)&data) & 0x02 ) {
			databits <<= 1;
		}
		if( !(data & 0x02) ) {
			outbits = 0x04;
		} else
		if( databits & 0x40000 ) {
			outbits = 0x00;
		} else {
			outbits = 0x08;
		}
		olddata = data;
	}
}

void	EXPAD_OekakidsTablet::Sync()
{
	nes->GetZapperPos( zapper_x, zapper_y );

	zapper_button = 0;
	//if( ::GetAsyncKeyState(VK_LBUTTON)&0x8000 )
	//	zapper_button = 0xFF;
}

void	EXPAD_OekakidsTablet::SetSyncData( INT type, LONG data )
{
	if( type == 0 ) {
		zapper_x = data;
	} else if( type == 1 ) {
		zapper_y = data;
	} else if( type == 2 ) {
		zapper_button = data?0xFF:0x00;
	}
}

LONG	EXPAD_OekakidsTablet::GetSyncData( INT type )
{
LONG	data = 0;

	if( type == 0 ) {
		data = zapper_x;
	} else if( type == 1 ) {
		data = zapper_y;
	} else if( type == 2 ) {
		data = zapper_button?0xFF:0x00;
	}
	return	data;
}

