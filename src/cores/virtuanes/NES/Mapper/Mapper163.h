//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class	Mapper163 : public Mapper
{
public:
	Mapper163( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	HSync(int scanline);
	// For state save
	void SaveState( LPBYTE p );
	void LoadState( LPBYTE p );

protected:
	//BYTE    strobe;
	//BYTE	security;
	//BYTE	trigger;
	//BYTE	rom_type;

	//BYTE	reg[2];
	BYTE laststrobe, trigger;
	BYTE reg[8];
};



