//////////////////////////////////////////////////////////////////////////
// Mapper076  Namcot 109 (èóê_ì]ê∂)                                     //
//////////////////////////////////////////////////////////////////////////
class	Mapper076 : public Mapper
{
public:
	Mapper076( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg;
private:
};
