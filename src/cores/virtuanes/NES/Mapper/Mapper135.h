//////////////////////////////////////////////////////////////////////////
// Mapper135  SACHEN CHEN                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper135 : public Mapper
{
public:
	Mapper135( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE    cmd;
	BYTE	chr0l, chr1l, chr0h, chr1h, chrch;
private:
	void	SetBank_PPU();
};
