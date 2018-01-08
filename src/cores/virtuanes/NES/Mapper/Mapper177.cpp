//////////////////////////////////////////////////////////////////////////
// Mapper177  HengGe                                                    //
//////////////////////////////////////////////////////////////////////////
void    Mapper177::Reset()
{
	SetPROM_32K_Bank( 0 );
	SP_rom = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if(crc==0xB5E83C9A) SP_rom=1;	//HengGe - Xing Ji Zheng Ba (C)
}

void	Mapper177::WriteLow( WORD addr, BYTE data )
{
	if((addr==0x4800)&&(SP_rom)){
		if(data& 0x01)	SetVRAM_Mirror( VRAM_HMIRROR );
		else			SetVRAM_Mirror( VRAM_VMIRROR );
	}
	if( addr>=0x6000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

void	Mapper177::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data );
	if(SP_rom==0){
		if(data&0x20)	SetVRAM_Mirror( VRAM_HMIRROR );
		else			SetVRAM_Mirror( VRAM_VMIRROR );
	}
}

