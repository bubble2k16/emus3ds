//////////////////////////////////////////////////////////////////////////
// Mapper189  Street Fighter 2 Yoko/Master Fighter 2                    //
//            âıë≈ò˙ê‡ Street Fighter IV (GOUDER)                       //
//////////////////////////////////////////////////////////////////////////
void	Mapper189::Reset()
{
	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	reg[0] = reg[1] = 0;

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	SetBank_PPU();

	irq_enable  = 0;
	irq_counter = 0;
	irq_latch   = 0;

	for( INT i = 0; i < 4; i++ ) {
		protect_dat[i] = 0;
	}
	lwd = 0xFF;

	patch = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x20ca2ad3 ) {	// Street Fighter IV (GOUDER)
		patch = 1;
		SetPROM_32K_Bank( 0 );

		// $4000-$5FFF
		SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );
	}
}

void	Mapper189::WriteLow( WORD addr, BYTE data )
{
	if( (addr & 0xFF00) == 0x4100 ) {
	// Street Fighter 2 YOKO
		SetPROM_32K_Bank( (data&0x30)>>4 );
	} else if( (addr & 0xFF00) == 0x6100 ) {
	// Master Fighter 2
		SetPROM_32K_Bank( data&0x03 );
	}

	if( patch ) {
	// Street Fighter IV (GOUDER)
		BYTE	a5000xordat[256] = {
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x49, 0x19, 0x09, 0x59, 0x49, 0x19, 0x09,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x51, 0x41, 0x11, 0x01, 0x51, 0x41, 0x11, 0x01,
			0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x00, 0x10, 0x40, 0x50, 0x00, 0x10, 0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x08, 0x18, 0x48, 0x58, 0x08, 0x18, 0x48, 0x58, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x58, 0x48, 0x18, 0x08, 0x58, 0x48, 0x18, 0x08,
			0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x59, 0x50, 0x40, 0x10, 0x00, 0x50, 0x40, 0x10, 0x00,
			0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x01, 0x11, 0x41, 0x51, 0x01, 0x11, 0x41, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			0x09, 0x19, 0x49, 0x59, 0x09, 0x19, 0x49, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
		};

		if( (addr >= 0x4800) && (addr <= 0x4FFF) ) {
			SetPROM_32K_Bank( ((data&0x10)>>3)+(data&0x1) );

			if( !nes->rom->Is4SCREEN() ) {
				if( data & 0x20 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
			}
		}
		if( (addr>=0x5000) && (addr<=0x57FF) ) {
			lwd = data;
		}
		if( (addr>=0x5800) && (addr<=0x5FFF) ) {
//			XRAM[0x1000+(addr & 3)] = 
			// $5800 "JMP $xxxx" write
			XRAM[0x1800+(addr & 3)] = 
			protect_dat[ addr & 3 ] = data ^ a5000xordat[ lwd ];
		}
	}
}

void	Mapper189::Write( WORD addr, BYTE data )
{
	switch( addr&0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_PPU();
			break;

		case	0x8001:
			reg[1] = data;
			SetBank_PPU();
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
			}
			break;

		case	0xA000:
			if( data&0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		SetVRAM_Mirror( VRAM_VMIRROR );
			break;

		case	0xC000:
			irq_counter = data;
			break;
		case	0xC001:
			irq_latch = data;
			break;
		case	0xE000:
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			irq_enable = 0xFF;
			break;
	}
}

void	Mapper189::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(--irq_counter) ) {
//				if( !(irq_counter--) ) {
					irq_counter = irq_latch;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper189::SetBank_PPU()
{
	if( patch ) {
		SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1,
				 chr4, chr5, chr6, chr7 );
	} else {
		if( VROM_1K_SIZE ) {
			if( reg[0] & 0x80 ) {
				SetVROM_8K_Bank( chr4, chr5, chr6, chr7,
						 chr01, chr01+1, chr23, chr23+1 );
			} else {
				SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1,
						 chr4, chr5, chr6, chr7 );
			}
		} else {
			if( reg[0] & 0x80 ) {
				SetCRAM_1K_Bank( 4, (chr01+0)&0x07 );
				SetCRAM_1K_Bank( 5, (chr01+1)&0x07 );
				SetCRAM_1K_Bank( 6, (chr23+0)&0x07 );
				SetCRAM_1K_Bank( 7, (chr23+1)&0x07 );
				SetCRAM_1K_Bank( 0, chr4&0x07 );
				SetCRAM_1K_Bank( 1, chr5&0x07 );
				SetCRAM_1K_Bank( 2, chr6&0x07 );
				SetCRAM_1K_Bank( 3, chr7&0x07 );
			} else {
				SetCRAM_1K_Bank( 0, (chr01+0)&0x07 );
				SetCRAM_1K_Bank( 1, (chr01+1)&0x07 );
				SetCRAM_1K_Bank( 2, (chr23+0)&0x07 );
				SetCRAM_1K_Bank( 3, (chr23+1)&0x07 );
				SetCRAM_1K_Bank( 4, chr4&0x07 );
				SetCRAM_1K_Bank( 5, chr5&0x07 );
				SetCRAM_1K_Bank( 6, chr6&0x07 );
				SetCRAM_1K_Bank( 7, chr7&0x07 );
			}
		}
	}
}

void	Mapper189::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = chr01;
	p[3] = chr23;
	p[4] = chr4;
	p[5] = chr5;
	p[6] = chr6;
	p[7] = chr7;
	p[ 8] = irq_enable;
	p[ 9] = irq_counter;
	p[10] = irq_latch;

	p[16] = protect_dat[0];
	p[17] = protect_dat[1];
	p[18] = protect_dat[2];
	p[19] = protect_dat[3];
	p[20] = lwd;
}

void	Mapper189::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	chr01  = p[2];
	chr23  = p[3];
	chr4   = p[4];
	chr5   = p[5];
	chr6   = p[6];
	chr7   = p[7];

	irq_enable  = p[ 8];
	irq_counter = p[ 9];
	irq_latch   = p[10];

	protect_dat[0] = p[16];
	protect_dat[1] = p[17];
	protect_dat[2] = p[18];
	protect_dat[3] = p[19];
	lwd = p[20];
}
