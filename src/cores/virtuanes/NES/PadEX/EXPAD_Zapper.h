//////////////////////////////////////////////////////////////////////////
// Zapper                                                               //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_Zapper : public EXPAD
{
public:
	EXPAD_Zapper( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4017();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	LONG	zapper_x, zapper_y;
	BYTE	zapper_button;
	BYTE	zapper_offscreen;

private:
};
