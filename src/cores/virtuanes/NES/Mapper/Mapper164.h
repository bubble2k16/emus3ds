//////////////////////////////////////////////////////////////////////////
// Mapper164  Pocket Monster Gold                                       //
//////////////////////////////////////////////////////////////////////////

class	Mapper164 : public Mapper
{
public:
	Mapper164( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

	void	PPU_ExtLatchX( INT x );
	void	PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg5000;
	BYTE	reg5100;
	BYTE	a3, p_mode;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
