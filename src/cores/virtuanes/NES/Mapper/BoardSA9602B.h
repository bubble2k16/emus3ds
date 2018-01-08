//////////////////////////////////////////////////////////////////////////
// BoardSA9602B                                                            //
//////////////////////////////////////////////////////////////////////////
class	BoardSA9602B : public Mapper
{
public:
	BoardSA9602B( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void    UpdatePrg(BYTE data);
	void    UpdateChr(BYTE data);
	void    UpdatePrg(WORD addr,BYTE data);
	void    UpdateChr(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

	void	HSync(INT scanline);

protected:
	BYTE    DRegBuf[8], reg[2];
	BYTE    A000B, MMC3cmd, CRAM_sav;
	BYTE    IRQCount, IRQLatch, IRQa, IRQReload;
private:
};
