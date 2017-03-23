//////////////////////////////////////////////////////////////////////////
// Mapper244                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper244 : public Mapper
{
public:
	Mapper244( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};