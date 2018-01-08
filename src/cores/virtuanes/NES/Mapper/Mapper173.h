//////////////////////////////////////////////////////////////////////////
// Mapper173                                                            //
//////////////////////////////////////////////////////////////////////////

class	Mapper173 : public Mapper
{
public:
	Mapper173( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ExRead ( WORD addr );
	void	ExWrite( WORD addr, BYTE data );
	BYTE	ReadLow ( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );
	void	Clock( INT cycles );
	void	PPU_Latch( WORD addr );
	void	PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[10];

	INT		irq_counter, irq_latch;
	BYTE	irq_enable, irq_repeat;
	BYTE	irq_occur;

private:
};
