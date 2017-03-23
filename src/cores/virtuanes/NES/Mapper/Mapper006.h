//////////////////////////////////////////////////////////////////////////
// Mapper006  FFE F4xxx                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper006 : public Mapper
{
public:
	Mapper006( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	INT	irq_counter;

private:
};
