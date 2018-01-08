//////////////////////////////////////////////////////////////////////////
// Mapper150  SACHEN                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper150::Reset()
{
	for( INT i = 0; i < 5; i++ ) {
		reg[i] = 0x00;
	}
	cmd=0;
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

BYTE	Mapper150::ReadLow( WORD addr )
{
	BYTE ret;
	if((addr&0x4100)==0x4100)
//    ret=(X.DB&0xC0)|((~cmd)&0x3F);
		ret=~cmd&0x3F;
	else
		ret=0;//ret=X.DB;
	return ret;
}

void	Mapper150::WriteLow( WORD addr, BYTE data )
{
	addr&=0x4101;
	if(addr==0x4100)
		cmd=data&7;
	else
	{
		switch(cmd)
		{
			case 2:reg[0]=data&1; reg[3]=(data&1)<<3;break;
			case 4:reg[4]=(data&1)<<2;break;
			case 5:reg[0]=data&7;break;
			case 6:reg[1]=data&3;break;
			case 7:reg[2]=data>>1;break;
		}
		SetPROM_32K_Bank( reg[0] );
		SetVROM_8K_Bank( reg[1]|reg[3]|reg[4] );
		switch(reg[2]&3)
		{
			case 0:SetVRAM_Mirror( VRAM_VMIRROR );break;
			case 1:SetVRAM_Mirror( VRAM_HMIRROR );break;
			case 2:SetVRAM_Mirror( VRAM_MIRROR3H );break;
			case 3:SetVRAM_Mirror( VRAM_MIRROR4L );break;
		}
	}

}

