//////////////////////////////////////////////////////////////////////////
// Mapper040  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper040 : public Mapper
{
public:
	Mapper040( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	INT	irq_line;

private:
};
