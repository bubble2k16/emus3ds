//////////////////////////////////////////////////////////////////////////
// BoardFK23C                                                         //
//////////////////////////////////////////////////////////////////////////
class	BoardFK23C : public Mapper
{
public:
	BoardFK23C( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);
	void	HSync(INT scanline);

protected:
	BYTE	unromchr;
	INT		dipswitch;
	BYTE	EXPREGS[8];
	BYTE	MMC3cmd;
	BYTE	DRegBuf[8];
	BYTE	A000B, A001B;
	BYTE	IRQCount, IRQLatch, IRQa, IRQReload;
private:
	void	MMC3CMDWrite(WORD addr, BYTE data);
	void	MMC3IRQWrite(WORD addr, BYTE data);
	void	PPUSW(WORD A, BYTE V);
	void	CPUSW(WORD A, BYTE V);
	void	FixFK23MMC3PRG(BYTE data);
	void	FixFK23MMC3CHR(BYTE data);
};
