//////////////////////////////////////////////////////////////////////////
// Exciting Boxing                                                      //
//////////////////////////////////////////////////////////////////////////
class	EXPAD_ExcitingBoxing : public EXPAD
{
public:
	EXPAD_ExcitingBoxing( NES* parent ) : EXPAD( parent ) {}

	void	Reset();

	BYTE	Read4017();
	void	Write4016( BYTE data );

	void	Sync();
	void	SetSyncData( INT type, LONG data );
	LONG	GetSyncData( INT type );

protected:
	BYTE	outbits;
	BYTE	padbits;

private:
};
