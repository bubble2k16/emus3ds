//////////////////////////////////////////////////////////////////////////
// Mapper232  Quattro Games                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper232 : public Mapper
{
public:
	Mapper232( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE    reg[2];
private:
};
