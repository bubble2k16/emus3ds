//////////////////////////////////////////////////////////////////////////
// Mapper095  Namcot 106M (Dragon Buster)                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper095 : public Mapper
{
public:
	Mapper095( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
