//////////////////////////////////////////////////////////////////////////
// Mapper142  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper142 : public Mapper
{
public:
	Mapper142( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	prg_sel;
	BYTE	irq_enable;
	INT	irq_counter;
private:
};

