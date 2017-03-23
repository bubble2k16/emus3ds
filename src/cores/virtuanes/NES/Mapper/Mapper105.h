//////////////////////////////////////////////////////////////////////////
// Mapper105  Nintendo World Championship                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper105 : public Mapper
{
public:
	Mapper105( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	init_state;
	BYTE	write_count;
	BYTE	bits;
	BYTE	reg[4];

	BYTE	irq_enable;
	INT	irq_counter;

private:
};
