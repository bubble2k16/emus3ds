//////////////////////////////////////////////////////////////////////////
// Space Shadow Gun                                                     //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_SpaceShadowGun : public EXPAD
{
public:
	EXPAD_SpaceShadowGun( NES* parent ) : EXPAD( parent ) {}

	void	Reset();
	void	Strobe();
	BYTE	Read4016();
	BYTE	Read4017();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	LONG	zapper_x, zapper_y;
	BYTE	bomb_bits;
	BYTE	zapper_button;

private:
};
