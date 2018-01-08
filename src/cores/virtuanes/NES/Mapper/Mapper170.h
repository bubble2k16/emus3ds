//////////////////////////////////////////////////////////////////////////
// Mapper170                                                            //
//////////////////////////////////////////////////////////////////////////

class	Mapper170 : public Mapper
{
public:
	Mapper170( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	PPU_Latch( WORD addr );
	void	PPU_ExtLatchX( INT x );
	void	PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr );
	BYTE	PPU_ExtLatchSP();

protected:
	BYTE	reg[8], Rom_Type;
	BYTE	dip_s;

	BYTE	WRAM00[128*1024];	
	BYTE	a3, p_mode, NT_data;
	BYTE	ex_slot2, ex_slot3;

private:
	void	SetPROM_Bank0();
};
