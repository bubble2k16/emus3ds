//////////////////////////////////////////////////////////////////////////
// Mapper000                                                            //
//////////////////////////////////////////////////////////////////////////
class	Mapper000 : public Mapper
{
public:
	Mapper000( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE sram;
	BYTE ram;
	BYTE bram;
	BYTE reg[4];

	void	_OutDebug(WORD A,BYTE V)
	{
		DEBUGOUT("W %.4X %.2X\n",A,V);
	}
	void	_OutDebug(WORD A)
	{
		DEBUGOUT("R %.4X\n",A);
	}
	// $8000-$FFFF Memory write
	void	Write( WORD A, BYTE V ) 	
	{
		_OutDebug(A,V);
	}

	// $8000-$FFFF Memory read
	BYTE	Read( WORD A)
	{
		return CPU_MEM_BANK[A>>13][A&0x1FFF];
	}

	// $4100-$7FFF Lower Memory read/write
	BYTE	ReadLow ( WORD A )
	{
		_OutDebug(A);
		return Mapper::ReadLow(A);
	}
	void	WriteLow( WORD A, BYTE V )
	{
		_OutDebug(A,V);
	}

	// $4018-$40FF Extention register read/write
	//BYTE	ExRead ( WORD A )	{ return 0x00; }
	void	ExWrite( WORD A, BYTE V ) 
	{
		//if(A>0x4017)
		//_OutDebug(A,V);
	}

protected:
private:
};
