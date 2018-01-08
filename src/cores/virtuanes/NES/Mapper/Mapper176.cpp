//////////////////////////////////////////////////////////////////////////
// Mapper176         ShuQiYu / HengGe / WaiXing                         //
//////////////////////////////////////////////////////////////////////////
void	Mapper176::Reset()
{
	if(PROM_16K_SIZE>32){
		SetPROM_32K_Bank( 0, 1, (PROM_8K_SIZE/2)-2, (PROM_8K_SIZE/2)-1 );	//For 1M byte Cartridge
	}else{
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
//	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if(VROM_1K_SIZE) SetVROM_8K_Bank( 0 );

	reg5000 = 0;
	reg5001 = 0;
	reg5010 = 0;
	reg5011 = 0;
	reg5013 = 0;
	reg5FF1 = 0;
	reg5FF2 = 0;
	we_sram  = 0;
	SBW = 0;
	sp_rom = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();

	if(crc == 0x095D8678	//[ES-0122] Shuang Yue Zhuan (C)
	|| crc == 0xD5F7AAEF){	//[ES-XXXX] Shen Feng Jian (C)
		sp_rom = 1;
		nes->SetSAVERAM_SIZE( 32*1024 );
	}

/*
	crc == 0x416C07A1		//[ES-1006] Meng Huan Zhi Xing IV (C) & WXN-梦幻之星	√
	crc == 0x94782FBD		//[ES-1057] San Guo Zhi - Xiong Ba Tian Xia (C)
	crc == 0xF9863ADF		//[ES-1066] Xi Chu Ba Wang (C)
	crc == 0xB511C04B		//[ES-1071] San Xia Wu Yi (C)
	crc == 0x1923A8C5		//[ES-1087] Shui Hu Shen Shou (C) & WXN-水浒神兽(fix)	√
	crc == 0x095D8678		//[ES-0122] Shuang Yue Zhuan (C)
	crc == 0x8f6ab5ac		//WXN-三国忠烈传		√
	crc == 0x99051cb5		//WXN-雄霸天下			√
	crc == 0xf29c8186		//WXN-大富翁2-上海大亨	√
	crc == 0xc768098b		//WXN-三侠五义			√
	crc == 0x49f22159		//WXN-超级大富翁		√
	crc == 0xf354d847		//WXN-格兰帝亚			√
	crc == 0x5ee2ef97		//WXN-帝国时代(fix)		√
	crc == 0x977d22c3		//WXN-破釜沉舟(fix)		√
	crc == 0xf1d803f3		//WXN-西楚霸王(fix)		√
	crc == 0x85dd49b6		//WXN-口袋金(fix)		√
	crc == 0x97b82f53		//WXN-爆笑三国(fix)		√
	crc == 0xce2ea530		//WXN-宠物翡翠(fix)		√
*/
}

BYTE	Mapper176::ReadLow( WORD addr )
{
	if(sp_rom==1){
		if( addr>=0x6000 ) {
			switch( we_sram ) {
				case 0xE4:
				case 0xEC: return WRAM[(addr&0x1FFF)+0x0000];
				case 0xE5:
				case 0xED: return WRAM[(addr&0x1FFF)+0x2000];
				case 0xE6:
				case 0xEE: return WRAM[(addr&0x1FFF)+0x4000];
				case 0xE7:
				case 0xEF: return WRAM[(addr&0x1FFF)+0x6000];
				default:   return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
			}
		}
	}
	return	Mapper::ReadLow( addr );
}

void	Mapper176::WriteLow( WORD addr, BYTE data )
{
//	DEBUGOUT("Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );

	switch( addr ) {
		case	0x5000:
			reg5000 = data;
			break;
		case	0x5001:			//[ES-1006] Meng Huan Zhi Xing IV (C)
			reg5001 = data;
			if(SBW) SetPROM_32K_Bank(reg5001);
			break;
		case	0x5010:
			reg5010 = data;
			if(reg5010==0x24) SBW=1;
			break;
		case	0x5011:
			reg5011 = data>>1;
			if(SBW)	SetPROM_32K_Bank(reg5011);
			break;
		case	0x5013:
			reg5013 = data;
			break;
		case	0x5ff1:
			reg5FF1 = data>>1;
			SetPROM_32K_Bank(reg5FF1);
			break;
		case	0x5ff2:
			reg5FF2 = data;
			SetVROM_8K_Bank(reg5FF2);
			break;
		}

	if(sp_rom==1){
		if( addr >= 0x6000 ) {
			switch( we_sram ) {
				case 0xE4:	//CPU_MEM_BANK
				case 0xEC:	//CPU_MEM_BANK
					WRAM[(addr&0x1FFF)+0x0000] = data;
					CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
					break;
				case 0xE5:	//SRAM
				case 0xED:	//SRAM
					WRAM[(addr&0x1FFF)+0x2000] = data;
					break;
				case 0xE6:
				case 0xEE:
					WRAM[(addr&0x1FFF)+0x4000] = data;
					break;
				case 0xE7:
				case 0xEF:
					WRAM[(addr&0x1FFF)+0x6000] = data;
					break;
				default:
					CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
					break;
			}
		}
	}else{
		if( addr >= 0x6000 ) {
//			if ( we_sram == 0 ){
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
//			}
		}
	}


}

void	Mapper176::Write( WORD addr, BYTE data )
{
//	DEBUGOUT("Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );

	if(addr==0xa000){
		data &= 0x03;
		if		( data == 0 )	SetVRAM_Mirror( VRAM_VMIRROR );
		else if	( data == 1 )	SetVRAM_Mirror( VRAM_HMIRROR );
		else if	( data == 2 )	SetVRAM_Mirror( VRAM_MIRROR4L );
		else					SetVRAM_Mirror( VRAM_MIRROR4H );
	}
	if(addr==0xa001){
//		we_sram = data & 0x03;
		we_sram = data;
	}
}

void	Mapper176::SaveState( LPBYTE p )
{
	p[0] = reg5000;
	p[1] = reg5001;
	p[2] = reg5010;
	p[3] = reg5011;
	p[4] = reg5013;
	p[5] = reg5FF1;
	p[6] = reg5FF2;
	p[7] = we_sram;
	p[8] = SBW;
	p[9] = sp_rom;
}

void	Mapper176::LoadState( LPBYTE p )
{
	reg5000 = p[0];
	reg5001 = p[1];
	reg5010 = p[2];
	reg5011 = p[3];
	reg5013 = p[4];
	reg5FF1 = p[5];
	reg5FF2 = p[6];
	we_sram = p[7];
	SBW = p[8];
	sp_rom = p[9];
}
