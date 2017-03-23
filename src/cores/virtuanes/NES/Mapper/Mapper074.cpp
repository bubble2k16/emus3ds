//////////////////////////////////////////////////////////////////////////
// Mapper074  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper074::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
	}
	prg0 = 0;
	prg1 = 1;
	SetBank_CPU();

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	SetBank_PPU();

	we_sram  = 0;	// Disable
	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();

	patch = 0;
	if( crc == 0x37ae04a8 ) {
		patch = 1;
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
}

void	Mapper074::Write( WORD addr, BYTE data )
{
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

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
			break;
		case	0xC000:
			reg[4] = data;
			irq_counter = data;
			irq_request = 0;
			break;
		case	0xC001:
			reg[5] = data;
			irq_latch = data;
			irq_request = 0;
			break;
		case	0xE000:
			reg[6] = data;
			irq_enable = 0;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			reg[7] = data;
			irq_enable = 1;
			irq_request = 0;
			break;
	}	
	
}

void	Mapper074::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable && !irq_request ) {
				if( scanline == 0 ) {
					if( irq_counter ) {
						irq_counter--;
					}
				}
				if( !(irq_counter--) ) {
					irq_request = 0xFF;
					irq_counter = irq_latch;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper074::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, prg1, prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper074::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		if( reg[0] & 0x80 ) {
//			SetVROM_8K_Bank( chr4, chr5, chr6, chr7,
//					 chr01, chr01+1, chr23, chr23+1 );
			SetBank_PPUSUB( 4, chr01+0 );
			SetBank_PPUSUB( 5, chr01+1 );
			SetBank_PPUSUB( 6, chr23+0 );
			SetBank_PPUSUB( 7, chr23+1 );
			SetBank_PPUSUB( 0, chr4 );
			SetBank_PPUSUB( 1, chr5 );
			SetBank_PPUSUB( 2, chr6 );
			SetBank_PPUSUB( 3, chr7 );
		} else {
//			SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1,
//					 chr4, chr5, chr6, chr7 );
			SetBank_PPUSUB( 0, chr01+0 );
			SetBank_PPUSUB( 1, chr01+1 );
			SetBank_PPUSUB( 2, chr23+0 );
			SetBank_PPUSUB( 3, chr23+1 );
			SetBank_PPUSUB( 4, chr4 );
			SetBank_PPUSUB( 5, chr5 );
			SetBank_PPUSUB( 6, chr6 );
			SetBank_PPUSUB( 7, chr7 );
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

void	Mapper074::SetBank_PPUSUB( int bank, int page )
{
	if( !patch && (page == 8 || page == 9) ) {
		SetCRAM_1K_Bank( bank, page & 7 );
	} else if( patch == 1 && page >= 128 ) {
		SetCRAM_1K_Bank( bank, page & 7 );
	} else {
		SetVROM_1K_Bank( bank, page );
	}
}

void	Mapper074::SaveState( LPBYTE p )
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
	p[19] = irq_request;
}

void	Mapper074::LoadState( LPBYTE p )
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
	irq_request = p[19];
}
