//////////////////////////////////////////////////////////////////////////
// Mapper162                                                            //
//////////////////////////////////////////////////////////////////////////


void	Mapper162::Reset()
{
	reg5000 = 0;
	reg5100 = 0;
	reg5200 = 0;
	reg5300 = 7;
	SetBank_CPU();
	SetBank_PPU();
}

void Mapper162::WriteLow(WORD addr, BYTE data)
{
	if(addr==0x5000){
		reg5000 = data;
		SetBank_CPU();
		SetBank_PPU();
	}else if(addr==0x5100){
		reg5100 = data;
		SetBank_CPU();
		SetBank_PPU();
	}else if(addr==0x5200){
		reg5200 = data;
		SetBank_CPU();
		SetBank_PPU();
	}else if(addr==0x5300){
		reg5300 = data;
	}else if(addr>=0x6000){
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}else{
		DEBUGOUT("write to %04x:%02x\n", addr, data);
	}

}

void	Mapper162::SetBank_CPU()
{
	BYTE bank;
	switch (reg5300)
	{
		case 4:
			bank = (( (reg5000 & 0xF) + ((reg5100 & 3) >> 1) ) | ((reg5200 & 1) << 4));
			break;
		case 7:
			bank = ((reg5000 & 0xF) | ((reg5200 & 1) << 4));
			break;
	}
	SetPROM_32K_Bank(bank);
}

void	Mapper162::SetBank_PPU()
{
	SetCRAM_8K_Bank(0);
}

void	Mapper162::HSync(int scanline)
{
	if( (reg5000&0x80) && nes->ppu->IsDispON() ) {
		if(scanline<127){
//			SetCRAM_4K_Bank(0, 0);
			SetCRAM_4K_Bank(4, 0);
		}else if(scanline<240){
//			SetCRAM_4K_Bank(0, 1);
			SetCRAM_4K_Bank(4, 1);
		}
	}
}


void	Mapper162::SaveState( LPBYTE p )
{
	p[0] = reg5000;
	p[1] = reg5100;
	p[2] = reg5200;
	p[3] = reg5300;
}

void	Mapper162::LoadState( LPBYTE p )
{

	reg5000 = p[0];
	reg5100 = p[1];
	reg5200 = p[2];
	reg5300 = p[3];
}