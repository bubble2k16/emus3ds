//////////////////////////////////////////////////////////////////////////
// Mapper007  AOROM/AMROM                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper007 : public Mapper
{
public:
	Mapper007( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	patch;
private:
};
