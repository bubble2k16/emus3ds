//////////////////////////////////////////////////////////////////////////
// Mapper065  Irem H3001                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper065 : public Mapper
{
public:
	Mapper065( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	HSync(INT scanline);
	void	Clock(INT cycles);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;

	BYTE	irq_enable;
	INT	irq_counter;
	INT	irq_latch;
private:
};
