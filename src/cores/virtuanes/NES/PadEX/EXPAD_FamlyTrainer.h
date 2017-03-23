//////////////////////////////////////////////////////////////////////////
// Famly Trainer                                                        //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_FamlyTrainer : public EXPAD
{
public:
	EXPAD_FamlyTrainer( NES* parent ) : EXPAD( parent ) {}

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
