//////////////////////////////////////////////////////////////////////////
// Mapper141                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper141::Reset()
{
	for(INT i=0;i<8;i++) reg[i]=0;
	SetVROM_8K_Bank(0);
	cmd=0;
	SetBank();
}

void	Mapper141::WriteLow( WORD addr, BYTE data )
{
	addr&=0x4101;
	if (addr==0x4100)
		cmd=data;
	else {
		reg[cmd&7]=data;
		SetBank();
	}
}

void	Mapper141::SetBank()
{
	SetPROM_32K_Bank(reg[5]&7);
	for(INT i=0;i<4;i++){
		int bank;
		if(reg[7]&1) bank=(reg[0]&7)|((reg[4]&7)<<3);
		else		 bank=(reg[i]&7)|((reg[4]&7)<<3);
		bank=(bank<<1)|(i&1);
		SetVROM_2K_Bank(i*2, bank);
	}
	if(reg[7]&1) SetVRAM_Mirror(VRAM_VMIRROR);
	else		 SetVRAM_Mirror(VRAM_HMIRROR);

}
