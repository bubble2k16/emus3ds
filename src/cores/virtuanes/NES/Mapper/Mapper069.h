//////////////////////////////////////////////////////////////////////////
// Mapper069  SunSoft FME-7                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper069 : public Mapper
{
public:
	Mapper069( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	Clock(INT cycles);
	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;

	BYTE	reg;
	BYTE	irq_enable;
	INT	irq_counter;
private:
};
