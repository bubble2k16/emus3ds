//////////////////////////////////////////////////////////////////////////
// Mapper174                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper174 : public Mapper
{
public:
	Mapper174( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );
protected:
	INT		reg[11];
	INT		prg0, prg1;
	INT		chr01, chr23, chr4, chr5, chr6, chr7;
private:
	void	SetBank_CPU();
	void	SetBank_CPU_L();
	void	SetBank_PPU();
};
