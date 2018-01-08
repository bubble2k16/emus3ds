//////////////////////////////////////////////////////////////////////////
// BoardCityFighter                                                 //
//////////////////////////////////////////////////////////////////////////
void	BoardCityFighter::Reset()
{
	for(int n=0;n<8;n++) chr_reg[n] = 0x00;
	prg_reg = 0;
	prg_mode = 0;
	mirr = 0;
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;
	SetBank();
}

void	BoardCityFighter::Write( WORD addr, BYTE data )
{
	switch (addr & 0xF00C) {
		case 0x9000: prg_reg = data & 0xC; mirr = data & 3; break;
		case 0x9004:
		case 0x9008:
		case 0x900C:
			if (addr & 0x800)
				nes->apu->Write(0x4011,(data&0xf)<<3);
			else
				prg_reg = data & 0xC;
			break;
		case 0xC000:
		case 0xC004:
		case 0xC008:
		case 0xC00C: prg_mode = data & 1; break;
		case 0xD000: chr_reg[0] = (chr_reg[0] & 0xF0) | (data & 0x0F); break;
		case 0xD004: chr_reg[0] = (chr_reg[0] & 0x0F) | (data << 4); break;
		case 0xD008: chr_reg[1] = (chr_reg[1] & 0xF0) | (data & 0x0F); break;
		case 0xD00C: chr_reg[1] = (chr_reg[1] & 0x0F) | (data << 4); break;
		case 0xA000: chr_reg[2] = (chr_reg[2] & 0xF0) | (data & 0x0F); break;
		case 0xA004: chr_reg[2] = (chr_reg[2] & 0x0F) | (data << 4); break;
		case 0xA008: chr_reg[3] = (chr_reg[3] & 0xF0) | (data & 0x0F); break;
		case 0xA00C: chr_reg[3] = (chr_reg[3] & 0x0F) | (data << 4); break;
		case 0xB000: chr_reg[4] = (chr_reg[4] & 0xF0) | (data & 0x0F); break;
		case 0xB004: chr_reg[4] = (chr_reg[4] & 0x0F) | (data << 4); break;
		case 0xB008: chr_reg[5] = (chr_reg[5] & 0xF0) | (data & 0x0F); break;
		case 0xB00C: chr_reg[5] = (chr_reg[5] & 0x0F) | (data << 4); break;
		case 0xE000: chr_reg[6] = (chr_reg[6] & 0xF0) | (data & 0x0F); break;
		case 0xE004: chr_reg[6] = (chr_reg[6] & 0x0F) | (data << 4); break;
		case 0xE008: chr_reg[7] = (chr_reg[7] & 0xF0) | (data & 0x0F); break;
		case 0xE00C: chr_reg[7] = (chr_reg[7] & 0x0F) | (data << 4); break;
		case 0xF000: irq_latch = (irq_latch & 0xF0) | (data & 0x0F); break;
		case 0xF004: irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4); break;
		case 0xF008:
			irq_enable = data & 0x03;
			if( irq_enable & 0x02 ) {
				irq_counter = irq_latch;
				irq_clock = 0;
			}
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		default:
			break;
	}
	SetBank();
}

void	BoardCityFighter::SetBank()
{
	SetPROM_32K_Bank( prg_reg >> 2 );
	if (!prg_mode) SetPROM_8K_Bank(6, prg_reg);
	for (int i = 0; i < 8; i++) SetVROM_1K_Bank(i, chr_reg[i]);
	switch (mirr) {
	case 0: SetVRAM_Mirror( VRAM_VMIRROR ); break;
	case 1: SetVRAM_Mirror( VRAM_HMIRROR ); break;
	case 2: SetVRAM_Mirror( VRAM_MIRROR4L ); break;
	case 3: SetVRAM_Mirror( VRAM_MIRROR4H ); break;
	}
}

void	BoardCityFighter::Clock( INT cycles )
{
	if( irq_enable & 0x02 ) {
		if( (irq_clock+=cycles) >= 0x72 ) {
			irq_clock -= 0x72;
			if( irq_counter == 0xFF ) {
				irq_counter = irq_latch;
				irq_enable = (irq_enable & 0x01) * 3;
				nes->cpu->SetIRQ( IRQ_MAPPER );
			} else {
				irq_counter++;
			}
		}
	}
}

void	BoardCityFighter::SaveState( LPBYTE p )
{
	//
}

void	BoardCityFighter::LoadState( LPBYTE p )
{
	//
}
