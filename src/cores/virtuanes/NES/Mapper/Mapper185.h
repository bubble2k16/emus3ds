//////////////////////////////////////////////////////////////////////////
// Mapper185  Character disable protect                                 //
//////////////////////////////////////////////////////////////////////////
class	Mapper185 : public Mapper
{
public:
	Mapper185( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

protected:
	BYTE	patch;

private:
};
