//////////////////////////////////////////////////////////////////////////
// Mapper151 VS-Unisystem                                               //
//////////////////////////////////////////////////////////////////////////
class	Mapper151 : public Mapper
{
public:
	Mapper151( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);
   
protected:

private:
};

class	Mapper156 : public Mapper
{
public:
	Mapper156( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Sync();
	BYTE	ReadLow ( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write(WORD addr, BYTE data);
   
protected:
	BYTE chrlo[8], chrhi[8], prg, mirr, mirrisused;
private:
};