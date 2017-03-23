//////////////////////////////////////////////////////////////////////////
// Mapper017  FFE F8xxx                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper017 : public Mapper
{
public:
	Mapper017( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	INT	irq_counter;
	INT	irq_latch;

private:
};
