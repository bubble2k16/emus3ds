//////////////////////////////////////////////////////////////////////////
// Mapper237                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper237 : public Mapper
{
public:
	Mapper237( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );
	BYTE	Read( WORD addr );

protected:
	BYTE    A0,A1,A2,dip_s;
	BYTE    D0,D1,D3,D5,D6;
private:
};
