//////////////////////////////////////////////////////////////////////////
// Mapper019  Namcot 106                                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper019 : public Mapper
{
public:
	Mapper019( NES* parent ) : Mapper(parent) {}

	void	Reset();

	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;
	BYTE	exsound_enable;

	BYTE	reg[3];
	BYTE	exram[128];

	BYTE	irq_enable;
	WORD	irq_counter;

private:
};
