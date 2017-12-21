//////////////////////////////////////////////////////////////////////////
// Mapper163  NanJing Games                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper163 : public Mapper
{
public:
	Mapper163( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);
	void	HSync( INT scanline );
	void	PPU_Latch( WORD addr );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[3];
	BYTE	strobe, trigger;
	WORD	security;
	BYTE	rom_type;

	INT		www,index;

private:
	void	jedi_table_init();
	BYTE	decode(uint8 code);
	BYTE	adpcm_decoder(BYTE data);
};



