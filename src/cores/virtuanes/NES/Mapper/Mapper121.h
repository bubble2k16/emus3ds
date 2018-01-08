//////////////////////////////////////////////////////////////////////////
// Mapper121                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper121 : public Mapper
{
public:
	Mapper121( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
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
	void	MMC3CMDWrite(WORD A, BYTE V);
	void	MMC3MIRWrite(WORD A, BYTE V);
	void	MMC3IRQWrite(WORD A, BYTE V);
	void	SetDATA();
	void	PPUSW( WORD addr, BYTE data );
	void	CPUSW( WORD addr, BYTE data );
	void	Fix121MMC3PRG(BYTE data);
	void	Fix121MMC3CHR(BYTE data);
};
