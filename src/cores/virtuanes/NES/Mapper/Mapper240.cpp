//////////////////////////////////////////////////////////////////////////
// Mapper240  Gen Ke Le Zhuan                                           //
//////////////////////////////////////////////////////////////////////////

void    Mapper240::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	KT_Piracy = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x7EA0D0C3		//[KT-1001] Jing Ke Xin Zhuan (C)
	 || crc == 0xC73EE9E2		//[KT-1002] Sheng Huo Lie Zhuan (C)
	 || crc == 0x8B98AE98		//[KT-1013] Zhan Guo Feng Yun (C)
	 || crc == 0xEB628838) {	//[KT-1014] Xia Ke Chuan Qi (C)
		KT_Piracy = 1;
	}
	if ( KT_Piracy == 1 ) SetPROM_32K_Bank( 0 );
}

void	Mapper240::WriteLow( WORD addr, BYTE data )
{
//	DEBUGOUT( "WriteLow - addr= %04x ; dat= %03x\n", addr, data );

	if( addr>=0x4020 && addr<0x6000 ) {
		if ( KT_Piracy == 1 ) {
			if(addr==0x4801) SetPROM_32K_Bank( (data&0xF)>>1 );
			if(addr==0x4803) SetVROM_8K_Bank(data&0xF);
		}else{
			SetPROM_32K_Bank( (data&0xF0)>>4 );
			SetVROM_8K_Bank(data&0xF);
		}
	}

	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;

}

void	Mapper240::Write( WORD addr, BYTE data )
{
//	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
}
