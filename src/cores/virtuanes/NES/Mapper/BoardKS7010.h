//////////////////////////////////////////////////////////////////////////
// BoardKS7010  EMC                                                     //
//////////////////////////////////////////////////////////////////////////

class	BoardKS7010 : public Mapper
{
public:
	BoardKS7010( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	ExWrite( WORD addr, BYTE data );
	BYTE	ReadLow ( WORD addr );

protected:
private:
};
