//////////////////////////////////////////////////////////////////////////
// Mapper179                                                            //
//////////////////////////////////////////////////////////////////////////

class	Mapper179 : public Mapper
{
public:
	Mapper179( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );
	void	HSync( INT scanline );
	void	Clock( INT cycles );
	void	PPU_Latch( WORD addr );
	void	PPU_ExtLatchX( INT x );
	void	PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

protected:
	BYTE	reg[8];

	BYTE	irq_enable;
	INT		irq_counter;
	INT		irq_latch;
	BYTE	irq_request;
	X24C02	x24c02;

private:
};
