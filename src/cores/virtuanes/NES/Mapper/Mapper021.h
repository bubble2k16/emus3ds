//////////////////////////////////////////////////////////////////////////
// Mapper021  Konami VRC4 (Address mask $F006 or $F0C0)                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper021 : public Mapper
{
public:
	Mapper021( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[9];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;

private:
};
