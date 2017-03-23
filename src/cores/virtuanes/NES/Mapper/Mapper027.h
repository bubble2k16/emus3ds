//////////////////////////////////////////////////////////////////////////
// Mapper027  Konami VRC4 (World Hero)                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper027 : public Mapper
{
public:
	Mapper027( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	WORD	reg[9];

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	INT	irq_clock;

private:
};
