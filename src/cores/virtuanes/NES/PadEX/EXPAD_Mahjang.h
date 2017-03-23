//////////////////////////////////////////////////////////////////////////
// Ide Yousuke Jissen Mahjang                                           //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_Mahjang : public EXPAD
{
public:
	EXPAD_Mahjang( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4017();
	void	Write4016( BYTE data );

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	outbits;
	DWORD	padbits;

private:
};
