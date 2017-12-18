//////////////////////////////////////////////////////////////////////////
// Mapper241  Education X-in-1                                          //
//////////////////////////////////////////////////////////////////////////
class	Mapper241 : public Mapper
{
public:
	Mapper241( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
	BYTE	SBPAL_fix;
private:
};
