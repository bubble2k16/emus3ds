//////////////////////////////////////////////////////////////////////////
// BoardCoolBoy                                                         //
//////////////////////////////////////////////////////////////////////////

//code by CaH4e3 from fceumm

void	BoardCoolBoy::Reset()
{
	if(!(VROM_8K_SIZE)) SetYCRAM_8K_Bank(0);

	EXPREGS[0] = EXPREGS[1] = EXPREGS[2] = EXPREGS[3] = 0;
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

	FixCBMMC3PRG(MMC3cmd);
	FixCBMMC3CHR(MMC3cmd);
}

void	BoardCoolBoy::WriteLow( WORD addr, BYTE data )
{
	if( addr >= 0x5000 && addr <= 0x5FFF ) XRAM[addr-0x4000] = data;
	if(addr>=0x6000)
	{
		if(A001B&0x80) CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
		if ((EXPREGS[3] & 0x90) != 0x80) { 
			EXPREGS[addr & 3] = data;
			FixCBMMC3PRG(MMC3cmd);
			FixCBMMC3CHR(MMC3cmd);
		}
	}
}

void	BoardCoolBoy::Write( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0x8000:
		if ((data & 0x40) != (MMC3cmd & 0x40))
			FixCBMMC3PRG(data);
		if ((data & 0x80) != (MMC3cmd & 0x80))
			FixCBMMC3CHR(data);
		MMC3cmd = data;
		break;
	case 0x8001:
	{
		int cbase = (MMC3cmd & 0x80) << 5;
		DRegBuf[MMC3cmd & 0x7] = data;
		switch (MMC3cmd & 0x07) {
		case 0:
			PPUSW((cbase ^ 0x000), data & (~1));
			PPUSW((cbase ^ 0x400), data | 1);
			break;
		case 1:
			PPUSW((cbase ^ 0x800), data & (~1));
			PPUSW((cbase ^ 0xC00), data | 1);
			break;
		case 2:
			PPUSW(cbase ^ 0x1000, data);
			break;
		case 3:
			PPUSW(cbase ^ 0x1400, data);
			break;
		case 4:
			PPUSW(cbase ^ 0x1800, data);
			break;
		case 5:
			PPUSW(cbase ^ 0x1C00, data);
			break;
		case 6:
			if (MMC3cmd&0x40) CPUSW(0xC000, data);
			else		   CPUSW(0x8000, data);
			break;
		case 7:
			CPUSW(0xA000, data);
			break;
		}
		break;
	}
	case 0xA000:
		A000B = data;
		if (!nes->rom->Is4SCREEN()) {
			if (A000B & 0x01)	SetVRAM_Mirror(VRAM_HMIRROR);
			else		SetVRAM_Mirror(VRAM_VMIRROR);
		}
		break;
	case 0xA001:
		A001B = data;
		break;
	case 0xC000: IRQReload = 0;IRQCount = data; break;
	case 0xC001: IRQReload = 0;IRQLatch = data; break;
	case 0xE000: IRQReload = 0;IRQa = 0;nes->cpu->ClrIRQ(IRQ_MAPPER); break;
	case 0xE001: IRQReload = 0;IRQa = 1; break;
	}

}

void	BoardCoolBoy::PPUSW(WORD A, BYTE V)
{
	uint32 mask = 0xFF ^ (EXPREGS[0] & 0x80);
	if (EXPREGS[3] & 0x10) {
		if (EXPREGS[3] & 0x40) {
			int cbase = (MMC3cmd & 0x80) << 5;
			switch (cbase ^ A) {
			case 0x0400:
			case 0x0C00: V &= 0x7F; break;
			}
		}
		SetYCRAM_1K_Bank(A>>10, 
			(V & 0x80 & mask) | ((((EXPREGS[0] & 0x08) << 4) & ~mask))
			| ((EXPREGS[2] & 0x0F) << 3)
			| ((A >> 10) & 7)
		);
	} else {
		if (EXPREGS[3] & 0x40) {
			int cbase = (MMC3cmd & 0x80) << 5;
			switch (cbase ^ A) {
			case 0x0000: V = DRegBuf[0]; break;
			case 0x0800: V = DRegBuf[1]; break;
			case 0x0400:
			case 0x0C00: V = 0; break;
			}
		}
		SetYCRAM_1K_Bank(A>>10, (V & mask) | (((EXPREGS[0] & 0x08) << 4) & ~mask));
	}
}

void	BoardCoolBoy::CPUSW(WORD A, BYTE V)
{
	uint32 mask = ((0x3F | (EXPREGS[1] & 0x40) | ((EXPREGS[1] & 0x20) << 2)) ^ ((EXPREGS[0] & 0x40) >> 2)) ^ ((EXPREGS[1] & 0x80) >> 2);
	uint32 base = ((EXPREGS[0] & 0x07) >> 0) | ((EXPREGS[1] & 0x10) >> 1) | ((EXPREGS[1] & 0x0C) << 2) | ((EXPREGS[0] & 0x30) << 2);

	if ((EXPREGS[3] & 0x40) && (V >= 0xFE) && !((MMC3cmd & 0x40) != 0)) {
		switch (A & 0xE000) {
		case 0xA000:
			if ((MMC3cmd & 0x40)) V = 0;
			break;
		case 0xC000:
			if (!(MMC3cmd & 0x40)) V = 0;
			break;
		case 0xE000:
			V = 0;
			break;
		}
	}

	if (!(EXPREGS[3] & 0x10))
		SetPROM_8K_Bank(A>>13, (((base << 4) & ~mask)) | (V & mask));
	else {
		mask &= 0xF0;
		uint8 emask;
		if ((((EXPREGS[1] & 2) != 0)))
			emask = (EXPREGS[3] & 0x0C) | ((A & 0x4000) >> 13);
		else
			emask = EXPREGS[3] & 0x0E;
		SetPROM_8K_Bank(A>>13, ((base << 4) & ~mask)
			| (V & mask)
			| emask
			| ((A & 0x2000) >> 13));
	}
}

void	BoardCoolBoy::FixCBMMC3PRG(BYTE data)
{
	if (data & 0x40) {
		CPUSW(0xC000, DRegBuf[6]);
		CPUSW(0x8000, ~1);
	} else {
		CPUSW(0x8000, DRegBuf[6]);
		CPUSW(0xC000, ~1);
	}
	CPUSW(0xA000, DRegBuf[7]);
	CPUSW(0xE000, ~0);
}

void	BoardCoolBoy::FixCBMMC3CHR(BYTE data)
{
	int cbase = (data & 0x80) << 5;
	PPUSW((cbase ^ 0x000), DRegBuf[0] & (~1));
	PPUSW((cbase ^ 0x400), DRegBuf[0] | 1);
	PPUSW((cbase ^ 0x800), DRegBuf[1] & (~1));
	PPUSW((cbase ^ 0xC00), DRegBuf[1] | 1);
	PPUSW(cbase ^ 0x1000, DRegBuf[2]);
	PPUSW(cbase ^ 0x1400, DRegBuf[3]);
	PPUSW(cbase ^ 0x1800, DRegBuf[4]);
	PPUSW(cbase ^ 0x1c00, DRegBuf[5]);
}

void	BoardCoolBoy::HSync(INT scanline)
{
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

