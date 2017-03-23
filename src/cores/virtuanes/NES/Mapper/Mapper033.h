//////////////////////////////////////////////////////////////////////////
// Mapper033  Taito TC0190                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper033 : public Mapper
{
public:
	Mapper033( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[7];

	BYTE	patch;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;

private:
	void	SetBank();
};
