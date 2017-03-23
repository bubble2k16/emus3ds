//////////////////////////////////////////////////////////////////////////
// Mapper187  Street Fighter Zero 2 97                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper187 : public Mapper
{
public:
	Mapper187( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

	void	Clock(INT cycles);
	void	HSync(INT scanline);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	prg[4];
	INT	chr[8];
	BYTE	bank[8];

	BYTE	ext_mode;
	BYTE	chr_mode;
	BYTE	ext_enable;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	last_write;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
