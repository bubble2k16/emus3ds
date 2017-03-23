//////////////////////////////////////////////////////////////////////////
// Mapper117  Sanko Gu(Tw)                                              //
//////////////////////////////////////////////////////////////////////////
class	Mapper117 : public Mapper
{
public:
	Mapper117( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);
	void	HSync( INT scanline );
	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
protected:
	BYTE	irq_enable;
	BYTE	irq_counter;
private:
};
