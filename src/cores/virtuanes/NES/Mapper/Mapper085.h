//////////////////////////////////////////////////////////////////////////
// Mapper085  Konami VRC7                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper085 : public Mapper
{
public:
	Mapper085( NES* parent ) : Mapper(parent) {}

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
