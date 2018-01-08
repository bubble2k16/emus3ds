//////////////////////////////////////////////////////////////////////////
// Mapper168                                                            //
//////////////////////////////////////////////////////////////////////////

class	Mapper168 : public Mapper
{
public:
	Mapper168( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	Write( WORD addr, BYTE data );

	void	PPU_Latch( WORD addr );
	void	PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg5000, reg5200, reg5300;
	BYTE	PPU_SW, NT_data;
	BYTE	Rom_Type;

private:
	void	SetBank_CPU();
};
