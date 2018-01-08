#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "DebugOut.h"
#include "typedef.h"
#include "macro.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"

#include "mapper.h"

#include "Config.h"

#include "BoardMMC3.h"

BYTE	MMC3_cmd;
BYTE	MMC3_DRegBuf[8];
BYTE	MMC3_A000B, MMC3_A001B;
BYTE    MMC3_IRQCount, MMC3_IRQLatch, MMC3_IRQa, MMC3_IRQReload;

// ------------------------- Generic MM3 Code ---------------------------

void MMC3_RegReset()
{
	MMC3_IRQReload = MMC3_IRQCount = MMC3_IRQLatch = MMC3_IRQa = 0;
	MMC3_cmd = MMC3_A000B = MMC3_A001B = 0;

	MMC3_DRegBuf[0] = 0;
	MMC3_DRegBuf[1] = 2;
	MMC3_DRegBuf[2] = 4;
	MMC3_DRegBuf[3] = 5;
	MMC3_DRegBuf[4] = 6;
	MMC3_DRegBuf[5] = 7;
	MMC3_DRegBuf[6] = 0;
	MMC3_DRegBuf[7] = 1;

	FixMMC3PRG(0);
	FixMMC3CHR(0);
}

void pwrap(WORD A, BYTE V)
{
	SetPROM_8K_Bank(A>>13, V&0x7F);
}

void cwrap(WORD A, BYTE V)
{
	if (VROM_1K_SIZE) SetVROM_1K_Bank(A>>10, V);
	else		  SetCRAM_1K_Bank(A>>10, V);
}

void mwrap(BYTE V)
{
//	if (!nes->rom->Is4SCREEN()) {
		if		( V == 0 )	SetVRAM_Mirror( VRAM_VMIRROR );
		else if ( V == 1 )	SetVRAM_Mirror( VRAM_HMIRROR );
		else if ( V == 2 )	SetVRAM_Mirror( VRAM_MIRROR4L );
		else				SetVRAM_Mirror( VRAM_MIRROR4H );
//	}
}

void FixMMC3PRG(BYTE V)
{
	if (V & 0x40) {
		pwrap(0xC000, MMC3_DRegBuf[6]);
		pwrap(0x8000, ~1);
	} else {
		pwrap(0x8000, MMC3_DRegBuf[6]);
		pwrap(0xC000, ~1);
	}
	pwrap(0xA000, MMC3_DRegBuf[7]);
	pwrap(0xE000, ~0);
}

void FixMMC3CHR(BYTE V)
{
	int cbase = (V & 0x80) << 5;
	cwrap((cbase ^ 0x000), MMC3_DRegBuf[0] & (~1));
	cwrap((cbase ^ 0x400), MMC3_DRegBuf[0] | 1);
	cwrap((cbase ^ 0x800), MMC3_DRegBuf[1] & (~1));
	cwrap((cbase ^ 0xC00), MMC3_DRegBuf[1] | 1);
	cwrap(cbase ^ 0x1000, MMC3_DRegBuf[2]);
	cwrap(cbase ^ 0x1400, MMC3_DRegBuf[3]);
	cwrap(cbase ^ 0x1800, MMC3_DRegBuf[4]);
	cwrap(cbase ^ 0x1c00, MMC3_DRegBuf[5]);

	mwrap(MMC3_A000B);
}

void MMC3_CMDWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
	case 0x8000:
		if ((V & 0x40) != (MMC3_cmd & 0x40))
			FixMMC3PRG(V);
		if ((V & 0x80) != (MMC3_cmd & 0x80))
			FixMMC3CHR(V);
		MMC3_cmd = V;
		break;
	case 0x8001:
	{
		int cbase = (MMC3_cmd & 0x80) << 5;
		MMC3_DRegBuf[MMC3_cmd & 0x7] = V;
		switch (MMC3_cmd & 0x07) {
		case 0:
			cwrap((cbase ^ 0x000), V & (~1));
			cwrap((cbase ^ 0x400), V | 1);
			break;
		case 1:
			cwrap((cbase ^ 0x800), V & (~1));
			cwrap((cbase ^ 0xC00), V | 1);
			break;
		case 2:
			cwrap(cbase ^ 0x1000, V);
			break;
		case 3:
			cwrap(cbase ^ 0x1400, V);
			break;
		case 4:
			cwrap(cbase ^ 0x1800, V);
			break;
		case 5:
			cwrap(cbase ^ 0x1C00, V);
			break;
		case 6:
			if (MMC3_cmd&0x40) pwrap(0xC000, V);
			else		   pwrap(0x8000, V);
			break;
		case 7:
			pwrap(0xA000, V);
			break;
		}
		break;
	}
	case 0xA000:
		MMC3_A000B = V;
		mwrap(MMC3_A000B);
		break;
	case 0xA001:
		MMC3_A001B = V;
		break;
	}
}

void MMC3_IRQWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
	case 0xC000: MMC3_IRQReload = 0;MMC3_IRQCount = V; break;
	case 0xC001: MMC3_IRQReload = 0;MMC3_IRQLatch = V; break;
	case 0xE000: MMC3_IRQReload = 0;MMC3_IRQa = 0; break;//nes->cpu->ClrIRQ(IRQ_MAPPER);
	case 0xE001: MMC3_IRQReload = 0;MMC3_IRQa = 1; break;
	}
}

void MMC3_HSync(INT scanline)
{
	if ((scanline >= 0 && scanline <= 239)) {
//		if (nes->ppu->IsDispON())
//		{
			if (MMC3_IRQa && !MMC3_IRQReload) {
				if (scanline == 0) {
					if (MMC3_IRQCount) {
						MMC3_IRQCount -= 1;
					}
				}
				if (!(MMC3_IRQCount)){
					MMC3_IRQReload = 0xFF;
					MMC3_IRQCount = MMC3_IRQLatch;
//					nes->cpu->SetIRQ(IRQ_MAPPER);
				}
				MMC3_IRQCount--;
			}
//		}
	}
}

// ----------------------------------------------------------------------
