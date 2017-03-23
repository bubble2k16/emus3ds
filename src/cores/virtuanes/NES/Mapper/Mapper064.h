//////////////////////////////////////////////////////////////////////////
// Mapper064  Tengen Rambo-1                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper064 : public Mapper
{
public:
	Mapper064( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	Clock( INT cycles );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[3];
	BYTE	irq_enable;
	BYTE	irq_mode;
	INT	irq_counter;
	INT	irq_counter2;
	BYTE	irq_latch;
	BYTE	irq_reset;

private:
};
