
static const Mapper172::FDC_CMD_DESC FdcCmdTableJ[32] =
{
	/* 0x00 */ { 1, 1, Mapper172::FdcNop },
	/* 0x01 */ { 1, 1, Mapper172::FdcNop },
	/* 0x02 */ { 9, 7, Mapper172::FdcReadTrack },
	/* 0x03 */ { 3, 0, Mapper172::FdcSpecify },
	/* 0x04 */ { 2, 1, Mapper172::FdcSenseDriveStatus },
	/* 0x05 */ { 9, 7, Mapper172::FdcWriteData },
	/* 0x06 */ { 9, 7, Mapper172::FdcReadData },
	/* 0x07 */ { 2, 0, Mapper172::FdcRecalibrate },
	/* 0x08 */ { 1, 2, Mapper172::FdcSenseIntStatus },
	/* 0x09 */ { 9, 7, Mapper172::FdcWriteDeletedData },
	/* 0x0A */ { 2, 7, Mapper172::FdcReadID },
	/* 0x0B */ { 1, 1, Mapper172::FdcNop },
	/* 0x0C */ { 9, 7, Mapper172::FdcReadDeletedData },
	/* 0x0D */ { 6, 7, Mapper172::FdcFormatTrack },
	/* 0x0E */ { 1, 1, Mapper172::FdcNop },
	/* 0x0F */ { 3, 0, Mapper172::FdcSeek },
	/* 0x10 */ { 1, 1, Mapper172::FdcNop },
	/* 0x11 */ { 9, 7, Mapper172::FdcScanEqual },
	/* 0x12 */ { 1, 1, Mapper172::FdcNop },
	/* 0x13 */ { 1, 1, Mapper172::FdcNop },
	/* 0x14 */ { 1, 1, Mapper172::FdcNop },
	/* 0x15 */ { 1, 1, Mapper172::FdcNop },
	/* 0x16 */ { 1, 1, Mapper172::FdcNop },
	/* 0x17 */ { 1, 1, Mapper172::FdcNop },
	/* 0x18 */ { 1, 1, Mapper172::FdcNop },
	/* 0x19 */ { 9, 7, Mapper172::FdcScanLowOrEqual },
	/* 0x1A */ { 1, 1, Mapper172::FdcNop },
	/* 0x1B */ { 1, 1, Mapper172::FdcNop },
	/* 0x1C */ { 1, 1, Mapper172::FdcNop },
	/* 0x1D */ { 9, 7, Mapper172::FdcScanHighOrEqual },
	/* 0x1E */ { 1, 1, Mapper172::FdcNop },
	/* 0x1F */ { 1, 1, Mapper172::FdcNop },
};

void	Mapper172::Reset()
{
	memcpy( YSRAM, PROM, 0x8000);

	ZEROMEMORY( JDRAM, sizeof(JDRAM) );
	ZEROMEMORY( JSRAM, sizeof(JSRAM) );
	ZEROMEMORY( JCRAM, sizeof(JCRAM) );
	
	SetPROM_32K_Bank( 0 );

	irq_enable = 0;
	irq_counter = 0;
	WE_SRAM = 0;
	WE_DRAM = 0;
	RE_DRAM = 1;
	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 0;
	reg[3] = 0;
	read_mode = 0;
	read_mode1 = 0;
	write_mode = 0;
	write_mode1 = 0;

	//for FDC code by fanoble
	lpDisk = NULL;
	DISK = NULL;
	if(Load_DiskIMG() != NULL) DISK = Load_DiskIMG();
	pFdcDataPtr = DISK;
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
	nFdcHeadAddres = 0;
	nFdcRecord = 0;
	nFdcNumber = 0;

}

BYTE	Mapper172::ReadLow( WORD addr )
{
//	if((addr<0x6000)&&(addr!=0x418E)) DEBUGOUT( "ReadLow  - addr= %04x ; dat= %03x\n", addr, Mapper::ReadLow( addr ) );

	switch (addr)
	{
		case 0x4188://FDC��״̬�Ĵ���(STATUS)
//			DEBUGOUT( "Read  0x4188 = %04X\n", nFdcMainStatus );
			return nFdcMainStatus;
			break;
		case 0x4189:
			if(read_mode){
				read_mode1++;
				if(read_mode1==512) read_mode=0;
				reg[2] = *(pFdcDataPtr++);
				return reg[2];
			}
//			DEBUGOUT( "Read  0x4189 = %04X\n", bFdcResults[bFdcCycle] );
			read_mode1 = 0;
			write_mode = 0;
			reg[2] = bFdcResults[bFdcCycle];
			bFdcCycle++;
			if (bFdcCycle == pFdcCmd->bRLength)
			{
				bFdcCycle = 0;
				bFdcPhase = FDC_PH_IDLE;
				nFdcMainStatus &= ~FDC_MS_DATA_IN;
				nFdcMainStatus |= FDC_MS_RQM;
			}
			return reg[2];
			break;
		case 0x418E:
			if (bFdcIrq)
				return 0xC0;
			else
				return 0x00;

			break;
		case 0x41AC:
			//
			break;
	}

	if((addr>=0x6000)&&(WE_SRAM==1)){
		return JSRAM[addr-0x6000];
	}

	if((addr>=0x6000)&&(RE_DRAM==1)){
//	if((addr>=0x6000)){
		return JDRAM[addr-0x6000+(reg[0]*0x8000)];
	}

	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return	Mapper::ReadLow( addr );
}

void	Mapper172::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x6000)&&(WE_SRAM==1)){
		JSRAM[addr-0x6000] = data;
	}

	if((addr>=0x6000)&&(WE_DRAM==1)){
//	if((addr>=0x6000)){
		JDRAM[addr-0x6000+(reg[1]*0x8000)] = data;
	}

	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;

//	if((addr<0x6000)&&(addr!=0x418E)) DEBUGOUT( "WriteLow - addr= %04x ; dat= %03x\n", addr, data );
	switch (addr)
	{
		case 0x4180:
			DEBUGOUT( "Write 0x4180 = %04X\n", data );
			//s????xxx: BIOS ROM bankswitch register.
			//Load page xxx (4K) of the BIOS ROM to the page frame at $e000.
			//Also, s might be an enable for SRAM at $6000-$7fff.
			//
			WE_SRAM = data & 0x80;

			SetPROM_4K_Bank(0xE000, data&7);
//			SetPROM_8K_Bank(7, data&7);
			break;
		case 0x4181:
			//P?mmppcc: Game Mode Control. 
			//P=1 enables MMC2 CHR-ROM switch
			//values of mm select game mode (00=old game, 01=new game, 10=MMC1, 11=MMC3)
			//values of pp select PRG page size (00=8k, 01=16k, 10=32k, 11=invalid)
			//values of cc select CHR page size (1 << cc KBytes). (same as Bung's doc)
			//
			DEBUGOUT( "Write 0x4181 = %04X\n", data );
			break;
		case 0x4182:
			//Rfffvvii: New game mode PPU Config. (same as Bung's doc)
			//
			DEBUGOUT( "Write 0x4181 = %04X\n", data );
			break;
		case 0x4183:
			//PPPPCCCC: PRG and CHR high address line masks (same as Bung's doc)
			//
			DEBUGOUT( "Write 0x4181 = %04X\n", data );
			break;
		case 0x4185:
			//
			break;
		case 0x4186:
			//Send Parallel data.
			break;
		case 0x4189:
//			DEBUGOUT( "Write 0x4189 = %04X\n", data );
			if(write_mode){
				write_mode1++;
				if(write_mode1==512) write_mode=0;
				*pFdcDataPtr = data;
				pFdcDataPtr++;
			}
			write_mode1 = 0;
			read_mode = 0;
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
					pFdcCmd = &FdcCmdTableJ[data & FDC_CC_MASK];
					// fall through
				case FDC_PH_COMMAND:
					bFdcCommands[bFdcCycle] = data;
					bFdcCycle++;
					if (bFdcCycle == pFdcCmd->bWLength)
					{
						bFdcPhase = FDC_PH_EXECUTION;

						pFdcCmd->pFun(this);

						if (pFdcCmd->bRLength)
						{
							nFdcMainStatus |= FDC_MS_DATA_IN;
							nFdcMainStatus |= FDC_MS_RQM;
							bFdcPhase = FDC_PH_RESULT;
							if((bFdcCommands[0]&0x0F)==0x06) read_mode = 1;
							if((bFdcCommands[0]&0x0F)==0x05) write_mode = 1;
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
		case 0x418A://DCR
//			DEBUGOUT( "Write 0x418A = %04X\n", data );
			data = data;
			break;
		case 0x418B://FDC���������Ĵ���(DOR)(���ֿ��ƼĴ���)
//			DEBUGOUT( "Write 0x418B = %04X\n", data );
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
		case 0x418C:
			//
			break;
		case 0x418D:
			data = data&0x03;
			if(data==0)			SetVRAM_Mirror( VRAM_VMIRROR );
			else if(data==1)	SetVRAM_Mirror( VRAM_HMIRROR );
			else if(data==2)	SetVRAM_Mirror( VRAM_MIRROR4L );
			else				SetVRAM_Mirror( VRAM_MIRROR4H );
			break;
		case 0x418E:
			//
			break;
		case 0x418F:
			reg[3] = data;
			break;
		case 0x4190:
			//????xxxx: Map 32k DRAM bank (of a possible 512k) to $6000-$DFFF for reading.
			//Reading and writing function independently of each other.
			WE_DRAM = 0;
			RE_DRAM = 1;
			reg[0] = data & 0x0F;
			SetJDRAM_32K_Bank( reg[0] );
			break;
		case 0x4191:
			//????xxxx: Map 32k DRAM bank to $6000-$DFFF for writing.
			WE_DRAM = 1;
			RE_DRAM = 0;
			reg[1] = data & 0x0F;
			SetJDRAM_32K_Bank( reg[1] );
			break;
		case 0x4194:
			//
			break;
		case 0x4198:
			//??xxxxxx: Map 8k DRAM bank to PPU 0x0000.
			SetJCRAM_8K_Bank( data & 0x3F );
			break;
		case 0x42FF:
			//
			break;
	}

}

BOOL	Mapper172::ReadHigh(WORD addr, LPBYTE pdata)
{
	if( addr>=0xE000 ) return FALSE;
	if( RE_DRAM == 0 ) return FALSE;
	*pdata = JDRAM[addr-0x6000+(reg[0]*0x8000)];
	return TRUE;
}

void	Mapper172::Write( WORD addr, BYTE data )
{
//	if(addr>=0xE000) DEBUGOUT( "Write    - addr= %04x ; dat= %03x\n", addr, data );

	if( (addr<0xE000)&&(WE_DRAM==1) ) {
//	if( (addr<0xE000) ) {
		JDRAM[addr-0x6000+(reg[1]*0x8000)] = data;
	}else{
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

void	Mapper172::Clock( INT cycles )
{
/*	if( irq_enable ) {
		if( (irq_counter+=cycles) >= 0x7FFF ) {
//			irq_counter = 0x7FFF;
//			nes->cpu->IRQ_NotPending();

			irq_enable  = 0;
//			irq_counter &= 0x7FFF;
			irq_counter = 0x7FFF; 
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}*/
}

void	Mapper172::PPU_Latch( WORD addr )
{
/*
	if(DirectInput.m_Sw[DIK_PAUSE]){
//		nes->Dump_JDRAM();
//		nes->Dump_CPULMEM();
//		nes->Dump_CPUHMEM();
//		nes->Dump_YWRAM();
	}
*/
}

LPBYTE	Mapper172::Load_DiskIMG()
{
	FILE *fp = NULL;
	if( !(lpDisk = (LPBYTE)malloc( 0x168000 )) ) {
		//throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
	}
	if( !(fp = ::fopen( "DrPCJr.img", "rb" )) ) {
		LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
		::wsprintf( szErrorString, szErrStr, "DrPCJr.img" );
		return NULL;
	}
	::fread(lpDisk, 0x168000, 1, fp);
	FCLOSE(fp);
	return lpDisk;
}

//for FDC code by fanoble
void Mapper172::FdcHardReset(void)
{
	DEBUGOUT( "FdcHardReset!!!\n" );
	bFdcDmaInt = FALSE;
	FdcSoftReset();
}
void Mapper172::FdcSoftReset(void)
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

//����������(READ DATA)
void Mapper172::FdcReadData(Mapper172* thiz)
{
	DEBUGOUT( "FdcReadData!!!\n" );
	BYTE C = thiz->bFdcCommands[2];	//�ŵ���
	BYTE H = thiz->bFdcCommands[3];	//��ͷ��
	BYTE R = thiz->bFdcCommands[4];	//������
	BYTE N = thiz->bFdcCommands[5];
	INT LBA = H * 18 + C * 36 + (R - 1);
	DEBUGOUT( "C = %04X\n", C );
	DEBUGOUT( "H = %04X\n", H );
	DEBUGOUT( "R = %04X\n", R );
	DEBUGOUT( "N = %04X\n", N );
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
	thiz->nFdcCylinder = C;
	thiz->nFdcHeadAddres = H;
	thiz->nFdcRecord = R;
	thiz->nFdcNumber = N;
}
//д��������(WRITE DATA)
void Mapper172::FdcWriteData(Mapper172* thiz)
{
	DEBUGOUT( "FdcWriteData!!!\n" );
	BYTE C = thiz->bFdcCommands[2];	//�ŵ���
	BYTE H = thiz->bFdcCommands[3];	//��ͷ��
	BYTE R = thiz->bFdcCommands[4];	//������
	BYTE N = thiz->bFdcCommands[5];
	INT LBA = H * 18 + C * 36 + (R - 1);
	DEBUGOUT( "C = %04X\n", C );
	DEBUGOUT( "H = %04X\n", H );
	DEBUGOUT( "R = %04X\n", R );
	DEBUGOUT( "N = %04X\n", N );
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
	thiz->nFdcCylinder = C;
	thiz->nFdcHeadAddres = H;
	thiz->nFdcRecord = R;
	thiz->nFdcNumber = N;
}
void Mapper172::FdcNop(Mapper172* thiz)
{
	DEBUGOUT( "FdcNop!!!\n" );
	thiz->nFDCStatus[0] = FDC_S0_IC1;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
}
void Mapper172::FdcReadTrack(Mapper172* thiz)
{
	DEBUGOUT( "FdcReadTrack!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcSpecify(Mapper172* thiz)
{
	DEBUGOUT( "FdcSpecify!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcSenseDriveStatus(Mapper172* thiz)
{
	DEBUGOUT( "FdcSenseDriveStatus!!!\n" );
	thiz->bFdcResults[0] = thiz->nFDCStatus[3];
}
void Mapper172::FdcRecalibrate(Mapper172* thiz)
{
	DEBUGOUT( "FdcRecalibrate!!!\n" );
	thiz->nFdcCylinder = 0;	//��ͷ�ص�0�ŵ�
	thiz->nFDCStatus[0] |= FDC_S0_SE;
}
void Mapper172::FdcSenseIntStatus(Mapper172* thiz)
{
	DEBUGOUT( "FdcSenseIntStatus!!!\n" );
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
	thiz->bFdcResults[1] = thiz->nFdcCylinder;
}
void Mapper172::FdcWriteDeletedData(Mapper172* thiz)
{
	DEBUGOUT( "FdcWriteDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcReadID(Mapper172* thiz)
{
	DEBUGOUT( "FdcReadID!!!\n" );
	thiz->nFDCStatus[0] = 0;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
	thiz->bFdcResults[1] = thiz->nFDCStatus[1];
	thiz->bFdcResults[2] = thiz->nFDCStatus[2];
	thiz->bFdcResults[3] = thiz->nFdcCylinder;
	thiz->bFdcResults[4] = thiz->nFdcHeadAddres;
	thiz->bFdcResults[5] = thiz->nFdcRecord;
	thiz->bFdcResults[6] = thiz->nFdcNumber;
}
void Mapper172::FdcReadDeletedData(Mapper172* thiz)
{
	DEBUGOUT( "FdcReadDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcFormatTrack(Mapper172* thiz)
{
	DEBUGOUT( "FdcFormatTrack!!!\n" );
	BYTE C = thiz->bFdcCommands[2];	//�ŵ���
	BYTE H = thiz->bFdcCommands[3];	//��ͷ��
	BYTE R = thiz->bFdcCommands[4];	//������
	BYTE N = thiz->bFdcCommands[5];

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
	thiz->nFdcCylinder = C;
	thiz->nFdcHeadAddres = H;
	thiz->nFdcRecord = R;
	thiz->nFdcNumber = N;
}
void Mapper172::FdcSeek(Mapper172* thiz)
{
	DEBUGOUT( "FdcSeek!!!\n" );
	BYTE US;
	US = thiz->bFdcCommands[1] & 3;
	thiz->nFdcHeadAddres = ( thiz->bFdcCommands[1] & 7 ) >> 2;
	thiz->nFdcCylinder = thiz->bFdcCommands[2];
	thiz->nFDCStatus[0] = FDC_S0_SE;
}
void Mapper172::FdcScanEqual(Mapper172* thiz)
{
	DEBUGOUT( "FdcScanEqual!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcScanLowOrEqual(Mapper172* thiz)
{
	DEBUGOUT( "FdcScanLowOrEqual!!!\n" );
	thiz = thiz;
}
void Mapper172::FdcScanHighOrEqual(Mapper172* thiz)
{
	DEBUGOUT( "FdcScanHighOrEqual!!!\n" );
	thiz = thiz;
}
