//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES EXPAD                                                       //
//                                                           Norix      //
//                                               written     2001/04/08 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
class	EXPAD
{
public:
	EXPAD( NES* parent ) : nes( parent ) {}
	virtual	~EXPAD() {};

// ÉÅÉìÉoä÷êî
	virtual	void	Reset() {}
	virtual	void	Strobe() {}
	virtual	BYTE	Read4016() { return 0x00; }
	virtual	BYTE	Read4017() { return 0x00; }
	virtual	void	Write4016( BYTE data ) {}
	virtual	void	Write4017( BYTE data ) {}

	// For synchronized
	virtual	void	Sync() {}

	virtual	void	SetSyncData( INT type, LONG data ) {}
	virtual	LONG	GetSyncData( INT type ) { return 0x00; }

protected:
	NES*	nes;
private:
};
