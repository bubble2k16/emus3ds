//////////////////////////////////////////////////////////////////////////
// BoardYoko                                     //
//////////////////////////////////////////////////////////////////////////
class	BoardYoko : public Mapper
{
public:
	BoardYoko( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow(WORD addr);
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);
	void	HSync( INT scanline );
//	void	Clock( INT cycles );

protected:
	BYTE	reg[8], low[4];
	BYTE	mode, bank, dip;
	BYTE	IRQa;
	WORD	IRQCount;
	BYTE	SPROM;
private:
	void	SetBank();
};
