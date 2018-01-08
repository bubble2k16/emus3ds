//////////////////////////////////////////////////////////////////////////
// BoardKS7030                                                          //
//////////////////////////////////////////////////////////////////////////

class	BoardKS7030 : public Mapper
{
public:
	BoardKS7030( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	ExWrite( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	BYTE	Read( WORD addr );
	void	Write( WORD addr, BYTE data );

protected:
	BYTE	reg0, reg1;
private:
	void	SetBank();
};
