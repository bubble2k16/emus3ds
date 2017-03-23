//////////////////////////////////////////////////////////////////////////
// Zapper                                                               //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_SpaceShadowGun::Reset()
{
	bomb_bits = 0;
	zapper_button = 0;
}

void	EXPAD_SpaceShadowGun::Strobe()
{
	bomb_bits = nes->pad->pad1bit & 0xFC;
	bomb_bits |= zapper_button & 0x02;
}

BYTE	EXPAD_SpaceShadowGun::Read4016()
{
BYTE	data = (bomb_bits & 0x01)<<1;
	bomb_bits >>= 1;
	return	data;
}

BYTE	EXPAD_SpaceShadowGun::Read4017()
{
BYTE	data = 0x08;

	if( zapper_button & 0x01 ) {
		data |= 0x10;
	}

	if( nes->GetZapperHit() ) {
		//if( DirectDraw.GetZapperHit() >= 0xFE )
		//	data &= ~0x08;
	}
	return	data;
}

void	EXPAD_SpaceShadowGun::Sync()
{
	nes->GetZapperPos( zapper_x, zapper_y );

	zapper_button = 0;
	//if( ::GetAsyncKeyState(VK_LBUTTON)&0x8000 )
	//	zapper_button |= 0x01;
	//if( ::GetAsyncKeyState(VK_RBUTTON)&0x8000 )
	//	zapper_button |= 0x02;
}

void	EXPAD_SpaceShadowGun::SetSyncData( INT type, LONG data )
{
	if( type == 0 ) {
		zapper_x = data;
	} else if( type == 1 ) {
		zapper_y = data;
	} else if( type == 2 ) {
		zapper_button = data;
	}
}

LONG	EXPAD_SpaceShadowGun::GetSyncData( INT type )
{
LONG	data = 0;

	if( type == 0 ) {
		data = zapper_x;
	} else if( type == 1 ) {
		data = zapper_y;
	} else if( type == 2 ) {
		data = zapper_button;
	}
	return	data;
}

