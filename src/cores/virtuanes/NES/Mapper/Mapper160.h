//////////////////////////////////////////////////////////////////////////
// Mapper160  PC-Aladdin                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper160 : public Mapper
{
public:
	Mapper160( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	refresh_type;
private:
};

