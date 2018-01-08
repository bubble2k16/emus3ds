//////////////////////////////////////////////////////////////////////////
// Mapper235  150-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper235 : public Mapper
{
public:
	Mapper235( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
