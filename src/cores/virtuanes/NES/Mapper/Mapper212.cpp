//////////////////////////////////////////////////////////////////////////
// Mapper212                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper212::Reset()
{
	SetPROM_32K_Bank(PROM_8K_SIZE-4,PROM_8K_SIZE-3,PROM_8K_SIZE-2,PROM_8K_SIZE-1);
	if(VROM_1K_SIZE) SetVROM_8K_Bank(0);
}

BYTE	Mapper212::ReadLow( WORD addr )
{
	return ~((addr&0x10)<<3);
}

void	Mapper212::Write( WORD addr, BYTE data )
{
	if (addr & 0x4000) {
		SetPROM_32K_Bank((addr >> 1) & 3);
	} else {
		SetPROM_16K_Bank(4, addr & 7);
		SetPROM_16K_Bank(6, addr & 7);
	}
	SetVROM_8K_Bank(addr & 7);
	if((addr>>3)&1)	SetVRAM_Mirror(VRAM_HMIRROR);
	else			SetVRAM_Mirror(VRAM_VMIRROR);
}
