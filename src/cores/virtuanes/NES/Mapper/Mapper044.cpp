//////////////////////////////////////////////////////////////////////////
// Mapper044  Super HiK 7-in-1                                          //
//////////////////////////////////////////////////////////////////////////
void	Mapper044::Reset()
{
	patch = 0;

	if( nes->rom->GetPROM_CRC() == 0x7eef434c ) {
		patch = 1;
	}

	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0;
	}

	bank = 0;
	prg0 = 0;
	prg1 = 1;

	// set VROM banks
	if( VROM_1K_SIZE ) {
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;
	} else {
		chr01 = chr23 = chr4 = chr5 = chr6 = chr7 = 0;
	}

	SetBank_CPU();
	SetBank_PPU();

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
}

void	Mapper044::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x6000 ) {
		if( patch ) {
			bank = (data & 0x06) >> 1;
		} else {
			bank = (data & 0x01) << 1;
		}
		SetBank_CPU();
		SetBank_PPU();
	}
}

void	Mapper044::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0x8001:
			reg[1] = data;
			switch( reg[0] & 0x07 ) {
				case	0x00:
					chr01 = data & 0xFE;
					SetBank_PPU();
					break;
				case	0x01:
					chr23 = data & 0xFE;
					SetBank_PPU();
					break;
				case	0x02:
					chr4 = data;
					SetBank_PPU();
					break;
				case	0x03:
					chr5 = data;
					SetBank_PPU();
					break;
				case	0x04:
					chr6 = data;
					SetBank_PPU();
					break;
				case	0x05:
					chr7 = data;
					SetBank_PPU();
					break;
				case	0x06:
					prg0 = data;
					SetBank_CPU();
					break;
				case	0x07:
					prg1 = data;
					SetBank_CPU();
					break;
			}
			break;
		case	0xA000:
			reg[2] = data;
			if( !nes->rom->Is4SCREEN() ) {
				if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			reg[3] = data;
			bank = data & 0x07;
			if( bank == 7 ) {
				bank = 6;
			}
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0xC000:
			reg[4] = data;
			irq_counter = data;
			break;
		case	0xC001:
			reg[5] = data;
			irq_latch = data;
			break;
		case	0xE000:
			reg[6] = data;
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			reg[7] = data;
			irq_enable = 1;
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper044::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(--irq_counter) ) {
					irq_counter = irq_latch;
//					nes->cpu->IRQ();
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper044::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_8K_Bank( 4, ((bank == 6)?0x1e:0x0e)|(bank<<4) );
		SetPROM_8K_Bank( 5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4) );
		SetPROM_8K_Bank( 6, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4) );
		SetPROM_8K_Bank( 7, ((bank == 6)?0x1f:0x0f)|(bank<<4) );
	} else {
		SetPROM_8K_Bank( 4, ((bank == 6)?0x1f&prg0:0x0f&prg0)|(bank<<4) );
		SetPROM_8K_Bank( 5, ((bank == 6)?0x1f&prg1:0x0f&prg1)|(bank<<4) );
		SetPROM_8K_Bank( 6, ((bank == 6)?0x1e:0x0e)|(bank<<4) );
		SetPROM_8K_Bank( 7, ((bank == 6)?0x1f:0x0f)|(bank<<4) );
	}
}

void	Mapper044::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		if( reg[0] & 0x80 ) {
			SetVROM_1K_Bank( 0, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7) );
			SetVROM_1K_Bank( 1, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7) );
			SetVROM_1K_Bank( 2, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7) );
			SetVROM_1K_Bank( 3, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7) );
			SetVROM_1K_Bank( 4, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7) );
			SetVROM_1K_Bank( 5, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7) );
			SetVROM_1K_Bank( 6, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7) );
			SetVROM_1K_Bank( 7, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7) );
		} else {
			SetVROM_1K_Bank( 0, ((bank == 6)?0xff&chr01:0x7f&chr01)|(bank<<7) );
			SetVROM_1K_Bank( 1, ((bank == 6)?0xff&(chr01+1):0x7f&(chr01+1))|(bank<<7) );
			SetVROM_1K_Bank( 2, ((bank == 6)?0xff&chr23:0x7f&chr23)|(bank<<7) );
			SetVROM_1K_Bank( 3, ((bank == 6)?0xff&(chr23+1):0x7f&(chr23+1))|(bank<<7) );
			SetVROM_1K_Bank( 4, ((bank == 6)?0xff&chr4:0x7f&chr4)|(bank<<7) );
			SetVROM_1K_Bank( 5, ((bank == 6)?0xff&chr5:0x7f&chr5)|(bank<<7) );
			SetVROM_1K_Bank( 6, ((bank == 6)?0xff&chr6:0x7f&chr6)|(bank<<7) );
			SetVROM_1K_Bank( 7, ((bank == 6)?0xff&chr7:0x7f&chr7)|(bank<<7) );
		}
	}
}

void	Mapper044::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i] = reg[i];
	}
	p[ 8] = prg0;
	p[ 9] = prg1;
	p[10] = chr01;
	p[11] = chr23;
	p[12] = chr4;
	p[13] = chr5;
	p[14] = chr6;
	p[15] = chr7;
	p[16] = irq_enable;
	p[17] = irq_counter;
	p[18] = irq_latch;
	p[19] = bank;
}

void	Mapper044::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
	}
	prg0  = p[ 8];
	prg1  = p[ 9];
	chr01 = p[10];
	chr23 = p[11];
	chr4  = p[12];
	chr5  = p[13];
	chr6  = p[14];
	chr7  = p[15];
	irq_enable  = p[16];
	irq_counter = p[17];
	irq_latch   = p[18];
	bank        = p[19];
}

