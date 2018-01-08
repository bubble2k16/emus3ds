//////////////////////////////////////////////////////////////////////////
// BoardLB12IN1                                                          //
//////////////////////////////////////////////////////////////////////////

class	BoardLB12IN1 : public Mapper
{
public:
	BoardLB12IN1( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	ExWrite( WORD addr, BYTE data );
	BYTE	ReadLow ( WORD addr );

protected:
private:
};
