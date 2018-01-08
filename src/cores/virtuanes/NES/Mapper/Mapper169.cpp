//////////////////////////////////////////////////////////////////////////
// Mapper169        for YuXing                                          //
//////////////////////////////////////////////////////////////////////////

//for FDC code by fanoble
static const Mapper169::FDC_CMD_DESC FdcCmdTable[32] =
{
	/* 0x00 */ { 1, 1, Mapper169::FdcNop },
	/* 0x01 */ { 1, 1, Mapper169::FdcNop },
	/* 0x02 */ { 9, 7, Mapper169::FdcReadTrack },
	/* 0x03 */ { 3, 0, Mapper169::FdcSpecify },
	/* 0x04 */ { 2, 1, Mapper169::FdcSenseDriveStatus },
	/* 0x05 */ { 9, 7, Mapper169::FdcWriteData },
	/* 0x06 */ { 9, 7, Mapper169::FdcReadData },
	/* 0x07 */ { 2, 0, Mapper169::FdcRecalibrate },
	/* 0x08 */ { 1, 2, Mapper169::FdcSenseIntStatus },
	/* 0x09 */ { 9, 7, Mapper169::FdcWriteDeletedData },
	/* 0x0A */ { 2, 7, Mapper169::FdcReadID },
	/* 0x0B */ { 1, 1, Mapper169::FdcNop },
	/* 0x0C */ { 9, 7, Mapper169::FdcReadDeletedData },
	/* 0x0D */ { 6, 7, Mapper169::FdcFormatTrack },
	/* 0x0E */ { 1, 1, Mapper169::FdcNop },
	/* 0x0F */ { 3, 0, Mapper169::FdcSeek },
	/* 0x10 */ { 1, 1, Mapper169::FdcNop },
	/* 0x11 */ { 9, 7, Mapper169::FdcScanEqual },
	/* 0x12 */ { 1, 1, Mapper169::FdcNop },
	/* 0x13 */ { 1, 1, Mapper169::FdcNop },
	/* 0x14 */ { 1, 1, Mapper169::FdcNop },
	/* 0x15 */ { 1, 1, Mapper169::FdcNop },
	/* 0x16 */ { 1, 1, Mapper169::FdcNop },
	/* 0x17 */ { 1, 1, Mapper169::FdcNop },
	/* 0x18 */ { 1, 1, Mapper169::FdcNop },
	/* 0x19 */ { 9, 7, Mapper169::FdcScanLowOrEqual },
	/* 0x1A */ { 1, 1, Mapper169::FdcNop },
	/* 0x1B */ { 1, 1, Mapper169::FdcNop },
	/* 0x1C */ { 1, 1, Mapper169::FdcNop },
	/* 0x1D */ { 9, 7, Mapper169::FdcScanHighOrEqual },
	/* 0x1E */ { 1, 1, Mapper169::FdcNop },
	/* 0x1F */ { 1, 1, Mapper169::FdcNop },
};

void	Mapper169::Reset()
{
	DWORD	crc = nes->rom->GetPROM_CRC();
	if(crc == 0x2B1B969E	//YX_V8.2-D
	|| crc == 0xA70FD0F3)	//YX_V8.3-D
	{
		YX_type = 0;	//D��
	}
	if(crc == 0x6085FEE8	//YX_V9.0-98
	|| crc == 0x2A1E4D89	//YX_V9.2-98
	|| crc == 0x5C6CE13E)	//YX_V9.2-F
	{
		YX_type = 1;	//98�� & F��
	}

	nes->ppu->SetExtLatchMode( TRUE );
	reg[0] = 0xFF;	//$5002
	reg[1] = 0x00;	//$4800
	reg[2] = 0x00;	//$5500
	reg[3] = 0x00;	//$5501
	reg[4] = 0x00;	//PPU_Latch
	reg[5] = 0x00;	//$8000
	reg[6] = 0x00;
	reg[7] = 0x01;
	cmd_4800_6 = 0x00;
	cmd_4800_7 = 0x00;
	cmd_4800_8 = 0x00;
	cmd_5500_3 = 0x00;
	cmd_5500_8 = 0x00;
	cmd_5501_8 = 0x00;
	key_map_row = 0x00;
	MMC3_mode = 0x00;

	SetPROM_32K_Bank( 0 );
	SetYCRAM_8K_Bank( 0 );
/*
//--for test disk program-----------------------------------------
	int fs;
	fs = 0x000000;
	fs = 0x100000;
	memcpy( YWRAM, PROM+fs, PROM_8K_SIZE*0x2000-fs);
	memcpy( YWRAM+0xFC000, PROM+(PROM_16K_SIZE-1)*0x4000, 0x4000);
	SetYWRAM_32K_Bank( 0, 1, 0x7e, 0x7f );
//----------------------------------------------------------------
*/

	lpDisk = NULL;
	DISK = NULL;
	if(Load_DiskIMG() != NULL) DISK = Load_DiskIMG();

	read_mode = 0;
	read_mode1 = 0;
	nnn = 0;

	//for FDC code by fanoble
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

BYTE	Mapper169::ReadLow( WORD addr )
{
//	if(addr>0x4207) DEBUGOUT( "ReadL  A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, Mapper::ReadLow( addr ), nes->GetScanline(), nes->cpu->GetTotalCycles() );
/*
	int key_map[14][8] = {	//YuXing_Keyboard data by tpu (fix by temryu)
		{DIK_ESCAPE, DIK_F9,         DIK_7,      DIK_R,       DIK_A,      0x00,          0x00,         DIK_LSHIFT},
		{0x00,       DIK_NUMPADENTER,0x00,       DIK_MULTIPLY,DIK_DIVIDE, DIK_UP,        DIK_BACKSPACE,DIK_F12   },
		{0x00,       0x00,           0x00,       DIK_ADD,     DIK_NUMLOCK,DIK_LEFT,      DIK_DOWN,     DIK_RIGHT },
		{0x00,       DIK_NUMPAD7,    DIK_NUMPAD8,DIK_SUBTRACT,DIK_NUMPAD9,DIK_F11,       DIK_END,      DIK_PGDN  },
		{DIK_F8,     DIK_6,          DIK_E,      DIK_RBRACKET,DIK_L,      DIK_Z,         DIK_N,        DIK_SPACE },
		{DIK_F7,     DIK_5,          DIK_W,      DIK_LBRACKET,DIK_K,      DIK_DELETE,    DIK_B,        DIK_SLASH },
		{DIK_F6,     DIK_4,          DIK_Q,      DIK_P,       DIK_J,      DIK_BACKSLASH, DIK_V,        DIK_PERIOD},
		{DIK_F5,     DIK_3,          DIK_EQUALS, DIK_O,       DIK_H,      DIK_APOSTROPHE,DIK_C,        DIK_COMMA },
		{DIK_F4,     DIK_2,          DIK_MINUS,  DIK_I,       DIK_G,      DIK_SEMICOLON, DIK_X,        DIK_M     },
		{DIK_F3,     DIK_1,          DIK_0,      DIK_U,       DIK_F,      DIK_CAPITAL,   DIK_LWIN,     0x00      },
		{DIK_F2,     DIK_GRAVE,      DIK_9,      DIK_Y,       DIK_D,      DIK_LCONTROL,  DIK_LMENU,    DIK_SCROLL},
		{DIK_F1,     DIK_RETURN,     DIK_8,      DIK_T,       DIK_S,      0x00,          DIK_RWIN,     DIK_APPS  },
		{DIK_DECIMAL,DIK_F10,        DIK_NUMPAD4,DIK_NUMPAD6, DIK_NUMPAD5,DIK_INSERT,    DIK_HOME,     DIK_PGUP  },
		{0x00,       DIK_NUMPAD0,    DIK_NUMPAD1,DIK_NUMPAD3, DIK_NUMPAD2,DIK_SYSRQ,     DIK_TAB,      DIK_PAUSE },
	};
	if( YX_type == 0 )	//for D (without new keys)
	{
		key_map[9][6] = 0x00;
		key_map[11][6] = 0x00;
		key_map[11][7] = 0x00;
	}

	switch( addr ) {
		case	0x4207:	//YuXing_Keyboard code by tpu
			int r, i, data;
			data = 0;
			for(r=0; r<14; r++){
				if(key_map_row&(1<<r)){
					for(i=0; i<8; i++){
						if(key_map[r][i])
							if(DirectInput.m_Sw[key_map[r][i]]) data |= 1<<i;
						//special cases
						if((r== 0)&&(i==7))
							if(DirectInput.m_Sw[DIK_RSHIFT   ]) data |= 1<<i;
						if((r==10)&&(i==5))
							if(DirectInput.m_Sw[DIK_RCONTROL ]) data |= 1<<i;
						if((r==10)&&(i==6))
							if(DirectInput.m_Sw[DIK_RMENU    ]) data |= 1<<i;
					}
				}
			}
			return data;
			break;

		case	0x5002:
			return reg[0];
			break;

		case	0x4304:	//FDC��״̬�Ĵ���(STATUS)
//			DEBUGOUT( "Read  0x4304 = %04X\n", nFdcMainStatus );
			return nFdcMainStatus;
			break;
		case	0x4305:	//FDC������???
			if(read_mode){
				read_mode1++;
				if(read_mode1==512) read_mode=0;
				if(read_mode1==512) nnn++;
//				if(read_mode1==512) DEBUGOUT( "nnn = %04X\n", nnn );
//				if(nFdcRecord==0x12) nFdcMainStatus = FDC_MS_RQM;
				reg[6] = *(pFdcDataPtr++);
				return reg[6];
			}
			DEBUGOUT( "Read  0x4305 = %04X\n", bFdcResults[bFdcCycle] );
			read_mode1 = 0;
			reg[6] = bFdcResults[bFdcCycle];
			bFdcCycle++;
			if (bFdcCycle == pFdcCmd->bRLength)
			{
				bFdcCycle = 0;
				bFdcPhase = FDC_PH_IDLE;
				nFdcMainStatus &= ~FDC_MS_DATA_IN;
				nFdcMainStatus |= FDC_MS_RQM;
//				if((bFdcCommands[0]&0x0F)==0x06) read_mode1 = 1;
			}
			return reg[6];
			break;

	}

	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];*/
	return Mapper::ReadLow( addr );
}

void	Mapper169::WriteLow( WORD addr, BYTE data )
{
	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;

//	if(addr>0x4207) DEBUGOUT( "WriteL A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x4200://DCR
			DEBUGOUT( "Write 0x4200 = %04X\n", data );
			//
			break;
		case	0x4201://FDC���������Ĵ���(DOR)(���ֿ��ƼĴ���)
			DEBUGOUT( "Write 0x4201 = %04X\n", data );
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
/*
			if (data & 0x80)
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
*/
			break;
		case	0x4205:	//FDCд����???
			DEBUGOUT( "Write 0x4205 = %04X\n", data );
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
					pFdcCmd = &FdcCmdTable[data & FDC_CC_MASK];
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

		case	0x4202:	//YuXing_Keyboard
			key_map_row &= 0xff00;
			key_map_row |= data;
			break;
		case	0x4203:	//YuXing_Keyboard
		case	0x4303:
			key_map_row &= 0x00ff;
			key_map_row |= (data & 0x3f)<<8;
			break;

		case	0x4800:
			reg[1] = data;
			cmd_4800_6 = reg[1]&0x20;	//��ȡ��6λ��RAM���أ�1�򿪣�0�ر�
			cmd_4800_7 = reg[1]&0x40;	//��ȡ��7λ
			cmd_4800_8 = reg[1]&0x80;	//��ȡ��8λ�����ڿ���PPU_ExtLatch

			SetBank_CPU();
			break;
		case	0x5500:
			reg[2] = data;
			cmd_5500_3 = reg[2]&0x04;	//��ȡ��3λ��������RAM�й�
			cmd_5500_8 = reg[2]&0x80;	//��ȡ��8λ�����ڿ���PPU_Latch

			SetBank_CPU();
			break;
		case	0x5501:
			reg[3] = data;
			cmd_5501_8 = reg[3]&0x80;	//��ȡ��8λ������MMC3ģʽ�����á����滭�塱ѡ��(???)

			SetYCRAM_8K_Bank( reg[3] & 0x7F );
			if(cmd_5501_8){
				MMC3_mode = 1;
				SetYWRAM_16K_Bank( 6, 0x1F );
			}else{
				MMC3_mode = 0;
			}
			break;
	}
}

void	Mapper169::Write( WORD addr, BYTE data )
{

	if(cmd_5500_3 && MMC3_mode){
		MMC3_WriteH( addr, data );
		return;
	}

	if(cmd_5500_3){
		if(cmd_4800_6){
			reg[5] = data&0x3F;
		}else{
			WriteCPU_PRAM( addr, data );
		}
	}

}

void	Mapper169::SetBank_CPU()
{
	if(cmd_5500_3 && MMC3_mode) return;

	if(cmd_5500_3){
		SetYWRAM_16K_Bank( 4, reg[5] );
		SetYWRAM_16K_Bank( 6, 0x3F );
	}else{
		SetPROM_32K_Bank(reg[1]&0x1F);
	}

//	DEBUGOUT( "reg[2] = %04X\n", reg[2] );

	SetPROM_Bank( 3, &YSRAM[0x2000*(reg[2]&0x3)], BANKTYPE_RAM );
//	CPU_MEM_PAGE[3]=reg[2]&0x3;

	if((reg[2]&0x3)==0) SetPROM_Bank( 3, &YWRAM[0x78000], BANKTYPE_RAM );

}

void	Mapper169::WriteCPU_PRAM( WORD addr, BYTE data )
{
	if(addr<0xC000){
		YWRAM[(addr-0x8000)+(reg[5]*0x4000)] = data;
	}else{
		YWRAM[addr+0xF0000] = data;	//HOME BANK
	}
}

void	Mapper169::PPU_Latch( WORD addr )
{
	if((addr&0xF000)==0x2000){
		reg[4]=(addr>>8)&0x03;
		if(cmd_4800_8 && cmd_5500_8){
			SetYCRAM_8K_Bank((reg[4]<<2)+(reg[3]&0x03));
			SetYCRAM_4K_Bank(0,reg[3]<<1);
		}
	}
/*
	if(DirectInput.m_Sw[DIK_PAUSE]){
		nes->Dump_YWRAM();
		nes->Dump_YSRAM();
		nes->Dump_CPULMEM();
		nes->Dump_CPUHMEM();
	}
*/
}
void	Mapper169::PPU_ExtLatchX( INT x )
{
	a3 = (x&1)<<3;
}
void	Mapper169::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	INT loopy_v = nes->ppu->GetPPUADDR();
	INT loopy_y = nes->ppu->GetTILEY();
	INT	tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT	attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT	attrsft = (ntbladr&0x0040)>>4;
	LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT	ntbl_x  = ntbladr&0x001F;
	INT	tileadr;
	attradr &= 0x3FF;
	attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;
	if(cmd_4800_8 && !cmd_5500_8){
		if(reg[4]&0x02)	tileadr |=  0x1000;
		else			tileadr &= ~0x1000;
		tileadr = (tileadr&0xfff7)|a3;
		chr_l = chr_h = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	}else{
		chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
		chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
	}
}
BYTE	Mapper169::PPU_ExtLatchSP()	//��������������ɫ���⣬δ��ȫ
{
	if(cmd_4800_8 && !cmd_5500_8)
		return 2;	//��ʱ���˴���������ԭ��δ֪�����п�����VirtuaNES���������⡣
	return 0;
}

void	Mapper169::MMC3_WriteH( WORD addr, BYTE data )
{
	switch(addr) {
		case	0x8000:
			MMC3_reg = data;
			MMC3_SetBank_CPU();
			MMC3_SetBank_PPU();
			break;
		case	0x8001:
			switch(MMC3_reg&0x07) {
				case	0x02:
					MMC3_chr4 = data;
					MMC3_SetBank_PPU();
					break;
				case	0x03:
					MMC3_chr5 = data;
					MMC3_SetBank_PPU();
					break;
				case	0x04:
					MMC3_chr6 = data;
					MMC3_SetBank_PPU();
					break;
				case	0x05:
					MMC3_chr7 = data;
					MMC3_SetBank_PPU();
					break;
				case	0x06:
					MMC3_prg0 = data;
					MMC3_SetBank_CPU();
					break;
				case	0x07:
					MMC3_prg1 = data;
					MMC3_SetBank_CPU();
					break;
			}
			break;
	}
}
void	Mapper169::MMC3_SetBank_CPU()
{
	SetYWRAM_32K_Bank( MMC3_prg0, MMC3_prg1, 0x3E, 0x3F );
}
void	Mapper169::MMC3_SetBank_PPU()
{
	SetYCRAM_1K_Bank( 4, MMC3_chr4&0x0F );
	SetYCRAM_1K_Bank( 5, MMC3_chr5&0x0F );
	SetYCRAM_1K_Bank( 6, MMC3_chr6&0x0F );
	SetYCRAM_1K_Bank( 7, MMC3_chr7&0x0F );
}

LPBYTE	Mapper169::Load_DiskIMG()
{
	FILE *fp = NULL;
	if( !(lpDisk = (LPBYTE)malloc( 0x168000 )) ) {
		//throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
	}
	if( !(fp = ::fopen( "YuXing.img", "rb" )) ) {
		LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
		::wsprintf( szErrorString, szErrStr, "YuXing.img" );
		///throw	szErrorString;
	}
	::fread(lpDisk, 0x168000, 1, fp);
	FCLOSE(fp);
	return lpDisk;
}

//for FDC code by fanoble
void Mapper169::FdcHardReset(void)
{
	DEBUGOUT( "FdcHardReset!!!\n" );
	bFdcDmaInt = FALSE;
	FdcSoftReset();
}
void Mapper169::FdcSoftReset(void)
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
void Mapper169::FdcReadData(Mapper169* thiz)
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
void Mapper169::FdcWriteData(Mapper169* thiz)
{
	DEBUGOUT( "FdcWriteData!!!\n" );
	thiz = thiz;
}

void Mapper169::FdcNop(Mapper169* thiz)
{
	DEBUGOUT( "FdcNop!!!\n" );
	thiz->nFDCStatus[0] = FDC_S0_IC1;
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
}
void Mapper169::FdcReadTrack(Mapper169* thiz)
{
	DEBUGOUT( "FdcReadTrack!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcSpecify(Mapper169* thiz)
{
	DEBUGOUT( "FdcSpecify!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcSenseDriveStatus(Mapper169* thiz)
{
	DEBUGOUT( "FdcSenseDriveStatus!!!\n" );
	thiz->bFdcResults[0] = thiz->nFDCStatus[3];
}

void Mapper169::FdcRecalibrate(Mapper169* thiz)
{
	DEBUGOUT( "FdcRecalibrate!!!\n" );
	thiz->nFdcCylinder = 0;	//��ͷ�ص�0�ŵ�
	thiz->nFDCStatus[0] |= FDC_S0_SE;
}
void Mapper169::FdcSenseIntStatus(Mapper169* thiz)
{
	DEBUGOUT( "FdcSenseIntStatus!!!\n" );
	thiz->bFdcResults[0] = thiz->nFDCStatus[0];
	thiz->bFdcResults[1] = thiz->nFdcCylinder;
}
void Mapper169::FdcWriteDeletedData(Mapper169* thiz)
{
	DEBUGOUT( "FdcWriteDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcReadID(Mapper169* thiz)
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
void Mapper169::FdcReadDeletedData(Mapper169* thiz)
{
	DEBUGOUT( "FdcReadDeletedData!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcFormatTrack(Mapper169* thiz)
{
	DEBUGOUT( "FdcFormatTrack!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcSeek(Mapper169* thiz)
{
	DEBUGOUT( "FdcSeek!!!\n" );
	BYTE US;
	US = thiz->bFdcCommands[1] & 3;
	thiz->nFdcHeadAddres = ( thiz->bFdcCommands[1] & 7 ) >> 2;
	thiz->nFdcCylinder = thiz->bFdcCommands[2];
	thiz->nFDCStatus[0] = FDC_S0_SE;
}
void Mapper169::FdcScanEqual(Mapper169* thiz)
{
	DEBUGOUT( "FdcScanEqual!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcScanLowOrEqual(Mapper169* thiz)
{
	DEBUGOUT( "FdcScanLowOrEqual!!!\n" );
	thiz = thiz;
}
void Mapper169::FdcScanHighOrEqual(Mapper169* thiz)
{
	DEBUGOUT( "FdcScanHighOrEqual!!!\n" );
	thiz = thiz;
}
