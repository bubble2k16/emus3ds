//////////////////////////////////////////////////////////////////////////
// Toprider                                                             //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_Toprider : public EXPAD
{
public:
	EXPAD_Toprider( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	void	Write4016( BYTE data );
	BYTE	Read4017();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	rider_bita;
	BYTE	rider_bitb;

	INT	rider_pos;
	INT	rider_accel;
	INT	rider_brake;
	BYTE	rider_button;
private:
};
