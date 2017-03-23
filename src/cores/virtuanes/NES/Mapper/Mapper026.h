//////////////////////////////////////////////////////////////////////////
// Mapper026  Konami VRC6 (PA0,PA1 reverse)                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper026 : public Mapper
{
public:
	Mapper026( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;
private:
};
