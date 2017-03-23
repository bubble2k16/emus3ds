//////////////////////////////////////////////////////////////////////////
// Mapper181  Hacker International Type2                                //
//////////////////////////////////////////////////////////////////////////
class	Mapper181 : public Mapper
{
public:
	Mapper181( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);

protected:
private:
};
