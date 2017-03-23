//////////////////////////////////////////////////////////////////////////
// Mapper234  Maxi-15                                                   //
//////////////////////////////////////////////////////////////////////////
class	Mapper234 : public Mapper
{
public:
	Mapper234( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);
	void	Read( WORD addr, BYTE data);

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[2];
private:
	void	SetBank();
};
