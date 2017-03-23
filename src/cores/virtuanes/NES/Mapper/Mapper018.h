//////////////////////////////////////////////////////////////////////////
// Mapper018  Jaleco SS8806                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper018 : public Mapper
{
public:
	Mapper018( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	Clock(INT cycles);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[11];

	BYTE	irq_enable;
	BYTE	irq_mode;
	INT	irq_latch;
	INT	irq_counter;

private:
};
