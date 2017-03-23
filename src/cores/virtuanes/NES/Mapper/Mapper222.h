//////////////////////////////////////////////////////////////////////////
// Mapper222                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper222 : public Mapper
{
public:
	Mapper222( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
private:
};

