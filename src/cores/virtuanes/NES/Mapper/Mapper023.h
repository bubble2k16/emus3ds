//////////////////////////////////////////////////////////////////////////
// Mapper023  Konami VRC2 type B                                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper023 : public Mapper
{
public:
	Mapper023( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	WORD	addrmask;

	BYTE	reg[9];
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;

private:
};
