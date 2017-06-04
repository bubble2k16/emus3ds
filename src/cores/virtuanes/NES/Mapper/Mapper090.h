//////////////////////////////////////////////////////////////////////////
// Mapper090  PC-JY-??                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper090 : public Mapper
{
public:
	Mapper090( NES* parent ) : Mapper(parent) 
	{ 
		is_209 = false; 
		is_211 = false; 
	}

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
	BOOL	is_209;
	BOOL	is_211;
	
	BYTE	patch;

	BYTE	prg_reg[4];
	BYTE	nth_reg[4], ntl_reg[4];
	BYTE	chh_reg[8], chl_reg[8];

	BYTE	nt_ram;
	BYTE	nt_mode;

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


class Mapper211 : public Mapper090
{
public:
	Mapper211( NES* parent ) : Mapper090(parent) 
	{
		is_211 = true;
	}
};


class Mapper209 : public Mapper090
{
public:
	Mapper209( NES* parent ) : Mapper090(parent) 
	{
		is_209 = true;
	}
};