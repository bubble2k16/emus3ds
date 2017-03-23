//////////////////////////////////////////////////////////////////////////
// Mapper140                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper140 : public Mapper
{
public:
	Mapper140( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    WriteLow( WORD addr, BYTE data );

protected:
private:
};

