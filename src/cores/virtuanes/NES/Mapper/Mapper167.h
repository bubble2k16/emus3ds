//////////////////////////////////////////////////////////////////////////
// Mapper004  Supor Computer V4.0                                       //
//////////////////////////////////////////////////////////////////////////

class	Mapper167 : public Mapper
{
public:
	Mapper167( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	regs[4];
	BYTE	rom_type;
private:
	void	SetBank_CPU();
	void	SetBank_PPU();
};
