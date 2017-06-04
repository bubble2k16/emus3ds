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

// CPU �������o���N
LPBYTE	CPU_MEM_BANK[8];	// 8K�P��
BYTE	CPU_MEM_TYPE[8];
INT	CPU_MEM_PAGE[8];	// �X�e�[�g�Z�[�u�p

// PPU �������o���N
LPBYTE	PPU_MEM_BANK[12];	// 1K�P��
BYTE	PPU_MEM_TYPE[12];
INT	PPU_MEM_PAGE[12];	// �X�e�[�g�Z�[�u�p
BYTE	CRAM_USED[16];		// �X�e�[�g�Z�[�u�p

u64		cycles_at_scanline_start;
u64		cycles_current;

int					PPU_UPDATE_QUEUE_WPTR;
int					PPU_UPDATE_QUEUE_RPTR;
TPPU_UPDATE_QUEUE	PPU_UPDATE_QUEUE[PPU_UPDATE_QUEUE_SIZE];

// NES������
BYTE	RAM [  8*1024];		// NES����RAM
BYTE	WRAM[128*1024];		// ���[�N/�o�b�N�A�b�vRAM
BYTE	DRAM[ 40*1024];		// �f�B�X�N�V�X�e��RAM
BYTE	XRAM[  8*1024];		// �_�~�[�o���N
BYTE	ERAM[ 32*1024];		// �g���@���pRAM
BYTE	MRAM[128*1024];		//byemu ר��

BYTE	CRAM[ 32*1024];		// �L�����N�^�p�^�[��RAM
BYTE	VRAM[  4*1024];		// �l�[���e�[�u��/�A�g���r���[�gRAM

BYTE	SPRAM[0x100];		// �X�v���C�gRAM
BYTE	BGPAL[0x10];		// BG�p���b�g
BYTE	SPPAL[0x10];		// SP�p���b�g

INT		PAL_Changed;

INT		DEEMPH_Previous;

// ���W�X�^
BYTE	CPUREG[0x18];		// Nes $4000-$4017
BYTE	PPUREG[0x04];		// Nes $2000-$2003

// Frame-IRQ���W�X�^($4017)
BYTE	FrameIRQ;

// PPU�������W�X�^
BYTE	PPU56Toggle;		// $2005-$2006 Toggle
BYTE	PPU7_Temp;		// $2007 read buffer
WORD	loopy_t;		// same as $2005/$2006
WORD	loopy_v;		// same as $2005/$2006
WORD	loopy_x;		// tile x offset

// ROM�f�[�^�|�C���^
LPBYTE	PROM;		// PROM ptr
LPBYTE	VROM;		// VROM ptr

LPBYTE	PROMPTR[16];
LPBYTE	VROMPTR[16];

INT PPROM_8K_SIZE[16];
INT PVROM_1K_SIZE[16];

// For dis...
LPBYTE	PROM_ACCESS = NULL;

// ROM �o���N�T�C�Y
INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;

//
// �S������/���W�X�^���̏�����
//
void	NesSub_MemoryInitial()
{
INT	i;

	// �������N���A
	ZEROMEMORY( RAM,  sizeof(RAM) );
	ZEROMEMORY( WRAM, sizeof(WRAM) );
	ZEROMEMORY( DRAM, sizeof(DRAM) );
	ZEROMEMORY( ERAM, sizeof(ERAM) );
	ZEROMEMORY( XRAM, sizeof(XRAM) );
	ZEROMEMORY( CRAM, sizeof(CRAM) );
	ZEROMEMORY( VRAM, sizeof(VRAM) );

	ZEROMEMORY( SPRAM, sizeof(SPRAM) );
	ZEROMEMORY( BGPAL, sizeof(BGPAL) );
	ZEROMEMORY( SPPAL, sizeof(SPPAL) );

	ZEROMEMORY( CPUREG, sizeof(CPUREG) );
	ZEROMEMORY( PPUREG, sizeof(PPUREG) );

	PAL_Changed = true;

	DEEMPH_Previous = -1;

	FrameIRQ = 0xC0;

	PROM = VROM = NULL;

	// 0 ���Z�h�~�΍�
	PROM_8K_SIZE = PROM_16K_SIZE = PROM_32K_SIZE = 1;
	VROM_1K_SIZE = VROM_2K_SIZE = VROM_4K_SIZE = VROM_8K_SIZE = 1;

	// �f�t�H���g�o���N�ݒ�
	for( i = 0; i < 8; i++ ) {
		CPU_MEM_BANK[i] = NULL;
		CPU_MEM_TYPE[i] = BANKTYPE_ROM;
		CPU_MEM_PAGE[i] = 0;
	}

	// ����RAM/WRAM
	SetPROM_Bank( 0, RAM,  BANKTYPE_RAM );
	SetPROM_Bank( 3, WRAM, BANKTYPE_RAM );

	// �_�~�[
	SetPROM_Bank( 1, XRAM, BANKTYPE_ROM );
	SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );

	for( i = 0; i < 8; i++ ) {
		CRAM_USED[i] = 0;
	}

	// PPU VROM�o���N�ݒ�
//	SetVRAM_Mirror( VRAM_MIRROR4 );
}


/*
void	SetPrg2 ( WORD A, INT bank )
{
	bank %= (PROM_8K_SIZE*4);
	int page = A>>11;
	CPU_MEM_BANK[page] = PROM+0x800*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;

}
void	SetPrg4 ( WORD A, INT bank )
{
	SetPrg2(A+0x000,bank*2+0);
	SetPrg2(A+0x800,bank*2+1);
}

void	SetPrg8 ( WORD A, INT bank )
{
	SetPrg2(A+0x0000,bank*4+0);
	SetPrg2(A+0x0800,bank*4+1);
	SetPrg2(A+0x1000,bank*4+2);
	SetPrg2(A+0x1800,bank*4+3);
}

void	SetPrg16 ( WORD A, INT bank )
{
	bank = bank*8;
	for(int i=0; i<8; i++)
	{
		SetPrg2(A,bank+i);
		A+=0x800;
	}
}

void	SetPrg32 ( WORD A, INT bank )
{
	bank = bank*16;
	for(int i=0; i<16; i++)
	{
		SetPrg2(A,bank+i);
		A+=0x800;
	}
}

void	SetPrg2r (int r,WORD A, INT bank )
{
	bank %= (PROM_8K_SIZE*4);
	int page = A>>11;
	CPU_MEM_BANK[page] = PROMPTR[r]+0x800*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;

}
void	SetPrg4r (int r, WORD A, INT bank )
{
	SetPrg2r(r,A+0x000,bank*2+0);
	SetPrg2r(r,A+0x800,bank*2+1);
}

void	SetPrg8r (int r, WORD A, INT bank )
{
	SetPrg2r(r,A+0x0000,bank*4+0);
	SetPrg2r(r,A+0x0800,bank*4+1);
	SetPrg2r(r,A+0x1000,bank*4+2);
	SetPrg2r(r,A+0x1800,bank*4+3);
}

void	SetPrg16r (int r, WORD A, INT bank )
{
	bank = bank*8;
	for(int i=0; i<8; i++)
	{
		SetPrg2r(r,A,bank+i);
		A+=0x800;
	}
}

void	SetPrg32r (int r, WORD A, INT bank )
{
	bank = bank*16;
	for(int i=0; i<16; i++)
	{
		SetPrg2r(r,A,bank+i);
		A+=0x800;
	}
}
*/

void	SetPrg8 ( WORD A, WORD bank )
{
	bank %= PROM_8K_SIZE;
	int page = A>>13;
	CPU_MEM_BANK[page] = PROM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;
}

void	SetPrg16 ( WORD A, WORD bank )
{
	bank = bank*2;
	for(int i=0; i<2; i++)
	{
		SetPrg8(A,bank+i);
		A+=0x2000;
	}
}

void	SetPrg32 ( WORD A, WORD bank )
{
	bank = bank*4;
	for(int i=0; i<4; i++)
	{
		SetPrg8(A,bank+i);
		A+=0x2000;
	}
}

void	SetPrg8r (int r, WORD A, WORD bank )
{
	bank %= PPROM_8K_SIZE[r];
	int page = A>>13;
	CPU_MEM_BANK[page] = PROMPTR[r]+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;
}

void	SetPrg16r (int r, WORD A, WORD bank )
{
	bank = bank*2;
	for(int i=0; i<2; i++)
	{
		SetPrg8r(r,A,bank+i);
		A+=0x2000;
	}
}

void	SetPrg32r (int r, WORD A, WORD bank )
{
	bank = bank*4;
	for(int i=0; i<4; i++)
	{
		SetPrg8r(r,A,bank+i);
		A+=0x2000;
	}
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

	nes->ppu->currentQ = wq;

}

#define CHANGE_PPU_MEM_BANK(v)  						\
	if (PPU_MEM_BANK[page] != (v)) 						\	
	{ 													\
	 	PPU_MEM_BANK[page] = (v); 						\
		UpdatePPU_MidScanline(page); 					\
	}													\

// This is for games that do bank-switching and change of the nametable
// mid-scanline. This way we can reduce graphical glitches in some games
// (eg. Marble Madness and Mother (Japan)) without resorting to using
// TILE_RENDER, which is very slow.
//
void 	UpdatePPU_MidScanline (int page)
{
	u64 cycles_diff = 
		cycles_current - cycles_at_scanline_start;
	int pixel = cycles_diff * 3; 
	int tile = pixel / 8 + 1;

	if (tile > 32)
		tile = 0;
	int scanline = nes->GetScanline();
	if (scanline == 0 || scanline >= 240)
		tile = 0;

/*
	if (page >= 0)
	{
		int s = nes->GetScanline();

		if ((s >= 64 && s <= 71) || s == 192 || s == 193)
		{
			printf ("%3d %3d (%2d) %2d = ", nes->GetScanline(), pixel, (int)tile, page);
			printf ("%8x\n", PPU_MEM_BANK[page] - VROM);
		}
	}
*/
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
			printf ("Oops!\n");
		}
	}
	if (page >= 0)
		wq->PPU_MEM_BANK[page] = PPU_MEM_BANK[page];
	wq->PPUREG = PPUREG[0];
	
	/*
	if (scanline < 24)
	{
	printf (" (%3d,%3d) t=%2d pg=%2d < ", nes->GetScanline(), pixel, (int)tile, page);
	if (page == -1)
		printf ("%02x|", PPUREG[0]);
	else
		printf ("%5x|", PPU_MEM_BANK[page] - VROM);
	printf ("%2d %2d\n", PPU_UPDATE_QUEUE_RPTR, PPU_UPDATE_QUEUE_WPTR);
	}
	*/
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

void	SetChr1r(int r, WORD A, INT bank)
{
	if(PVROM_1K_SIZE[r]==0)
		return;

	bank %= PVROM_1K_SIZE[r];
	int page = A>>10;
	
	//PPU_MEM_BANK[page] = VROMPTR[r]+0x0400*bank;
	CHANGE_PPU_MEM_BANK(VROMPTR[r]+0x0400*bank);

	PPU_MEM_TYPE[page] = BANKTYPE_VROM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetChr2r(int r, WORD A, INT bank)
{
	SetChr1r(r,A+0x0000,bank*2+0);
	SetChr1r(r,A+0x0400,bank*2+1);
}

void	SetChr4r(int r, WORD A, INT bank)
{
	SetChr1r(r,A+0x0000,bank*4+0);
	SetChr1r(r,A+0x0400,bank*4+1);
	SetChr1r(r,A+0x0800,bank*4+2);
	SetChr1r(r,A+0x0C00,bank*4+3);
}
void	SetChr8r(int r, WORD A, INT bank)
{
	for( INT i = 0; i < 8; i++ ) {
		SetChr1r(r, A+0x400*i, bank*8+i );
	}
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

