//////////////////////////////////////////////////////////////////////////
// Mapper004  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
#define	MMC3_IRQ_KLAX		1
#define	MMC3_IRQ_SHOUGIMEIKAN	2
#define	MMC3_IRQ_DAI2JISUPER	3
#define	MMC3_IRQ_DBZ2		4
#define	MMC3_IRQ_ROCKMAN3	5

void	Mapper004::Reset()
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
	irq_latch = 0xFF;
	irq_request = 0;
	irq_preset = 0;
	irq_preset_vbl = 0;

	// IRQ�^�C�v�ݒ�
	nes->SetIrqType( NES::IRQ_CLOCK );
	irq_type = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x5c707ac4 ) {	// Mother(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xcb106f49 ) {	// F-1 Sensation(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x1170392a ) {	// Karakuri Kengou Den - Musashi Road - Karakuri Nin Hashiru!(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x14a01c70 ) {	// Gun-Dec(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xeffeea40 ) {	// For Klax(J)
		irq_type = MMC3_IRQ_KLAX;
		nes->SetIrqType( NES::IRQ_HSYNC );
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xc17ae2dc ) {	// God Slayer - Haruka Tenkuu no Sonata(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x126ea4a0 ) {	// Summer Carnival '92 - Recca(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x1f2f4861 ) {	// J League Fighting Soccer - The King of Ace Strikers(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x5a6860f1 ) {	// Shougi Meikan '92(J)
		irq_type = MMC3_IRQ_SHOUGIMEIKAN;
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xae280e20 ) {	// Shougi Meikan '93(J)
		irq_type = MMC3_IRQ_SHOUGIMEIKAN;
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xe19a2473 ) {	// Sugoro Quest - Dice no Senshi Tachi(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x702d9b33 ) {	// Star Wars - The Empire Strikes Back(Victor)(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xa9a0d729 ) {	// Dai Kaijuu - Deburas(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xc5fea9f2 ) {	// Dai 2 Ji - Super Robot Taisen(J)
		irq_type = MMC3_IRQ_DAI2JISUPER;
	}
	if( crc == 0xd852c2f7 ) {	// Time Zone(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xecfd3c69 ) {	// Taito Chase H.Q.(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x7a748058 ) {	// Tom & Jerry (and Tuffy)(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xaafe699c ) {	// Ninja Ryukenden 3 - Yomi no Hakobune(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x6cc62c06 ) {	// Hoshi no Kirby - Yume no Izumi no Monogatari(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x877dba77 ) {	// My Life My Love - Boku no Yume - Watashi no Negai(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x6f96ed15 ) {	// Max Warrior - Wakusei Kaigenrei(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x8685f366 ) {	// Matendouji(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x8635fed1 ) {	// Mickey Mouse 3 - Yume Fuusen(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x26ff3ea2 ) {	// Yume Penguin Monogatari(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x7671bc51 ) {	// Red Ariimaa 2(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xade11141 ) {	// Wanpaku Kokkun no Gourmet World(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0x7c7ab58e ) {	// Walkuere no Bouken - Toki no Kagi Densetsu(J)
		nes->SetRenderMethod( NES::POST_RENDER );
	}
	if( crc == 0x26ff3ea2 ) {	// Yume Penguin Monogatari(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x126ea4a0 ) {	// Summer Carnival '92 Recca(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0x1d2e5018		// Rockman 3(J)
	 || crc == 0x6b999aaf ) {	// Megaman 3(U)
		irq_type = MMC3_IRQ_ROCKMAN3;
	}

	if( crc == 0xd88d48d7 ) {	// Kick Master(U)
		irq_type = MMC3_IRQ_ROCKMAN3;
	}

	if( crc == 0xA67EA466 ) {	// Alien 3(U)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0xe763891b ) {	// DBZ2
		irq_type = MMC3_IRQ_DBZ2;
	}

	// VS-Unisystem
	vs_patch = 0;
	vs_index = 0;

	if( crc == 0xeb2dba63		// VS TKO Boxing
	 || crc == 0x98cfe016 ) {	// VS TKO Boxing (Alt)
		vs_patch = 1;
	}
	if( crc == 0x135adf7c ) {	// VS Atari RBI Baseball
		vs_patch = 2;
	}
	if( crc == 0xf9d3b0a3		// VS Super Xevious
	 || crc == 0x9924980a		// VS Super Xevious (b1)
	 || crc == 0x66bb838f ) {	// VS Super Xevious (b2)
		vs_patch = 3;
	}
}

BYTE	Mapper004::ReadLow( WORD addr )
{
	if( !vs_patch ) {
		if( addr >= 0x5000 && addr <= 0x5FFF ) {
			return	XRAM[addr-0x4000];
		}
	} else if( vs_patch == 1 ) {
		// VS TKO Boxing Security
		if( addr == 0x5E00 ) {
			vs_index = 0;
			return	0x00;
		} else if( addr == 0x5E01 ) {
			BYTE	VS_TKO_Security[32] = {
				0xff, 0xbf, 0xb7, 0x97, 0x97, 0x17, 0x57, 0x4f,
				0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90, 0x94, 0x14,
				0x56, 0x4e, 0x6f, 0x6b, 0xeb, 0xa9, 0xb1, 0x90,
				0xd4, 0x5c, 0x3e, 0x26, 0x87, 0x83, 0x13, 0x00 };
			return	VS_TKO_Security[(vs_index++) & 0x1F];
		}
	} else if( vs_patch == 2 ) {
		// VS Atari RBI Baseball Security
		if( addr == 0x5E00 ) {
			vs_index = 0;
			return	0x00;
		} else if( addr == 0x5E01 ) {
			if( vs_index++ == 9 )
				return	0x6F;
			else
				return	0xB4;
		}
	} else if( vs_patch == 3 ) {
		// VS Super Xevious
		switch( addr ) {
			case	0x54FF:
				return	0x05;
			case	0x5678:
				if( vs_index )
					return	0x00;
				else
					return	0x01;
				break;
			case	0x578f:
				if( vs_index )
					return	0xD1;
				else
					return	0x89;
				break;
			case	0x5567:
				if( vs_index ) {
					vs_index = 0;
					return	0x3E;
				} else {
					vs_index = 1;
					return	0x37;
				}
				break;
			default:
				break;
		}
	}

	return	Mapper::ReadLow( addr );
}

void	Mapper004::WriteLow( WORD addr, BYTE data )
{
	if( addr >= 0x5000 && addr <= 0x5FFF ) {
		XRAM[addr-0x4000] = data;
	} else {
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper004::Write( WORD addr, BYTE data )
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
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			break;
		case	0xC000:
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			reg[4] = data;
			//printf ("Set counter: %d @ %d\n", data, nes->GetScanline());
			if( irq_type == MMC3_IRQ_KLAX || irq_type == MMC3_IRQ_ROCKMAN3 ) {
				irq_counter = data;
			} else {
				irq_latch = data;
			}
			if( irq_type == MMC3_IRQ_DBZ2 ) {
				irq_latch = 0x07;
			}
			break;
		case	0xC001:
			reg[5] = data;
			//printf ("Load counter: %d @ %d\n", data, nes->GetScanline());
			if( irq_type == MMC3_IRQ_KLAX || irq_type == MMC3_IRQ_ROCKMAN3 ) {
				irq_latch = data;
			} else {
				if( (nes->GetScanline() < 240) || (irq_type == MMC3_IRQ_SHOUGIMEIKAN) ) {
					irq_counter |= 0x80;
					irq_preset = 0xFF;
				} else {
					irq_counter |= 0x80;
					irq_preset_vbl = 0xFF;
					irq_preset = 0;
				}
			}
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

//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper004::Clock( INT cycles )
{
//	if( irq_request && (nes->GetIrqType() == NES::IRQ_CLOCK) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper004::HSync( INT scanline )
{
	if( irq_type == MMC3_IRQ_KLAX ) {
		if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( irq_counter == 0 ) {
					irq_counter = irq_latch;
					irq_request = 0xFF;
				}
				if( irq_counter > 0 ) {
					irq_counter--;
				}
			}
		}
		if( irq_request ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	} else if( irq_type == MMC3_IRQ_ROCKMAN3 ) {
		if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(--irq_counter) ) {
					irq_request = 0xFF;
					irq_counter = irq_latch;
				}
			}
		}
		if( irq_request ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	} else {
		if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
			if( irq_preset_vbl ) {
				irq_counter = irq_latch;
				irq_preset_vbl = 0;
			}
			if( irq_preset ) {
				irq_counter = irq_latch;
				irq_preset = 0;
				if( irq_type == MMC3_IRQ_DAI2JISUPER && scanline == 0 ) {
					irq_counter--;
				}
			} else if( irq_counter > 0 ) {
				irq_counter--;
			}

			if( irq_counter == 0 ) {
				if( irq_enable ) {
					irq_request = 0xFF;

					nes->cpu->SetIRQ( IRQ_MAPPER );

#if	0
{
LPBYTE	lpScn = nes->ppu->GetScreenPtr();

	lpScn = lpScn+(256+16)*nes->GetScanline();

	for( INT i = 0; i < 16; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
				}
				irq_preset = 0xFF;
			}
		}
	}
//	if( irq_request && (nes->GetIrqType() == NES::IRQ_HSYNC) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper004::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( PROM_8K_SIZE-2, prg1, prg0, PROM_8K_SIZE-1 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper004::SetBank_PPU()
{
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

void	Mapper004::SaveState( LPBYTE p )
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
	p[17] = (BYTE)irq_counter;
	p[18] = irq_latch;
	p[19] = irq_request;
	p[20] = irq_preset;
	p[21] = irq_preset_vbl;
}

void	Mapper004::LoadState( LPBYTE p )
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
	irq_counter = (INT)p[17];
	irq_latch   = p[18];
	irq_request = p[19];
	irq_preset  = p[20];
	irq_preset_vbl = p[21];
}
