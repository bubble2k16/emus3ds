//////////////////////////////////////////////////////////////////////////
// Mapper150  SACHEN                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper150 : public Mapper
{
public:
	Mapper150( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );

protected:
	BYTE	reg[5];
	BYTE	cmd;
private:
};

