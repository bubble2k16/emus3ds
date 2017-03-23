//////////////////////////////////////////////////////////////////////////
// Mapper251                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper251 : public Mapper
{
public:
	Mapper251( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    WriteLow( WORD addr, BYTE data );
	void    Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[11];
	BYTE	breg[4];
private:
	void	SetBank();
};

