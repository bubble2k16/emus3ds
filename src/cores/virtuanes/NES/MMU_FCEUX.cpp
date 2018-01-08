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
u8 *PRGptr[32];
int	PRGram[32];
int	PRGmask2[32];
int	PRGmask4[32];
int	PRGmask8[32];
int	PRGmask16[32];
int	PRGmask32[32];

int	CHRsize[32];
u8 *CHRptr[32];
int	CHRram[32];
int	CHRmask1[32];
int	CHRmask2[32];
int	CHRmask4[32];
int	CHRmask8[32];


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
void setpageptr(int s, uint32 A, uint8 *p, int ram)
{
	uint32 AB = A >> 11;
	uint32 bank = A >> 13;
	int x;

	if (p)
    {
		for (x = (s >> 3) - 1; x >= 0; x--) 
        {
			//PRGIsRAM[AB + x] = ram;
			//Page[AB + x] = p - A;
            SetPROM_Bank( bank + x, RAM + (x * 0x2000), ram ? BANKTYPE_RAM : BANKTYPE_ROM );
		}
    }
	else
    {
        printf ("setpageptr, p = 0\n");
        /*
		for (x = (s >> 1) - 1; x >= 0; x--) {
			PRGIsRAM[AB + x] = 0;
			Page[AB + x] = 0;

            SetPROM_Bank( bank, RAM, ram ? BANKTYPE_RAM : BANKTYPE_ROM );
		} 
        */   
    }
}


void setprg8r(int r, uint32 A, uint32 V) {
	if (PRGsize[r] >= 8192) 
    {
		V &= PRGmask8[r];
		setpageptr(8, A, PRGptr[r] ? (&PRGptr[r][V << 13]) : 0, PRGram[r]);
	} 
    else 
    {
        printf ("setprg32 not supported for size < 8192\n");
        /*
		uint32 VA = V << 2;
		int x;
		for (x = 0; x < 4; x++)
			setpageptr(2, A + (x << 11), PRGptr[r] ? (&PRGptr[r][((VA + x) & PRGmask2[r]) << 11]) : 0, PRGram[r]);
        */
	}
}


void setprg16r(int r, uint32 A, uint32 V) {
	if (PRGsize[r] >= 16384) 
    {
		V &= PRGmask16[r];
		setpageptr(16, A, PRGptr[r] ? (&PRGptr[r][V << 14]) : 0, PRGram[r]);
	} 
    else 
    {
        printf ("setprg32 not supported for size < 16384\n");
        /*
		uint32 VA = V << 3;
		int x;

		for (x = 0; x < 8; x++)
			setpageptr(2, A + (x << 11), PRGptr[r] ? (&PRGptr[r][((VA + x) & PRGmask2[r]) << 11]) : 0, PRGram[r]);
        */
	}
}

void setprg32r(int r, uint32 A, uint32 V) {
	if (PRGsize[r] >= 32768) 
    {
		V &= PRGmask32[r];
		setpageptr(32, A, PRGptr[r] ? (&PRGptr[r][V << 15]) : 0, PRGram[r]);
	} 
    else 
    {
        printf ("setprg32 not supported for size < 32768\n");
        /*
		uint32 VA = V << 4;
		int x;

		for (x = 0; x < 16; x++)
			setpageptr(2, A + (x << 11), PRGptr[r] ? (&PRGptr[r][((VA + x) & PRGmask2[r]) << 11]) : 0, PRGram[r]);
        */
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


void SetupCartPRGMapping(int chip, uint8 *p, uint32 size, int ram) {
	PRGptr[chip] = p;
	PRGsize[chip] = size;

	PRGmask2[chip] = (size >> 11) - 1;
	PRGmask4[chip] = (size >> 12) - 1;
	PRGmask8[chip] = (size >> 13) - 1;
	PRGmask16[chip] = (size >> 14) - 1;
	PRGmask32[chip] = (size >> 15) - 1;

	PRGram[chip] = ram ? 1 : 0;
}

void SetupCartCHRMapping(int chip, uint8 *p, uint32 size, int ram) {
	CHRptr[chip] = p;
	CHRsize[chip] = size;

	CHRmask1[chip] = (size >> 10) - 1;
	CHRmask2[chip] = (size >> 11) - 1;
	CHRmask4[chip] = (size >> 12) - 1;
	CHRmask8[chip] = (size >> 13) - 1;

	CHRram[chip] = ram;
}