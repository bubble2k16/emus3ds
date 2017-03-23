//////////////////////////////////////////////////////////////////////////
// Mapper012  Subor Computer V4.0                                       //
//////////////////////////////////////////////////////////////////////////


void	Mapper167::Reset()
{
	DWORD crc;

	regs[0] = 0;
	regs[1] = 0;
	regs[2] = 0;
	regs[3] = 0;

	crc = nes->rom->GetPROM_CRC();
	if(crc==0x82F1Fb96){
		// Subor Computer(Russia)
		rom_type = 1;
	}else{
		// Subor Computer(Chinese)
		rom_type = 0;
	}

	SetBank_CPU();
	SetBank_PPU();
}

void Mapper167::Write(WORD addr, BYTE data)
{
	int idx;

	idx = (addr>>13)&0x03;
	regs[idx]=data;
	SetBank_CPU();
	SetBank_PPU();
//	DEBUGOUT("write to %04x:%02x\n", addr, data);
}


void	Mapper167::SetBank_CPU()
{
	int base, bank;

	base = ((regs[0]^regs[1])&0x10)<<1;
	bank = (regs[2]^regs[3])&0x1f;
	
	if(regs[1]&0x08){
		bank &= 0xfe;
		if(rom_type==0){
			SetPROM_16K_Bank(4, base+bank+1);
			SetPROM_16K_Bank(6, base+bank+0);
		}else{
			SetPROM_16K_Bank(6, base+bank+1);
			SetPROM_16K_Bank(4, base+bank+0);
		}
//		DEBUGOUT("32K MODE!\n");
	}else{
		if(regs[1]&0x04){
			SetPROM_16K_Bank(4, 0x1f);
			SetPROM_16K_Bank(6, base+bank);
//			DEBUGOUT("HIGH 16K MODE!\n");
		}else{
			SetPROM_16K_Bank(4, base+bank);
			if(rom_type==0){
				SetPROM_16K_Bank(6, 0x20);
			}else{
				SetPROM_16K_Bank(6, 0x07);
			}
//			DEBUGOUT("LOW  16K MODE!\n");
		}
	}
	

}

void	Mapper167::SetBank_PPU()
{
	SetCRAM_8K_Bank(0);
}

void	Mapper167::SaveState( LPBYTE p )
{
	p[0] = regs[0];
	p[1] = regs[1];
	p[2] = regs[2];
	p[3] = regs[3];
	p[4] = rom_type;
}

void	Mapper167::LoadState( LPBYTE p )
{
	regs[0] = p[0];
	regs[1] = p[1];
	regs[2] = p[2];
	regs[3] = p[3];
	rom_type = p[4];
}