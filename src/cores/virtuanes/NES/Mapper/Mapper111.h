//////////////////////////////////////////////////////////////////////////
// Mapper111  Nintendo MMC1 Hack                                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper111 : public Mapper
{
public:
	Mapper111( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	WORD	last_addr;

	BYTE	patch;
	BYTE	wram_patch;
	BYTE	wram_bank;
	BYTE	wram_count;

	BYTE	reg[4];
	BYTE	shift, regbuf;

private:
};
