//////////////////////////////////////////////////////////////////////////
// Mapper212                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper212 : public Mapper
{
public:
	Mapper212( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	Write( WORD addr, BYTE data );

protected:
private:
};
