//////////////////////////////////////////////////////////////////////////
// Mapper118  IQS MMC3                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper118::Reset()
{
INT	i;

	for( i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
	}

	prg0 = 0;
	prg1 = 1;
	SetBank_CPU();

	if( VROM_1K_SIZE ) {
		chr01 = 0;
		chr23 = 2;
		chr4  = 4;
		chr5  = 5;
		chr6  = 6;
		chr7  = 7;

		SetBank_PPU();
	} else {
		chr01 = 0;
		chr23 = 0;
		chr4  = 0;
		chr5  = 0;
		chr6  = 0;
		chr7  = 0;
	}

	we_sram  = 0;	// Disable
	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
}

void	Mapper118::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0x8001:
			reg[1] = data;

			if( (reg[0] & 0x80) ) {
				if( (reg[0] & 0x07) == 2 ) {
					if( data & 0x80 ) SetVRAM_Mirror( VRAM_MIRROR4L );
					else		  SetVRAM_Mirror( VRAM_MIRROR4H );
				}
			} else {
				if( (reg[0] & 0x07) == 0 ) {
					if( data & 0x80 ) SetVRAM_Mirror( VRAM_MIRROR4L );
					else		  SetVRAM_Mirror( VRAM_MIRROR4H );
				}
			}

			switch( reg[0] & 0x07 ) {
				case	0x00:
					if( VROM_1K_SIZE ) {
						chr01 = data & 0xFE;
						SetBank_PPU();
					}
					break;
				case	0x01:
					if( VROM_1K_SIZE ) {
						chr23 = data & 0xFE;
						SetBank_PPU();
					}
					break;
				case	0x02:
					if( VROM_1K_SIZE ) {
						chr4 = data;
						SetBank_PPU();
					}
					break;
				case	0x03:
					if( VROM_1K_SIZE ) {
						chr5 = data;
						SetBank_PPU();
					}
					break;
				case	0x04:
					if( VROM_1K_SIZE ) {
						chr6 = data;
						SetBank_PPU();
					}
					break;
				case	0x05:
					if( VROM_1K_SIZE ) {
						chr7 = data;
						SetBank_PPU();
					}
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
			break;
	}
}

void	Mapper118::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(irq_counter--) ) {
					irq_counter = irq_latch;
//					nes->cpu->IRQ();
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper118::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, prg1, prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper118::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		if( reg[0] & 0x80 ) {
			SetVROM_8K_Bank( chr4, chr5, chr6, chr7,
					 chr01, chr01+1, chr23, chr23+1 );
		} else {
			SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1,
					 chr4, chr5, chr6, chr7 );
		}
	}
}

void	Mapper118::SaveState( LPBYTE p )
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
}

void	Mapper118::LoadState( LPBYTE p )
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
}
