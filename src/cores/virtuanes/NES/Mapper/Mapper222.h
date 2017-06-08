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

class	Mapper220 : public Mapper
{
public:
	Mapper220( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Sync();
	void    Write( WORD addr, BYTE data );

protected:
private:
	BYTE reg[8], mirror;
};

