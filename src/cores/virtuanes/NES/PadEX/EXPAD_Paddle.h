//////////////////////////////////////////////////////////////////////////
// Paddle                                                               //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_Paddle : public EXPAD
{
public:
	EXPAD_Paddle( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4016();
	BYTE	Read4017();
	void	Write4016( BYTE data );

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	paddle_bits;
	BYTE	paddle_data;

	BYTE	paddle_posold;

	LONG	paddle_x;
	BYTE	paddle_button;
private:
};
