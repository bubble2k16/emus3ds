//////////////////////////////////////////////////////////////////////////
// Mapper202  150-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper202 : public Mapper
{
public:
	Mapper202( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	ExWrite( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

protected:
private:
	void	WriteSub( WORD addr, BYTE data );
};
