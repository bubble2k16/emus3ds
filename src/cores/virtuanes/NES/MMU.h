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

extern	BYTE	nnn;

// CPU ƒƒ‚ƒŠƒoƒ“ƒN
extern	LPBYTE	CPU_MEM_BANK[8];	// 8K’PˆÊ
extern	BYTE	CPU_MEM_TYPE[8];
extern	INT	CPU_MEM_PAGE[8];	// ƒXƒe[ƒgƒZ[ƒu—p

// PPU ƒƒ‚ƒŠƒoƒ“ƒN
extern	LPBYTE	PPU_MEM_BANK[12];	// 1K’PˆÊ
extern	BYTE	PPU_MEM_TYPE[12];
extern	INT		PPU_MEM_PAGE[12];	// ƒXƒe[ƒgƒZ[ƒu—p
extern	BYTE	CRAM_USED[16];		// ƒXƒe[ƒgƒZ[ƒu—p
extern	LPBYTE	VROM_WRITED;		// for mapper 74



// NESƒƒ‚ƒŠ
extern	BYTE	RAM [  8*1024];		// NES“à‘ŸRAM
extern	BYTE	WRAM[128*1024];		// ƒ[ƒN/ƒoƒbƒNƒAƒbƒvRAM
extern	BYTE	DRAM[ 40*1024];		// ƒfƒBƒXƒNƒVƒXƒeƒ€RAM
extern	BYTE	XRAM[  8*1024];		// ƒ_ƒ~[ƒoƒ“ƒN
extern	BYTE	ERAM[ 32*1024];		// Šg’£‹@Ší—pRAM
extern  BYTE	MRAM[128*1024];		//byemu ר��

extern	BYTE	VRAM[  4*1024];		// ƒl[ƒ€ƒe[ƒuƒ‹/ƒAƒgƒŠƒrƒ…[ƒgRAM
extern	BYTE	CRAM[ 32*1024];		// ƒLƒƒƒ‰ƒNƒ^ƒpƒ^[ƒ“RAM

extern	BYTE	YWRAM[1024*1024];	// for YuXing 98/F 1024K PRam
extern	BYTE	YSRAM[  32*1024];	// for YuXing 98/F 32K SRam
extern	BYTE	YCRAM[ 128*1024];	// for YuXing 98/F 128K CRam

extern	BYTE	BDRAM[ 512*1024];	// for BBK 512K PRam
extern	BYTE	BSRAM[  32*1024];	// for BBK 32K SRam
extern	BYTE	BCRAM[ 512*1024];	// for BBK 512K CRam

extern	BYTE	JDRAM[ 512*1024];	// for DrPCJr 512K PRam
extern	BYTE	JSRAM[   8*1024];	// for DrPCJr 8K SRam
extern	BYTE	JCRAM[ 512*1024];	// for DrPCJr 512K CRam

extern	BYTE	tempRAM[ 4*1024];

extern	BYTE	WAVRAM[256];

extern	BYTE	SPRAM[0x100];		// ƒXƒvƒ‰ƒCƒgRAM
extern	BYTE	BGPAL[0x10];		// BGƒpƒŒƒbƒg
extern	BYTE	SPPAL[0x10];		// SPƒpƒŒƒbƒg

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

extern INT		PAL_Changed;

extern INT		DEEMPH_Previous;

#define BGPAL_CHANGED		0x1
#define SPPAL_CHANGED		0x2

// ���W�X�^
extern	BYTE	CPUREG[0x18];		// Nes $4000-$4017
extern	BYTE	PPUREG[0x04];		// Nes $2000-$2003

// Frame-IRQƒŒƒWƒXƒ^($4017)
extern	BYTE	FrameIRQ;

// PPU“à•”ƒŒƒWƒXƒ^
extern	BYTE	PPU56Toggle;		// $2005-$2006 Toggle
extern	BYTE	PPU7_Temp;		// $2007 read buffer
extern	WORD	loopy_t;		// same as $2005/$2006
extern	WORD	loopy_v;		// same as $2005/$2006
extern	WORD	loopy_x;		// tile x offset

// ROMƒf[ƒ^ƒ|ƒCƒ“ƒ^
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

// ROM ƒoƒ“ƒNƒTƒCƒY
extern	INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
extern	INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;


// ŠÖ”
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

// for YuXing 98/F 1024K PRam
extern	void	SetYWRAM_8K_Bank ( BYTE page, INT bank );
extern	void	SetYWRAM_16K_Bank( BYTE page, INT bank );
extern	void	SetYWRAM_32K_Bank( INT bank );
extern	void	SetYWRAM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 );
// for YuXing 98/F 128K CRam
extern	void	SetYCRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetYCRAM_2K_Bank( BYTE page, INT bank );
extern	void	SetYCRAM_4K_Bank( BYTE page, INT bank );
extern	void	SetYCRAM_8K_Bank( INT bank );

extern	void	SetBDRAM_8K_Bank( BYTE page, INT bank );
extern	void	SetBDRAM_16K_Bank( BYTE page, INT bank );
extern	void	SetBDRAM_32K_Bank( INT bank );

extern	void	SetPROM_4K_Bank( WORD addr, INT bank );

extern	void	SetJCRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetJCRAM_2K_Bank( BYTE page, INT bank );
extern	void	SetJCRAM_4K_Bank( BYTE page, INT bank );
extern	void	SetJCRAM_8K_Bank( INT bank );
extern	void	SetJCRAM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
				 INT bank4, INT bank5, INT bank6, INT bank7 );
extern	void	SetJDRAM_8K_Bank ( BYTE page, INT bank );
extern	void	SetJDRAM_32K_Bank( INT bank );

extern	void	SetOBCRAM_1K_Bank( BYTE page, INT bank );


// ƒƒ‚ƒŠƒ^ƒCƒv
// For PROM (CPU)
#define	BANKTYPE_ROM	0x00
#define	BANKTYPE_RAM	0xFF
#define	BANKTYPE_DRAM	0x01
#define	BANKTYPE_MAPPER	0x80
// For VROM/VRAM/CRAM (PPU)
#define	BANKTYPE_VROM	0x00
#define	BANKTYPE_CRAM	0x01
#define	BANKTYPE_YCRAM	0x02
#define	BANKTYPE_JCRAM	0x03
#define	BANKTYPE_VRAM	0x80

// ƒ~ƒ‰[ƒ^ƒCƒv
#define	VRAM_HMIRROR	0x00	// Horizontal
#define	VRAM_VMIRROR	0x01	// Virtical
#define	VRAM_MIRROR4	0x02	// All screen
#define	VRAM_MIRROR4L	0x03	// PA10 LŒÅ’è $2000-$23FF‚Ìƒ~ƒ‰[
#define	VRAM_MIRROR4H	0x04	// PA10 HŒÅ’è $2400-$27FF‚Ìƒ~ƒ‰[
#define	VRAM_MIRROR3H	0x05

#endif	// !__MMU_INCLUDED__

