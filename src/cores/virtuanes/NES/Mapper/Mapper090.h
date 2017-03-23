//////////////////////////////////////////////////////////////////////////
// Mapper090  PC-JY-??                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper090 : public Mapper
{
public:
	Mapper090( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow (WORD addr);
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

	void	HSync(INT scanline);
	void	Clock(INT cycles);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;

	BYTE	prg_reg[4];
	BYTE	nth_reg[4], ntl_reg[4];
	BYTE	chh_reg[8], chl_reg[8];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	irq_preset;
	BYTE	irq_offset;

	BYTE	prg_6000, prg_E000;
	BYTE	prg_size, chr_size;
	BYTE	mir_mode, mir_type;

	BYTE	key_val;
	BYTE	mul_val1, mul_val2;
	BYTE	sw_val;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
	void	SetBank_VRAM();
};
