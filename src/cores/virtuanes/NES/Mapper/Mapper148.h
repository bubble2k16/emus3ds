//////////////////////////////////////////////////////////////////////////
// Mapper148  SACHEN                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper148 : public Mapper
{
public:
	Mapper148( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void    Write( WORD addr, BYTE data );

protected:
private:
};

