//////////////////////////////////////////////////////////////////////////
// Mapper166                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper166 : public Mapper
{
public:
	Mapper166( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);
	void	PPU_Latch( WORD addr );

protected:
	BYTE	reg[2];
private:
};
