//////////////////////////////////////////////////////////////////////////
// BoardYoko                                                            //
//////////////////////////////////////////////////////////////////////////

//code by CaH4e3 from fceumm

void	BoardYoko::Reset()
{
	dip = 2;
	dip = (dip + 1) & 3;
	mode = bank = 0;

	IRQCount = IRQa = 0;

	SetBank();

	INT BoardNo = NES_ROM_get_unifBoardID(nes->rom->GetBoardName());
	if(BoardNo==642) SPROM = 0;	//Master Fighter VI' (YOKO-Y1) [U][!]
	if(BoardNo==644) SPROM = 1;	//Master Fighter VI' (Unl,EJ-4003)

}

BYTE	BoardYoko::ReadLow( WORD addr )
{
	if((addr>=0x5000)&&(addr<=0x53FF)) return (((BYTE)(addr>>8)) & 0xFC) | dip;

	if((addr>=0x5400)&&(addr<=0x5FFF)) return low[addr & 3];

	if( addr >= 0x6000 ) return Mapper::ReadLow( addr );

	return	(BYTE)(addr>>8);
}

void	BoardYoko::WriteLow( WORD addr, BYTE data )
{
	if((addr>=0x5400)&&(addr<=0x5FFF)) low[addr & 3] = data;
}

void	BoardYoko::Write( WORD addr, BYTE data )
{
	if(SPROM==0){
		switch (addr & 0x8C17) {
		case 0x8000: bank = data; SetBank(); break;
		case 0x8400: mode = data; SetBank(); break;
		case 0x8800: IRQCount &= 0xFF00; IRQCount |= data; nes->cpu->ClrIRQ(IRQ_MAPPER); break;
		case 0x8801: IRQa = mode & 0x80; IRQCount &= 0xFF; IRQCount |= data << 8; break;
		case 0x8c00: reg[0] = data; SetBank(); break;
		case 0x8c01: reg[1] = data; SetBank(); break;
		case 0x8c02: reg[2] = data; SetBank(); break;
		case 0x8c10: reg[3] = data; SetBank(); break;
		case 0x8c11: reg[4] = data; SetBank(); break;
		case 0x8c16: reg[5] = data; SetBank(); break;
		case 0x8c17: reg[6] = data; SetBank(); break;
		}
	}else if(SPROM==1){
		switch (addr){
		case 0x9001: bank = data; SetBank(); break;
		case 0x9000: mode = (data&0xFE)+(~(data&1)); SetBank(); break;
		case 0xC000: IRQCount &= 0xFF00; IRQCount |= data; nes->cpu->ClrIRQ(IRQ_MAPPER); break;
		case 0xC001: IRQa = mode & 0x80; IRQCount &= 0xFF; IRQCount |= data << 8; break;
		case 0x8000:
		case 0x8C00: reg[0] = data; SetBank(); break;
		case 0x8C01: reg[1] = data; SetBank(); break;
		case 0x8C02: reg[2] = data; SetBank(); break;
		case 0xC002:
		case 0xA000: reg[3] = data; SetBank(); break;
		case 0xA001: reg[4] = data; SetBank(); break;
		case 0xB002: reg[5] = data; SetBank(); break;
		case 0xB003: reg[6] = data; SetBank(); break;
		}
	}
}

void	BoardYoko::SetBank() {
	if(mode&1)	SetVRAM_Mirror( VRAM_HMIRROR );
	else		SetVRAM_Mirror( VRAM_VMIRROR );
	SetVROM_2K_Bank(0, reg[3]);
	SetVROM_2K_Bank(2, reg[4]);
	SetVROM_2K_Bank(4, reg[5]);
	SetVROM_2K_Bank(6, reg[6]);
	if (mode & 0x10) {
		uint32 base = (bank & 8) << 1;
		SetPROM_8K_Bank(4, (reg[0] & 0x0f) | base);
		SetPROM_8K_Bank(5, (reg[1] & 0x0f) | base);
		SetPROM_8K_Bank(6, (reg[2] & 0x0f) | base);
		SetPROM_8K_Bank(7, 0x0f | base);
	} else {
		if (mode & 8)
			SetPROM_32K_Bank(bank >> 1);
		else {
			SetPROM_16K_Bank(4, bank);
			SetPROM_16K_Bank(6, 7);
		}
	}
}

void	BoardYoko::HSync( INT scanline )
{
	if( IRQa ) {
		if( IRQCount <= 113 ) {
			IRQa = 0;
			nes->cpu->SetIRQ( IRQ_TRIGGER );
		} else {
			IRQCount -= 113;
		}
	}
}
