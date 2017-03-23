//////////////////////////////////////////////////////////////////////////
// Mapper082  Taito C075                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper082 : public Mapper
{
public:
	Mapper082( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;

private:
};
