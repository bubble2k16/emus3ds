//////////////////////////////////////////////////////////////////////////
// BoardFK23C                                                         //
//////////////////////////////////////////////////////////////////////////

//code by CaH4e3 from fceumm

void	BoardFK23C::Reset()
{
	INT BoardNo = NES_ROM_get_unifBoardID(nes->rom->GetBoardName());
	if((BoardNo==379)||(nes->rom->GetMapperNo()==176)){		//BMC-FK23C
		EXPREGS[0] = 4;
		EXPREGS[1] = 0xFF;
		EXPREGS[2] = EXPREGS[3] = 0;
	}else if(BoardNo==635){	//BMC-FK23CA
		EXPREGS[0] = EXPREGS[1] = EXPREGS[2] = EXPREGS[3] = 0;
	}
	dipswitch = unromchr = 0;
	EXPREGS[4] = EXPREGS[5] = EXPREGS[6] = EXPREGS[7] = 0xFF;

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

	FixFK23MMC3PRG(MMC3cmd);
	FixFK23MMC3CHR(MMC3cmd);
}

void	BoardFK23C::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x5000)&&(addr<=0x5FFF))
	{
		if (dipswitch) {
			if (addr & (1 << (dipswitch + 3))) {
				EXPREGS[addr & 3] = data;
				FixFK23MMC3PRG(MMC3cmd);
				FixFK23MMC3CHR(MMC3cmd);
			}
		} else {
			EXPREGS[addr & 3] = data;
			FixFK23MMC3PRG(MMC3cmd);
			FixFK23MMC3CHR(MMC3cmd);
		}
		if (EXPREGS[3] & 2) EXPREGS[0] &= ~7;
	}
}

void	BoardFK23C::Write( WORD addr, BYTE data )
{
	if (EXPREGS[0] & 0x40) {
		if (EXPREGS[0] & 0x30)
			unromchr = 0;
		else {
			unromchr = data & 3;
			FixFK23MMC3CHR(MMC3cmd);
		}
	} else {
		if ((addr == 0x8001) && (EXPREGS[3] & 2) && (MMC3cmd & 8)) {
			EXPREGS[4 | (MMC3cmd & 3)] = data;
			FixFK23MMC3PRG(MMC3cmd);
			FixFK23MMC3CHR(MMC3cmd);
		} else
		if (addr < 0xC000) {
			if (!(VROM_1K_SIZE)) {
				if ((addr == 0x8000) && (data == 0x46))
					data = 0x47;
				else if ((addr == 0x8000) && (data == 0x47))
					data = 0x46;
			}
			MMC3CMDWrite(addr, data);
			FixFK23MMC3PRG(MMC3cmd);
		} else
			MMC3IRQWrite(addr, data);
	}
}

void	BoardFK23C::MMC3CMDWrite( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0x8000:
		if ((data & 0x40) != (MMC3cmd & 0x40))
			FixFK23MMC3PRG(data);
		if ((data & 0x80) != (MMC3cmd & 0x80))
			FixFK23MMC3CHR(data);
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

void	BoardFK23C::MMC3IRQWrite( WORD addr, BYTE data )
{
	switch (addr & 0xE001) {
	case 0xC000: IRQReload = 0;IRQCount = data; break;
	case 0xC001: IRQReload = 0;IRQLatch = data; break;
	case 0xE000: IRQReload = 0;IRQa = 0;nes->cpu->ClrIRQ(IRQ_MAPPER); break;
	case 0xE001: IRQReload = 0;IRQa = 1; break;
	}
}

void	BoardFK23C::PPUSW(WORD A, BYTE V)
{
	if (VROM_1K_SIZE){
		if (EXPREGS[0] & 0x40)
			SetVROM_8K_Bank(EXPREGS[2] | unromchr);
		else if (EXPREGS[0] & 0x20) {
			SetVROM_1K_Bank(A>>10, V);
		} else {
			uint16 base = (EXPREGS[2] & 0x7F) << 3;
			if (EXPREGS[3] & 2) {
				int cbase = (MMC3cmd & 0x80) << 5;
				SetVROM_1K_Bank(A>>10, V | base);
				SetVROM_1K_Bank((0x0000 ^ cbase)>>10, DRegBuf[0] | base);
				SetVROM_1K_Bank((0x0400 ^ cbase)>>10, EXPREGS[6] | base);
				SetVROM_1K_Bank((0x0800 ^ cbase)>>10, DRegBuf[1] | base);
				SetVROM_1K_Bank((0x0c00 ^ cbase)>>10, EXPREGS[7] | base);
			} else
				SetVROM_1K_Bank(A>>10, V | base);
		}
	}else{
		if (EXPREGS[0] & 0x40)
			SetYCRAM_8K_Bank(EXPREGS[2] | unromchr);
		else if (EXPREGS[0] & 0x20) {
			SetYCRAM_1K_Bank(A>>10, V);
		} else {
			uint16 base = (EXPREGS[2] & 0x7F) << 3;
			if (EXPREGS[3] & 2) {
				int cbase = (MMC3cmd & 0x80) << 5;
				SetYCRAM_1K_Bank(A>>10, V | base);
				SetYCRAM_1K_Bank((0x0000 ^ cbase)>>10, DRegBuf[0] | base);
				SetYCRAM_1K_Bank((0x0400 ^ cbase)>>10, EXPREGS[6] | base);
				SetYCRAM_1K_Bank((0x0800 ^ cbase)>>10, DRegBuf[1] | base);
				SetYCRAM_1K_Bank((0x0c00 ^ cbase)>>10, EXPREGS[7] | base);
			} else
				SetYCRAM_1K_Bank(A>>10, V | base);
		}
	}
}

void	BoardFK23C::CPUSW(WORD A, BYTE V)
{
	uint32 bank = (EXPREGS[1] & 0x1F);
	uint32 hiblock = ((EXPREGS[0] & 8) << 4) | ((EXPREGS[0] & 0x80) << 1) | ((!(VROM_1K_SIZE)) ? ((EXPREGS[2] & 0x40) << 3) : 0);
	uint32 block = (EXPREGS[1] & 0x60) | hiblock;
	uint32 extra = (EXPREGS[3] & 2);
	switch (EXPREGS[0] & 7) {
	case 0:
		SetPROM_8K_Bank(A>>13, (block << 1) | (V & 0x3F));
		if (extra) {
			SetPROM_8K_Bank(6, EXPREGS[4]);
			SetPROM_8K_Bank(7, EXPREGS[5]);
		}
		break;
	case 1:
		SetPROM_8K_Bank(A>>13, ((hiblock | (EXPREGS[1] & 0x70)) << 1) | (V & 0x1F));
		if (extra) {
			SetPROM_8K_Bank(6, EXPREGS[4]);
			SetPROM_8K_Bank(7, EXPREGS[5]);
		}
		break;
	case 2:
		SetPROM_8K_Bank(A>>13, ((hiblock | (EXPREGS[1] & 0x78)) << 1) | (V & 0x0F));
		if (extra) {
			SetPROM_8K_Bank(6, EXPREGS[4]);
			SetPROM_8K_Bank(7, EXPREGS[5]);
		}
		break;
	case 3:
		SetPROM_16K_Bank(4, (bank | block));
		SetPROM_16K_Bank(6, (bank | block));
		break;
	case 4:
		SetPROM_32K_Bank((bank | block) >> 1);
		break;
	}
	SetPROM_Bank( 3, &WRAM[0x2000*(A001B&3)], BANKTYPE_RAM );
}

void	BoardFK23C::FixFK23MMC3PRG(BYTE data)
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

void	BoardFK23C::FixFK23MMC3CHR(BYTE data)
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

void	BoardFK23C::HSync(INT scanline)
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

