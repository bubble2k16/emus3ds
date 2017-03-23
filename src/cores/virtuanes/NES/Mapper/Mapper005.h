//////////////////////////////////////////////////////////////////////////
// Mapper005  Nintendo MMC5                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper005 : public Mapper
{
public:
	Mapper005( NES* parent ) : Mapper(parent) {}

	void	Reset();

	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	void	PPU_ExtLatchX( INT x );
	void	PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	sram_size;

	BYTE	prg_size;		// $5100
	BYTE	chr_size;		// $5101
	BYTE	sram_we_a, sram_we_b;	// $5102-$5103
	BYTE	graphic_mode;		// $5104
	BYTE	nametable_mode;		// $5105
	BYTE	nametable_type[4];	// $5105 use

	BYTE	sram_page;		// $5113

	BYTE	fill_chr, fill_pal;	// $5106-$5107
	BYTE	split_control;		// $5200
	BYTE	split_scroll;		// $5201
	BYTE	split_page;		// $5202

	BYTE	split_x;
	WORD	split_addr;
	WORD	split_yofs;

	BYTE	chr_type;
	BYTE	chr_mode;		// $5120-$512B use
	BYTE	chr_page[2][8];		// $5120-$512B
	LPBYTE	BG_MEM_BANK[8];		// BGパターン用バンク
	BYTE	BG_MEM_PAGE[8];

	BYTE	irq_status;		// $5204(R)
	BYTE	irq_enable;		// $5204(W)
	BYTE	irq_line;		// $5203
	BYTE	irq_scanline;
	BYTE	irq_clear;		// HSyncで使用
	BYTE	irq_type;

	BYTE	mult_a, mult_b;		// $5205-$5206

private:
	void	SetBank_CPU( WORD addr, BYTE data );
	void	SetBank_SRAM( BYTE page, BYTE data );
	void	SetBank_PPU();
};
