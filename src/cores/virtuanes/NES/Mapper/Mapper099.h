//////////////////////////////////////////////////////////////////////////
// Mapper099  VS-Unisystem                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper099 : public Mapper
{
public:
	Mapper099( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ExRead( WORD addr );
	void	ExWrite( WORD addr, BYTE data );

protected:
	BYTE	coin;
private:
};
