//////////////////////////////////////////////////////////////////////////
// Mapper163  NanJing Games                                             //
//////////////////////////////////////////////////////////////////////////
void Mapper163::Reset()
{
	/*
	reg[1] = 0xFF;
	strobe = 1;
	security = trigger = reg[0] = 0x00;
	rom_type = 0;
	SetPROM_32K_Bank(15);

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0xb6a10d5d ) {	// Hu Lu Jin Gang (NJ039) (Ch) [dump]
		SetPROM_32K_Bank(0);
	}
	if( crc == 0xf52468e7 ) {	// San Guo Wu Shuang - Meng Jiang Zhuan (NJ047) (Ch) [dump]
		rom_type = 1;
	}
	*/
	memset(reg,0,8);
	laststrobe=1;
	SetPROM_32K_Bank((reg[0]<<4)|(reg[1]&0xF));
	//SetPROM_32K_Bank(0);
	SetCRAM_8K_Bank(0);
}

BYTE Mapper163::ReadLow( WORD A )
{
	/*
	if((addr>=0x5000 && addr<0x6000))
	{
		switch (addr & 0x7700)
		{
			case 0x5100:
					return security;
					break;
			case 0x5500:
					if(trigger)
						return security;
					else
						return 0;
					break;
		}
		return 4;
	}
	else if( addr>=0x6000 ) {
		return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}
	return Mapper::ReadLow( addr );
	*/
	if( (A<0x6000)&&(A>=0x5000) )
	{
		switch (A&0x7700)
		{
		case 0x5100: return reg[2]|reg[0]|reg[1]|reg[3]^0xff; break;
		case 0x5500: if(trigger)
						 return reg[2]|reg[1]; // Lei Dian Huang Bi Ka Qiu Chuan Shuo (NJ046) may broke other games
					 else
						 return 0;
		}
		return 4;
	}
	else 
		return Mapper::ReadLow(A);
}

void Mapper163::WriteLow(WORD A, BYTE V)
{
	/*
	if((addr>=0x4020 && addr<0x6000))
	{
		DEBUGOUT("W %.4X %.2X\n",addr,data);
		if(addr==0x5101){
			if(strobe && !data){
				trigger ^= 1;
//				trigger ^= 0xFF;
			}
			strobe = data;
		}else if(addr==0x5100 && data==6){
			SetPROM_32K_Bank(3);
		}
		else{
			switch (addr & 0x7300)
			{
				case 0x5000:
						reg[1]=data;
						SetPROM_32K_Bank( (reg[1] & 0xF) | (reg[0] << 4) );
						if(!(reg[1]&0x80)&&(nes->ppu->GetScanlineNo()<128))
							SetCRAM_8K_Bank(0);
						if(rom_type==1) SetCRAM_8K_Bank(0);
						break;
				case 0x5200:
						reg[0]=data;
						SetPROM_32K_Bank( (reg[1] & 0xF) | (reg[0] << 4) );
						break;
				case 0x5300:
						security=data;
						break;
			}
		}
	}
	else if( addr>=0x6000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
	*/
	if( (A<0x6000)&&(A>=0x5000) )
	{
		if(A==0x5300)
		{
			A=A+1-1;
		}

		if(A==0x5101)
		{
			if(laststrobe&&!V)
			{
				trigger^=1;
			}
			laststrobe=V;
		}else if(A==0x5100&&V==6) //damn thoose protected games
			SetPROM_32K_Bank(3);
		else
		{
			switch (A&0x7300)
			{
			case 0x5200: reg[0]=V; SetPROM_32K_Bank( (reg[0]<<4)|reg[1]&0xF);SetCRAM_8K_Bank(0); break;
			case 0x5000: reg[1]=V; SetPROM_32K_Bank( (reg[0]<<4)|reg[1]&0xF);SetCRAM_8K_Bank(0); if(!(reg[1]&0x80)&&(nes->GetScanline()<128)) SetCRAM_8K_Bank(0); break;
			case 0x5300: reg[2]=V; break;
			case 0x5100: reg[3]=V; SetPROM_32K_Bank( (reg[0]<<4)|reg[1]&0xF);SetCRAM_8K_Bank(0); break;
			}
		}
	}	
	//071
	else if((A&0xFF00)==0x4800)
	{
		//512K×¨ÓÃ
		if(PROM_8K_SIZE==64)
		{
			reg[A&3]=V;
			if(A==0x4802)
				SetPROM_32K_Bank( ((reg[1]>>1)&3)| (reg[2]<<2) ); 
		}
	}
	else
		Mapper::WriteLow(A,V);
}

void	Mapper163::HSync(int scanline)
{
	//sline = scanline;
	/*
	if( (reg[1]&0x80) && nes->ppu->IsDispON() ) {
		if(scanline==127){
			SetCRAM_4K_Bank(0, 1);
			SetCRAM_4K_Bank(4, 1);
		}
		if (rom_type==1){
			if(scanline<127){
				SetCRAM_4K_Bank(0, 0);
				SetCRAM_4K_Bank(4, 0);
			}
		}else{
			if(scanline==239){
				SetCRAM_4K_Bank(0, 0);
				SetCRAM_4K_Bank(4, 0);
			}
		}
	}
	*/
    if(reg[1]&0x80)
    {
      if(scanline==239)
      {
		  SetCRAM_4K_Bank(0, 0);
		  SetCRAM_4K_Bank(4, 0);
      }
      else if(scanline==127)
      {
		  SetCRAM_4K_Bank(0, 1);
		  SetCRAM_4K_Bank(4, 1);
      }
    }
}

void	Mapper163::SaveState( LPBYTE p )
{
	//p[0] = reg[0];
	//p[1] = reg[1];
	for(int i=0;i<8;i++)
		p[i]=reg[i];
	p[8]=laststrobe;
	p[9]=trigger;
}

void	Mapper163::LoadState( LPBYTE p )
{

	//reg[0] = p[0];
	//reg[1] = p[1];
	for(int i=0;i<8;i++)
		reg[i]=p[i];
	laststrobe= p[8];
	trigger = p[9];
}
