//////////////////////////////////////////////////////////////////////////
// Mapper088  Namcot 118                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper088 : public Mapper
{
public:
	Mapper088( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
	BYTE	patch;

private:
};
