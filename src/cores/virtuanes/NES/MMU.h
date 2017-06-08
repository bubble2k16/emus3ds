//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Memory Management Unit                                      //
//                                                           Norix      //
//                                               written     2001/02/21 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#ifndef	__MMU_INCLUDED__
#define	__MMU_INCLUDED__

#include "typedef.h"
#include "macro.h"

// CPU �������o���N
extern	LPBYTE	CPU_MEM_BANK[8];	// 8K�P��
extern	BYTE	CPU_MEM_TYPE[8];
extern	INT	CPU_MEM_PAGE[8];	// �X�e�[�g�Z�[�u�p

// PPU �������o���N
extern	LPBYTE	PPU_MEM_BANK[12];	// 1K�P��
extern	BYTE	PPU_MEM_TYPE[12];
extern	INT	PPU_MEM_PAGE[12];	// �X�e�[�g�Z�[�u�p
extern	BYTE	CRAM_USED[16];		// �X�e�[�g�Z�[�u�p

extern	u64		cycles_at_scanline_start;
extern	u64		cycles_current;


#define PPU_UPDATE_QUEUE_SIZE	64
typedef struct
{
	int		TILE_NO;
	LPBYTE	PPU_MEM_BANK[12];
	int		PPUREG;
} TPPU_UPDATE_QUEUE;

extern	INT					PPU_UPDATE_QUEUE_WPTR;
extern	INT					PPU_UPDATE_QUEUE_RPTR;
extern	TPPU_UPDATE_QUEUE	PPU_UPDATE_QUEUE[PPU_UPDATE_QUEUE_SIZE];


// NES������
extern	BYTE	RAM [  8*1024];		// NES����RAM
extern	BYTE	WRAM[128*1024];		// ���[�N/�o�b�N�A�b�vRAM
extern	BYTE	DRAM[ 40*1024];		// �f�B�X�N�V�X�e��RAM
extern	BYTE	XRAM[  8*1024];		// �_�~�[�o���N
extern	BYTE	ERAM[ 32*1024];		// �g���@���pRAM
extern  BYTE	MRAM[128*1024];		//byemu ר��

extern	BYTE	CRAM[ 32*1024];		// �L�����N�^�p�^�[��RAM
extern	BYTE	VRAM[  4*1024];		// �l�[���e�[�u��/�A�g���r���[�gRAM

extern	BYTE	SPRAM[0x100];		// �X�v���C�gRAM
extern	BYTE	BGPAL[0x10];		// BG�p���b�g
extern	BYTE	SPPAL[0x10];		// SP�p���b�g

extern INT		PAL_Changed;

extern INT		DEEMPH_Previous;

#define BGPAL_CHANGED		0x1
#define SPPAL_CHANGED		0x2

// ���W�X�^
extern	BYTE	CPUREG[0x18];		// Nes $4000-$4017
extern	BYTE	PPUREG[0x04];		// Nes $2000-$2003

// Frame-IRQ���W�X�^($4017)
extern	BYTE	FrameIRQ;

// PPU�������W�X�^
extern	BYTE	PPU56Toggle;		// $2005-$2006 Toggle
extern	BYTE	PPU7_Temp;		// $2007 read buffer
extern	WORD	loopy_t;		// same as $2005/$2006
extern	WORD	loopy_v;		// same as $2005/$2006
extern	WORD	loopy_x;		// tile x offset

// ROM�f�[�^�|�C���^
extern	LPBYTE	PROM;		// PROM ptr
extern	LPBYTE	VROM;		// VROM ptr

extern LPBYTE	PROMPTR[16];
extern LPBYTE	VROMPTR[16];

extern INT PPROM_8K_SIZE[16];
extern INT PVROM_1K_SIZE[16];

#ifdef	_DATATRACE
// For dis...
extern	LPBYTE	PROM_ACCESS;
#endif

// ROM �o���N�T�C�Y
extern	INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
extern	INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;


// �֐�
extern	void	NesSub_MemoryInitial();

extern	void	SetPROM_Bank( BYTE page, LPBYTE ptr, BYTE type );
extern	void	SetPROM_8K_Bank ( BYTE page, INT bank );
extern	void	SetPROM_16K_Bank( BYTE page, INT bank );
extern	void	SetPROM_32K_Bank( INT bank );
extern	void	SetPROM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 );

extern	void	ResetPPU_MidScanline ();
extern	void 	UpdatePPU_MidScanline (int page);

#define CHANGE_PPU_MEM_BANK(v)  						\
	if (PPU_MEM_BANK[page] != (v)) 						\	
	{ 													\
	 	PPU_MEM_BANK[page] = (v); 						\
		UpdatePPU_MidScanline(page); 					\
	}													\

extern	void	SetVROM_Bank( BYTE page, LPBYTE ptr, BYTE type );
extern	void	SetVROM_1K_Bank( BYTE page, INT bank );
extern	void	SetVROM_2K_Bank( BYTE page, INT bank );
extern	void	SetVROM_4K_Bank( BYTE page, INT bank );
extern	void	SetVROM_8K_Bank( INT bank );
extern	void	SetVROM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
				 INT bank4, INT bank5, INT bank6, INT bank7 );

extern	void	SetCRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_2K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_4K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_8K_Bank( INT bank );

extern	void	SetVRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetVRAM_Bank( INT bank0, INT bank1, INT bank2, INT bank3 );
extern	void	SetVRAM_Mirror( INT type );
extern	void	SetVRAM_Mirror( INT bank0, INT bank1, INT bank2, INT bank3 );

// �������^�C�v
// For PROM (CPU)
#define	BANKTYPE_ROM	0x00
#define	BANKTYPE_RAM	0xFF
#define	BANKTYPE_DRAM	0x01
#define	BANKTYPE_MAPPER	0x80
// For VROM/VRAM/CRAM (PPU)
#define	BANKTYPE_VROM	0x00
#define	BANKTYPE_CRAM	0x01
#define	BANKTYPE_VRAM	0x80

// �~���[�^�C�v
#define	VRAM_HMIRROR	0x00	// Horizontal
#define	VRAM_VMIRROR	0x01	// Virtical
#define	VRAM_MIRROR4	0x02	// All screen
#define	VRAM_MIRROR4L	0x03	// PA10 L�Œ� $2000-$23FF�̃~���[
#define	VRAM_MIRROR4H	0x04	// PA10 H�Œ� $2400-$27FF�̃~���[


#endif	// !__MMU_INCLUDED__

