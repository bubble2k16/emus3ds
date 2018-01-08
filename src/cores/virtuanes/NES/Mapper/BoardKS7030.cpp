//////////////////////////////////////////////////////////////////////////
// BoardKS7030                                                          //
//////////////////////////////////////////////////////////////////////////

//code by CaH4e3 from fceumm

void	BoardKS7030::Reset()
{
	reg0 = reg1 = 0x1F;
	SetBank();
}

void	BoardKS7030::ExWrite( WORD addr, BYTE data )
{
	if(addr==0x4025)
		if(data&0x08) SetVRAM_Mirror( VRAM_HMIRROR );
		else		  SetVRAM_Mirror( VRAM_VMIRROR );
}

BYTE	BoardKS7030::ReadLow( WORD addr )
{
	if((addr>=0x6000)&&(addr<=0x7FFF))
	{
		if ((addr >= 0x6000) && (addr <= 0x6BFF)) {
			return DRAM[addr - 0x6000];
		} else if ((addr >= 0x6C00) && (addr <= 0x6FFF)) {
			return CPU_MEM_BANK[(0xC800+(addr-0x6C00))>>13][(0xC800+(addr-0x6C00))&0x1FFF];
		} else if ((addr >= 0x7000) && (addr <= 0x7FFF)) {
			return CPU_MEM_BANK[(0xB800+(addr-0x7000))>>13][(0xB800+(addr-0x7000))&0x1FFF];
		}
	}
	return Mapper::ReadLow( addr );
}

void	BoardKS7030::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x6000)&&(addr<=0x7FFF))
	{
		if ((addr >= 0x6000) && (addr <= 0x6BFF)) {
			DRAM[addr - 0x6000] = data;
		} else if ((addr >= 0x6C00) && (addr <= 0x6FFF)) {
			CPU_MEM_BANK[(0xC800+(addr-0x6C00))>>13][(0xC800+(addr-0x6C00))&0x1FFF] = data;
		} else if ((addr >= 0x7000) && (addr <= 0x7FFF)) {
			CPU_MEM_BANK[(0xB800+(addr-0x7000))>>13][(0xB800+(addr-0x7000))&0x1FFF] = data;
		}
	}
}

BYTE	BoardKS7030::Read( WORD addr)
{
	if((addr>=0xB800)&&(addr<=0xD7FF))
	{
		if ((addr >= 0xB800) && addr <= 0xBFFF) {
			return DRAM[0x0C00 + (addr - 0xB800)];
		} else if ((addr >= 0xC000) && addr <= 0xCBFF) {
			return CPU_MEM_BANK[(0xCC00+(addr-0xC000))>>13][(0xCC00+(addr-0xC000))&0x1FFF];
		} else if ((addr >= 0xCC00) && addr <= 0xD7FF) {
			return DRAM[0x1400 + (addr - 0xCC00)];
		}
	}
	return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
}

void	BoardKS7030::Write( WORD addr, BYTE data )
{
	if((addr>=0x8000)&&(addr<=0x8FFF))
	{
		reg0 = addr & 0x07;
		SetBank();
	}
	else if((addr>=0x9000)&&(addr<=0x9FFF))
	{
		reg1 = addr & 0x0F;
		SetBank();
	}
	else if((addr>=0xB800)&&(addr<=0xD7FF))
	{
		if ((addr >= 0xB800) && (addr <= 0xBFFF)) {
			DRAM[0x0C00 + (addr - 0xB800)] = data;
		} else if ((addr >= 0xC000) && (addr <= 0xCBFF)) {
			CPU_MEM_BANK[(0xCC00+(addr-0xC000))>>13][(0xCC00+(addr-0xC000))&0x1FFF] = data;
		} else if ((addr >= 0xCC00) && (addr <= 0xD7FF)) {
			DRAM[0x1400 + (addr - 0xCC00)] = data;
		}
	}
}

void	BoardKS7030::SetBank()
{
	SetPROM_32K_Bank(3);
	SetCRAM_8K_Bank(0);
	memcpy( &CPU_MEM_BANK[5][0x1800], PROM+(0x1000*(reg0%0x20))+0x0000, 0x0800);
	memcpy( &CPU_MEM_BANK[6][0x0000], PROM+(0x1000*(reg0%0x20))+0x0800, 0x0800);
	memcpy( &CPU_MEM_BANK[6][0x0800], PROM+(0x1000*((reg1+8)%0x20)), 0x1000);
}

