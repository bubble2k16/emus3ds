//////////////////////////////////////////////////////////////////////////
// Mapper114                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper114 : public Mapper
{
public:
	Mapper114( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    WriteLow( WORD addr, BYTE data );
	void    Write( WORD addr, BYTE data );
	void	Clock( INT cycles );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
protected:
	BYTE	reg_m, reg_a, reg_b[8];
	BYTE	reg_c;
	BYTE	irq_counter;
	BYTE	irq_occur;
private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};

