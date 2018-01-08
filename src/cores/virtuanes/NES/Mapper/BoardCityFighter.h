//////////////////////////////////////////////////////////////////////////
// BoardCityFighter                                                            //
//////////////////////////////////////////////////////////////////////////
class	BoardCityFighter : public Mapper
{
public:
	BoardCityFighter( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	prg_reg, prg_mode, mirr;
	BYTE	chr_reg[8];
	BYTE	irq_enable, irq_counter, irq_latch;
	INT		irq_clock;
private:
	void	SetBank();
};
