//////////////////////////////////////////////////////////////////////////
// Mapper083  Cony                                                      //
//////////////////////////////////////////////////////////////////////////
class	Mapper083 : public Mapper
{
public:
	Mapper083( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	SoftReset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[3];
	BYTE	dipreg;
	INT	chr_bank;
	BYTE	irq_enable;
	INT	irq_counter;

	BYTE	patch;

private:
};
