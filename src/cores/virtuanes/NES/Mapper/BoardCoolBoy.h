//////////////////////////////////////////////////////////////////////////
// BoardCoolBoy                                                         //
//////////////////////////////////////////////////////////////////////////
class	BoardCoolBoy : public Mapper
{
public:
	BoardCoolBoy( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);
	void	HSync(INT scanline);

protected:
	BYTE	EXPREGS[8];
	BYTE	MMC3cmd;
	BYTE	DRegBuf[8];
	BYTE	A000B, A001B;
	BYTE	IRQCount, IRQLatch, IRQa, IRQReload;
private:
	void	PPUSW(WORD A, BYTE V);
	void	CPUSW(WORD A, BYTE V);
	void	FixCBMMC3PRG(BYTE data);
	void	FixCBMMC3CHR(BYTE data);
};
