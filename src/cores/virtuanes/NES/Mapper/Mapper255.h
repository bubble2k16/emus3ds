//////////////////////////////////////////////////////////////////////////
// Mapper255  110-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper255 : public Mapper
{
public:
	Mapper255( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE    reg[4];
private:
};
