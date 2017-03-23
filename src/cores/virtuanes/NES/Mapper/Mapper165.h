//////////////////////////////////////////////////////////////////////////
// Mapper165   Fire Emblem Chinese version                                                         //
//////////////////////////////////////////////////////////////////////////
class	Mapper165 : public Mapper
{
public:
	Mapper165( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	PPU_ChrLatch( WORD addr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	we_sram;
	BYTE	latch;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
	void	SetBank_PPUSUB( int bank, int page );
};
