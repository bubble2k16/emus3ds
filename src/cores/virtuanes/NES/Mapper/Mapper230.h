//////////////////////////////////////////////////////////////////////////
// Mapper230  22-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper230 : public Mapper
{
public:
	Mapper230( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	rom_sw;
private:
};
