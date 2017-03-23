//////////////////////////////////////////////////////////////////////////
// Zapper                                                               //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_Zapper::Reset()
{
	zapper_button = 0;
	zapper_offscreen = 0;
}

BYTE	EXPAD_Zapper::Read4017()
{
BYTE	data = 0x08;

	if( zapper_button ) {
		data |= 0x10;
		zapper_offscreen = zapper_button & 0x02;
	}

	if( nes->GetZapperHit() ) {
		//if( DirectDraw.GetZapperHit() >= 0x40 )
		//	data &= ~0x08;
	}

	// Fire offscreen with right mouse button
	if ( zapper_offscreen )
		data |= 0x08;

	// Fire at the screen with left mouse button
	if ( zapper_button & 0x01 )
		zapper_offscreen = 0;

	return	data;
}

void	EXPAD_Zapper::Sync()
{
	nes->GetZapperPos( zapper_x, zapper_y );

	zapper_button = 0;
	/*if( ::GetAsyncKeyState(VK_LBUTTON)&0x8000 )
		zapper_button |= 0x01;
	else if( ::GetAsyncKeyState(VK_RBUTTON)&0x8000 )
		zapper_button |= 0x02;*/
}

void	EXPAD_Zapper::SetSyncData( INT type, LONG data )
{
	if( type == 0 ) {
		zapper_x = data;
	} else if( type == 1 ) {
		zapper_y = data;
	} else if( type == 2 ) {
		zapper_button = (BYTE)(data&0x03);
	}
}

LONG	EXPAD_Zapper::GetSyncData( INT type )
{
LONG	data = 0;

	if( type == 0 ) {
		data = zapper_x;
	} else if( type == 1 ) {
		data = zapper_y;
	} else if( type == 2 ) {
		data = (zapper_button&0x03);
	}
	return	data;
}