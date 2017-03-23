//////////////////////////////////////////////////////////////////////////
// Mapper068  SunSoft Mapper 4 (After Burner II)                        //
//////////////////////////////////////////////////////////////////////////
class	Mapper068 : public Mapper
{
public:
	Mapper068( NES* parent ) : Mapper(parent) {}

	void	Reset();
//	BYTE	ExRead( WORD addr );
//	void	ExWrite( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[4];
	BYTE	coin;
private:
	void	SetBank();
};
