//////////////////////////////////////////////////////////////////////////
// Mapper057                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper057 : public Mapper
{
public:
	Mapper057( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
private:
};

