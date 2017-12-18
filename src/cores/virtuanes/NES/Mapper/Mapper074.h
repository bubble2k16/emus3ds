//////////////////////////////////////////////////////////////////////////
// Mapper074                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper074 : public Mapper
{
public:
	Mapper074( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1, prg2, prg3;
	BYTE	chr01, chr1, chr23, chr3, chr4, chr5, chr6, chr7;
	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;

	BYTE	sp_rom, bank;
	BYTE	we_sram;
	BYTE	JMaddr, JMaddrDAT[3];

	BYTE	reg5000;
	BYTE	reg5001;
	BYTE	reg5002;
	BYTE	reg5003;
	BYTE	reg5010;
	BYTE	reg5011;
	BYTE	reg5012;
	BYTE	reg5013;
	BYTE	reg5FF3;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
	void	Decode_PROM();
};
