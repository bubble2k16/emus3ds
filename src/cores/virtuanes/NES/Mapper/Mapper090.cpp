//////////////////////////////////////////////////////////////////////////
// Mapper090  PC-JY-??                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper090::Reset()
{
	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

	patch = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x2a268152 ) {
		patch = 1;
	}
	if( crc == 0x2224b882 ) {
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
	irq_occur = 0;
	irq_preset = 0;
	irq_offset = 0;

	prg_6000 = 0;
	prg_E000 = 0;
	prg_size = 0;
	chr_size = 0;
	mir_mode = 0;
	mir_type = 0;

	key_val = 0;
	mul_val1 = mul_val2 = 0;

	for( INT i = 0; i < 4; i++ ) {
		prg_reg[i] = PROM_8K_SIZE-4+i;
		ntl_reg[i] = 0;
		nth_reg[i] = 0;
		chl_reg[i] = i;
		chh_reg[i] = 0;
		chl_reg[i+4] = i+4;
		chh_reg[i+4] = 0;
	}

	if( sw_val )
		sw_val = 0x00;
	else
		sw_val = 0xFF;

//	nes->SetRenderMethod( NES::PRE_ALL_RENDER );
}

BYTE	Mapper090::ReadLow( WORD addr )
{
DEBUGOUT( "RD:%04X\n", addr );

	switch( addr ) {
		case	0x5000:
			return	sw_val?0x00:0xFF;
		case	0x5800:
			return	(BYTE)(mul_val1*mul_val2);
		case	0x5801:
			return	(BYTE)((mul_val1*mul_val2)>>8);
		case	0x5803:
			return	key_val;
	}

	if( addr >= 0x6000 ) {
		return	Mapper::ReadLow( addr );
	}

//	return	sw_val?0x00:0xFF;
	return	(BYTE)(addr>>8);
}

void	Mapper090::WriteLow( WORD addr, BYTE data )
{
DEBUGOUT( "WR:%04X %02X\n", addr, data );

	if( addr == 0x5800 ) {
		mul_val1 = data;
	} else
	if( addr == 0x5801 ) {
		mul_val2 = data;
	} else
	if( addr == 0x5803 ) {
		key_val = data;
	}
}

void	Mapper090::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF007 ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			prg_reg[addr&3] = data;
			SetBank_CPU();
			break;

		case	0x9000:
		case	0x9001:
		case	0x9002:
		case	0x9003:
		case	0x9004:
		case	0x9005:
		case	0x9006:
		case	0x9007:
			chl_reg[addr&7] = data;
			SetBank_PPU();
			break;

		case	0xA000:
		case	0xA001:
		case	0xA002:
		case	0xA003:
		case	0xA004:
		case	0xA005:
		case	0xA006:
		case	0xA007:
			chh_reg[addr&7] = data;
			SetBank_PPU();
			break;

		case	0xB000:
		case	0xB001:
		case	0xB002:
		case	0xB003:
			ntl_reg[addr&3] = data;
			SetBank_VRAM();
			break;

		case	0xB004:
		case	0xB005:
		case	0xB006:
		case	0xB007:
			nth_reg[addr&3] = data;
			SetBank_VRAM();
			break;

		case	0xC002:
			irq_enable = 0;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xC003:
			irq_enable = 0xFF;
			irq_preset = 0xFF;
			break;
		case	0xC004:
			break;
		case	0xC005:
			if( irq_offset & 0x80 ) {
				irq_latch = data ^ (irq_offset | 1);
			} else {
				irq_latch = data | (irq_offset&0x27);
			}
			irq_preset = 0xFF;
			break;
		case	0xC006:
			if( patch ) {
				irq_offset = data;
			}
			break;

		case	0xD000:
			prg_6000 = data & 0x80;
			prg_E000 = data & 0x04;
			prg_size = data & 0x03;
			chr_size = (data & 0x18)>>3;
			mir_mode = data & 0x20;
			SetBank_CPU();
			SetBank_PPU();
			SetBank_VRAM();
			break;

		case	0xD001:
			mir_type = data & 0x03;
			SetBank_VRAM();
			break;

		case	0xD003:
			break;
	}
}

void	Mapper090::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_preset ) {
				irq_counter = irq_latch;
				irq_preset = 0;
			}
			if( irq_counter ) {
				irq_counter--;
			}
			if( !irq_counter ) {
				if( irq_enable ) {
//					irq_occur = 0xFF;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper090::Clock(INT cycles)
{
//	if( irq_occur ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper090::SetBank_CPU()
{
	if( prg_size == 0 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else
	if( prg_size == 1 ) {
		SetPROM_32K_Bank( prg_reg[1]*2, prg_reg[1]*2+1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else
	if( prg_size == 2 ) {
		if( prg_E000 ) {
			SetPROM_32K_Bank( prg_reg[0], prg_reg[1], prg_reg[2], prg_reg[3] );
		} else {
			if( prg_6000 ) {
				SetPROM_8K_Bank( 3, prg_reg[3] );
			}
			SetPROM_32K_Bank( prg_reg[0], prg_reg[1], prg_reg[2], PROM_8K_SIZE-1 );
		}
	} else {
		SetPROM_32K_Bank( prg_reg[3], prg_reg[2], prg_reg[1], prg_reg[0] );
	}
}

void	Mapper090::SetBank_PPU()
{
INT	bank[8];

	for( INT i = 0; i < 8; i++ ) {
		bank[i] = ((INT)chh_reg[i]<<8)|((INT)chl_reg[i]);
	}

	if( chr_size == 0 ) {
		SetVROM_8K_Bank( bank[0] );
	} else
	if( chr_size == 1 ) {
		SetVROM_4K_Bank( 0, bank[0] );
		SetVROM_4K_Bank( 4, bank[4] );
	} else
	if( chr_size == 2 ) {
		SetVROM_2K_Bank( 0, bank[0] );
		SetVROM_2K_Bank( 2, bank[2] );
		SetVROM_2K_Bank( 4, bank[4] );
		SetVROM_2K_Bank( 6, bank[6] );
	} else {
		SetVROM_8K_Bank( bank[0], bank[1], bank[2], bank[3], bank[4], bank[5], bank[6], bank[7] );
	}
}

void	Mapper090::SetBank_VRAM()
{
INT	bank[4];

	for( INT i = 0; i < 4; i++ ) {
		bank[i] = ((INT)nth_reg[i]<<8)|((INT)ntl_reg[i]);
	}

	if( !patch && mir_mode ) {
		for( INT i = 0; i < 4; i++ ) {
			if( !nth_reg[i] && (ntl_reg[i] == (BYTE)i) ) {
				mir_mode = 0;
			}
		}

		if( mir_mode ) {
			SetVROM_1K_Bank(  8, bank[0] );
			SetVROM_1K_Bank(  9, bank[1] );
			SetVROM_1K_Bank( 10, bank[2] );
			SetVROM_1K_Bank( 11, bank[3] );
		}
	} else {
		if( mir_type == 0 ) {
			SetVRAM_Mirror( VRAM_VMIRROR );
		} else
		if( mir_type == 1 ) {
			SetVRAM_Mirror( VRAM_HMIRROR );
		} else {
			SetVRAM_Mirror( VRAM_MIRROR4L );
		}
	}
}

void	Mapper090::SaveState( LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		p[i] = prg_reg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[i+ 4] = chh_reg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[i+12] = chl_reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i+20] = nth_reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i+24] = ntl_reg[i];
	}
	p[28] = irq_enable;
	p[29] = irq_counter;
	p[30] = irq_latch;
	p[31] = prg_6000;
	p[32] = prg_E000;
	p[33] = prg_size;
	p[34] = chr_size;
	p[35] = mir_mode;
	p[36] = mir_type;
	p[37] = mul_val1;
	p[38] = mul_val2;
	p[39] = key_val;
	p[40] = irq_occur;
	p[41] = irq_preset;
	p[42] = irq_offset;
}

void	Mapper090::LoadState( LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		prg_reg[i] = p[i];
	}
	for( i = 0; i < 8; i++ ) {
		chh_reg[i] = p[i+ 4];
	}
	for( i = 0; i < 8; i++ ) {
		chl_reg[i] = p[i+12];
	}
	for( i = 0; i < 4; i++ ) {
		nth_reg[i] = p[i+20];
	}
	for( i = 0; i < 4; i++ ) {
		ntl_reg[i] = p[i+24];
	}
	irq_enable  = p[28];
	irq_counter = p[29];
	irq_latch   = p[30];
	prg_6000    = p[31];
	prg_E000    = p[32];
	prg_size    = p[33];
	chr_size    = p[34];
	mir_mode    = p[35];
	mir_type    = p[36];
	mul_val1    = p[37];
	mul_val2    = p[38];
	key_val     = p[39];
	irq_occur   = p[40];
	irq_preset  = p[41];
	irq_offset  = p[42];
}
