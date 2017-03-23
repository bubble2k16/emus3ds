//////////////////////////////////////////////////////////////////////////
// Mapper228  Action 52                                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper228 : public Mapper
{
public:
	Mapper228( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};
