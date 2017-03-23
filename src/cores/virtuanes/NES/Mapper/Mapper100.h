//////////////////////////////////////////////////////////////////////////
// Mapper100  Nesticle MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper100 : public Mapper
{
public:
	Mapper100( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
