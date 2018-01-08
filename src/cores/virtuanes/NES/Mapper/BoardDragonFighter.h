//////////////////////////////////////////////////////////////////////////
// BoardDragonFighter                                                   //
//////////////////////////////////////////////////////////////////////////
class	BoardDragonFighter : public Mapper
{
public:
	BoardDragonFighter( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow(WORD addr);
	void	Write(WORD addr, BYTE data);
	void	HSync(INT scanline);

protected:
	BYTE	chr0, chr1, reg[2];
	BYTE	irq_enable, irq_counter, irq_latch, irq_request;
private:
};
