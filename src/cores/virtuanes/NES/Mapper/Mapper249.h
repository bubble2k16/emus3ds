//////////////////////////////////////////////////////////////////////////
// Mapper249  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper249 : public Mapper
{
public:
	Mapper249( NES* parent ) : Mapper(parent) {}
	int    MMC4prg,MMC4chr;

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	//BYTE	ReadLow( WORD addr );
	void	Clock( INT cycles );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
	BYTE	spdata;
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;
	BYTE	patch;	
	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;

private:

};
