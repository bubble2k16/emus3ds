//////////////////////////////////////////////////////////////////////////
// Gyromite							Å@Å@ //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_Gyromite : public EXPAD
{
public:
	EXPAD_Gyromite( NES* parent ) : EXPAD( parent ) {}

	void	Reset();
	void	Strobe();

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	INT	CheckSignal();

	// TV Light signals
	WORD	code[8];

	WORD	bits;
	BYTE	arms;
	BYTE	spot;

private:
};
