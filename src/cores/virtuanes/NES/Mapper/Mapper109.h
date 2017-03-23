//////////////////////////////////////////////////////////////////////////
// Mapper109 SACHEN The Great Wall SA-019                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper109 : public Mapper
{
public:
	Mapper109( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
	BYTE	chr0, chr1, chr2, chr3;
	BYTE	chrmode0, chrmode1;

private:
	void	SetBank_PPU();
};
