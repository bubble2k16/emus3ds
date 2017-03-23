//////////////////////////////////////////////////////////////////////////
// Mapper110                                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper110 : public Mapper
{
public:
	Mapper110( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg0, reg1;
private:
};