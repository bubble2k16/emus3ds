//////////////////////////////////////////////////////////////////////////
// Mapper071  Camerica                                                  //
//////////////////////////////////////////////////////////////////////////
class	Mapper071 : public Mapper
{
public:
	Mapper071( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow(WORD addr, BYTE data);
	void	Write(WORD addr, BYTE data);

protected:
private:
};
