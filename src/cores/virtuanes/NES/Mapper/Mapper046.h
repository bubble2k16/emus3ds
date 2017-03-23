//////////////////////////////////////////////////////////////////////////
// Mapper046  Rumble Station                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper046 : public Mapper
{
public:
	Mapper046( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	int     reg[4];

private:
	void	SetBank();
};
