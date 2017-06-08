//////////////////////////////////////////////////////////////////////////
// Mapper194 ���{���@�_�o�o                                             //
//////////////////////////////////////////////////////////////////////////
class	Mapper194 : public Mapper
{
public:
	Mapper194( NES* parent ) : Mapper(parent) {}

	void	Reset();
	void	Write(WORD addr, BYTE data);

protected:
private:
};



//////////////////////////////////////////////////////////////////////////
// Mapper192  WaiXingTypeC Base ON Nintendo MMC3                        //
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Mapper192                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper192 : public Mapper
{
public:
	Mapper192( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
private:
	void	SetBank_CPU();
	void	SetBank_PPU();
	void	SetBank_PPUSUB( int bank, int page );
};

class	Mapper195 : public Mapper
{
public:
	Mapper195( NES* parent ) : Mapper(parent) {}

	
	void	Reset();
	void	Write( WORD addr, BYTE data );
	void	WriteLow( WORD addr, BYTE data );
	BYTE	ReadLow( WORD addr );

	void	HSync( INT scanline );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	reg[8];
	BYTE	prg0, prg1;
	BYTE	chr01, chr23, chr4, chr5, chr6, chr7;
	BYTE	we_sram;

	BYTE	irq_type;
	BYTE	irq_enable;
	BYTE	irq_counter;
	BYTE	irq_latch;
	BYTE	irq_request;
private:
	void	SetBank_CPU();
	void	SetBank_PPU();
	void	SetBank_PPUSUB( int bank, int page );
};
