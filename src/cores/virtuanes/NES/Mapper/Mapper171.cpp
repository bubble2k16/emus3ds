/*******************************************************************************
 * NES Mapper for BBK
 *
 *  Author:  <fanoble@yeah.net>
 *
 *  Create:   2014-06-24, by fanoble
 *******************************************************************************
 */

static const Mapper171::FDC_CMD_DESC FdcCmdTableB[32] =
{
	/* 0x00 */ { 1, 1, Mapper171::FdcNop },
	/* 0x01 */ { 1, 1, Mapper171::FdcNop },
	/* 0x02 */ { 9, 7, Mapper171::FdcReadTrack },
	/* 0x03 */ { 3, 0, Mapper171::FdcSpecify },
	/* 0x04 */ { 2, 1, Mapper171::FdcSenseDriveStatus },
	/* 0x05 */ { 9, 7, Mapper171::FdcWriteData },
	/* 0x06 */ { 9, 7, Mapper171::FdcReadData },
	/* 0x07 */ { 2, 0, Mapper171::FdcRecalibrate },
	/* 0x08 */ { 1, 2, Mapper171::FdcSenseIntStatus },
	/* 0x09 */ { 9, 7, Mapper171::FdcWriteDeletedData },
	/* 0x0A */ { 2, 7, Mapper171::FdcReadID },
	/* 0x0B */ { 1, 1, Mapper171::FdcNop },
	/* 0x0C */ { 9, 7, Mapper171::FdcReadDeletedData },
	/* 0x0D */ { 6, 7, Mapper171::FdcFormatTrack },
	/* 0x0E */ { 1, 1, Mapper171::FdcNop },
	/* 0x0F */ { 3, 0, Mapper171::FdcSeek },
	/* 0x10 */ { 1, 1, Mapper171::FdcNop },
	/* 0x11 */ { 9, 7, Mapper171::FdcScanEqual },
	/* 0x12 */ { 1, 1, Mapper171::FdcNop },
	/* 0x13 */ { 1, 1, Mapper171::FdcNop },
	/* 0x14 */ { 1, 1, Mapper171::FdcNop },
	/* 0x15 */ { 1, 1, Mapper171::FdcNop },
	/* 0x16 */ { 1, 1, Mapper171::FdcNop },
	/* 0x17 */ { 1, 1, Mapper171::FdcNop },
	/* 0x18 */ { 1, 1, Mapper171::FdcNop },
	/* 0x19 */ { 9, 7, Mapper171::FdcScanLowOrEqual },
	/* 0x1A */ { 1, 1, Mapper171::FdcNop },
	/* 0x1B */ { 1, 1, Mapper171::FdcNop },
	/* 0x1C */ { 1, 1, Mapper171::FdcNop },
	/* 0x1D */ { 9, 7, Mapper171::FdcScanHighOrEqual },
	/* 0x1E */ { 1, 1, Mapper171::FdcNop },
	/* 0x1F */ { 1, 1, Mapper171::FdcNop },
};

void Mapper171::Reset()
{
	DISK = Load_DiskIMG();

	pFdcDataPtr = DISK;

	// BIOS fixed to Page FE
	BIOS = BDRAM + 0x78000;

	memset(BDRAM, 0, sizeof(BDRAM));

	// 4xxx - 7xxx
	SetPROM_Bank(2, BIOS + 0x0000, BANKTYPE_RAM);
	SetPROM_Bank(3, BIOS + 0x2000, BANKTYPE_RAM);

	// 8xxx - Bxxx
	SetPROM_Bank(4, BDRAM + 0x0000, BANKTYPE_RAM);
	SetPROM_Bank(5, BDRAM + 0x2000, BANKTYPE_RAM);

	// Cxxx - Fxxx
	SetPROM_16K_Bank(6, 7);

//	SetPROM_32K_Bank(0);

	SetYCRAM_8K_Bank(0);

	nPageCD = 0;
	nPageEF = 0;

	// FDC
	bFdcIrq = FALSE;
	bFdcHwReset = FALSE;
	bFdcSoftReset = FALSE;
	bFdcDmaInt = FALSE;
	nFdcDrvSel = 0;
	nFdcMotor = 0;
	nFdcMainStatus = FDC_MS_RQM;
	nFDCStatus[0] = 0;
	nFDCStatus[1] = 0;
	nFDCStatus[2] = 0;
	nFDCStatus[3] = 0;
	bFdcCycle = 0;
	bFdcPhase = FDC_PH_IDLE;
	nFdcCylinder = 0;

	p_mode = 0;
	NT_data = 0;
	nes->ppu->SetExtLatchMode( TRUE );

	irq_enable = 0;
	irq_counter = 0;

	reg[0]=0;
	reg[1]=0;
	reg[2]=0;
	reg[3]=0;

}

// 8000 - FFFF
BOOL Mapper171::ReadHigh(WORD addr, LPBYTE pdata)
{
	if (addr < 0xFF00 ||
		addr > 0xFFEF)
		return FALSE;

	switch (addr)
	{
		case 0xFF18:	// ��������ϵͳ����;
	DEBUGOUT( "ReadH  A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, bFdcResults[bFdcCycle]&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			*pdata = 0x8F;
			return TRUE;

		case 0xFF80:	// FDCDMADackIO
			*pdata = *pFdcDataPtr++;
			return TRUE;
		case 0xFF88:	// FDCDMATcIO
			*pdata = *pFdcDataPtr++;
			return TRUE;
		case 0xFF90:	// FDCDRQPortI/FDCCtrlPortO
			// I: D6 : FDC DRQ
			*pdata = 0x40;
			return TRUE;
		case 0xFF98:	// FDCIRQPortI/FDCDMADackIO
			// I: D6 : IRQ
			if (bFdcIrq)
				*pdata = 0x40;
			else
				*pdata = 0;
			return TRUE;

		case 0xFFA0:	// FDCResetPortO/FDCStatPortI
			// I: D7 : FDC ready
			// I: D6 : FDC dir
			// I: D5 : FDC busy
			*pdata = nFdcMainStatus;
//	DEBUGOUT( "ReadH  A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, nFdcMainStatus&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			return TRUE;
		case 0xFFA8:	// FDCDataPortIO
			*pdata = bFdcResults[bFdcCycle];
			bFdcCycle++;
			if (bFdcCycle == pFdcCmd->bRLength)
			{
				// prepare for next command
				bFdcCycle = 0;
				bFdcPhase = FDC_PH_IDLE;

				nFdcMainStatus &= ~FDC_MS_DATA_IN;
				nFdcMainStatus |= FDC_MS_RQM;
			}
			return TRUE;
		case 0xFFB8:	// FDCChangePortI/FDCSpeedPortO
			// I: D7 : Disk changed
			*pdata = 0;
			return TRUE;

		default:
			DEBUGOUT("Read %02X\n", addr);
			break;
	}

	return FALSE;
}

void Mapper171::Write(WORD addr, BYTE data)
{

	if (addr < 0xFF00)
	{
		CPU_MEM_BANK[addr >> 13][addr & 0x1FFF] = data;
//		Write_PRAM( addr, data );
		return;
	}

	DEBUGOUT( "WriteH A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	switch (addr&0xFF)	//0xFF00-0xFFFF
	{
		case 0x00:
			reg[1] = data;
			break;
		case 0x01:	// VideoCtrlPort
			// D3 : C000-FFFF map to DRAM/-ROM
			// D2 : INT count/-load
			// D[1:0] : 6116's AV10 connection
			//          00: AV10
			//          01: AV11
			//          10: AV0
			//          11: AV1

			p_mode = data&0x40;
			reg[2] = data&0x08;

			reg[0] = data&0x03;
			if(reg[0]==0)		SetVRAM_Mirror( VRAM_VMIRROR );
			else if(reg[0]==1)	SetVRAM_Mirror( VRAM_HMIRROR );
			else if(reg[0]==2)	SetVRAM_Mirror( VRAM_MIRROR4L );
			else				SetVRAM_Mirror( VRAM_MIRROR4H );

			if(data&0x4){
				irq_enable = 1;
				nes->cpu->ClrIRQ( IRQ_MAPPER );
			}else{
//				irq_enable = 0;
				nes->cpu->ClrIRQ( IRQ_MAPPER );
			}

			if(data&8){
				// map C000-FFF to DRAM
//				int nOffset;
//				nOffset = nPageCD * 0x2000;
//				nOffset &= 0x7FFFF;	// truncate to 512K
//				SetPROM_Bank(6, BDRAM + nOffset, BANKTYPE_RAM);
//				nOffset = nPageEF * 0x2000;
//				nOffset &= 0x7FFFF;	// truncate to 512K
//				SetPROM_Bank(7, BDRAM + nOffset, BANKTYPE_RAM);

				SetBDRAM_8K_Bank(6 , nPageCD&0x3F);
				SetBDRAM_8K_Bank(7 , nPageEF&0x3F);
			}
			else{
				// map C000-FFF to ROM
				SetPROM_16K_Bank(6, 7);
			}
			break;

//		case 0x02:	// IntCountPortL
			// D[4:0] : counter
//			break;

		case 0x03:	// VideoDataPort0
			data &= 0x0F;
			SetYCRAM_2K_Bank( 0, data);
			break;

		case 0x04:	// DRAMPagePort
//			data &= 0x1F;	//  512K
//			SetPROM_Bank(4, BDRAM + data * 0x4000 + 0x0000, BANKTYPE_RAM);
//			SetPROM_Bank(5, BDRAM + data * 0x4000 + 0x2000, BANKTYPE_RAM);

			SetBDRAM_16K_Bank(4 , data&0x1F);
			if(((reg[1]==4)||p_mode)&&(data<0xE0)) SetPROM_16K_Bank(4, data&7);
			break;
/*
		case 0x0A:
			break;
		case 0x1A:
			reg[3] = data;
			break;
		case 0x12:
//			reg12 = data;
			break;
		case 0x22:
//			reg22 = data;
			break;
*/
		case 0x0B:	// VideoDataPort1
			data &= 0x0F;	// 32K
			SetYCRAM_2K_Bank( 2, data);
			break;

//		case 0x10:	// SoundPort0/SpeakInitPort
			//
//			break;

		case 0x13:	// VideoDataPort2
			data &= 0x0F;	// 32K
			SetYCRAM_2K_Bank( 4, data);
			break;

		case 0x14:
			data &= 0x3F;
			SetBDRAM_8K_Bank(4 , data);
			break;

//		case 0x18:	// ��������ϵͳ����;
//			data = data;
//			break;

		case 0x1B:	// VideoDataPort3
			data &= 0x0F;	// 32K
			SetYCRAM_2K_Bank( 6, data);
			break;

		case 0x1C:
			data &= 0x3F;
			SetBDRAM_8K_Bank(5 , data);
			break;

		case 0x24:	// DRAMPagePortCD
			nPageCD = data;
			if(reg[2]) SetBDRAM_8K_Bank(6 , data&0x3F);
			break;
		case 0x2C:	// DRAMPagePortEF
			nPageEF = data;
			if(reg[2]) SetBDRAM_8K_Bank(7 , data&0x3F);
			break;

//		case 0x40:	// PCDaCtPortO/PCCDataPort
			// D4 : Signal to PC's ERROR
			// D[3:0] : Output PC data
//			break;
//		case 0x48:	// PCCtrlPortI/PCCStatsPortI/PCDataPortIH
			// D[7:4] : PC input data
			// D3 : Signal of PC's pin STB
//			break;
//		case 0x50:	// PCDataPortIL/PCCCtrlPoutO
			// D[3:0] : PC input data
//			break;

//		case 0x80:	// FDCDMADackIO
//			data = data;
//			break;
//		case 0x88:	// FDCDMATcIO
//			data = data;
//			break;

		case 0x90:	// FDCDRQPortI/FDCCtrlPortO
			// O: D5 : Drv B motor
			// O: D4 : Drv A motor
			// O: D3 : Enable INT and DMA
			// O: D2 : not FDC Reset
			// O: D[1:0] : Drv sel

			bFdcDmaInt = (data & 8) ? TRUE : FALSE;
			nFdcDrvSel = data & 3;
			nFdcMotor = data >> 4;

			if (data & 4)
			{
				if (bFdcSoftReset)
				{
					FdcSoftReset();

					bFdcSoftReset = FALSE;

					// irq after soft reset
					if (0 == nFdcDrvSel)
						bFdcIrq = TRUE;		// Drv A Only
					else
						bFdcIrq = FALSE;
				}
			}
			else
			{
				if (!bFdcSoftReset)
				{
					bFdcSoftReset = TRUE;
					bFdcIrq = FALSE;
				}
			}

			break;
//		case 0x98:	// FDCIRQPortI/FDCDMADackIO
			// I: D6 : IRQ
//			data = data;
//			break;
		case 0xA0:	// FDCResetPortO/FDCStatPortI
			// O: D6 : FDC pin reset
			if (data & 0x40)
			{
				if (!bFdcHwReset)
				{
					bFdcHwReset = TRUE;
					bFdcIrq = FALSE;
				}
			}
			else
			{
				if (bFdcHwReset)
				{
					FdcHardReset();
					bFdcHwReset = FALSE;
				}
			}
			break;
		case 0xA8:	// FDCDataPortIO
			switch (bFdcPhase)
			{
				case FDC_PH_EXECUTION:
				case FDC_PH_RESULT:
					// ERROR
					break;
				case FDC_PH_IDLE:
				default:
					bFdcCycle = 0;
					bFdcPhase = FDC_PH_COMMAND;
					pFdcCmd = &FdcCmdTableB[data & FDC_CC_MASK];
					// fall through
				case FDC_PH_COMMAND:
					bFdcCommands[bFdcCycle] = data;
					bFdcCycle++;
					if (bFdcCycle == pFdcCmd->bWLength)
					{
						bFdcPhase = FDC_PH_EXECUTION;
//						nFdcMainStatus &= ~FDC_MS_RQM;
						pFdcCmd->pFun(this);
						if (pFdcCmd->bRLength)	// prepare for reading
						{
							nFdcMainStatus |= FDC_MS_DATA_IN;
							bFdcPhase = FDC_PH_RESULT;
						}
						else
						{
							bFdcPhase = FDC_PH_IDLE;
						}
						bFdcCycle = 0;
					}
					break;
			}
			break;
//		case 0xB8:	// FDCChangePortI/FDCSpeedPortO
			// I: D7 : Disk changed
			// O: D[1:0] : 00 500kbps(1.2M, 1.44M)
			//             01 300kbps(360K) 
			//             10 250kbps(720K)
//			data = data;
//			break;

//		case 0x09:
//		case 0x17:
//			data = data;
//			break;

		// code patch
//		case 0x2B:
//			CPU_MEM_BANK[7][addr & 0x1FFF] = data;
//			break;

		default:
			DEBUGOUT("Write %02X -> [%04X]\n", data, addr);
			break;
	}
}

// 4100 - 7FFF
BYTE Mapper171::ReadLow(WORD addr)
{
	BYTE data = 0;

	if (addr < 0x4400)
	{
		// 4100 - 43FF, IO for system
		data = BIOS[addr - 0x4000];
	}
	else if (addr < 0x5800)
	{
		// 4400 - 57FF, IO for ROM/RAM
		data = BIOS[addr - 0x4000];
	}
	else if (addr < 0x6000)
	{
		// 5800 - 5FFF, 2K RAM
		data = BIOS[addr - 0x4000];
	}
	else if (addr < 0x6100)
	{
		// 6000 - 60FF, reserved
		data = BIOS[addr - 0x4000];
	}
	else
	{
		// 6100 - 7FFF, RAM
		data = BIOS[addr - 0x4000];
	}

	return data;
}

// 4100 - 7FFF
void Mapper171::WriteLow(WORD addr, BYTE data)
{
	if (addr < 0x4400)
	{
		// 4100 - 43FF, IO for system
		BIOS[addr - 0x4000] = data;
	}
	else if (addr < 0x5800)
	{
		// 4400 - 57FF, IO for ROM/RAM
		BIOS[addr - 0x4000] = data;
	}
	else if (addr < 0x6000)
	{
		// 5800 - 5FFF, 2K RAM
		BIOS[addr - 0x4000] = data;
	}
	else if (addr < 0x6100)
	{
		// 6000 - 60FF, reserved
		BIOS[addr - 0x4000] = data;
	}
	else
	{
		// 6100 - 7FFF, RAM
		BIOS[addr - 0x4000] = data;
	}
}

void	Mapper171::Write_PRAM( WORD addr, BYTE data )
{
	if(addr<0xC000){
//		BDRAM[(addr-0x8000)+(bank*0x4000)] = data;
		CPU_MEM_BANK[addr >> 13][addr & 0x1FFF] = data;
	}else{
		BDRAM[addr+0x70000] = data;
	}
}
/*
void	Mapper171::Clock( INT cycles )
{
	if( irq_enable ) {
		if( (irq_counter+=cycles) >= 0x7FFF ) {
//			irq_counter = 0x7FFF;
//			nes->cpu->IRQ_NotPending();

			irq_enable  = 0;
//			irq_counter &= 0x7FFF;
			irq_counter = 0x7FFF;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper171::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
//		SetYCRAM_2K_Bank( 0, (scanline&0xF0)>>4);
		if( irq_enable ) {

			irq_enable  = 0;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}
*/

void	Mapper171::PPU_Latch( WORD addr )
{
	if((addr&0xF000)==0x2000){
		NT_data=(addr>>6)&0x0F;
		if(p_mode){
//			SetYCRAM_2K_Bank( 0, NT_data);
//			SetYCRAM_2K_Bank( 2, NT_data);
//			SetYCRAM_2K_Bank( 4, NT_data);
//			SetYCRAM_2K_Bank( 6, NT_data);
		}
	}
/*
	if((DirectInput.m_Sw[DIK_PGUP])&&(DirectInput.m_Sw[DIK_PGDN])){
		DISK = Change_DiskIMG();
		pFdcDataPtr = DISK;
	}

	if(DirectInput.m_Sw[DIK_PAUSE]){
		nes->Dump_BDRAM();
		nes->Dump_YCRAM();
		nes->Dump_VRAM();
		nes->Dump_CPULMEM();
		nes->Dump_CPUHMEM();
	}
*/
}
void	Mapper171::PPU_ExtLatchX( INT x )
{
	a3 = (x&1)<<3;
}
void	Mapper171::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	INT loopy_v = nes->ppu->GetPPUADDR();
	INT loopy_y = nes->ppu->GetTILEY();
	INT tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT attrsft = (ntbladr&0x0040)>>4;
	LPBYTE pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT ntbl_x  = ntbladr&0x001F;
	INT tileadr, ntb;

	ntb = (ntbladr>>6)&0x0F;

		if(p_mode){
//			SetYCRAM_2K_Bank( 0, ntb);
//			SetYCRAM_2K_Bank( 2, NT_data);
//			SetYCRAM_2K_Bank( 4, NT_data);
//			SetYCRAM_2K_Bank( 6, NT_data);
		}

//	DEBUGOUT("ntb=%04X tileofs=%02X\n", ntb, tileofs );

	attradr &= 0x3FF;
	attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

	chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
}

LPBYTE	Mapper171::Load_DiskIMG()
{
	FILE *fp = NULL;
	if( !(lpDisk = (LPBYTE)malloc( 0x168000 )) ) {
		//throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
	}
	if( !(fp = ::fopen( img_fname, "rb" )) ) {
		LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
		::wsprintf( szErrorString, szErrStr, img_fname );
		//throw	szErrorString;
	}
	::fread(lpDisk, 0x168000, 1, fp);
	FCLOSE(fp);
	return lpDisk;
}
LPBYTE	Mapper171::Change_DiskIMG()
{
	FILE *fp = NULL;
	if( !(lpDisk = (LPBYTE)malloc( 0x168000 )) ) {
		//throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
	}
	if( !(fp = ::fopen( "BBK.IMG", "rb" )) ) {
		LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
		::wsprintf( szErrorString, szErrStr, "BBK.img" );
		//throw	szErrorString;
	}
	::fread(lpDisk, 0x168000, 1, fp);
	FCLOSE(fp);
	return lpDisk;
}

// for FDC
void Mapper171::FdcHardReset(void)
{
	DEBUGOUT( "FdcHardReset!!!\n" );
	bFdcDmaInt = FALSE;
	nFdcDrvSel = 0;
	nFdcMotor = 0;
	nFdcMainStatus = FDC_MS_RQM;
	nFDCStatus[0] = 0;
	nFDCStatus[1] = 0;
	nFDCStatus[2] = 0;
	nFDCStatus[3] = 0;
	bFdcCycle = 0;
	bFdcPhase = FDC_PH_IDLE;
}
void Mapper171::FdcSoftReset(void)
{
	DEBUGOUT( "FdcSoftReset!!!\n" );
	nFdcDrvSel = 0;
	nFdcMotor = 0;
	nFdcMainStatus = FDC_MS_RQM;
	nFDCStatus[0] = 0;
	nFDCStatus[1] = 0;
	nFDCStatus[2] = 0;
	nFDCStatus[3] = 0;
	bFdcCycle = 0;
	bFdcPhase = FDC_PH_IDLE;
}
void Mapper171::FdcNop(Mapper171* thiz)
{
	DEBUGOUT( "FdcNop!!!\n" );
	thiz->nFDCStatus[0] = FDC_S0_IC1;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
}
void Mapper171::FdcReadTrack(Mapper171* thiz)
{
	DEBUGOUT( "FdcReadTrack!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcSpecify(Mapper171* thiz)
{
	DEBUGOUT( "FdcSpecify!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcSenseDriveStatus(Mapper171* thiz)
{
	DEBUGOUT( "FdcSenseDriveStatus!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcWriteData(Mapper171* thiz)
{
	DEBUGOUT( "FdcWriteData!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcReadData(Mapper171* thiz)
{
	DEBUGOUT( "FdcReadData!!!\n" );
	BYTE C = thiz->bFdcCommands[2];
	BYTE H = thiz->bFdcCommands[3];
	BYTE R = thiz->bFdcCommands[4];
	BYTE N = thiz->bFdcCommands[5];
	INT LBA;
	LBA = H * 18 + C * 36 + (R - 1);
	thiz->pFdcDataPtr = thiz->DISK + LBA * 512;
	R++;
	if (19 == R)
	{
		R = 1;
		H++;
		if (2 == H)
		{
			C++;
			if (80 == C)
				C = 0;
		}
	}
	thiz->nFDCStatus[0] = 0;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0]; // ST0
	thiz->bFdcResults[1] = thiz->nFDCStatus[1]; // ST1
	thiz->bFdcResults[2] = thiz->nFDCStatus[2]; // ST2
	thiz->bFdcResults[3] = C;
	thiz->bFdcResults[4] = H;
	thiz->bFdcResults[5] = R;
	thiz->bFdcResults[6] = N;
}
void Mapper171::FdcRecalibrate(Mapper171* thiz)
{
	DEBUGOUT( "FdcRecalibrate!!!\n" );
	BYTE US;
	US = thiz->bFdcCommands[1] & 3;
	if (0 == US)
		thiz->nFDCStatus[0] = FDC_S0_SE;
	else
		thiz->nFDCStatus[0] = FDC_S0_SE | FDC_S0_IC0;
}
void Mapper171::FdcSenseIntStatus(Mapper171* thiz)
{
	DEBUGOUT( "FdcSenseIntStatus!!!\n" );
	if (0 == thiz->nFdcDrvSel)	// Drv A Only
		thiz->nFDCStatus[0] = FDC_S0_IC0 | FDC_S0_IC1;
	else
		thiz->nFDCStatus[0] = FDC_S0_SE | FDC_S0_IC0;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
	thiz->bFdcResults[1] = thiz->nFdcCylinder;	// PCN
}
void Mapper171::FdcWriteDeletedData(Mapper171* thiz)
{
	DEBUGOUT( "FdcWriteDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcReadID(Mapper171* thiz)
{
	DEBUGOUT( "FdcReadID!!!\n" );
	thiz->nFDCStatus[0] = 0;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
}
void Mapper171::FdcReadDeletedData(Mapper171* thiz)
{
	DEBUGOUT( "FdcReadDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcFormatTrack(Mapper171* thiz)
{
	DEBUGOUT( "FdcFormatTrack!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcSeek(Mapper171* thiz)
{
	DEBUGOUT( "FdcSeek!!!\n" );
	// new cylinder number
	BYTE NCN;
	BYTE US;
	US = thiz->bFdcCommands[1] & 3;
	NCN = thiz->bFdcCommands[2];
	thiz->nFdcCylinder = NCN;
	thiz->nFDCStatus[0] = FDC_S0_SE;
}
void Mapper171::FdcScanEqual(Mapper171* thiz)
{
	DEBUGOUT( "FdcScanEqual!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcScanLowOrEqual(Mapper171* thiz)
{
	DEBUGOUT( "FdcScanLowOrEqual!!!\n" );
	thiz = thiz;
}
void Mapper171::FdcScanHighOrEqual(Mapper171* thiz)
{
	DEBUGOUT( "FdcScanHighOrEqual!!!\n" );
	thiz = thiz;
}

/*******************************************************************************
                           E N D  O F  F I L E
*******************************************************************************/
