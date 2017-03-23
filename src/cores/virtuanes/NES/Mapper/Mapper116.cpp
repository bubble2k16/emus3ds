//////////////////////////////////////////////////////////////////////////
// Mapper116 CartSaint : —H—VAV‹­—ñ“`                                   //
//////////////////////////////////////////////////////////////////////////

void	Mapper116::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
	}

	prg0 = prg0L = 0;
	prg1 = prg1L = 1;
	prg2 = PROM_8K_SIZE-2;
	prg3 = PROM_8K_SIZE-1;

	ExPrgSwitch = 0;
	ExChrSwitch = 0;

	SetBank_CPU();

	if( VROM_1K_SIZE ) {
		chr0 = 0;
		chr1 = 1;
		chr2 = 2;
		chr3 = 3;
		chr4 = 4;
		chr5 = 5;
		chr6 = 6;
		chr7 = 7;
		SetBank_PPU();
	} else {
		chr0 = chr2 = chr4 = chr5 = chr6 = chr7 = 0;
		chr1 = chr3 = 1;
	}

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;

//	nes->SetFrameIRQmode( FALSE );
}

void	Mapper116::WriteLow( WORD addr, BYTE data )
{
DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if( (addr & 0x4100) == 0x4100 ) {
		ExChrSwitch = data;
		SetBank_PPU();
	}
}

void	Mapper116::Write( WORD addr, BYTE data )
{
DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

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
					chr0 = data & 0xFE;
					chr1 = chr0+1;
					SetBank_PPU();
					break;
				case	0x01:
					chr2 = data & 0xFE;
					chr3 = chr2+1;
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
					prg0 =  data;
					SetBank_CPU();
					break;
				case	0x07:
					prg1 =  data;
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
			break;
		case	0xC000:
			reg[4] = data;
			irq_counter = data;
//			irq_enable = 0xFF;
			break;
		case	0xC001:
			reg[5] = data;
			irq_latch = data;
			break;
		case	0xE000:
			reg[6] = data;
			irq_counter = irq_latch;
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			reg[7] = data;
			irq_enable = 0xFF;
			break;
	}
}

void	Mapper116::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( irq_counter <= 0 ) {
			if( irq_enable ) {
//				nes->cpu->IRQ_NotPending();
				nes->cpu->SetIRQ( IRQ_MAPPER );

#if	0
DEBUGOUT( "IRQ L=%3d\n", scanline );
{
LPBYTE	lpScn = nes->ppu->GetScreenPtr();

	lpScn = lpScn+(256+16)*scanline;

	for( INT i = 0; i < 64; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
				return;
			}
		}

		if( nes->ppu->IsDispON() ) {
			irq_counter--;
		}
	}

#if	0
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(irq_counter--) ) {
//					irq_counter = irq_latch;
					nes->cpu->IRQ_NotPending();
				}
			}
		}
	}
#endif
}


void	Mapper116::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, prg1, prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper116::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		if( ExChrSwitch & 0x04 ) {
			INT	chrbank = 256;
			SetVROM_8K_Bank( chrbank+chr4, chrbank+chr5,
					 chrbank+chr6, chrbank+chr7,
					 chr0, chr1,
					 chr2, chr3 );
		} else {
			INT	chrbank = 0;
			SetVROM_8K_Bank( chrbank+chr4, chrbank+chr5,
					 chrbank+chr6, chrbank+chr7,
					 chr0, chr1,
					 chr2, chr3 );
		}

#if	0
		if( reg[0] & 0x80 ) {
//			SetVROM_8K_Bank( chrbank+chr4, chrbank+chr5,
//					 chrbank+chr6, chrbank+chr7,
//					 chrbank+chr0, chrbank+chr1,
//					 chrbank+chr2, chrbank+chr3 );
			SetVROM_8K_Bank( chrbank+chr4, chrbank+chr5,
					 chrbank+chr6, chrbank+chr7,
					 chr0, chr1,
					 chr2, chr3 );
		} else {
			SetVROM_8K_Bank( chr0, chr1,
					 chr2, chr3,
					 chrbank+chr4, chrbank+chr5,
					 chrbank+chr6, chrbank+chr7 );
		}
#endif
	}
}

void	Mapper116::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i] = reg[i];
	}
	p[ 8] = prg0;
	p[ 9] = prg1;
	p[10] = prg2;
	p[11] = prg3;
	p[12] = chr0;
	p[13] = chr1;
	p[14] = chr2;
	p[15] = chr3;
	p[16] = chr4;
	p[17] = chr5;
	p[18] = chr6;
	p[19] = chr7;
	p[20] = irq_enable;
	p[21] = irq_counter;
	p[22] = irq_latch;
	p[23] = ExPrgSwitch;
	p[24] = prg0L;
	p[25] = prg1L;
	p[26] = ExChrSwitch;
}

void	Mapper116::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
	}
	prg0  = p[ 8];
	prg1  = p[ 9];
	prg2  = p[10];
	prg3  = p[11];
	chr0  = p[12];
	chr1  = p[13];
	chr2  = p[14];
	chr3  = p[15];
	chr4  = p[16];
	chr5  = p[17];
	chr6  = p[18];
	chr7  = p[19];
	irq_enable  = p[20];
	irq_counter = p[21];
	irq_latch   = p[22];
	ExPrgSwitch = p[23];
	prg0L = p[24];
	prg1L = p[25];
	ExChrSwitch = p[26];
}
