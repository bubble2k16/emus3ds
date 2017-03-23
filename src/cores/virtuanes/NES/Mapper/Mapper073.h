//////////////////////////////////////////////////////////////////////////
// Mapper073  Konami VRC3                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper073 : public Mapper
{
public:
	Mapper073( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	INT	irq_counter;

private:
};
