//////////////////////////////////////////////////////////////////////////
// Mapper012  DBZ5                                                      //
//////////////////////////////////////////////////////////////////////////

class	Mapper012 : public Mapper
{
public:
	Mapper012( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow(WORD addr, BYTE data);
	BYTE	ReadLow(WORD addr);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	DWORD	vb0, vb1;
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
	BYTE	irq_preset;
	BYTE	irq_preset_vbl;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
