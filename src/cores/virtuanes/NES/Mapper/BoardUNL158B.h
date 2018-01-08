//////////////////////////////////////////////////////////////////////////
// BoardUNL158B                                                            //
//////////////////////////////////////////////////////////////////////////
class	BoardUNL158B : public Mapper
{
public:
	BoardUNL158B( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
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
	void	Fix158BMMC3PRG(BYTE data);
	void	Fix158BMMC3CHR(BYTE data);
};
