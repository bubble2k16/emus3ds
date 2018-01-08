//////////////////////////////////////////////////////////////////////////
// Mapper141                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper141 : public Mapper
{
public:
	Mapper141( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    WriteLow( WORD addr, BYTE data );

protected:
	BYTE	reg[8];
	BYTE	cmd;
private:
	void	SetBank();
};

