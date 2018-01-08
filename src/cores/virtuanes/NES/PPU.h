//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES PPU core                                                    //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__PPU_INCLUDED__
#define	__PPU_INCLUDED__

#include "typedef.h"
#include "macro.h"

// class prototype
class	NES;

// PPU Control Register #1	PPU #0
#define	PPU_VBLANK_BIT		0x80
#define	PPU_SPHIT_BIT		0x40		// �Ⴄ�H
#define	PPU_SP16_BIT		0x20
#define	PPU_BGTBL_BIT		0x10
#define	PPU_SPTBL_BIT		0x08
#define	PPU_INC32_BIT		0x04
#define	PPU_NAMETBL_BIT		0x03

// PPU Control Register #2	PPU #1
#define	PPU_BGCOLOR_BIT		0xE0
#define	PPU_SPDISP_BIT		0x10
#define	PPU_BGDISP_BIT		0x08
#define	PPU_SPCLIP_BIT		0x04
#define	PPU_BGCLIP_BIT		0x02
#define	PPU_COLORMODE_BIT	0x01

// PPU Status Register		PPU #2
#define	PPU_VBLANK_FLAG		0x80
#define	PPU_SPHIT_FLAG		0x40
#define	PPU_SPMAX_FLAG		0x20
#define	PPU_WENABLE_FLAG	0x10

// SPRITE Attribute
#define	SP_VMIRROR_BIT		0x80
#define	SP_HMIRROR_BIT		0x40
#define	SP_PRIORITY_BIT		0x20
#define	SP_COLOR_BIT		0x03

class	PPU
{
public:
	PPU( NES* parent );
	virtual	~PPU();

	enum { SCREEN_WIDTH = 256+16, SCREEN_HEIGHT = 240 };

	void	Reset();

	BYTE	Read ( WORD addr );
	void	Write( WORD addr, BYTE data );

	void	DMA( BYTE data );

	void	VBlankStart();
	void	VBlankEnd();

	void	FrameStart();
	void	FrameEnd();

	void	SetRenderScanline( INT scanline );

	void	ScanlineStart();
	void	ScanlineNext();

	WORD	GetPPUADDR()	{ return loopy_v; }
	WORD	GetTILEY()	{ return loopy_y; }

	// Scanline base render
	void	ScanlineUpTo( INT scanline, BOOL bMax, BOOL bLeftClip, INT tile );
	void	Scanline( INT scanline, BOOL bMax, BOOL bLeftClip );
	void	DummyScanline( INT scanline );

	// For mapper
	void	SetExtLatchMode( BOOL bMode )	{ bExtLatch = bMode; }
	void	SetChrLatchMode( BOOL bMode )	{ bChrLatch = bMode; }
	void	SetExtNameTableMode( BOOL bMode ) { bExtNameTable = bMode; }
	void	SetExtMonoMode( BOOL bMode )	{ bExtMono = bMode; }
	BOOL	GetExtMonoMode()	{ return bExtMono; }

	BOOL	IsDispON()	{ return PPUREG[1]&(PPU_BGDISP_BIT|PPU_SPDISP_BIT); }
	BOOL	IsBGON()	{ return PPUREG[1]&PPU_BGDISP_BIT; }
	BOOL	IsSPON()	{ return PPUREG[1]&PPU_SPDISP_BIT; }

	BYTE	GetBGCOLOR()	{ return (PPUREG[1]&PPU_BGCOLOR_BIT)>>5; }

	BOOL	IsBGCLIP()	{ return PPUREG[1]&PPU_BGCLIP_BIT; }
	BOOL	IsSPCLIP()	{ return PPUREG[1]&PPU_SPCLIP_BIT; }

	BOOL	IsMONOCROME()	{ return PPUREG[1]&PPU_COLORMODE_BIT; }

	BOOL	IsVBLANK()	{ return PPUREG[2]&PPU_VBLANK_FLAG; }
	BOOL	IsSPHIT()	{ return PPUREG[2]&PPU_SPHIT_FLAG; }
	BOOL	IsSPMAX()	{ return PPUREG[2]&PPU_SPMAX_FLAG; }
	BOOL	IsPPUWE()	{ return PPUREG[2]&PPU_WENABLE_FLAG; }

	BOOL	IsSprite0( INT scanline );

	void	SetScreenPtr( LPBYTE lpScn, LPBYTE lpMode ) { /*lpScreen = lpScn;*/ lpColormode = lpMode; }
	void	SetScreenRGBAPtr( DWORD *lpScn, LPBYTE lpMode ) { lpScreen32 = lpScn; lpColormode = lpMode; }
	//LPBYTE	GetScreenPtr()	{ return lpScreen; }
	u32		*GetScreen32Ptr()	{ return lpScreen32; }

	INT	GetScanlineNo()	{ return ScanlineNo; }

	// For VS-Unisystem
	void	SetVSMode( BOOL bMode ) { bVSMode = bMode; }
	void	SetVSSecurity( BYTE data ) { VSSecurityData = data; }
	void	SetVSColorMap( INT nColorMap ) { nVSColorMap = nColorMap; }

	TPPU_UPDATE_QUEUE *currentQ;

protected:
	INT		chrLUT1[65536];		
	INT		chrLUT2[65536];		
	NES*	nes;

	BOOL	bExtLatch;	// For MMC5
	BOOL	bChrLatch;	// For MMC2/MMC4
	BOOL	bExtNameTable;	// For Super Monkey no Dai Bouken
	BOOL	bExtMono;	// For Final Fantasy

	WORD	loopy_y;
	WORD	loopy_shift;

	//LPBYTE	lpScreen;
	DWORD 	*lpScreen32;
	//LPBYTE	lpScanline;
	INT	ScanlineNo;
	LPBYTE	lpColormode;

	typedef	struct	tagSPRITE {
		BYTE	y;
		BYTE	tile;
		BYTE	attr;
		BYTE	x;
	} SPRITE, *LPSPRITE;

	// Reversed bits
	BYTE	Bit2Rev[256];

	// For VS-Unisystem
	BOOL	bVSMode;
	BYTE	VSSecurityData;
	INT	nVSColorMap;
	static	BYTE	VSColorMap[5][64];
private:
};


#endif	// !__PPU_INCLUDED__

