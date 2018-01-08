//////////////////////////////////////////////////////////////////////////
// BoardOneBus                                                          //
//////////////////////////////////////////////////////////////////////////

//code by CaH4e3 from fceumm

void	BoardOneBus::Reset()
{
	inv_hack = 0;
	pcm_enable = pcm_irq = 0;
	pcm_clock = 0xE1;
	IRQReload = IRQCount = IRQa = 0;
	memset(cpu410x, 0x00, sizeof(cpu410x));
	memset(ppu201x, 0x00, sizeof(ppu201x));
	memset(apu40xx, 0x00, sizeof(apu40xx));
	SetBank();

	count = 0;
}

void	BoardOneBus::WriteExPPU( WORD addr, BYTE data )	//Write $2010-$201F
{
	ppu201x[addr&0x0F] = data;
	SetBank();
}

BYTE	BoardOneBus::ReadExAPU ( WORD addr )			//Read $4000-$403F
{
	uint8 result = nes->apu->Read(addr);
	switch (addr & 0x3f) {
	case 0x15:
		if (apu40xx[0x30] & 0x10) {
			result = (result & 0x7f) | pcm_irq;
		}
		break;
	}
	return result;
}

void	BoardOneBus::WriteExAPU( WORD addr, BYTE data )	//Write $4000-$403F
{
	apu40xx[addr & 0x3f] = data;
	switch (addr & 0x3f) {
	case 0x12:
		if (apu40xx[0x30] & 0x10) {
			pcm_addr = data << 6;
		}
		break;
	case 0x13:
		if (apu40xx[0x30] & 0x10) {
			pcm_size = (data << 4) + 1;
		}
		break;
	case 0x15:
		if (apu40xx[0x30] & 0x10) {
			pcm_enable = data & 0x10;
			if (pcm_irq) {
				nes->cpu->ClrIRQ(IRQ_MAPPER);
				pcm_irq = 0;
			}
			if (pcm_enable)
				pcm_latch = pcm_clock;
			data &= 0xef;
		}
		break;
	}
	nes->apu->Write(addr,data);
}

void	BoardOneBus::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x4100)&&(addr<=0x410F))					//Write $4100-$410F
	{
		switch (addr & 0xf) {
		case 0x1: IRQLatch = data & 0xfe; break;
		case 0x2: IRQReload = 1; break;
		case 0x3: nes->cpu->ClrIRQ(IRQ_MAPPER); IRQa = 0; break;
		case 0x4: IRQa = 1; break;
		default:
			cpu410x[addr & 0xf] = data;
			SetBank();
		}
	}
}

void	BoardOneBus::Write( WORD addr, BYTE data )
{
	switch (addr & 0xe001) {
	case 0x8000: mmc3cmd = (mmc3cmd & 0x38) | (data & 0xc7); SetBank(); break;
	case 0x8001:
	{
		switch (mmc3cmd & 7) {
		case 0: ppu201x[0x6] = data; SetBankPPU(); break;
		case 1: ppu201x[0x7] = data; SetBankPPU(); break;
		case 2: ppu201x[0x2] = data; SetBankPPU(); break;
		case 3: ppu201x[0x3] = data; SetBankPPU(); break;
		case 4: ppu201x[0x4] = data; SetBankPPU(); break;
		case 5: ppu201x[0x5] = data; SetBankPPU(); break;
		case 6: cpu410x[0x7] = data; SetBankCPU(); break;
		case 7: cpu410x[0x8] = data; SetBankCPU(); break;
		}
		break;
	}
	case 0xa000: mirror = data; SetBankPPU(); break;
	case 0xc000: IRQLatch = data & 0xfe; break;
	case 0xc001: IRQReload = 1; break;
	case 0xe000: nes->cpu->ClrIRQ(IRQ_MAPPER); IRQa = 0; break;
	case 0xe001: IRQa = 1; break;
	}
}

void	BoardOneBus::SetBank()
{
	SetBankCPU();
	SetBankPPU();
}

void	BoardOneBus::SetBankCPU()
{
	uint8 bankmode = cpu410x[0xb] & 7;
	uint8 mask = (bankmode == 0x7) ? (0xff) : (0x3f >> bankmode);
	uint32 block = ((cpu410x[0x0] & 0xf0) << 4) + (cpu410x[0xa] & (~mask));
	uint32 pswap = (mmc3cmd & 0x40) << 8;

	uint8 bank0 = cpu410x[0x7 ^ inv_hack];
	uint8 bank1 = cpu410x[0x8 ^ inv_hack];
	uint8 bank2 = (cpu410x[0xb] & 0x40) ? (cpu410x[0x9]) : (~1);
	uint8 bank3 = ~0;

	SetPROM_8K_Bank((0x8000^pswap)>>13, block | (bank0 & mask));
	SetPROM_8K_Bank(5, block | (bank1 & mask));
	SetPROM_8K_Bank((0xc000^pswap)>>13, block | (bank2 & mask));
	SetPROM_8K_Bank(7, block | (bank3 & mask));
}

void	BoardOneBus::SetBankPPU()
{
	static const uint8 midx[8] = { 0, 1, 2, 0, 3, 4, 5, 0 };
	uint8 mask = 0xff >> midx[ppu201x[0xa] & 7];
	uint32 block = ((cpu410x[0x0] & 0x0f) << 11) + ((ppu201x[0x8] & 0x70) << 4) + (ppu201x[0xa] & (~mask));
	uint32 cswap = (mmc3cmd & 0x80) << 5;

	uint8 bank0 = ppu201x[0x6] & (~1);
	uint8 bank1 = ppu201x[0x6] | 1;
	uint8 bank2 = ppu201x[0x7] & (~1);
	uint8 bank3 = ppu201x[0x7] | 1;
	uint8 bank4 = ppu201x[0x2];
	uint8 bank5 = ppu201x[0x3];
	uint8 bank6 = ppu201x[0x4];
	uint8 bank7 = ppu201x[0x5];

	SetOBCRAM_1K_Bank((0x0000^cswap)>>10, block | (bank0 & mask));
	SetOBCRAM_1K_Bank((0x0400^cswap)>>10, block | (bank1 & mask));
	SetOBCRAM_1K_Bank((0x0800^cswap)>>10, block | (bank2 & mask));
	SetOBCRAM_1K_Bank((0x0c00^cswap)>>10, block | (bank3 & mask));
	SetOBCRAM_1K_Bank((0x1000^cswap)>>10, block | (bank4 & mask));
	SetOBCRAM_1K_Bank((0x1400^cswap)>>10, block | (bank5 & mask));
	SetOBCRAM_1K_Bank((0x1800^cswap)>>10, block | (bank6 & mask));
	SetOBCRAM_1K_Bank((0x1c00^cswap)>>10, block | (bank7 & mask));

	if(mirror&0x01)	SetVRAM_Mirror(VRAM_HMIRROR);
	else			SetVRAM_Mirror(VRAM_VMIRROR);

}

void	BoardOneBus::HSync( INT scanline )
{
	if ((scanline >= 0 && scanline <= 239)) {
		if (nes->ppu->IsDispON())
		{
			uint32 count = IRQCount;
			if (!count || IRQReload) {
				IRQCount = IRQLatch;
				IRQReload = 0;
			} else
				IRQCount--;
			if (count && !IRQCount) {
				if (IRQa)
					nes->cpu->SetIRQ(IRQ_MAPPER);
			}
		}
	}
}

void	BoardOneBus::Clock( INT cycles )
{
	if (pcm_enable) {
		pcm_latch -= cycles;
		if (pcm_latch <= 0) {
			pcm_latch += pcm_clock;
			pcm_size--;
			if (pcm_size < 0) {
				pcm_irq = 0x80;
				pcm_enable = 0;
				nes->cpu->SetIRQ(IRQ_MAPPER);
			} else {
				uint16 addr = pcm_addr | ((apu40xx[0x30]^3) << 14);
				uint8 raw_pcm = CPU_MEM_BANK[addr>>13][addr&0x1FFF] >> 1;

//				YWRAM[count] = CPU_MEM_BANK[addr>>13][addr&0x1FFF];
//				count++;

				nes->apu->Write(0x4011, raw_pcm);
				pcm_addr++;
				pcm_addr &= 0x7FFF;
			}
		}
	}
}

void	BoardOneBus::PPU_Latch( WORD addr )
{
/*
	if(DirectInput.m_Sw[DIK_PAUSE]){
//		nes->Dump_YWRAM();
	}
*/
}

void	BoardOneBus::SaveState( LPBYTE p )
{
	//
}

void	BoardOneBus::LoadState( LPBYTE p )
{
	//
}
