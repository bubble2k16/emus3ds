//////////////////////////////////////////////////////////////////////////
// Board831128C  SunSoft FME-7                                             //
//////////////////////////////////////////////////////////////////////////
class	Board831128C : public Mapper
{
public:
	Board831128C( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	HSync( INT scanline );
	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	bn;
	BYTE	irq_enable;
	INT		irq_counter;
//	BYTE	irq_counter;
	BYTE	irq_latch;
	INT		irq_clock;
private:
};
