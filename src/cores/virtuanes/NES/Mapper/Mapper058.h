//////////////////////////////////////////////////////////////////////////
// Mapper058                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper058 : public Mapper
{
public:
	Mapper058( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
private:
};

