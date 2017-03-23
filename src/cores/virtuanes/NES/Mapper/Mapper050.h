//////////////////////////////////////////////////////////////////////////
// Mapper050  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper050 : public Mapper
{
public:
	Mapper050( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    ExWrite( WORD addr, BYTE data );
	void    WriteLow( WORD addr, BYTE data );
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
private:
};

