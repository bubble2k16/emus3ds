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

extern	BYTE MMC3_cmd;
extern	BYTE MMC3_DRegBuf[8];
extern	BYTE MMC3_A000B, MMC3_A001B;
extern	BYTE MMC3_IRQCount, MMC3_IRQLatch, MMC3_IRQa, MMC3_IRQReload;

extern	void MMC3_RegReset();
extern	void pwrap(WORD A, BYTE V);
extern	void cwrap(WORD A, BYTE V);
extern	void mwrap(BYTE V);
extern	void FixMMC3PRG(BYTE V);
extern	void FixMMC3CHR(BYTE V);
extern	void MMC3_CMDWrite(WORD A, BYTE V);
extern	void MMC3_IRQWrite(WORD A, BYTE V);
extern	void MMC3_HSync(INT scanline);
