//////////////////////////////////////////////////////////////////////////
// Mapper199  WaiXingTypeG Base ON Nintendo MMC3                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper199 : public Mapper
{
public:
	Mapper199( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg[4];
	BYTE	chr[8];
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
