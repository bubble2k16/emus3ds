//////////////////////////////////////////////////////////////////////////
// Mapper162  Pocket Monster Gold                                       //
//////////////////////////////////////////////////////////////////////////

class	Mapper162 : public Mapper
{
public:
	Mapper162( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	HSync(int scanline);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg5000;
	BYTE	reg5100;
	BYTE	reg5200;
	BYTE	reg5300;

private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
