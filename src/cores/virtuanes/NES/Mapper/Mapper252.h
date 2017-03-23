//////////////////////////////////////////////////////////////////////////
// Mapper252                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper252 : public Mapper
{
public:
	Mapper252( NES* parent ) : Mapper(parent) {}

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
	BYTE	irq_occur;
	INT	irq_clock;
private:
};
