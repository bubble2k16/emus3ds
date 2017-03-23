//////////////////////////////////////////////////////////////////////////
// Mapper070  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper070 : public Mapper
{
public:
	Mapper070( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	patch;
private:
};