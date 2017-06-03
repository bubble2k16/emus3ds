//////////////////////////////////////////////////////////////////////////
// Mapper253                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper253 : public Mapper
{
public:
	Mapper253( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write( WORD addr, BYTE data );

	void	Clock( INT cycles );
	void	Sync(void);
	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	 BYTE chrlo[8], chrhi[8], prg[2], mirr, vlock;
	 DWORD IRQa, IRQCount, IRQLatch, IRQClock; 
private:
};
