//////////////////////////////////////////////////////////////////////////
//  MMU FCEUX-Compatible Functions
//  
//  These functions make it easy to port mappers from FCEUX.
//
//////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "typedef.h"
#include "macro.h"
#include "mmu_fceux.h"
#include "mmu.h"

// For compatibility with FCEUX mappers
// 
int	PRGsize[32];


// For compatibility with FCEUX mappers
// 
void	setprg8 ( WORD A, WORD bank )
{
	bank %= PROM_8K_SIZE;
	int page = A>>13;
	CPU_MEM_BANK[page] = PROM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;
}

void	setprg16 ( WORD A, WORD bank )
{
	bank = bank*2;
	for(int i=0; i<2; i++)
	{
		setprg8(A,bank+i);
		A+=0x2000;
	}
}

void	setprg32 ( WORD A, WORD bank )
{
	bank = bank*4;
	for(int i=0; i<4; i++)
	{
		setprg8(A,bank+i);
		A+=0x2000;
	}
}


// For compatibility with FCEUX mappers
// 
void	setchr1(WORD A, INT bank)
{
	int page = A>>10;
	
    if (VROM)
    {
        CHANGE_PPU_MEM_BANK(VROM+0x0400*bank);
        PPU_MEM_TYPE[page] = BANKTYPE_VROM;
        PPU_MEM_PAGE[page] = bank;
    }
    else
    {
        CHANGE_PPU_MEM_BANK(CRAM+0x0400*bank);
        PPU_MEM_TYPE[page] = BANKTYPE_CRAM;
        PPU_MEM_PAGE[page] = bank;
    }

}

void	setchr2(WORD A, INT bank)
{
	setchr1(A+0x0000,bank*2+0);
	setchr1(A+0x0400,bank*2+1);
}

void	setchr4(WORD A, INT bank)
{
	setchr1(A+0x0000,bank*4+0);
	setchr1(A+0x0400,bank*4+1);
	setchr1(A+0x0800,bank*4+2);
	setchr1(A+0x0C00,bank*4+3);
}

void	setchr8(INT bank)
{
	for( INT i = 0; i < 8; i++ ) {
		setchr1(0x400*i, bank*8+i );
	}
}

// For compatibility with FCEUX mappers
//
void setmirror(int t) 
{
	SetVRAM_Mirror(t);
}


void setmirrorw(int a, int b, int c, int d) 
{
	SetVRAM_Bank(a, b, c, d);
}

readfunc ARead[0x10000];
writefunc BWrite[0x10000];

// For compatibility with FCEUX mappers
//
u8 ANull(u32 A)
{
	return 0;	// TODO: FCEUX returns X.DB
}

void BNull(u32 A, u8 V)
{

}

u8 CartBR(u32 A)
{
	return	CPU_MEM_BANK[A>>13][A&0x1FFF];
}

void CartBW(u32 A, u8 V)
{
}

void SetReadHandler(s32 start, s32 end, readfunc func) 
{
	s32 x;

	if (!func)
		func = ANull;

	for (x = end; x >= start; x--) 
		ARead[x] = func;
}


void SetWriteHandler(s32 start, s32 end, writefunc func) 
{
	s32 x;

	if (!func)
		func = BNull;

	for (x = end; x >= start; x--) {
		BWrite[x] = func;
	}
}
