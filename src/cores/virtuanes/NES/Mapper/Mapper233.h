//////////////////////////////////////////////////////////////////////////
// Mapper233  42-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper233 : public Mapper
{
public:
	Mapper233( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
private:
};
