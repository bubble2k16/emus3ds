//////////////////////////////////////////////////////////////////////////
// Mapper010 Nintendo MMC4                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper010 : public Mapper
{
public:
	Mapper010( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	PPU_ChrLatch( WORD addr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[4];
	BYTE	latch_a, latch_b;

private:
};
