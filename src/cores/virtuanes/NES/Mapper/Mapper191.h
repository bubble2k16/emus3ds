//////////////////////////////////////////////////////////////////////////
// Mapper191 SACHEN Super Cartridge Xin1 (Ver.1-9)                      //
//           SACHEN Q-BOY Support                                       //
//////////////////////////////////////////////////////////////////////////
class	Mapper191 : public Mapper
{
public:
	Mapper191( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	highbank;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
