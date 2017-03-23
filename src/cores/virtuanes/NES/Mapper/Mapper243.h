//////////////////////////////////////////////////////////////////////////
// Mapper243  PC-Sachen/Hacker                                          //
//////////////////////////////////////////////////////////////////////////
class	Mapper243 : public Mapper
{
public:
	Mapper243( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[4];

private:
};
