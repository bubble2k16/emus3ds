//////////////////////////////////////////////////////////////////////////
// Mapper190  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper190 : public Mapper
{
public:
	Mapper190( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );
	void	WriteLow(WORD addr, BYTE data);

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	cbase;		/* PowerOn OR RESET : cbase=0 */
	BYTE	mp190_lcchk;	/* PowerOn OR RESET */
	BYTE	mp190_lcmd;
	BYTE	mp190_cmd;

	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE    lowoutdata;

private:

};
