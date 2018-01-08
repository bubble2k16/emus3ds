//////////////////////////////////////////////////////////////////////////
// Mapper121                                                            //
//////////////////////////////////////////////////////////////////////////

void	Mapper121::Reset()
{
	EXPREGS[3] = 0x80;
	EXPREGS[5] = 0;
	EXPREGS[0] = EXPREGS[1] = EXPREGS[2] = EXPREGS[4] = EXPREGS[6] = EXPREGS[7] = 0;

	IRQReload = IRQCount = IRQLatch = IRQa = 0;
	MMC3cmd = A000B = A001B = 0;

	DRegBuf[0] = 0;
	DRegBuf[1] = 2;
	DRegBuf[2] = 4;
	DRegBuf[3] = 5;
	DRegBuf[4] = 6;
	DRegBuf[5] = 7;
	DRegBuf[6] = 0;
	DRegBuf[7] = 1;

	Fix121MMC3PRG(MMC3cmd);
	Fix121MMC3CHR(MMC3cmd);
}

BYTE	Mapper121::ReadLow( WORD addr )
{
	DEBUGOUT( "MPRWR A=%04X L=%3d CYC=%d\n", addr&0xFFFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if((addr>=0x5000)&&(addr<=0x5FFF)) return EXPREGS[4];
//	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return	Mapper::ReadLow( addr );
}

void	Mapper121::WriteLow( WORD addr, BYTE data )
{
	DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if((addr>=0x5000)&&(addr<=0x5FFF)){
		const uint8 prot_array[4] = { 0x83, 0x83, 0x42, 0x00 };
		EXPREGS[4] = prot_array[data & 3];
		if ((addr & 0x5180) == 0x5180) {
			EXPREGS[3] = data;
			Fix121MMC3PRG(MMC3cmd);
			Fix121MMC3CHR(MMC3cmd);
		}
	}
//	if(addr>=0x6000) Mapper::WriteLow(addr, data);
}

void	Mapper121::Write( WORD addr, BYTE data )
{
	if(addr<0xA000){
	DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
		switch (addr & 0xE003) {
		case 0x8000:
			MMC3CMDWrite(addr, data);
			Fix121MMC3PRG(MMC3cmd);
			break;
		case 0x8001:
			EXPREGS[6] = ((data&1)<<5)|((data&2)<<3)|((data&4)<<1)|((data&8)>>1)|((data&0x10)>>3)|((data&0x20)>>5);
			if (!EXPREGS[7]) SetDATA();
			MMC3CMDWrite(addr, data);
			Fix121MMC3PRG(MMC3cmd);
			break;
		case 0x8003:
			EXPREGS[5] = data;
			SetDATA();
			MMC3CMDWrite(0x8000, data);
			Fix121MMC3PRG(MMC3cmd);
			break;
		}
	}else{
		MMC3MIRWrite(addr, data);
		MMC3IRQWrite(addr, data);
	}
}

void Mapper121::MMC3CMDWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
	case 0x8000:
		if ((V & 0x40) != (MMC3cmd & 0x40))
			Fix121MMC3PRG(V);
		if ((V & 0x80) != (MMC3cmd & 0x80))
			Fix121MMC3CHR(V);
		MMC3cmd = V;
		break;
	case 0x8001:
	{
		int cbase = (MMC3cmd & 0x80) << 5;
		DRegBuf[MMC3cmd & 0x7] = V;
		switch (MMC3cmd & 0x07) {
		case 0:
			PPUSW((cbase ^ 0x000), V & (~1));
			PPUSW((cbase ^ 0x400), V | 1);
			break;
		case 1:
			PPUSW((cbase ^ 0x800), V & (~1));
			PPUSW((cbase ^ 0xC00), V | 1);
			break;
		case 2:
			PPUSW(cbase ^ 0x1000, V);
			break;
		case 3:
			PPUSW(cbase ^ 0x1400, V);
			break;
		case 4:
			PPUSW(cbase ^ 0x1800, V);
			break;
		case 5:
			PPUSW(cbase ^ 0x1C00, V);
			break;
		case 6:
			if (MMC3cmd&0x40) CPUSW(0xC000, V);
			else		   CPUSW(0x8000, V);
			break;
		case 7:
			CPUSW(0xA000, V);
			break;
		}
		break;
	}
	}
}

void Mapper121::MMC3MIRWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
		case 0xA000:
			A000B = V;
			if (A000B & 0x01)	SetVRAM_Mirror(VRAM_HMIRROR);
			else				SetVRAM_Mirror(VRAM_VMIRROR);
			break;
		case 0xA001:
			A001B = V;
			break;
	}
}

void Mapper121::MMC3IRQWrite(WORD A, BYTE V)
{
	switch (A & 0xE001) {
		case 0xC000: IRQReload = 0;IRQCount = V; break;
		case 0xC001: IRQReload = 0;IRQLatch = V; break;
		case 0xE000: IRQReload = 0;IRQa = 0;nes->cpu->ClrIRQ(IRQ_MAPPER); break;
		case 0xE001: IRQReload = 0;IRQa = 1; break;
	}
}

void	Mapper121::SetDATA()
{
	switch (EXPREGS[5] & 0x3F) {
	case 0x20: EXPREGS[7] = 1; EXPREGS[0] = EXPREGS[6]; break;
	case 0x29: EXPREGS[7] = 1; EXPREGS[0] = EXPREGS[6]; break;
	case 0x26: EXPREGS[7] = 0; EXPREGS[0] = EXPREGS[6]; break;
	case 0x2B: EXPREGS[7] = 1; EXPREGS[0] = EXPREGS[6]; break;
	case 0x2C: EXPREGS[7] = 1; if (EXPREGS[6]) EXPREGS[0] = EXPREGS[6]; break;
	case 0x3C:
	case 0x3F: EXPREGS[7] = 1; EXPREGS[0] = EXPREGS[6]; break;
	case 0x28: EXPREGS[7] = 0; EXPREGS[1] = EXPREGS[6]; break;
	case 0x2A: EXPREGS[7] = 0; EXPREGS[2] = EXPREGS[6]; break;
	case 0x2F: break;
	default:   EXPREGS[5] = 0; break;
	}
}

void	Mapper121::PPUSW( WORD addr, BYTE data )
{
	if ((nes->rom->GetPROM_SIZE()*2)==nes->rom->GetVROM_SIZE()) {
		SetVROM_1K_Bank(addr>>10, data | ((EXPREGS[3] & 0x80) << 1));
	} else {
		if ((addr & 0x1000) == ((MMC3cmd & 0x80) << 5))
			SetVROM_1K_Bank(addr>>10, data | 0x100);
		else
			SetVROM_1K_Bank(addr>>10, data);
	}
}

void	Mapper121::CPUSW( WORD addr, BYTE data )
{
	if (EXPREGS[5] & 0x3F) {
		SetPROM_8K_Bank(addr>>13, (data & 0x1F) | ((EXPREGS[3] & 0x80) >> 2));
		SetPROM_8K_Bank(7, (EXPREGS[0]) | ((EXPREGS[3] & 0x80) >> 2));
		SetPROM_8K_Bank(6, (EXPREGS[1]) | ((EXPREGS[3] & 0x80) >> 2));
		SetPROM_8K_Bank(5, (EXPREGS[2]) | ((EXPREGS[3] & 0x80) >> 2));
	} else {
		SetPROM_8K_Bank(addr>>13, (data & 0x1F) | ((EXPREGS[3] & 0x80) >> 2));
	}
}

void	Mapper121::Fix121MMC3PRG(BYTE data)
{
	if (data & 0x40) {
		CPUSW(0xC000, DRegBuf[6]);
		CPUSW(0x8000, PROM_8K_SIZE-2);
	} else {
		CPUSW(0x8000, DRegBuf[6]);
		CPUSW(0xC000, PROM_8K_SIZE-2);
	}
	CPUSW(0xA000, DRegBuf[7]);
	CPUSW(0xE000, PROM_8K_SIZE-1);
}

void	Mapper121::Fix121MMC3CHR(BYTE data)
{
	int cbase = (data & 0x80) << 5;
	PPUSW((cbase ^ 0x0000), DRegBuf[0] & (~1));
	PPUSW((cbase ^ 0x0400), DRegBuf[0] | 1);
	PPUSW((cbase ^ 0x0800), DRegBuf[1] & (~1));
	PPUSW((cbase ^ 0x0C00), DRegBuf[1] | 1);
	PPUSW((cbase ^ 0x1000), DRegBuf[2]);
	PPUSW((cbase ^ 0x1400), DRegBuf[3]);
	PPUSW((cbase ^ 0x1800), DRegBuf[4]);
	PPUSW((cbase ^ 0x1c00), DRegBuf[5]);

	if (A000B & 0x01)	SetVRAM_Mirror(VRAM_HMIRROR);
	else				SetVRAM_Mirror(VRAM_VMIRROR);
}

void	Mapper121::HSync(INT scanline)
{
//	DEBUGOUT( "MPRWR L=%3d CYC=%d\n", nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if ((scanline >= 0 && scanline <= 239)) {
		if (nes->ppu->IsDispON())
		{
			if (IRQa && !IRQReload) {
				if (scanline == 0) {
					if (IRQCount) {
						IRQCount -= 1;
					}
				}
				if (!(IRQCount)){
					IRQReload = 0xFF;
					IRQCount = IRQLatch;
					nes->cpu->SetIRQ(IRQ_MAPPER);
				}
				IRQCount--;
			}
		}
	}
}

