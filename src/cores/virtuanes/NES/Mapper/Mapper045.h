//////////////////////////////////////////////////////////////////////////
// Mapper045  1000000-in-1                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper045 : public Mapper
{
public:
	Mapper045( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow(WORD addr, BYTE data);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	patch;
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	chr0, chr1, chr2, chr3, chr4, chr5, chr6, chr7;
	BYTE	p[4];
	INT	c[8];
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_latched;
	BYTE	irq_reset;
private:
	void	SetBank_CPU_4( INT data );
	void	SetBank_CPU_5( INT data );
	void	SetBank_CPU_6( INT data );
	void	SetBank_CPU_7( INT data );
	void	SetBank_PPU();

};
