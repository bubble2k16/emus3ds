//////////////////////////////////////////////////////////////////////////
// Mapper087  Konami 74161/32                                           //
//////////////////////////////////////////////////////////////////////////
class	Mapper087 : public Mapper
{
public:
	Mapper087( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

protected:
private:
};
