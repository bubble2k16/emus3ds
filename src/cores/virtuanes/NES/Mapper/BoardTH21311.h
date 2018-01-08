//////////////////////////////////////////////////////////////////////////
// BoardTH21311                                                         //
//////////////////////////////////////////////////////////////////////////

class	BoardTH21311 : public Mapper
{
public:
	BoardTH21311( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

//	void	Clock( INT cycles );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	irq_enable;
	INT		irq_counter;
private:
};
