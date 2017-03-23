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

// CPU �������o���N
LPBYTE	CPU_MEM_BANK[8];	// 8K�P��
BYTE	CPU_MEM_TYPE[8];
INT	CPU_MEM_PAGE[8];	// �X�e�[�g�Z�[�u�p

// PPU �������o���N
LPBYTE	PPU_MEM_BANK[12];	// 1K�P��
BYTE	PPU_MEM_TYPE[12];
INT	PPU_MEM_PAGE[12];	// �X�e�[�g�Z�[�u�p
BYTE	CRAM_USED[16];		// �X�e�[�g�Z�[�u�p

// NES������
BYTE	RAM [  8*1024];		// NES����RAM
BYTE	WRAM[128*1024];		// ���[�N/�o�b�N�A�b�vRAM
BYTE	DRAM[ 40*1024];		// �f�B�X�N�V�X�e��RAM
BYTE	XRAM[  8*1024];		// �_�~�[�o���N
BYTE	ERAM[ 32*1024];		// �g���@���pRAM

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

// PPU VROM bank
void	SetVROM_Bank( BYTE page, LPBYTE ptr, BYTE type )
{
	PPU_MEM_BANK[page] = ptr;
	PPU_MEM_TYPE[page] = type;
	PPU_MEM_PAGE[page] = 0;
}

void	SetVROM_1K_Bank( BYTE page, INT bank )
{
	bank %= VROM_1K_SIZE;
	PPU_MEM_BANK[page] = VROM+0x0400*bank;
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
	PPU_MEM_BANK[page] = CRAM+0x0400*bank;
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
	PPU_MEM_BANK[page] = VRAM+0x0400*bank;
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

