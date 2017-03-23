//////////////////////////////////////////////////////////////////////////
// Mapper182  PC-SuperDonkeyKong                                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper182 : public Mapper
{
public:
	Mapper182( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
	BYTE	irq_enable;
	BYTE	irq_counter;
private:
};

