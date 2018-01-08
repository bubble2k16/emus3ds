//////////////////////////////////////////////////////////////////////////
// Mapper052  Konami VRC2 type B                                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper052 : public Mapper
{
public:
	Mapper052( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );
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
