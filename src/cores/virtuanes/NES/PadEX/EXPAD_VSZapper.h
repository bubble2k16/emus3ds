//////////////////////////////////////////////////////////////////////////
// VS-Unisystem Zapper                                                  //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_VSZapper : public EXPAD
{
public:
	EXPAD_VSZapper( NES* parent ) : EXPAD( parent ) {}

	void	Reset();
	void	Strobe();

	BYTE	Read4016();
	BYTE	Read4017();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	readlatch[2];

	LONG	zapper_x, zapper_y;
	BYTE	zapper_button;
	BYTE	zapper_offscreen;

private:
};
