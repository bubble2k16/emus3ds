//////////////////////////////////////////////////////////////////////////
// BoardUNL158B Blood Of Jurassic (GD-98)                               //
//////////////////////////////////////////////////////////////////////////

static uint8 lut[8] = { 0x00, 0x00, 0x00, 0x01, 0x02, 0x04, 0x0F, 0x00 };

void	BoardUNL158B::Reset()
{
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

	Fix158BMMC3PRG(MMC3cmd);
	Fix158BMMC3CHR(MMC3cmd);

	nes->pad->SetExController( 3 );
}

BYTE	BoardUNL158B::ReadLow( WORD addr )
{
	if((addr>=0x5000)&&(addr<=0x5007)) return lut[addr&7];
	return Mapper::ReadLow( addr );
}

void	BoardUNL158B::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x5000)&&(addr<=0x5007)){
		EXPREGS[addr & 7] = data;
		switch(addr & 7) {
		case 0:
			Fix158BMMC3PRG(MMC3cmd);
			break;
		case 7:
			//
			break;
		}
	}
}
void	BoardUNL158B::Write( WORD addr, BYTE data )
{
	MMC3CMDWrite(addr, data);
	MMC3IRQWrite(addr, data);
}

void	BoardUNL158B::MMC3CMDWrite( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0x8000:
		if ((data & 0x40) != (MMC3cmd & 0x40))
			Fix158BMMC3PRG(data);
		if ((data & 0x80) != (MMC3cmd & 0x80))
			Fix158BMMC3CHR(data);
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
	}
}

void	BoardUNL158B::MMC3IRQWrite( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0xC000: IRQReload = 0;IRQCount = data; break;
	case 0xC001: IRQReload = 0;IRQLatch = data; break;
	case 0xE000: IRQReload = 0;IRQa = 0;nes->cpu->ClrIRQ(IRQ_MAPPER); break;
	case 0xE001: IRQReload = 0;IRQa = 1; break;
	}
}

void	BoardUNL158B::PPUSW(WORD A, BYTE V)
{
	SetVROM_1K_Bank(A>>10, V);
}

void	BoardUNL158B::CPUSW(WORD A, BYTE V)
{
	if (EXPREGS[0] & 0x80) {
		uint32 bank = EXPREGS[0] & 7; 
		if(EXPREGS[0] & 0x20) { // 32Kb mode
			SetPROM_32K_Bank(bank >> 1);
		} else {				// 16Kb mode
			SetPROM_16K_Bank(4, bank);
			SetPROM_16K_Bank(6, bank);
		}
	} else {
		SetPROM_8K_Bank(A>>13, V & 0xF);
	}
}

void	BoardUNL158B::Fix158BMMC3PRG(BYTE data)
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

void	BoardUNL158B::Fix158BMMC3CHR(BYTE data)
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

void	BoardUNL158B::HSync(INT scanline)
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

void	BoardUNL158B::SaveState( LPBYTE p )
{
	//
}

void	BoardUNL158B::LoadState( LPBYTE p )
{
	//
}
