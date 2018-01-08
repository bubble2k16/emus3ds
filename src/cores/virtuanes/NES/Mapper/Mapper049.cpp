//////////////////////////////////////////////////////////////////////////
// Mapper120  Tobidase Daisakusen                                       //
//////////////////////////////////////////////////////////////////////////
#include "BoardMMC3.h"

void	Mapper049::Reset()
{
//	nes->ppu->SetVromWrite(1);
	MMC3_RegReset();
	temp = 0;
//	nes->ppu->SetExtLatchMode( TRUE );
}

void	Mapper049::WriteLow( WORD addr, BYTE data )
{
	DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if ((addr&0x4100)==0x4100) {
		temp=data;
//		temp &= 0x03;
//		if		( temp == 0 )	SetVRAM_Mirror( VRAM_VMIRROR );
//		else if ( temp == 1 )	SetVRAM_Mirror( VRAM_HMIRROR );
//		else if ( temp == 2 )	SetVRAM_Mirror( VRAM_MIRROR4L );
//		else					SetVRAM_Mirror( VRAM_MIRROR4H );
		FixCHR(MMC3_cmd);
	}
}

void	Mapper049::Write( WORD addr, BYTE data )
{
//	DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	MMC3CMDWrite(addr, data);
	MMC3_IRQWrite(addr, data);

	if((addr&0xE001)==0xE000) nes->cpu->ClrIRQ(IRQ_MAPPER);
}

void	Mapper049::MMC3CMDWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
	case 0x8000:
		if ((V & 0x40) != (MMC3_cmd & 0x40))
			FixMMC3PRG(V);
		if ((V & 0x80) != (MMC3_cmd & 0x80))
			FixCHR(V);
		MMC3_cmd = V;
		break;
	case 0x8001:
	{
		int cbase = (MMC3_cmd & 0x80) << 5;
		MMC3_DRegBuf[MMC3_cmd & 0x7] = V;
		switch (MMC3_cmd & 0x07) {
		case 0:
			wrapc((cbase ^ 0x000), V & (~1));
			wrapc((cbase ^ 0x400), V | 1);
			break;
		case 1:
			wrapc((cbase ^ 0x800), V & (~1));
			wrapc((cbase ^ 0xC00), V | 1);
			break;
		case 2:
			wrapc(cbase ^ 0x1000, V);
			break;
		case 3:
			wrapc(cbase ^ 0x1400, V);
			break;
		case 4:
			wrapc(cbase ^ 0x1800, V);
			break;
		case 5:
			wrapc(cbase ^ 0x1C00, V);
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

void	Mapper049::wrapc(WORD A, BYTE V)
{
	 if(temp&2)
		SetCRAM_8K_Bank(0);
	 else
		SetVROM_1K_Bank(A>>10,V);
}

void	Mapper049::FixCHR(BYTE V)
{
	int cbase = (V & 0x80) << 5;
	wrapc((cbase ^ 0x000), MMC3_DRegBuf[0] & (~1));
	wrapc((cbase ^ 0x400), MMC3_DRegBuf[0] | 1);
	wrapc((cbase ^ 0x800), MMC3_DRegBuf[1] & (~1));
	wrapc((cbase ^ 0xC00), MMC3_DRegBuf[1] | 1);
	wrapc(cbase ^ 0x1000, MMC3_DRegBuf[2]);
	wrapc(cbase ^ 0x1400, MMC3_DRegBuf[3]);
	wrapc(cbase ^ 0x1800, MMC3_DRegBuf[4]);
	wrapc(cbase ^ 0x1c00, MMC3_DRegBuf[5]);

	mwrap(MMC3_A000B);
}

void	Mapper049::HSync( INT scanline )
{
	if ((scanline >= 0 && scanline <= 239)) {
		if (nes->ppu->IsDispON())
		{
			if (MMC3_IRQa && !MMC3_IRQReload) {
				if (scanline == 0) {
					if (MMC3_IRQCount) {
						MMC3_IRQCount -= 1;
					}
				}
				if (!(MMC3_IRQCount)){
					MMC3_IRQReload = 0xFF;
					MMC3_IRQCount = MMC3_IRQLatch;
					nes->cpu->SetIRQ(IRQ_MAPPER);
				}
				MMC3_IRQCount--;
			}
		}
	}
}

void	Mapper049::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	INT loopy_v = nes->ppu->GetPPUADDR();
	INT loopy_y = nes->ppu->GetTILEY();
	INT tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT attrsft = (ntbladr&0x0040)>>4;
	LPBYTE pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT ntbl_x  = ntbladr&0x001F;
	INT tileadr, ntb;

	ntb = (ntbladr>>10)&3;

	if(ntb==2)
	tileofs |= 0x1000;
	else if(ntb && temp)
	tileofs |= 0x1000;
	else
	tileofs |= 0x0000;

	attradr &= 0x3FF;
	attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

	chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
}

void	Mapper049::PPU_Latch( WORD addr )
{
/*
	if(DirectInput.m_Sw[DIK_PAUSE]){
		nes->Dump_CRAM();
	}
*/
}
