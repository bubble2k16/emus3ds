//////////////////////////////////////////////////////////////////////////
// Mapper015  100-in-1 chip                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper015 : public Mapper
{
public:
	Mapper015( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
	void	Sync (void);
	uint8 PRG[4];
	uint8 Mirror;
private:
};
