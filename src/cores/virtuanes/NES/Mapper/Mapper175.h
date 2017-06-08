class	Mapper175 : public Mapper
{
public:
	Mapper175( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Read( WORD addr, BYTE data);
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p ){}
	void	LoadState( LPBYTE p ){}

protected:
	BYTE	reg_dat;
};

class	Mapper176 : public Mapper
{
public:
	Mapper176( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	WriteLow( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p ){}
	void	LoadState( LPBYTE p ){}

protected:
	BYTE prg,chr;
	void Sync();
};

class	Mapper177 : public Mapper
{
public:
	Mapper177( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p ){}
	void	LoadState( LPBYTE p ){}

protected:
	BYTE	reg;
	void Sync();
};
class	Mapper178 : public Mapper
{
public:
	Mapper178( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p ){}
	void	LoadState( LPBYTE p ){}

protected:
	BYTE	reg[3];
	BYTE	banknum;

	
	void	SetBank_CPU();
};