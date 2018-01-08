//////////////////////////////////////////////////////////////////////////
// Mapper132  TXC(Qi Wang)                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper132 : public Mapper
{
public:
	Mapper132( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	regs[4];
private:
};
