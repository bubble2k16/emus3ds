//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES PPU core                                                    //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "VirtuaNESres.h"

#include "typedef.h"
#include "macro.h"

#include "DebugOut.h"
#include "App.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "rom.h"
#include "mapper.h"

#include "3dsdbg.h"
#include "3dsopt.h"

BYTE	PPU::VSColorMap[5][64] = {
	{	0x35, 0xFF, 0x16, 0x22, 0x1C, 0xFF, 0xFF, 0x15,
		0xFF, 0x00, 0x27, 0x05, 0x04, 0x27, 0x08, 0x30,
		0x21, 0xFF, 0xFF, 0x29, 0x3C, 0xFF, 0x36, 0x12,
		0xFF, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
		0xFF, 0x31, 0xFF, 0x2A, 0x2C, 0x0C, 0xFF, 0xFF,
		0xFF, 0x07, 0x34, 0x06, 0x13, 0xFF, 0x26, 0x0F,
		0xFF, 0x19, 0x10, 0x0A, 0xFF, 0xFF, 0xFF, 0x17,
		0xFF, 0x11, 0x09, 0xFF, 0xFF, 0x25, 0x18, 0xFF 
	},
	{	0xFF, 0x27, 0x18, 0xFF, 0x3A, 0x25, 0xFF, 0x31,
////		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0xFF, 0x0B,
//		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0xFF, 0x1A,
		0x16, 0x13, 0x38, 0x34, 0x20, 0x23, 0x31, 0x1A,
		0xFF, 0x21, 0x06, 0xFF, 0x1B, 0x29, 0xFF, 0x22,
		0xFF, 0x24, 0xFF, 0xFF, 0xFF, 0x08, 0xFF, 0x03,
		0xFF, 0x36, 0x26, 0x33, 0x11, 0xFF, 0x10, 0x02,
		0x14, 0xFF, 0x00, 0x09, 0x12, 0x0F, 0xFF, 0x30,
		0xFF, 0xFF, 0x2A, 0x17, 0x0C, 0x01, 0x15, 0x19,
		0xFF, 0x2C, 0x07, 0x37, 0xFF, 0x05, 0xFF, 0xFF 
	},
#if	1
	{	0xFF, 0xFF, 0xFF, 0x10, 0x1A, 0x30, 0x31, 0x09,
		0x01, 0x0F, 0x36, 0x08, 0x15, 0xFF, 0xFF, 0xF0,
		0x22, 0x1C, 0xFF, 0x12, 0x19, 0x18, 0x17, 0xFF,
		0x00, 0xFF, 0xFF, 0x02, 0x16, 0x06, 0xFF, 0x35,
		0x23, 0xFF, 0x8B, 0xF7, 0xFF, 0x27, 0x26, 0x20,
		0x29, 0xFF, 0x21, 0x24, 0x11, 0xFF, 0xEF, 0xFF,
		0x2C, 0xFF, 0xFF, 0xFF, 0x07, 0xF9, 0x28, 0xFF,
		0x0A, 0xFF, 0x32, 0x37, 0x13, 0xFF, 0xFF, 0x0C 
	},
#else
	{	0xFF, 0xFF, 0xFF, 0x10, 0x0B, 0x30, 0x31, 0x09,	// 00-07
		0x01, 0x0F, 0x36, 0x08, 0x15, 0xFF, 0xFF, 0x3C,	// 08-0F
		0x22, 0x1C, 0xFF, 0x12, 0x19, 0x18, 0x17, 0x1B,	// 10-17
		0x00, 0xFF, 0xFF, 0x02, 0x16, 0x06, 0xFF, 0x35,	// 18-1F
		0x23, 0xFF, 0x8B, 0x3C, 0xFF, 0x27, 0x26, 0x20,	// 20-27
		0x29, 0x04, 0x21, 0x24, 0x11, 0xFF, 0xEF, 0xFF,	// 28-2F
		0x2C, 0xFF, 0xFF, 0xFF, 0x07, 0x39, 0x28, 0xFF,	// 30-37
		0x0A, 0xFF, 0x32, 0x38, 0x13, 0x3B, 0xFF, 0x0C 	// 38-3F
	},
#endif
#if	0
	{	0x18, 0xFF, 0x1C, 0x89, 0x0F, 0xFF, 0x01, 0x17,
		0x10, 0x0F, 0x2A, 0xFF, 0x36, 0x37, 0x1A, 0xFF,
		0x25, 0xFF, 0x12, 0xFF, 0x0F, 0xFF, 0xFF, 0x26,
		0xFF, 0xFF, 0x22, 0xFF, 0xFF, 0x0F, 0x3A, 0x21,
		0x05, 0x0A, 0x07, 0xC2, 0x13, 0xFF, 0x00, 0x15,
		0x0C, 0xFF, 0x11, 0xFF, 0xFF, 0x38, 0xFF, 0xFF,
		0xFF, 0xFF, 0x08, 0x45, 0xFF, 0xFF, 0x30, 0x3C,
		0x0F, 0x27, 0xFF, 0x60, 0x29, 0xFF, 0x30, 0x09 
	},
#else
	{	0x18, 0xFF, 0x1C, 0x89, 0x0F, 0xFF, 0x01, 0x17,	// 00-07
		0x10, 0x0F, 0x2A, 0xFF, 0x36, 0x37, 0x1A, 0xFF,	// 08-0F
		0x25, 0xFF, 0x12, 0xFF, 0x0F, 0xFF, 0xFF, 0x26,	// 10-17
		0xFF, 0xFF, 0x22, 0xFF, 0xFF, 0x0F, 0x3A, 0x21,	// 18-1F
		0x05, 0x0A, 0x07, 0xC2, 0x13, 0xFF, 0x00, 0x15,	// 20-27
		0x0C, 0xFF, 0x11, 0xFF, 0xFF, 0x38, 0xFF, 0xFF,	// 28-2F
		0xFF, 0xFF, 0x08, 0x16, 0xFF, 0xFF, 0x30, 0x3C,	// 30-37
		0x0F, 0x27, 0xFF, 0x60, 0x29, 0xFF, 0x30, 0x09 	// 38-3F
	},
#endif
	{
	// Super Xevious/Gradius
		0x35, 0xFF, 0x16, 0x22, 0x1C, 0x09, 0xFF, 0x15,	// 00-07
		0x20, 0x00, 0x27, 0x05, 0x04, 0x28, 0x08, 0x30,	// 08-0F
		0x21, 0xFF, 0xFF, 0x29, 0x3C, 0xFF, 0x36, 0x12,	// 10-17
		0xFF, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,	// 18-1F
		0xFF, 0x31, 0xFF, 0x2A, 0x2C, 0x0C, 0x1B, 0xFF,	// 20-27
		0xFF, 0x07, 0x34, 0x06, 0xFF, 0x25, 0x26, 0x0F,	// 28-2F
		0xFF, 0x19, 0x10, 0x0A, 0xFF, 0xFF, 0xFF, 0x17,	// 30-37
		0xFF, 0x11, 0x1A, 0xFF, 0x38, 0xFF, 0x18, 0x3A,	// 38-3F
	}
};

PPU::PPU( NES* parent ) : nes(parent)
{
	//lpScreen = NULL;
	lpColormode = NULL;
	lpScreen32 = NULL;		// Bug fix.

	bVSMode = FALSE;
	nVSColorMap = -1;
	VSSecurityData = 0;

	// ���E���]�}�X�N�e�[�u��
	for( INT i = 0; i < 256; i++ ) {
		BYTE	m = 0x80;
		BYTE	c = 0;
		for( INT j = 0; j < 8; j++ ) {
			if( i&(1<<j) ) {
				c |= m;
			}
			m >>= 1;
		}
		Bit2Rev[i] = c;
	}

	for (INT chr_h = 0; chr_h < 256; chr_h++)
		for (INT chr_l = 0; chr_l < 256; chr_l++)
		{
			chrLUT1[chr_h * 256 + chr_l] = ((chr_l>>1)&0x55)|(chr_h&0xAA);
			chrLUT2[chr_h * 256 + chr_l] = (chr_l&0x55)|((chr_h<<1)&0xAA);
		}
}

PPU::~PPU()
{
}

void	PPU::Reset()
{
	bExtLatch = FALSE;
	bChrLatch = FALSE;
	bExtNameTable = FALSE;
	bExtMono = FALSE;

	PPUREG[0] = PPUREG[1] = 0;

	PPU56Toggle = 0;

	PPU7_Temp = 0xFF;	// VS Excitebike�ł��������Ȃ�($2006���ǂ݂ɍs���o�O������)
//	PPU7_Temp = 0;

	loopy_v = loopy_t = 0;
	loopy_x = loopy_y = 0;
	loopy_shift = 0;

	//if( lpScreen )
	//	::memset( lpScreen, 0x3F, SCREEN_WIDTH*SCREEN_HEIGHT*sizeof(BYTE) );

	if( lpScreen32 )
		::memset( lpScreen32, 0x00, 512*256*4 );

	if( lpColormode )
		::memset( lpColormode, 0, SCREEN_HEIGHT*sizeof(BYTE) );

}

BYTE	PPU::Read( WORD addr )
{
BYTE	data = 0x00;

	switch( addr ) {
		// Write only Register
		case	0x2000: // PPU Control Register #1(W)
		case	0x2001: // PPU Control Register #2(W)
		case	0x2003: // SPR-RAM Address Register(W)
		case	0x2005: // PPU Scroll Register(W2)
		case	0x2006: // VRAM Address Register(W2)
			data = PPU7_Temp;	// ����
			break;
		// Read/Write Register
		case	0x2002: // PPU Status Register(R)
//DEBUGOUT( "2002 RD L:%3d C:%8d\n", ScanlineNo, nes->cpu->GetTotalCycles() );
			data = PPUREG[2] | VSSecurityData;
			PPU56Toggle = 0;
			PPUREG[2] &= ~PPU_VBLANK_FLAG;
			break;
		case	0x2004: // SPR_RAM I/O Register(RW)
			data = SPRAM[ PPUREG[3]++ ];
			break;
		case	0x2007: // VRAM I/O Register(RW)
			WORD	addr = loopy_v & 0x3FFF;
			data = PPU7_Temp;
			if( PPUREG[0] & PPU_INC32_BIT ) loopy_v+=32;
			else				loopy_v++;
			if( addr >= 0x3000 ) {
				if( addr >= 0x3F00 ) {
//					data &= 0x3F;
					if( !(addr&0x0010) ) {
						return	BGPAL[addr&0x000F];
					} else {
						return	SPPAL[addr&0x000F];
					}
				}
				addr &= 0xEFFF;
			}
			PPU7_Temp = PPU_MEM_BANK[addr>>10][addr&0x03FF];
	}

	return	data;
}

void	PPU::Write( WORD addr, BYTE data )
{
	if( bVSMode && VSSecurityData ) {
		if( addr == 0x2000 ) {
			addr = 0x2001;
		} else if( addr == 0x2001 ){
			addr = 0x2000;
		}
	}

	switch( addr ) {
		// Read only Register
		case	0x2002: // PPU Status register(R)
			break;
		// Write Register
		case	0x2000: // PPU Control Register #1(W)
			// NameTable select
			// t:0000110000000000=d:00000011
			loopy_t = (loopy_t & 0xF3FF)|(((WORD)data & 0x03)<<10);

			if( (data & 0x80) && !(PPUREG[0] & 0x80) && (PPUREG[2] & 0x80) ) {
				nes->cpu->NMI();	// hmm..
			}
//DEBUGOUT( "W2000 %02X O:%02X S:%02X L:%3d C:%8d\n", data, PPUREG[0], PPUREG[2], ScanlineNo, nes->cpu->GetTotalCycles() );

			if (PPUREG[0] != data)
			{
				PPUREG[0] = data;
				UpdatePPU_MidScanline (-1);
			}
			break;
		case	0x2001: // PPU Control Register #2(W)
//DEBUGOUT( "W2001 %02X L:%3d C:%8d\n", data, ScanlineNo, nes->cpu->GetTotalCycles() );
			PPUREG[1] = data;
			break;
		case	0x2003: // SPR-RAM Address Register(W)
			PPUREG[3] = data;
			break;
		case	0x2004: // SPR_RAM I/O Register(RW)
			SPRAM[ PPUREG[3]++ ] = data;
			break;

		case	0x2005: // PPU Scroll Register(W2)
//DEBUGOUT( "SCR WRT L:%3d C:%8d\n", ScanlineNo, nes->cpu->GetTotalCycles() );
			//printf ("$2005 = %d, sl=%d, c=%d\n", data, ScanlineNo, nes->cpu->GetTotalCycles());
			if( !PPU56Toggle ) {
			// First write
				// tile X t:0000000000011111=d:11111000
				loopy_t = (loopy_t & 0xFFE0)|(((WORD)data)>>3);
				// scroll offset X x=d:00000111
				loopy_x = data & 0x07;
			} else {
			// Second write
				// tile Y t:0000001111100000=d:11111000
				loopy_t = (loopy_t & 0xFC1F)|((((WORD)data) & 0xF8)<<2);
				// scroll offset Y t:0111000000000000=d:00000111
				loopy_t = (loopy_t & 0x8FFF)|((((WORD)data) & 0x07)<<12);
			}
			PPU56Toggle = !PPU56Toggle;
			break;
		case	0x2006: // VRAM Address Register(W2)
			//printf ("$2006 = %d, sl=%d, c=%d\n", data, ScanlineNo, nes->cpu->GetTotalCycles());
			if( !PPU56Toggle ) {
			// First write
				// t:0011111100000000=d:00111111
				// t:1100000000000000=0
				loopy_t = (loopy_t & 0x00FF)|((((WORD)data) & 0x3F)<<8);
			} else {
			// Second write
				// t:0000000011111111=d:11111111
				loopy_t = (loopy_t & 0xFF00)|(WORD)data;
				// v=t
				loopy_v = loopy_t;

				nes->mapper->PPU_Latch( loopy_v );
			}
			PPU56Toggle = !PPU56Toggle;
			break;

		case	0x2007: // VRAM I/O Register(RW)
			WORD	vaddr = loopy_v & 0x3FFF;
			if( PPUREG[0] & PPU_INC32_BIT ) loopy_v+=32;
			else				loopy_v++;

			if( vaddr >= 0x3000 ) {
				if( vaddr >= 0x3F00 ) {
					data &= 0x3F;
					if( bVSMode && nVSColorMap != -1 ) {
						BYTE	temp = VSColorMap[nVSColorMap][data];
						if( temp != 0xFF ) {
							data = temp & 0x3F;
						}
					}

					if( !(vaddr&0x000F) ) {
						BGPAL[0] = SPPAL[0] = data;
					} else if( !(vaddr&0x0010) ) {
						BGPAL[vaddr&0x000F] = data;
					} else {
						SPPAL[vaddr&0x000F] = data;
					}
					PAL_Changed = true;
					BGPAL[0x04] = BGPAL[0x08] = BGPAL[0x0C] = BGPAL[0x00];
					SPPAL[0x00] = SPPAL[0x04] = SPPAL[0x08] = SPPAL[0x0C] = BGPAL[0x00];
					return;
				}
				vaddr &= 0xEFFF;
			}
			if( PPU_MEM_TYPE[vaddr>>10] != BANKTYPE_VROM ) {
				PPU_MEM_BANK[vaddr>>10][vaddr&0x03FF] = data;
			}
			break;
	}
}

void	PPU::DMA( BYTE data )
{
WORD	addr = data<<8;

	for( INT i = 0; i < 256; i++ ) {
		SPRAM[i] = nes->Read( addr+i );
	}
}

void	PPU::VBlankStart()
{
	PPUREG[2] |= PPU_VBLANK_FLAG;
//	PPUREG[2] |= PPU_SPHIT_FLAG;	// VBlank�˓����ɕK��ON�H
}

void	PPU::VBlankEnd()
{
	PPUREG[2] &= ~PPU_VBLANK_FLAG;
	// VBlank�E�o���ɃN���A������
	// �G�L�T�C�g�o�C�N�ŏd�v
	PPUREG[2] &= ~PPU_SPHIT_FLAG;
}

void	PPU::FrameStart()
{
	if( PPUREG[1] & (PPU_SPDISP_BIT|PPU_BGDISP_BIT) ) {
		loopy_v = loopy_t;
		loopy_shift = loopy_x;
		loopy_y = (loopy_v&0x7000)>>12;
	}

	//if( lpScreen ) {
	//	::memset( lpScreen, 0x3F, SCREEN_WIDTH*sizeof(BYTE) );
	//}
	if( lpColormode ) {
		lpColormode[0] = 0;
	}
}

void	PPU::FrameEnd()
{
}

void	PPU::SetRenderScanline( INT scanline )
{
	ScanlineNo = scanline;
	if( scanline < 240 ) {
		//lpScanline = lpScreen+SCREEN_WIDTH*scanline;
	}
}

void	PPU::ScanlineStart()
{
	if( PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) {
		loopy_v = (loopy_v & 0xFBE0)|(loopy_t & 0x041F);
		loopy_shift = loopy_x;
		loopy_y = (loopy_v&0x7000)>>12;
		nes->mapper->PPU_Latch( 0x2000 + (loopy_v & 0x0FFF) );
	}

	int ptr = (PPU_UPDATE_QUEUE_WPTR - 1) & (PPU_UPDATE_QUEUE_SIZE - 1);
	currentQ = &PPU_UPDATE_QUEUE[ptr];
	PPU_UPDATE_QUEUE_RPTR = PPU_UPDATE_QUEUE_WPTR;
	//printf ("SLSTART: %d %d\n", PPU_UPDATE_QUEUE_RPTR, PPU_UPDATE_QUEUE_WPTR);

	//if (nes->GetScanline() >= 48 && nes->GetScanline() <= 50)
	//	printf ("  SLSTART: loopy_v = %x, loopy_y = %x @ %d\n", loopy_v, loopy_y, nes->GetScanline());
}

void	PPU::ScanlineNext()
{
	if( PPUREG[1] & (PPU_BGDISP_BIT|PPU_SPDISP_BIT) ) {
		if( (loopy_v & 0x7000) == 0x7000 ) {
			loopy_v &= 0x8FFF;
			if( (loopy_v & 0x03E0) == 0x03A0 ) {
				loopy_v ^= 0x0800;
				loopy_v &= 0xFC1F;
			} else {
				if( (loopy_v & 0x03E0) == 0x03E0 ) {
					loopy_v &= 0xFC1F;
				} else {
					loopy_v += 0x0020;
				}
			}
		} else {
			loopy_v += 0x1000;
		}
		loopy_y = (loopy_v&0x7000)>>12;
	}

	//if (nes->GetScanline() >= 48 && nes->GetScanline() <= 50)
	//	printf ("  SLNEXT: loopy_v = %x, loopy_y = %x @ %d\n", loopy_v, loopy_y, nes->GetScanline());
}

#include "palette.h"

u32 bgPalette[16];
u32 sprPalette[16];
BYTE	BGwrite[33+1];


void	PPU::Scanline( INT scanline, BOOL bMax, BOOL bLeftClip )
{
	//BYTE	BGmono[33+1];

	t3dsStartTiming(30, "Scanline:BG");
	ZEROMEMORY( BGwrite, sizeof(BGwrite) );
	//ZEROMEMORY( BGmono, sizeof(BGmono) );

	// Linecolor mode
	//lpColormode[scanline] = ((PPUREG[1]&PPU_BGCOLOR_BIT)>>5)|((PPUREG[1]&PPU_COLORMODE_BIT)<<7);

	// Deemphasis / Grayscale (4-bits)
	//   0000rgbx   (x = grayscale)
	//    
	int deemph = ((PPUREG[1]&PPU_BGCOLOR_BIT)>>4) | ((PPUREG[1]&PPU_COLORMODE_BIT));

	if (DEEMPH_Previous != deemph || PAL_Changed)
	{
		for (int i = 0; i < 16; i++)
			bgPalette[i] = rgbaPalette[deemph][BGPAL[i]];
		for (int i = 0; i < 16; i++)
			sprPalette[i] = rgbaPalette[deemph][SPPAL[i]];

		DEEMPH_Previous = deemph;
		PAL_Changed = 0;
	}

	u32 *pScnRGBA = &lpScreen32[512 * scanline];

	#define COPY_BG_TILES \
		pScnRGBA[0] = pScnRGBA[-8]; \
		pScnRGBA[1] = pScnRGBA[-7]; \
		pScnRGBA[2] = pScnRGBA[-6]; \
		pScnRGBA[3] = pScnRGBA[-5]; \
		pScnRGBA[4] = pScnRGBA[-4]; \
		pScnRGBA[5] = pScnRGBA[-3]; \
		pScnRGBA[6] = pScnRGBA[-2]; \
		pScnRGBA[7] = pScnRGBA[-1]; \

	#define RENDER_BG_TILES \
		register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA); \
		register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA); \
		u32 *tilePalette = &bgPalette[attr]; \
		pScnRGBA[0] = tilePalette[(c1>>6)]; \
		pScnRGBA[1] = tilePalette[(c2>>6)]; \
		pScnRGBA[2] = tilePalette[(c1>>4)&3]; \
		pScnRGBA[3] = tilePalette[(c2>>4)&3]; \
		pScnRGBA[4] = tilePalette[(c1>>2)&3]; \
		pScnRGBA[5] = tilePalette[(c2>>2)&3]; \
		pScnRGBA[6] = tilePalette[c1&3]; \
		pScnRGBA[7] = tilePalette[c2&3]; \


	//if (scanline == 1)
	//	printf ("bExtLatch=%d, bExtNameTable=%d, R=%d\n", bExtLatch, bExtNameTable, nes->GetRenderMethod());

	// Render BG
	if( !(PPUREG[1]&PPU_BGDISP_BIT) ) 
	{
		//::memset( lpScanline, BGPAL[0], SCREEN_WIDTH );
		for (int i = 0; i < 256 + 8; i++)
			pScnRGBA[i] = bgPalette[0];

		if( nes->GetRenderMethod() == NES::TILE_RENDER ) {
			nes->EmulationCPU( FETCH_CYCLES*4*32 );
		}

	} else {
		if( nes->GetRenderMethod() != NES::TILE_RENDER ) {
			if( !bExtLatch ) {
				// Without Extension Latch
				//LPBYTE	pScn = lpScanline+(8-loopy_shift);
				LPBYTE	pBGw = BGwrite;
				pScnRGBA += (8-loopy_shift);


				INT	tileofs = (PPUREG[0] & PPU_BGTBL_BIT)<<8;
				//INT tileofs = 0;
				INT	ntbladr = 0x2000+(loopy_v&0x0FFF);
				INT	attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
				INT	ntbl_x  = ntbladr&0x001F;
				INT	attrsft = (ntbladr&0x0040)>>4;

				LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];

				if (nes->GetRenderMethod() == NES::POST_RENDER)
				{
					pNTBL = currentQ->PPU_MEM_BANK[ntbladr>>10];
					tileofs = (currentQ->PPUREG & PPU_BGTBL_BIT)<<8;
				}
				//LPBYTE	pNTBL = 0;

				INT	tileadr;
				INT	cache_tile = 0xFFFF0000;
				BYTE	cache_attr = 0xFF;

				BYTE	chr_h, chr_l, attr;

				attradr &= 0x3FF;

				for( INT i = 0; i < 33; i++ ) 
				{
					// This allows mid-scanline CHR bank switching
					//
					if (nes->GetRenderMethod() == NES::POST_RENDER)
					{
						TPPU_UPDATE_QUEUE *nextQ = &PPU_UPDATE_QUEUE[PPU_UPDATE_QUEUE_RPTR];
						
						if (PPU_UPDATE_QUEUE_RPTR != PPU_UPDATE_QUEUE_WPTR && 
							nextQ->TILE_NO == i)
						{
							currentQ = nextQ;
							PPU_UPDATE_QUEUE_RPTR = (PPU_UPDATE_QUEUE_RPTR + 1) & (PPU_UPDATE_QUEUE_SIZE - 1);

							tileofs = (currentQ->PPUREG & PPU_BGTBL_BIT)<<8;
							pNTBL = currentQ->PPU_MEM_BANK[ntbladr>>10];
							cache_tile = 0;
							//if (scanline < 24)
							//printf ("  R: %3d, %2d / %2d %2d\n", scanline, i, PPU_UPDATE_QUEUE_RPTR, PPU_UPDATE_QUEUE_WPTR);
								
						}
					}

					tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;
					attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;

					if( cache_tile == tileadr && cache_attr == attr ) {
						COPY_BG_TILES
						*(pBGw+0) = *(pBGw-1);
					} else {
						cache_tile = tileadr;
						cache_attr = attr;
						chr_l = currentQ->PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
						chr_h = currentQ->PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
						*pBGw = chr_h|chr_l;

						LPBYTE	pBGPAL = &BGPAL[attr];
						{
							RENDER_BG_TILES
						}
					}
					//pScn+=8;
					pScnRGBA+=8;
					pBGw++;

					// Character latch(For MMC2/MMC4)
					if( bChrLatch ) {
						nes->mapper->PPU_ChrLatch( tileadr );
					}

					if( ++ntbl_x == 32 ) {
						ntbl_x = 0;
						ntbladr ^= 0x41F;
						attradr = 0x03C0+((ntbladr&0x0380)>>4);
						pNTBL = currentQ->PPU_MEM_BANK[ntbladr>>10];
					} else {
						ntbladr++;
					}
				}
			} else {
				// With Extension Latch(For MMC5)
				//LPBYTE	pScn = lpScanline+(8-loopy_shift);
				LPBYTE	pBGw = BGwrite;
				pScnRGBA += (8-loopy_shift);

				INT	ntbladr = 0x2000+(loopy_v&0x0FFF);
				INT	ntbl_x  = ntbladr & 0x1F;

				INT	cache_tile = 0xFFFF0000;
				BYTE	cache_attr = 0xFF;

				BYTE	chr_h, chr_l, attr, exattr;

				for( INT i = 0; i < 33; i++ ) {
					nes->mapper->PPU_ExtLatchX( i );
					nes->mapper->PPU_ExtLatch( ntbladr, chr_l, chr_h, exattr );
					attr = exattr&0x0C;

					if( cache_tile != (((INT)chr_h<<8)+(INT)chr_l) || cache_attr != attr ) {
						cache_tile = (((INT)chr_h<<8)+(INT)chr_l);
						cache_attr = attr;
						*pBGw = chr_h|chr_l;

						LPBYTE	pBGPAL = &BGPAL[attr];
						{
							RENDER_BG_TILES
						}
					} else {
						COPY_BG_TILES
						*(pBGw+0) = *(pBGw-1);
					}
					//pScn+=8;
					pScnRGBA+=8;
					pBGw++;

					if( ++ntbl_x == 32 ) {
						ntbl_x = 0;
						ntbladr ^= 0x41F;
					} else {
						ntbladr++;
					}
				}
			}
		} else {
			if( !bExtLatch ) {
				// Without Extension Latch
				if( !bExtNameTable ) {
					//LPBYTE	pScn = lpScanline+(8-loopy_shift);
					LPBYTE	pBGw = BGwrite;
					pScnRGBA += (8-loopy_shift);

					INT	ntbladr = 0x2000+(loopy_v&0x0FFF);
					INT	attradr = 0x03C0+((loopy_v&0x0380)>>4);
					INT	ntbl_x  = ntbladr&0x001F;
					INT	attrsft = (ntbladr&0x0040)>>4;
					LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];

					INT	tileadr;
					INT	cache_tile = 0xFFFF0000;
					BYTE	cache_attr = 0xFF;
					BYTE	cache_mono = 0x00;

					BYTE	chr_h, chr_l, attr;

					for( INT i = 0; i < 33; i++ ) {
						tileadr = ((PPUREG[0]&PPU_BGTBL_BIT)<<8)+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

						if( i != 0 ) {
							nes->EmulationCPU( FETCH_CYCLES*4 );
						}

						attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;

						if( cache_tile != tileadr || cache_attr != attr ) {
							cache_tile = tileadr;
							cache_attr = attr;

							chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
							chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
							*pBGw = chr_l|chr_h;

							LPBYTE	pBGPAL = &BGPAL[attr];
							{
								RENDER_BG_TILES
							}
						} else {
							COPY_BG_TILES
							*(pBGw+0) = *(pBGw-1);
						}
						//pScn+=8;
						pScnRGBA+=8;
						pBGw++;

						// Character latch(For MMC2/MMC4)
						if( bChrLatch ) {
							nes->mapper->PPU_ChrLatch( tileadr );
						}

						if( ++ntbl_x == 32 ) {
							ntbl_x = 0;
							ntbladr ^= 0x41F;
							attradr = 0x03C0+((ntbladr&0x0380)>>4);
							pNTBL = PPU_MEM_BANK[ntbladr>>10];
						} else {
							ntbladr++;
						}
					}
				} else {
					//LPBYTE	pScn = lpScanline+(8-loopy_shift);
					LPBYTE	pBGw = BGwrite;
					pScnRGBA += (8-loopy_shift);

					INT	ntbladr;
					INT	tileadr;
					INT	cache_tile = 0xFFFF0000;
					BYTE	cache_attr = 0xFF;
					BYTE	cache_mono = 0x00;

					BYTE	chr_h, chr_l, attr;

					WORD	loopy_v_tmp = loopy_v;

					for( INT i = 0; i < 33; i++ ) {
						if( i != 0 ) {
							nes->EmulationCPU( FETCH_CYCLES*4 );
						}

						ntbladr = 0x2000+(loopy_v&0x0FFF);
						tileadr = ((PPUREG[0]&PPU_BGTBL_BIT)<<8)+PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+((loopy_v&0x7000)>>12);
						attr = ((PPU_MEM_BANK[ntbladr>>10][0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2)]>>(((ntbladr&0x40)>>4)+(ntbladr&0x02)))&3)<<2;

						if( cache_tile != tileadr || cache_attr != attr ) {
							cache_tile = tileadr;
							cache_attr = attr;

							chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
							chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
							*pBGw = chr_l|chr_h;

							LPBYTE	pBGPAL = &BGPAL[attr];
							{
								RENDER_BG_TILES
							}
						} else {
							COPY_BG_TILES
							*(pBGw+0) = *(pBGw-1);
						}
						//pScn+=8;
						pScnRGBA+=8;
						pBGw++;

						// Character latch(For MMC2/MMC4)
						if( bChrLatch ) {
							nes->mapper->PPU_ChrLatch( tileadr );
						}

						if( (loopy_v & 0x1F) == 0x1F ) {
							loopy_v ^= 0x041F;
						} else {
							loopy_v++;
						}
					}
					loopy_v = loopy_v_tmp;
				}
			} else {
				// With Extension Latch(For MMC5)
				//LPBYTE	pScn = lpScanline+(8-loopy_shift);
				LPBYTE	pBGw = BGwrite;
				pScnRGBA += (8-loopy_shift);

				INT	ntbladr = 0x2000+(loopy_v&0x0FFF);
				INT	ntbl_x  = ntbladr & 0x1F;

				INT	cache_tile = 0xFFFF0000;
				BYTE	cache_attr = 0xFF;

				BYTE	chr_h, chr_l, attr, exattr;

				for( INT i = 0; i < 33; i++ ) {
					if( i != 0 ) {
						nes->EmulationCPU( FETCH_CYCLES*4 );
					}
					nes->mapper->PPU_ExtLatchX( i );
					nes->mapper->PPU_ExtLatch( ntbladr, chr_l, chr_h, exattr );
					attr = exattr&0x0C;

					if( cache_tile != (((INT)chr_h<<8)+(INT)chr_l) || cache_attr != attr ) {
						cache_tile = (((INT)chr_h<<8)+(INT)chr_l);
						cache_attr = attr;
						*pBGw = chr_l|chr_h;

						LPBYTE	pBGPAL   = &BGPAL[attr];
						{
							RENDER_BG_TILES
						}
					} else {
						COPY_BG_TILES
						*(pBGw+0) = *(pBGw-1);
					}
					//pScn+=8;
					pScnRGBA+=8;
					pBGw++;

					if( ++ntbl_x == 32 ) {
						ntbl_x = 0;
						ntbladr ^= 0x41F;
					} else {
						ntbladr++;
					}
				}
			}
		}

		if( !(PPUREG[1]&PPU_BGCLIP_BIT) && bLeftClip ) {
			//LPBYTE	pScn = lpScanline+8;
			//for( INT i = 0; i < 8; i++ ) {
			//	pScn[i] = BGPAL[0];
			//}
			u32 *pScnRGBA = &lpScreen32[512 * scanline];
			uint32 pal0 = bgPalette[0];
			for (int i = 0; i < 8; i++)
				pScnRGBA[i+8] = pal0;
		}
	}

	// Render sprites
	PPUREG[2] &= ~PPU_SPMAX_FLAG;

	t3dsEndTiming(30);

	// �\�����ԊO�ł����΃L�����Z��
	if( scanline > 239 )
	{
		return;
	}

	if( !(PPUREG[1]&PPU_SPDISP_BIT) ) {
		return;
	}

	t3dsStartTiming(31, "Scanline:OBJ");

	BYTE	SPwrite[33+1];
	INT	spmax;
	INT	spraddr, sp_y, sp_h;
	BYTE	chr_h, chr_l;
	LPSPRITE sp;

	LPBYTE	pBGw = BGwrite;
	LPBYTE	pSPw = SPwrite;
	LPBYTE	pBit2Rev = Bit2Rev;


	// Construct a list of sprite indexes that are visible 
	// in this line
	//
	INT		spriteCount = 0;
	sp = (LPSPRITE)SPRAM;
	sp_h = (PPUREG[0]&PPU_SP16_BIT)?15:7;
	INT		visibleSprites[64];
	for( INT i = 0; i < 64; i++, sp++ ) {
		sp_y = scanline - (sp->y+1);
		if( sp_y != (sp_y & sp_h) )
			continue;
		
		//visibleSprites[spriteCount++] = i;
		spriteCount = 1;
		break;
	}

	if (spriteCount > 0)
	{

	ZEROMEMORY( SPwrite, sizeof(SPwrite) );

	spmax = 0;
	sp = (LPSPRITE)SPRAM;
	sp_h = (PPUREG[0]&PPU_SP16_BIT)?15:7;

	// Left clip
	if( !(PPUREG[1]&PPU_SPCLIP_BIT) && bLeftClip ) {
		SPwrite[0] = 0xFF;
	}

	// Modify this loop to use the sprite index list
	//
	for( INT i = 0; i < 64; i++, sp++ ) {
	//for (INT sprite = 0; sprite < spriteCount; sprite++) {
	//	INT i = visibleSprites[sprite];
	//	sp = (LPSPRITE) &SPRAM[i * 4];

		sp_y = scanline - (sp->y+1);
		// �X�L�������C������SPRITE�����݂��邩���`�F�b�N
		if( sp_y != (sp_y & sp_h) )
			continue;

		if( !(PPUREG[0]&PPU_SP16_BIT) ) {
		// 8x8 Sprite
			spraddr = (((INT)PPUREG[0]&PPU_SPTBL_BIT)<<9)+((INT)sp->tile<<4);
			if( !(sp->attr&SP_VMIRROR_BIT) )
				spraddr += sp_y;
			else
				spraddr += 7-sp_y;
		} else {
		// 8x16 Sprite
			spraddr = (((INT)sp->tile&1)<<12)+(((INT)sp->tile&0xFE)<<4);
			if( !(sp->attr&SP_VMIRROR_BIT) )
				spraddr += ((sp_y&8)<<1)+(sp_y&7);
			else
				spraddr += ((~sp_y&8)<<1)+(7-(sp_y&7));
		}
		// Character pattern
		chr_l = PPU_MEM_BANK[spraddr>>10][ spraddr&0x3FF   ];
		chr_h = PPU_MEM_BANK[spraddr>>10][(spraddr&0x3FF)+8];

		// Character latch(For MMC2/MMC4)
		if( bChrLatch ) {
			nes->mapper->PPU_ChrLatch( spraddr );
		}

		// pattern mask
		if( sp->attr&SP_HMIRROR_BIT ) {
			chr_l = pBit2Rev[chr_l];
			chr_h = pBit2Rev[chr_h];
		}
		BYTE	SPpat = chr_l|chr_h;

		// Sprite hitcheck
		if( i == 0 && !(PPUREG[2]&PPU_SPHIT_FLAG) ) {
			INT	BGpos = ((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3;
			INT	BGsft = 8-((loopy_shift+sp->x)&7);
			BYTE	BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

			if( SPpat & BGmsk ) {
				PPUREG[2] |= PPU_SPHIT_FLAG;
			}
		}

		// Sprite mask
		INT	SPpos = sp->x/8;
		INT	SPsft = 8-(sp->x&7);
		BYTE	SPmsk = (((WORD)pSPw[SPpos+0]<<8)|(WORD)pSPw[SPpos+1])>>SPsft;
		WORD	SPwrt = (WORD)SPpat<<SPsft;
		pSPw[SPpos+0] |= SPwrt >> 8;
		pSPw[SPpos+1] |= SPwrt & 0xFF;
		SPpat &= ~SPmsk;

		if( sp->attr&SP_PRIORITY_BIT ) 
		{
		// BG > SP priority
			INT	BGpos = ((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3;
			INT	BGsft = 8-((loopy_shift+sp->x)&7);
			BYTE	BGmsk = (((WORD)pBGw[BGpos+0]<<8)|(WORD)pBGw[BGpos+1])>>BGsft;

			SPpat &= ~BGmsk;
		}

		// Attribute
		LPBYTE	pSPPAL = &SPPAL[(sp->attr&SP_COLOR_BIT)<<2];
		// Ptr
		//LPBYTE	pScn   = lpScanline+sp->x+8;

		u32     *objPalette = &sprPalette[(sp->attr&SP_COLOR_BIT)<<2];
		u32 	*pScnRGBA = &lpScreen32[512 * scanline + sp->x + 8];

		if( !bExtMono ) {
			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA); \
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA); \
			/*
			if( SPpat&0x80 ) pScn[0] = pSPPAL[(c1>>6)];
			if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3];
			if( SPpat&0x40 ) pScn[1] = pSPPAL[(c2>>6)];
			if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3];
			if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3];
			if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3];
			if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3];
			if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3];
			*/
			/*
			#define RENDER_SPRITE_NIBBLE \
			{ \
				if( (SPpat >> 4)&0x80 ) pScnRGBA[0] = objPalette[(c1>>6)]; \
				if( (SPpat >> 4)&0x40 ) pScnRGBA[1] = objPalette[(c2>>6)]; \
				if( (SPpat >> 4)&0x20 ) pScnRGBA[2] = objPalette[(c1>>4)&3]; \ 
				if( (SPpat >> 4)&0x10 ) pScnRGBA[3] = objPalette[(c2>>4)&3]; \
				break; \
			} 

			switch ((SPpat >> 4) & 0xf)
			{
				case 0: RENDER_SPRITE_NIBBLE
				case 1: RENDER_SPRITE_NIBBLE
				case 2: RENDER_SPRITE_NIBBLE
				case 3: RENDER_SPRITE_NIBBLE
				case 4: RENDER_SPRITE_NIBBLE
				case 5: RENDER_SPRITE_NIBBLE
				case 6: RENDER_SPRITE_NIBBLE
				case 7: RENDER_SPRITE_NIBBLE
				case 8: RENDER_SPRITE_NIBBLE
				case 9: RENDER_SPRITE_NIBBLE
				case 10: RENDER_SPRITE_NIBBLE
				case 11: RENDER_SPRITE_NIBBLE
				case 12: RENDER_SPRITE_NIBBLE
				case 13: RENDER_SPRITE_NIBBLE
				case 14: RENDER_SPRITE_NIBBLE
				case 15: RENDER_SPRITE_NIBBLE
			}

			#define RENDER_SPRITE_NIBBLE \
			{ \
				if( SPpat&0x08 ) pScnRGBA[4] = objPalette[(c1>>2)&3]; \
				if( SPpat&0x04 ) pScnRGBA[5] = objPalette[(c2>>2)&3]; \
				if( SPpat&0x02 ) pScnRGBA[6] = objPalette[c1&3]; \
				if( SPpat&0x01 ) pScnRGBA[7] = objPalette[c2&3]; \
				break; \
			} \

			switch (SPpat & 0xf)
			{
				case 0: RENDER_SPRITE_NIBBLE
				case 1: RENDER_SPRITE_NIBBLE
				case 2: RENDER_SPRITE_NIBBLE
				case 3: RENDER_SPRITE_NIBBLE
				case 4: RENDER_SPRITE_NIBBLE
				case 5: RENDER_SPRITE_NIBBLE
				case 6: RENDER_SPRITE_NIBBLE
				case 7: RENDER_SPRITE_NIBBLE
				case 8: RENDER_SPRITE_NIBBLE
				case 9: RENDER_SPRITE_NIBBLE
				case 10: RENDER_SPRITE_NIBBLE
				case 11: RENDER_SPRITE_NIBBLE
				case 12: RENDER_SPRITE_NIBBLE
				case 13: RENDER_SPRITE_NIBBLE
				case 14: RENDER_SPRITE_NIBBLE
				case 15: RENDER_SPRITE_NIBBLE

			}
			*/
			
			if (SPpat & 0xf0)
			{
				if (SPpat & 0xc0)
				{
					if( SPpat&0x80 ) pScnRGBA[0] = objPalette[(c1>>6)];
					if( SPpat&0x40 ) pScnRGBA[1] = objPalette[(c2>>6)];
				}
				if (SPpat & 0x30)
				{
					if( SPpat&0x20 ) pScnRGBA[2] = objPalette[(c1>>4)&3];
					if( SPpat&0x10 ) pScnRGBA[3] = objPalette[(c2>>4)&3];
				}
			}
			if (SPpat & 0x0f)
			{
				if (SPpat & 0x0c)
				{
					if( SPpat&0x08 ) pScnRGBA[4] = objPalette[(c1>>2)&3];
					if( SPpat&0x04 ) pScnRGBA[5] = objPalette[(c2>>2)&3];
				}
				if (SPpat & 0x03)
				{
				if( SPpat&0x02 ) pScnRGBA[6] = objPalette[c1&3];
				if( SPpat&0x01 ) pScnRGBA[7] = objPalette[c2&3];
				}
			}
			
		} 
		else 
		{
		// Monocrome effect (for Final Fantasy)
			//BYTE	mono = BGmono[((sp->x&0xF8)+((loopy_shift+(sp->x&7))&8))>>3];

			register INT	c1 = ((chr_l>>1)&0x55)|(chr_h&0xAA); \
			register INT	c2 = (chr_l&0x55)|((chr_h<<1)&0xAA); \
			/*
			if( SPpat&0x80 ) pScn[0] = pSPPAL[c1>>6]	|mono;
			if( SPpat&0x08 ) pScn[4] = pSPPAL[(c1>>2)&3]	|mono;
			if( SPpat&0x40 ) pScn[1] = pSPPAL[c2>>6]	|mono;
			if( SPpat&0x04 ) pScn[5] = pSPPAL[(c2>>2)&3]	|mono;
			if( SPpat&0x20 ) pScn[2] = pSPPAL[(c1>>4)&3]	|mono;
			if( SPpat&0x02 ) pScn[6] = pSPPAL[c1&3]		|mono;
			if( SPpat&0x10 ) pScn[3] = pSPPAL[(c2>>4)&3]	|mono;
			if( SPpat&0x01 ) pScn[7] = pSPPAL[c2&3]		|mono;*/
			if( SPpat&0x80 ) pScnRGBA[0] = objPalette[(c1>>6)];
			if( SPpat&0x40 ) pScnRGBA[1] = objPalette[(c2>>6)];
			if( SPpat&0x20 ) pScnRGBA[2] = objPalette[(c1>>4)&3];
			if( SPpat&0x10 ) pScnRGBA[3] = objPalette[(c2>>4)&3];
			if( SPpat&0x08 ) pScnRGBA[4] = objPalette[(c1>>2)&3];
			if( SPpat&0x04 ) pScnRGBA[5] = objPalette[(c2>>2)&3];
			if( SPpat&0x02 ) pScnRGBA[6] = objPalette[c1&3];
			if( SPpat&0x01 ) pScnRGBA[7] = objPalette[c2&3];
		}

		if( ++spmax > 8-1 ) {
			if( !bMax )
				break;
		}
	}
	if( spmax > 8-1 ) {
		PPUREG[2] |= PPU_SPMAX_FLAG;
	}
	}

	t3dsEndTiming(31);	
}

// �X�v���C�g�O���q�b�g���邩���m���Ȃ����C���H
BOOL	PPU::IsSprite0( INT scanline )
{
	// �X�v���C�gorBG���\���̓L�����Z��(�q�b�g���Ȃ�)
	if( (PPUREG[1]&(PPU_SPDISP_BIT|PPU_BGDISP_BIT)) != (PPU_SPDISP_BIT|PPU_BGDISP_BIT) )
		return	FALSE;

	// ���Ƀq�b�g���Ă������L�����Z��
	if( PPUREG[2]&PPU_SPHIT_FLAG )
		return	FALSE;

	if( !(PPUREG[0]&PPU_SP16_BIT) ) {
	// 8x8
		if( (scanline < (INT)SPRAM[0]+1) || (scanline > ((INT)SPRAM[0]+7+1)) )
			return	FALSE;
	} else {
	// 8x16
		if( (scanline < (INT)SPRAM[0]+1) || (scanline > ((INT)SPRAM[0]+15+1)) )
			return	FALSE;
	}

	return	TRUE;
}

void	PPU::DummyScanline( INT scanline )
{
INT	i;
INT	spmax;
INT	sp_h;
LPSPRITE sp;

	PPUREG[2] &= ~PPU_SPMAX_FLAG;

	// �X�v���C�g���\���̓L�����Z��
	if( !(PPUREG[1]&PPU_SPDISP_BIT) )
		return;

	// �\�����ԊO�ł����΃L�����Z��
	if( scanline < 0 || scanline > 239 )
		return;

	sp = (LPSPRITE)SPRAM;
	sp_h = (PPUREG[0]&PPU_SP16_BIT)?15:7;

	spmax = 0;
	// Sprite Max check
	for( i = 0; i < 64; i++, sp++ ) {
		// �X�L�������C������SPRITE�����݂��邩���`�F�b�N
		if( (scanline < (INT)sp->y+1) || (scanline > ((INT)sp->y+sp_h+1)) ) {
			continue;
		}

		if( ++spmax > 8-1 ) {
			PPUREG[2] |= PPU_SPMAX_FLAG;
			break;
		}
	}
}

