//////////////////////////////////////////////////////////////////////////
// VS-Unisystem Zapper                                                  //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_VSZapper::Reset()
{
	DEBUGOUT( "EXPAD_VSZapper::Reset\n" );

	readlatch[0] = 0;
	readlatch[1] = 0;
	zapper_button = 0;
	zapper_offscreen = 0;
}

void	EXPAD_VSZapper::Strobe()
{
	readlatch[0] = 0x10;
	readlatch[1] = 0x00;

	if( zapper_button ) {
		readlatch[0] |= (zapper_button & 0x01) ? 0x80 : 0x00;
		zapper_offscreen = zapper_button & 0x02;
	}

	if( nes->GetZapperHit() ) {
		//if( DirectDraw.GetZapperHit() >= 0x40 ) {
		//	readlatch[0] |= 0x40;
		//}
	}

	// Fire offscreen with right mouse button
	if ( zapper_offscreen ) {
		readlatch[0] &= ~0x40;
	}

	// Fire at the screen with left mouse button
	if( zapper_button & 0x01 )  {
		zapper_button = 0;
	}
}

BYTE	EXPAD_VSZapper::Read4016()
{
BYTE	data = 0x00;

	data = readlatch[0] & 0x01;
	readlatch[0] >>= 1;

	// Coin 1
	if( Config.ButtonCheck( 0, Config.controller.nVSUnisystem ) )
		data |= 0x20;
	// Coin 2
	if( Config.ButtonCheck( 1, Config.controller.nVSUnisystem ) )
		data |= 0x40;
	// Service
	if( Config.ButtonCheck( 2, Config.controller.nVSUnisystem ) ) {
		data |= 0x04;
	}

	// Dip-Switch
	data |= ((nes->GetVSDipSwitch()<<3) & 0x18);

	return	data;
}

BYTE	EXPAD_VSZapper::Read4017()
{
BYTE	data = 0x00;

	data = readlatch[1] & 0x01;
	readlatch[1] >>= 1;

	// Dip-Switch
	data |= (nes->GetVSDipSwitch() & 0xFC);

	return	data;
}

void	EXPAD_VSZapper::Sync()
{
	nes->GetZapperPos( zapper_x, zapper_y );

	zapper_button = 0;
	//if( ::GetAsyncKeyState(VK_LBUTTON)&0x8000 )
	//	zapper_button |= 0x01;
	//if( ::GetAsyncKeyState(VK_RBUTTON)&0x8000 )
	//	zapper_button |= 0x02;
}

void	EXPAD_VSZapper::SetSyncData( INT type, LONG data )
{
	if( type == 0 ) {
		zapper_x = data;
	} else if( type == 1 ) {
		zapper_y = data;
	} else if( type == 2 ) {
		zapper_button = data&0xFF;
		readlatch[0] = ((BYTE)((data>> 8)&0x000000FF));
		readlatch[1] = ((BYTE)((data>>16)&0x000000FF));
	}
}

LONG	EXPAD_VSZapper::GetSyncData( INT type )
{
LONG	data = 0;

	if( type == 0 ) {
		data = zapper_x;
	} else if( type == 1 ) {
		data = zapper_y;
	} else if( type == 2 ) {
		data = (zapper_button&0xFF)|
		       ((readlatch[0]&0xFF)<< 8)|
		       ((readlatch[1]&0xFF)<<16);
	}
	return	data;
}

