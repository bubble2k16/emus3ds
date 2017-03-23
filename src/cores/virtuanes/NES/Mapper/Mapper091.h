//////////////////////////////////////////////////////////////////////////
// Mapper091  PC-HK-SF3                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper091 : public Mapper
{
public:
	Mapper091( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	irq_enable;
	BYTE	irq_counter;
private:
};
