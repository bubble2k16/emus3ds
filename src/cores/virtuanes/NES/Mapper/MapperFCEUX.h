//////////////////////////////////////////////////////////////////////////
// This mapper wraps an existing FCEUX wrapper's code                   //
//                                                                      //
// This is here for easy porting of unimplemented mappers from FCEUX.   //
//////////////////////////////////////////////////////////////////////////
class	MapperFCEUX : public Mapper
{
protected:
    SFORMAT *_stateRegs;
	void (*_power)(void);
	void (*_reset)(void);
	void (*_restore)(int);

public:
	MapperFCEUX( NES* parent, void (*power)(void), void (*reset)(void), void (*restore)(int), SFORMAT *stateRegs ) : Mapper(parent) 
    {
        SetReadHandler(0x0000, 0xFFFF, ANull);
        SetWriteHandler(0x0000, 0xFFFF, BNull);
        
        _power = power;
        _reset = reset;
        _restore = restore;
        _stateRegs = stateRegs;

    }

    void    Reset(void)
    {
        if (_power) _power();
        if (_reset) _reset();
    }

	// $8000-$FFFF Memory read/write
	void	Write( WORD addr, BYTE data )
    {
        BWrite[addr](addr, data);
    }

	// $4100-$7FFF Lower Memory read/write
	BYTE	ReadLow ( WORD addr )
    {
        return ARead[addr](addr);
    }
	void	WriteLow( WORD addr, BYTE data )
    {
        BWrite[addr](addr, data);
    }

	// $4018-$40FF Extention register read/write
	BYTE	ExRead ( WORD addr )	
    { 
        return ARead[addr](addr); 
    }
	void	ExWrite( WORD addr, BYTE data ) 
    {
        BWrite[addr](addr, data);
    }
    

	// For state save
	BOOL	IsStateSave() 
    { 
        if (_stateRegs)
            return TRUE; 
        else
            return FALSE;
    }

	void	SaveState( LPBYTE p )
    {
        SFORMAT *t = _stateRegs;
        int pIndex = 0;
        while (t->v)
        {
            u8 *bptr = t->v;
            s32 size = t->s & 0xff;

            while (size > 0)
            {
                p[pIndex++] = *bptr;
                size --;
                bptr ++;
            }
        }
    }

	void	LoadState( LPBYTE p )
    {
        SFORMAT *t = _stateRegs;
        int pIndex = 0;
        while (t->v)
        {
            u8 *bptr = t->v;
            s32 size = t->s & 0xff;

            while (size > 0)
            {
                *bptr = p[pIndex++];
                size --;
                bptr ++;
            }
        }
        if (_restore) _restore(0);
    }

private:
};
