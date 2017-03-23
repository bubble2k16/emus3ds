//////////////////////////////////////////////////////////////////////////
// Mapper231  20-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper231 : public Mapper
{
public:
	Mapper231( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
