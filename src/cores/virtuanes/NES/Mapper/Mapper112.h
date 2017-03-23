//////////////////////////////////////////////////////////////////////////
// Mapper112  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper112 : public Mapper
{
public:
	Mapper112( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[4];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
