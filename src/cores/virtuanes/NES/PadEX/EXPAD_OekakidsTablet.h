//////////////////////////////////////////////////////////////////////////
// Oekakids Tablet                                                      //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_OekakidsTablet : public EXPAD
{
public:
	EXPAD_OekakidsTablet( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4017();
	void	Write4016( BYTE data );

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	olddata;
	BYTE	outbits;
	DWORD	databits;

	LONG	zapper_x, zapper_y;
	BYTE	zapper_button;
private:
};
