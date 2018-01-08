//////////////////////////////////////////////////////////////////////////
// BoardOneBus                                                          //
//////////////////////////////////////////////////////////////////////////
class	BoardOneBus : public Mapper
{
public:
	BoardOneBus( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );
	void	HSync( INT scanline );
	void	Clock( INT cycles );

	void	WriteExPPU( WORD addr, BYTE data );
	BYTE	ReadExAPU ( WORD addr );
	void	WriteExAPU( WORD addr, BYTE data );
	void	PPU_Latch( WORD addr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	cpu410x[16], ppu201x[16], apu40xx[64];
	BYTE	IRQCount, IRQa, IRQReload, IRQLatch;
	BYTE	inv_hack, mmc3cmd, mirror;
	BYTE	pcm_enable, pcm_irq;
	INT		pcm_addr, pcm_size, pcm_latch, pcm_clock;

	DWORD	count;
private:
	void	SetBank();
	void	SetBankCPU();
	void	SetBankPPU();
};
