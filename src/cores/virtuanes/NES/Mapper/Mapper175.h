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
