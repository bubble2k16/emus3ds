//////////////////////////////////////////////////////////////////////////
// Mapper176                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper176 : public Mapper
{
public:
	Mapper176( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void    WriteLow( WORD addr, BYTE data );
	void	Write(WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg5000;
	BYTE	reg5001;
	BYTE	reg5010;
	BYTE	reg5011;
	BYTE	reg5013;
	BYTE	reg5FF1;
	BYTE	reg5FF2;
	BYTE	we_sram;
	BYTE	SBW, sp_rom;
private:
};

