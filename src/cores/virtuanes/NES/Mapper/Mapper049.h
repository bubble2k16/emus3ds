//////////////////////////////////////////////////////////////////////////
// Mapper120  Tobidase Daisakusen                                       //
//////////////////////////////////////////////////////////////////////////
class	Mapper049 : public Mapper
{
public:
	Mapper049( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );
	void	HSync( INT scanline );
	void	PPU_Latch( WORD addr );
	void	PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

protected:
	BYTE	temp;
	INT		temp1;
private:
	void	MMC3CMDWrite(WORD A, BYTE V);
	void	wrapc(WORD A, BYTE V);
	void	FixCHR(BYTE V);
};
