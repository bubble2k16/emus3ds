//////////////////////////////////////////////////////////////////////////
// Mapper067  SunSoft Mapper 3                                          //
//////////////////////////////////////////////////////////////////////////
class	Mapper067 : public Mapper
{
public:
	Mapper067( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	Clock(INT cycles);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	BYTE	irq_occur;
	BYTE	irq_toggle;
	INT	irq_counter;

private:
};
