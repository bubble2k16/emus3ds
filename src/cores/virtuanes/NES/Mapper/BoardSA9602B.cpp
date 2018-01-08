////////////////////////////////////////////////////////////////////////////
// BoardSA9602B Mei Shao Nv Meng Gong Chang (Princess Maker)(Sachen)(Unl)[!] //
////////////////////////////////////////////////////////////////////////////

void	BoardSA9602B::Reset()
{
	CRAM_sav = 3 ;
	memcpy(&CRAM[CRAM_sav*0x2000], &WRAM[0], 0x2000);
	reg[0] = 0;
	reg[1] = 0;
	IRQCount = IRQLatch = IRQa = MMC3cmd = 0;
	DRegBuf[0] = 0;
	DRegBuf[1] = 2;
	DRegBuf[2] = 4;
	DRegBuf[3] = 5;
	DRegBuf[4] = 6;
	DRegBuf[5] = 7;
	DRegBuf[6] = 0;
	DRegBuf[7] = 1;
	A000B = 0;
	UpdatePrg(0);
	UpdateChr(0);
}

void	BoardSA9602B::Write( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0x8000:
		reg[0] = data;
		if ((data & 0x40) != (MMC3cmd & 0x40))
			UpdatePrg(data);
		if ((data & 0x80) != (MMC3cmd & 0x80))
			UpdateChr(data);
		MMC3cmd = data;
		break;
	case 0x8001:
	{
		if ((reg[0] & 7) < 6) {
			reg[1] = data >> 6;
			UpdatePrg(MMC3cmd);
		}
		int cbase = (MMC3cmd & 0x80) << 5;
		DRegBuf[MMC3cmd & 0x7] = data;
		switch (MMC3cmd & 0x07) {
		case 0:
			UpdateChr((cbase ^ 0x000), data & (~1));
			UpdateChr((cbase ^ 0x400), data | 1);
			break;
		case 1:
			UpdateChr((cbase ^ 0x800), data & (~1));
			UpdateChr((cbase ^ 0xC00), data | 1);
			break;
		case 2:
			UpdateChr(cbase ^ 0x1000, data);
			break;
		case 3:
			UpdateChr(cbase ^ 0x1400, data);
			break;
		case 4:
			UpdateChr(cbase ^ 0x1800, data);
			break;
		case 5:
			UpdateChr(cbase ^ 0x1C00, data);
			break;
		case 6:
			if (MMC3cmd & 0x40)
				UpdatePrg(0xC000, data);
			else
				UpdatePrg(0x8000, data);
			break;
		case 7:
			UpdatePrg(0xA000, data);
			break;
		}
		break;
	}
	case 0xA000:
		A000B = data;
		if (!nes->rom->Is4SCREEN()) {
			if (A000B & 0x01) SetVRAM_Mirror(VRAM_HMIRROR);
			else			  SetVRAM_Mirror(VRAM_VMIRROR);
		}
		break;
	case 0xC000: IRQReload = 0;IRQCount = data; break;
	case 0xC001: IRQReload = 0;IRQLatch = data; break;
	case 0xE000: IRQReload = 0;IRQa = 0;nes->cpu->ClrIRQ(IRQ_MAPPER); break;
	case 0xE001: IRQReload = 0;IRQa = 1; break;
	}
}

void	BoardSA9602B::UpdatePrg(WORD addr, BYTE data)
{
	SetPROM_8K_Bank(addr>>13, (data&0x3F)|(reg[1]<<6));
	if (MMC3cmd & 0x40)
		SetPROM_8K_Bank(4, 0x3E);
	else
		SetPROM_8K_Bank(6, 0x3E);
	SetPROM_8K_Bank(7, 0x3F);
}

void	BoardSA9602B::UpdateChr(WORD addr, BYTE data)
{
		SetCRAM_1K_Bank(addr >> 10, data);
}

void	BoardSA9602B::UpdatePrg(BYTE data)
{
	if (data & 0x40) {
		UpdatePrg(0xC000, DRegBuf[6]);
		UpdatePrg(0x8000, ~1);
	}
	else {
		UpdatePrg(0x8000, DRegBuf[6]);
		UpdatePrg(0xC000, ~1);
	}
	UpdatePrg(0xA000, DRegBuf[7]);
	UpdatePrg(0xE000, ~0);
}

void	BoardSA9602B::UpdateChr(BYTE data)
{
	BYTE cbase = (data & 0x80) << 5;
	UpdateChr((cbase ^ 0x000), DRegBuf[0] & (~1));
	UpdateChr((cbase ^ 0x400), DRegBuf[0] | 1);
	UpdateChr((cbase ^ 0x800), DRegBuf[1] & (~1));
	UpdateChr((cbase ^ 0xC00), DRegBuf[1] | 1);
	UpdateChr(cbase ^ 0x1000, DRegBuf[2]);
	UpdateChr(cbase ^ 0x1400, DRegBuf[3]);
	UpdateChr(cbase ^ 0x1800, DRegBuf[4]);
	UpdateChr(cbase ^ 0x1c00, DRegBuf[5]);
	if (!nes->rom->Is4SCREEN()) {
		if (A000B & 0x01) SetVRAM_Mirror(VRAM_HMIRROR);
		else			  SetVRAM_Mirror(VRAM_VMIRROR);
	}
}

void	BoardSA9602B::HSync(INT scanline)
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
	memcpy(&WRAM[0], &CRAM[CRAM_sav*0x2000], 0x2000);
}

void	BoardSA9602B::SaveState( LPBYTE p )
{
	//
}

void	BoardSA9602B::LoadState( LPBYTE p )
{
	//
}
