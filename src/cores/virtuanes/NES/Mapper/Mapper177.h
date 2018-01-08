//////////////////////////////////////////////////////////////////////////
// Mapper177  HengGe                                                    //
//////////////////////////////////////////////////////////////////////////
class	Mapper177 : public Mapper
{
public:
	Mapper177( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);
	void	WriteLow(WORD addr, BYTE data);

protected:
	BYTE SP_rom;
private:
};
