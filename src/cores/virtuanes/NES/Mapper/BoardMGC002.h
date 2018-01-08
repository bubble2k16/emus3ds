//////////////////////////////////////////////////////////////////////////
// BoardMGC002                                                          //
//////////////////////////////////////////////////////////////////////////
class	BoardMGC002 : public Mapper
{
public:
	BoardMGC002( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	from_mode, game_set;
	BYTE	reg[4], shift, regbuf;
	WORD	last_addr;
private:
};
