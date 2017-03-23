//////////////////////////////////////////////////////////////////////////
// Mapper008  FFE F3xxx                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper008 : public Mapper
{
public:
	Mapper008( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
