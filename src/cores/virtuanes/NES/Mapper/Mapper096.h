//////////////////////////////////////////////////////////////////////////
// Mapper096  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper096 : public Mapper
{
public:
	Mapper096( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	PPU_Latch( WORD addr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[2];

private:
	void	SetBank();
};
