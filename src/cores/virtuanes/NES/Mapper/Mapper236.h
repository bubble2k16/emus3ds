//////////////////////////////////////////////////////////////////////////
// Mapper236  800-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper236 : public Mapper
{
public:
	Mapper236( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE    bank, mode;
private:
};
