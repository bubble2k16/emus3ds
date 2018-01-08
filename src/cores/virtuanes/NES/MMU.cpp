//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Memory Management Unit                                      //
//                                                           Norix      //
//                                               written     2001/02/21 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "typedef.h"
#include "macro.h"
#include "DebugOut.h"

#include "mmu.h"
#include "nes.h"
#include "ppu.h"
#include "cpu.h"

BYTE	nnn;

// CPU ƒƒ‚ƒŠƒoƒ“ƒN
LPBYTE	CPU_MEM_BANK[8];	// 8K’PˆÊ
BYTE	CPU_MEM_TYPE[8];
INT	CPU_MEM_PAGE[8];	// ƒXƒe[ƒgƒZ[ƒu—p

// PPU ƒƒ‚ƒŠƒoƒ“ƒN
LPBYTE	PPU_MEM_BANK[12];	// 1K’PˆÊ
BYTE	PPU_MEM_TYPE[12];
INT		PPU_MEM_PAGE[12];	// ƒXƒe[ƒgƒZ[ƒu—p
BYTE	CRAM_USED[16];		// ƒXƒe[ƒgƒZ[ƒu—p
LPBYTE	VROM_WRITED;		// for mapper 74

// NESƒƒ‚ƒŠ
BYTE	RAM [  8*1024];		// NES“à‘ŸRAM
BYTE	WRAM[128*1024];		// ƒ[ƒN/ƒoƒbƒNƒAƒbƒvRAM
BYTE	DRAM[ 40*1024];		// ƒfƒBƒXƒNƒVƒXƒeƒ€RAM
BYTE	XRAM[  8*1024];		// ƒ_ƒ~[ƒoƒ“ƒN
BYTE	ERAM[ 32*1024];		// Šg’£‹@Ší—pRAM
BYTE	MRAM[128*1024];		//byemu ר��

BYTE	VRAM[  4*1024];		// ƒl[ƒ€ƒe[ƒuƒ‹/ƒAƒgƒŠƒrƒ…[ƒgRAM
BYTE	CRAM[ 32*1024];		// ƒLƒƒƒ‰ƒNƒ^ƒpƒ^[ƒ“RAM

BYTE	YWRAM[1024*1024];	// for YuXing 98/F 1024K PRam
BYTE	YSRAM[  32*1024];	// for YuXing 98/F 32K SRam
BYTE	YCRAM[ 128*1024];	// for YuXing 98/F 128K CRam

BYTE	BDRAM[ 512*1024];	// for BBK 512K PRam
BYTE	BSRAM[  32*1024];	// for BBK 32K SRam
BYTE	BCRAM[ 512*1024];	// for BBK 512K CRam

BYTE	JDRAM[ 512*1024];	// for DrPCJr 512K PRam
BYTE	JSRAM[   8*1024];	// for DrPCJr 8K SRam
BYTE	JCRAM[ 512*1024];	// for DrPCJr 512K CRam

BYTE	tempRAM[ 4*1024];

BYTE	WAVRAM[256];

BYTE	SPRAM[0x100];		// ƒXƒvƒ‰ƒCƒgRAM
BYTE	BGPAL[0x10];		// BGƒpƒŒƒbƒg
BYTE	SPPAL[0x10];		// SPƒpƒŒƒbƒg

u64		cycles_at_scanline_start;
u64		cycles_current;

int					PPU_UPDATE_QUEUE_WPTR;
int					PPU_UPDATE_QUEUE_RPTR;
TPPU_UPDATE_QUEUE	PPU_UPDATE_QUEUE[PPU_UPDATE_QUEUE_SIZE];

INT		PAL_Changed;

INT		DEEMPH_Previous;

// ���W�X�^
BYTE	CPUREG[0x18];		// Nes $4000-$4017
BYTE	PPUREG[0x04];		// Nes $2000-$2003

// Frame-IRQƒŒƒWƒXƒ^($4017)
BYTE	FrameIRQ;

// PPU“à•”ƒŒƒWƒXƒ^
BYTE	PPU56Toggle;		// $2005-$2006 Toggle
BYTE	PPU7_Temp;		// $2007 read buffer
WORD	loopy_t;		// same as $2005/$2006
WORD	loopy_v;		// same as $2005/$2006
WORD	loopy_x;		// tile x offset

// ROMƒf[ƒ^ƒ|ƒCƒ“ƒ^
LPBYTE	PROM;		// PROM ptr
LPBYTE	VROM;		// VROM ptr

LPBYTE	PROMPTR[16];
LPBYTE	VROMPTR[16];

INT PPROM_8K_SIZE[16];
INT PVROM_1K_SIZE[16];

// For dis...
LPBYTE	PROM_ACCESS = NULL;

// ROM ƒoƒ“ƒNƒTƒCƒY
INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;

//
// ‘Sƒƒ‚ƒŠ/ƒŒƒWƒXƒ^“™‚Ì‰Šú‰»
//
void	NesSub_MemoryInitial()
{
INT	i;

	// ƒƒ‚ƒŠƒNƒŠƒA
	ZEROMEMORY( RAM,  sizeof(RAM) );
	ZEROMEMORY( WRAM, sizeof(WRAM) );
	ZEROMEMORY( DRAM, sizeof(DRAM) );
	ZEROMEMORY( ERAM, sizeof(ERAM) );
	ZEROMEMORY( XRAM, sizeof(XRAM) );
	ZEROMEMORY( CRAM, sizeof(CRAM) );
	ZEROMEMORY( VRAM, sizeof(VRAM) );

	ZEROMEMORY( YWRAM, sizeof(YWRAM) );
	ZEROMEMORY( YSRAM, sizeof(YSRAM) );
	ZEROMEMORY( YCRAM, sizeof(YCRAM) );

	ZEROMEMORY( BDRAM, sizeof(BDRAM) );
	ZEROMEMORY( BSRAM, sizeof(BSRAM) );
	ZEROMEMORY( BCRAM, sizeof(BCRAM) );

	ZEROMEMORY( JDRAM, sizeof(JDRAM) );
	ZEROMEMORY( JSRAM, sizeof(JSRAM) );
	ZEROMEMORY( JCRAM, sizeof(JCRAM) );

	ZEROMEMORY( SPRAM, sizeof(SPRAM) );
	ZEROMEMORY( BGPAL, sizeof(BGPAL) );
	ZEROMEMORY( SPPAL, sizeof(SPPAL) );

	ZEROMEMORY( CPUREG, sizeof(CPUREG) );
	ZEROMEMORY( PPUREG, sizeof(PPUREG) );

	PAL_Changed = true;

	DEEMPH_Previous = -1;

	FrameIRQ = 0xC0;

	PROM = VROM = NULL;

	// 0 œŽZ–hŽ~‘Îô
	PROM_8K_SIZE = PROM_16K_SIZE = PROM_32K_SIZE = 1;
	VROM_1K_SIZE = VROM_2K_SIZE = VROM_4K_SIZE = VROM_8K_SIZE = 1;

	// ƒfƒtƒHƒ‹ƒgƒoƒ“ƒNÝ’è
	for( i = 0; i < 8; i++ ) {
		CPU_MEM_BANK[i] = NULL;
		CPU_MEM_TYPE[i] = BANKTYPE_ROM;
		CPU_MEM_PAGE[i] = 0;
	}

	// “à‘ŸRAM/WRAM
	SetPROM_Bank( 0, RAM,  BANKTYPE_RAM );
	SetPROM_Bank( 3, WRAM, BANKTYPE_RAM );

	// ƒ_ƒ~[
	SetPROM_Bank( 1, XRAM, BANKTYPE_ROM );
	SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );

	for( i = 0; i < 8; i++ ) {
		CRAM_USED[i] = 0;
	}
	VROM_WRITED = CRAM+28*1024;

	// PPU VROMƒoƒ“ƒNÝ’è
//	SetVRAM_Mirror( VRAM_MIRROR4 );
}



// CPU ROM bank
void	SetPROM_Bank( BYTE page, LPBYTE ptr, BYTE type )
{
	CPU_MEM_BANK[page] = ptr;
	CPU_MEM_TYPE[page] = type;
	CPU_MEM_PAGE[page] = 0;
}

void	SetPROM_8K_Bank( BYTE page, INT bank )
{
	bank %= PROM_8K_SIZE;
	CPU_MEM_BANK[page] = PROM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;
}

void	SetPROM_16K_Bank( BYTE page, INT bank )
{
	SetPROM_8K_Bank( page+0, bank*2+0 );
	SetPROM_8K_Bank( page+1, bank*2+1 );
}

void	SetPROM_32K_Bank( INT bank )
{
	SetPROM_8K_Bank( 4, bank*4+0 );
	SetPROM_8K_Bank( 5, bank*4+1 );
	SetPROM_8K_Bank( 6, bank*4+2 );
	SetPROM_8K_Bank( 7, bank*4+3 );
}

void	SetPROM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetPROM_8K_Bank( 4, bank0 );
	SetPROM_8K_Bank( 5, bank1 );
	SetPROM_8K_Bank( 6, bank2 );
	SetPROM_8K_Bank( 7, bank3 );
}


extern NES *nes;

void	ResetPPU_MidScanline ()
{
	PPU_UPDATE_QUEUE_RPTR = 1;
	PPU_UPDATE_QUEUE_WPTR = 1;

	TPPU_UPDATE_QUEUE *wq = &PPU_UPDATE_QUEUE[0];
	
	wq->TILE_NO = 0;
	wq->PPU_MEM_BANK[0] = PPU_MEM_BANK[0];
	wq->PPU_MEM_BANK[1] = PPU_MEM_BANK[1];
	wq->PPU_MEM_BANK[2] = PPU_MEM_BANK[2];
	wq->PPU_MEM_BANK[3] = PPU_MEM_BANK[3];
	wq->PPU_MEM_BANK[4] = PPU_MEM_BANK[4];
	wq->PPU_MEM_BANK[5] = PPU_MEM_BANK[5];
	wq->PPU_MEM_BANK[6] = PPU_MEM_BANK[6];
	wq->PPU_MEM_BANK[7] = PPU_MEM_BANK[7];
	wq->PPU_MEM_BANK[8] = PPU_MEM_BANK[8];
	wq->PPU_MEM_BANK[9] = PPU_MEM_BANK[9];
	wq->PPU_MEM_BANK[10] = PPU_MEM_BANK[10];
	wq->PPU_MEM_BANK[11] = PPU_MEM_BANK[11];

	wq->PPUREG = PPUREG[0];	

	if (nes != NULL && nes->ppu != NULL)
		nes->ppu->currentQ = wq;
}

// This is for games that do bank-switching and change of the nametable
// mid-scanline. This way we can reduce graphical glitches in some games
// (eg. Marble Madness and Mother (Japan)) without resorting to using
// TILE_RENDER, which is very slow.
//
void 	UpdatePPU_MidScanline (int page)
{
	if (nes != NULL && nes->ppu != NULL)
		if (nes->ppu->GetChrLatchMode())
			return;

	u64 cycles_diff = 
		cycles_current - cycles_at_scanline_start;
	int pixel = cycles_diff * 3; 
	int tile = pixel / 8 + 1;

	if (tile > 32)
		tile = 0;
	if (nes == NULL)
		return;
	
	int scanline = nes->GetScanline();
	if (scanline == 0 || scanline >= 240)
		tile = 0;

	int prev_ptr = (PPU_UPDATE_QUEUE_WPTR - 1) & (PPU_UPDATE_QUEUE_SIZE - 1);
	int cur_pur = PPU_UPDATE_QUEUE_WPTR;

	TPPU_UPDATE_QUEUE *pq = &PPU_UPDATE_QUEUE[prev_ptr];
	TPPU_UPDATE_QUEUE *wq = &PPU_UPDATE_QUEUE[cur_pur];

	if (PPU_UPDATE_QUEUE_WPTR != PPU_UPDATE_QUEUE_RPTR &&
		pq->TILE_NO == tile)
	{
		//printf (" (%d,%d) ", PPU_UPDATE_QUEUE_RPTR, PPU_UPDATE_QUEUE_WPTR);
		//printf (" ");
		wq = pq;
	}
	else
	{
		//printf ("*");
		wq->TILE_NO = tile;
		wq->PPU_MEM_BANK[0] = PPU_MEM_BANK[0];
		wq->PPU_MEM_BANK[1] = PPU_MEM_BANK[1];
		wq->PPU_MEM_BANK[2] = PPU_MEM_BANK[2];
		wq->PPU_MEM_BANK[3] = PPU_MEM_BANK[3];
		wq->PPU_MEM_BANK[4] = PPU_MEM_BANK[4];
		wq->PPU_MEM_BANK[5] = PPU_MEM_BANK[5];
		wq->PPU_MEM_BANK[6] = PPU_MEM_BANK[6];
		wq->PPU_MEM_BANK[7] = PPU_MEM_BANK[7];
		wq->PPU_MEM_BANK[8] = PPU_MEM_BANK[8];
		wq->PPU_MEM_BANK[9] = PPU_MEM_BANK[9];
		wq->PPU_MEM_BANK[10] = PPU_MEM_BANK[10];
		wq->PPU_MEM_BANK[11] = PPU_MEM_BANK[11];

		PPU_UPDATE_QUEUE_WPTR = (PPU_UPDATE_QUEUE_WPTR + 1) & (PPU_UPDATE_QUEUE_SIZE - 1);

		if (PPU_UPDATE_QUEUE_WPTR == PPU_UPDATE_QUEUE_RPTR)
		{
			//printf ("Oops!\n");
		}
	}

	/*
	if (page == -1)
		printf ("UP_MS: Y=%03d tile=%2d PPUREG = %d\n", nes->GetScanline(), tile, PPUREG[0]);
	else
		printf ("UP_MS: Y=%03d tile=%2d [%d] = %x\n", nes->GetScanline(), tile, page, PPU_MEM_BANK[page]);
	*/
	if (page >= 0)
		wq->PPU_MEM_BANK[page] = PPU_MEM_BANK[page];
	wq->PPUREG = PPUREG[0];
	
}


// PPU VROM bank
void	SetVROM_Bank( BYTE page, LPBYTE ptr, BYTE type )
{
	//PPU_MEM_BANK[page] = ptr;
	CHANGE_PPU_MEM_BANK(ptr);
	PPU_MEM_TYPE[page] = type;
	PPU_MEM_PAGE[page] = 0;
}

void	SetVROM_1K_Bank( BYTE page, INT bank )
{
	bank %= VROM_1K_SIZE;
	//PPU_MEM_BANK[page] = VROM+0x0400*bank;
	CHANGE_PPU_MEM_BANK(VROM+0x0400*bank);
	
	PPU_MEM_TYPE[page] = BANKTYPE_VROM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetVROM_2K_Bank( BYTE page, INT bank )
{
	SetVROM_1K_Bank( page+0, bank*2+0 );
	SetVROM_1K_Bank( page+1, bank*2+1 );
}

void	SetVROM_4K_Bank( BYTE page, INT bank )
{
	SetVROM_1K_Bank( page+0, bank*4+0 );
	SetVROM_1K_Bank( page+1, bank*4+1 );
	SetVROM_1K_Bank( page+2, bank*4+2 );
	SetVROM_1K_Bank( page+3, bank*4+3 );
}

void	SetVROM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetVROM_1K_Bank( i, bank*8+i );
	}
}

void	SetVROM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
			 INT bank4, INT bank5, INT bank6, INT bank7 )
{
	SetVROM_1K_Bank( 0, bank0 );
	SetVROM_1K_Bank( 1, bank1 );
	SetVROM_1K_Bank( 2, bank2 );
	SetVROM_1K_Bank( 3, bank3 );
	SetVROM_1K_Bank( 4, bank4 );
	SetVROM_1K_Bank( 5, bank5 );
	SetVROM_1K_Bank( 6, bank6 );
	SetVROM_1K_Bank( 7, bank7 );
}




void	SetCRAM_1K_Bank( BYTE page, INT bank )
{
	bank &= 0x1F;
	if (VROM == NULL)
		bank &= 0x7;

	//PPU_MEM_BANK[page] = CRAM+0x0400*bank;
	CHANGE_PPU_MEM_BANK(CRAM+0x0400*bank);

	PPU_MEM_TYPE[page] = BANKTYPE_CRAM;
	PPU_MEM_PAGE[page] = bank;

	CRAM_USED[bank>>2] = 0xFF;	// CRAM�g�p�t���O
}

void	SetCRAM_2K_Bank( BYTE page, INT bank )
{
	SetCRAM_1K_Bank( page+0, bank*2+0 );
	SetCRAM_1K_Bank( page+1, bank*2+1 );
}

void	SetCRAM_4K_Bank( BYTE page, INT bank )
{
	SetCRAM_1K_Bank( page+0, bank*4+0 );
	SetCRAM_1K_Bank( page+1, bank*4+1 );
	SetCRAM_1K_Bank( page+2, bank*4+2 );
	SetCRAM_1K_Bank( page+3, bank*4+3 );
}

void	SetCRAM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetCRAM_1K_Bank( i, bank*8+i );	// fix
	}
}

void	SetVRAM_1K_Bank( BYTE page, INT bank )
{
	bank &= 3;
	//PPU_MEM_BANK[page] = VRAM+0x0400*bank;
	CHANGE_PPU_MEM_BANK(VRAM+0x0400*bank);

	PPU_MEM_TYPE[page] = BANKTYPE_VRAM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetVRAM_Bank( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetVRAM_1K_Bank(  8, bank0 );
	SetVRAM_1K_Bank(  9, bank1 );
	SetVRAM_1K_Bank( 10, bank2 );
	SetVRAM_1K_Bank( 11, bank3 );
}

void	SetVRAM_Mirror( INT type )
{
	switch( type ) {
		case	VRAM_HMIRROR:
			SetVRAM_Bank( 0, 0, 1, 1 );
			break;
		case	VRAM_VMIRROR:
			SetVRAM_Bank( 0, 1, 0, 1 );
			break;
		case	VRAM_MIRROR4L:
			SetVRAM_Bank( 0, 0, 0, 0 );
			break;
		case	VRAM_MIRROR4H:
			SetVRAM_Bank( 1, 1, 1, 1 );
			break;
		case	VRAM_MIRROR4:
			SetVRAM_Bank( 0, 1, 2, 3 );
			break;
	}
}

void	SetVRAM_Mirror( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetVRAM_1K_Bank(  8, bank0 );
	SetVRAM_1K_Bank(  9, bank1 );
	SetVRAM_1K_Bank( 10, bank2 );
	SetVRAM_1K_Bank( 11, bank3 );
}

// for YuXing 98/F 1024K PRam
void	SetYWRAM_8K_Bank( BYTE page, INT bank )
{
	bank %= 0x80;
	CPU_MEM_BANK[page] = YWRAM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_RAM;
	CPU_MEM_PAGE[page] = bank;
}
void	SetYWRAM_16K_Bank( BYTE page, INT bank )
{
	SetYWRAM_8K_Bank( page+0, bank*2+0 );
	SetYWRAM_8K_Bank( page+1, bank*2+1 );
}
void	SetYWRAM_32K_Bank( INT bank )
{
	SetYWRAM_8K_Bank( 4, bank*4+0 );
	SetYWRAM_8K_Bank( 5, bank*4+1 );
	SetYWRAM_8K_Bank( 6, bank*4+2 );
	SetYWRAM_8K_Bank( 7, bank*4+3 );
}
void	SetYWRAM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetYWRAM_8K_Bank( 4, bank0 );
	SetYWRAM_8K_Bank( 5, bank1 );
	SetYWRAM_8K_Bank( 6, bank2 );
	SetYWRAM_8K_Bank( 7, bank3 );
}

// for YuXing 98/F 128K CRam
// Up for CoolBoy 256K
void	SetYCRAM_1K_Bank( BYTE page, INT bank )
{
//	bank &= 0x7F;
	bank &= 0xFF;
	PPU_MEM_BANK[page] = YCRAM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_YCRAM;
	PPU_MEM_PAGE[page] = bank;
}
void	SetYCRAM_2K_Bank( BYTE page, INT bank )
{
	SetYCRAM_1K_Bank( page+0, bank*2+0 );
	SetYCRAM_1K_Bank( page+1, bank*2+1 );
}
void	SetYCRAM_4K_Bank( BYTE page, INT bank )
{
	SetYCRAM_1K_Bank( page+0, bank*4+0 );
	SetYCRAM_1K_Bank( page+1, bank*4+1 );
	SetYCRAM_1K_Bank( page+2, bank*4+2 );
	SetYCRAM_1K_Bank( page+3, bank*4+3 );
}
void	SetYCRAM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetYCRAM_1K_Bank( i, bank*8+i );
	}
}

//
void	SetBDRAM_8K_Bank( BYTE page, INT bank )
{
	bank %= 0x40;
	CPU_MEM_BANK[page] = BDRAM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_RAM;
	CPU_MEM_PAGE[page] = bank;
}
void	SetBDRAM_16K_Bank( BYTE page, INT bank )
{
	SetBDRAM_8K_Bank( page+0, bank*2+0 );
	SetBDRAM_8K_Bank( page+1, bank*2+1 );
}
void	SetBDRAM_32K_Bank( INT bank )
{
	SetBDRAM_8K_Bank( 4, bank*4+0 );
	SetBDRAM_8K_Bank( 5, bank*4+1 );
	SetBDRAM_8K_Bank( 6, bank*4+2 );
	SetBDRAM_8K_Bank( 7, bank*4+3 );
}

//------------------------------------------------------------

void	SetJDRAM_8K_Bank( BYTE page, INT bank )
{
	bank %= 0x40;
	CPU_MEM_BANK[page] = JDRAM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_RAM;
	CPU_MEM_PAGE[page] = bank;
}
void	SetJDRAM_32K_Bank( INT bank )
{
	SetJDRAM_8K_Bank( 3, bank*4+0 );
	SetJDRAM_8K_Bank( 4, bank*4+1 );
	SetJDRAM_8K_Bank( 5, bank*4+2 );
	SetJDRAM_8K_Bank( 6, bank*4+3 );
}

void	SetJCRAM_1K_Bank( BYTE page, INT bank )
{
	bank %= 0x200;
	PPU_MEM_BANK[page] = JCRAM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_JCRAM;
	PPU_MEM_PAGE[page] = bank;
}
void	SetJCRAM_2K_Bank( BYTE page, INT bank )
{
	SetJCRAM_1K_Bank( page+0, bank*2+0 );
	SetJCRAM_1K_Bank( page+1, bank*2+1 );
}
void	SetJCRAM_4K_Bank( BYTE page, INT bank )
{
	SetJCRAM_1K_Bank( page+0, bank*4+0 );
	SetJCRAM_1K_Bank( page+1, bank*4+1 );
	SetJCRAM_1K_Bank( page+2, bank*4+2 );
	SetJCRAM_1K_Bank( page+3, bank*4+3 );
}
void	SetJCRAM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetJCRAM_1K_Bank( i, bank*8+i );
	}
}
void	SetJCRAM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
			 INT bank4, INT bank5, INT bank6, INT bank7 )
{
	SetJCRAM_1K_Bank( 0, bank0 );
	SetJCRAM_1K_Bank( 1, bank1 );
	SetJCRAM_1K_Bank( 2, bank2 );
	SetJCRAM_1K_Bank( 3, bank3 );
	SetJCRAM_1K_Bank( 4, bank4 );
	SetJCRAM_1K_Bank( 5, bank5 );
	SetJCRAM_1K_Bank( 6, bank6 );
	SetJCRAM_1K_Bank( 7, bank7 );
}

void	SetOBCRAM_1K_Bank( BYTE page, INT bank )
{
	bank %= (PROM_8K_SIZE*8);
	PPU_MEM_BANK[page] = PROM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_ROM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetPROM_4K_Bank( WORD addr, INT bank )
{
	bank %= (PROM_8K_SIZE*2);
	memcpy( &CPU_MEM_BANK[addr>>13][addr&0x1FFF], PROM+0x1000*bank, 0x1000);
//	memcpy( &CPU_MEM_BANK[addr>>13][addr&0x1FFF], YSRAM+0x1000*bank, 0x1000);
	CPU_MEM_TYPE[addr>>13] = BANKTYPE_ROM;
	CPU_MEM_PAGE[addr>>13] = 0;
}
