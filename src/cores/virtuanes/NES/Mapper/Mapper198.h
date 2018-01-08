//////////////////////////////////////////////////////////////////////////
// Mapper198  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper198 : public Mapper
{
public:
	Mapper198( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );
	void	WriteLow(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );


protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

	BYTE	adr5000buf[1024*4];

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
