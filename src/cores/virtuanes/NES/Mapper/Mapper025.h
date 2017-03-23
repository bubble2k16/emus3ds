//////////////////////////////////////////////////////////////////////////
// Mapper025  Konami VRC4 (Normal)                                      //
//////////////////////////////////////////////////////////////////////////
class	Mapper025 : public Mapper
{
public:
	Mapper025( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[11];
	BYTE	irq_enable;
	BYTE	irq_latch;
	BYTE	irq_occur;
	BYTE	irq_counter;
	INT	irq_clock;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
