//////////////////////////////////////////////////////////////////////////
// Mapper004  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper004 : public Mapper
{
public:
	Mapper004( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
	BYTE	irq_preset;
	BYTE	irq_preset_vbl;

	BYTE	vs_patch;
	BYTE	vs_index;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
