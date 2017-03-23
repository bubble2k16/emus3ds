//////////////////////////////////////////////////////////////////////////
// Mapper001  Nintendo MMC1                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper001::Reset()
{
	reg[0] = 0x0C;		// D3=1,D2=1
	reg[1] = reg[2] = reg[3] = 0;
	shift = regbuf = 0;

	patch = 0;
	wram_patch = 0;

	if( PROM_16K_SIZE < 32 ) {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	} else {
		// For 512K/1M byte Cartridge
		SetPROM_16K_Bank( 4, 0 );
		SetPROM_16K_Bank( 6, 16-1 );

		patch = 1;
	}

	if( VROM_8K_SIZE ) {
//		SetVROM_8K_Bank( 0 );
	}

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0xb8e16bd0 ) {	// Snow Bros.(J)
		patch = 2;
	}
//	if( crc == 0x9b565541 ) {	// Triathron, The(J)
//		nes->SetFrameIRQmode( FALSE );
//	}
	if( crc == 0xc96c6f04 ) {	// Venus Senki(J)
		nes->SetRenderMethod( NES::POST_ALL_RENDER );
	}
//	if( crc == 0x5e3f7004 ) {	// Softball Tengoku(J)
//	}

	if( crc == 0x4d2edf70 ) {	// Night Rider(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xcd2a73f0 ) {	// Pirates!(U)
		nes->SetRenderMethod( NES::TILE_RENDER );
		patch = 2;
	}

//	if( crc == 0x09efe54b ) {	// Majaventure - Mahjong Senki(J)
//		nes->SetFrameIRQmode( FALSE );
//	}

	if( crc == 0x11469ce3 ) {	// Viva! Las Vegas(J)
	}
	if( crc == 0xd878ebf5 ) {	// Ninja Ryukenden(J)
		nes->SetRenderMethod( NES::POST_ALL_RENDER );
	}

//	if( crc == 0x7bd7b849 ) {	// Nekketsu Koukou - Dodgeball Bu(J)
//	}

	if( crc == 0x466efdc2 ) {	// Final Fantasy(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
		nes->ppu->SetExtMonoMode( TRUE );
	}
	if( crc == 0xc9556b36 ) {	// Final Fantasy I&II(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
		nes->ppu->SetExtMonoMode( TRUE );
		nes->SetSAVERAM_SIZE( 16*1024 );
		wram_patch = 2;
	}

	if( crc == 0x717e1169 ) {	// Cosmic Wars(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}

	if( crc == 0xC05D2034 ) {	// Snake's Revenge(U)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}

	if( crc == 0xb8747abf		// Best Play - Pro Yakyuu Special(J)
	 || crc == 0x29449ba9		// Nobunaga no Yabou - Zenkoku Ban(J)
	 || crc == 0x2b11e0b0		// Nobunaga no Yabou - Zenkoku Ban(J)(alt)
	 || crc == 0x4642dda6		// Nobunaga's Ambition(U)
	 || crc == 0xfb69743a		// Aoki Ookami to Shiroki Mejika - Genghis Khan(J)
	 || crc == 0x2225c20f		// Genghis Khan(U)
	 || crc == 0xabbf7217		// Sangokushi(J)
	) {

		nes->SetSAVERAM_SIZE( 16*1024 );
		wram_patch = 1;
		wram_bank  = 0;
		wram_count = 0;
	}
}

void	Mapper001::Write( WORD addr, BYTE data )
{
//	DEBUGOUT( "MMC1 %04X=%02X\n", addr&0xFFFF,data&0xFF );

	if( wram_patch == 1 && addr == 0xBFFF ) {
		wram_count++;
		wram_bank += data&0x01;
		if( wram_count == 5 ) {
			if( wram_bank ) {
				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
			} else {
				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
			}
			wram_bank = wram_count = 0;
		}
	}

	if( patch != 1 ) {
		if((addr & 0x6000) != (last_addr & 0x6000)) {
			shift = regbuf = 0;
		}
		last_addr = addr;
	}

	if( data & 0x80 ) {
		shift = regbuf = 0;
//		reg[0] = 0x0C;		// D3=1,D2=1
		reg[0] |= 0x0C;		// D3=1,D2=1 残りはリセットされない
		return;
	}

	if( data&0x01 ) regbuf |= 1<<shift;
	if( ++shift < 5 )
		return;
	addr = (addr&0x7FFF)>>13;
	reg[addr] = regbuf;

//	DEBUGOUT( "MMC1 %d=%02X\n", addr&0xFFFF,regbuf&0xFF );

	regbuf = 0;
	shift = 0;

	if( patch != 1 ) {
	// For Normal Cartridge
		switch( addr ) {
			case	0:
				if( reg[0] & 0x02 ) {
					if( reg[0] & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
					else		    SetVRAM_Mirror( VRAM_VMIRROR );
				} else {
					if( reg[0] & 0x01 ) SetVRAM_Mirror( VRAM_MIRROR4H );
					else		    SetVRAM_Mirror( VRAM_MIRROR4L );
				}
				break;
			case	1:
				// Register #1
				if( VROM_1K_SIZE ) {
					if( reg[0] & 0x10 ) {
						// CHR 4K bank lower($0000-$0FFF)
						SetVROM_4K_Bank( 0, reg[1] );
						// CHR 4K bank higher($1000-$1FFF)
						SetVROM_4K_Bank( 4, reg[2] );
					} else {
						// CHR 8K bank($0000-$1FFF)
						SetVROM_8K_Bank( reg[1]>>1 );
					}
				} else {
					// For Romancia
					if( reg[0] & 0x10 ) {
						SetCRAM_4K_Bank( 0, reg[1] );
					}
				}
				break;
			case	2:
				// Register #2
				if( VROM_1K_SIZE ) {
					if( reg[0] & 0x10 ) {
						// CHR 4K bank lower($0000-$0FFF)
						SetVROM_4K_Bank( 0, reg[1] );
						// CHR 4K bank higher($1000-$1FFF)
						SetVROM_4K_Bank( 4, reg[2] );
					} else {
						// CHR 8K bank($0000-$1FFF)
						SetVROM_8K_Bank( reg[1]>>1 );
					}
				} else {
					// For Romancia
					if( reg[0] & 0x10 ) {
						SetCRAM_4K_Bank( 4, reg[2] );
					}
				}
				break;
			case	3:
				if( !(reg[0] & 0x08) ) {
				// PRG 32K bank ($8000-$FFFF)
					SetPROM_32K_Bank( reg[3]>>1 );
				} else {
					if( reg[0] & 0x04 ) {
					// PRG 16K bank ($8000-$BFFF)
						SetPROM_16K_Bank( 4, reg[3] );
						SetPROM_16K_Bank( 6, PROM_16K_SIZE-1 );
					} else {
					// PRG 16K bank ($C000-$FFFF)
						SetPROM_16K_Bank( 6, reg[3] );
						SetPROM_16K_Bank( 4, 0 );
					}
				}
				break;
		}
	} else {
		// For 512K/1M byte Cartridge
		INT	PROM_BASE = 0;
		if( PROM_16K_SIZE >= 32 ) {
			PROM_BASE = reg[1] & 0x10;
		}

		// For FinalFantasy I&II
		if( wram_patch == 2 ) {
			if( !(reg[1] & 0x18) ) {
				SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
			} else {
				SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
			}
		}

		// Register #0
		if( addr == 0 ) {
			if( reg[0] & 0x02 ) {
				if( reg[0] & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		    SetVRAM_Mirror( VRAM_VMIRROR );
			} else {
				if( reg[0] & 0x01 ) SetVRAM_Mirror( VRAM_MIRROR4H );
				else		    SetVRAM_Mirror( VRAM_MIRROR4L );
			}
		}
		// Register #1
		if( VROM_1K_SIZE ) {
			if( reg[0] & 0x10 ) {
			// CHR 4K bank lower($0000-$0FFF)
				SetVROM_4K_Bank( 0, reg[1] );
			} else {
			// CHR 8K bank($0000-$1FFF)
				SetVROM_8K_Bank( reg[1]>>1 );
			}
		} else {
			// For Romancia
			if( reg[0] & 0x10 ) {
				SetCRAM_4K_Bank( 0, reg[1] );
			}
		}
		// Register #2
		if( VROM_1K_SIZE ) {
			if( reg[0] & 0x10 ) {
			// CHR 4K bank higher($1000-$1FFF)
				SetVROM_4K_Bank( 4, reg[2] );
			}
		} else {
			// For Romancia
			if( reg[0] & 0x10 ) {
				SetCRAM_4K_Bank( 4, reg[2] );
			}
		}
		// Register #3
		if( !(reg[0] & 0x08) ) {
		// PRG 32K bank ($8000-$FFFF)
			SetPROM_32K_Bank( (reg[3]&(0xF+PROM_BASE))>>1 );
		} else {
			if( reg[0] & 0x04 ) {
			// PRG 16K bank ($8000-$BFFF)
				SetPROM_16K_Bank( 4, PROM_BASE+(reg[3]&0x0F) );
				if( PROM_16K_SIZE >= 32 ) SetPROM_16K_Bank( 6, PROM_BASE+16-1 );
			} else {
			// PRG 16K bank ($C000-$FFFF)
				SetPROM_16K_Bank( 6, PROM_BASE+(reg[3]&0x0F) );
				if( PROM_16K_SIZE >= 32 ) SetPROM_16K_Bank( 4, PROM_BASE );
			}
		}
	}
}

void	Mapper001::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = reg[2];
	p[3] = reg[3];
	p[4] = shift;
	p[5] = regbuf;

	p[6] = wram_bank;
	p[7] = wram_count;
}

void	Mapper001::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	reg[2] = p[2];
	reg[3] = p[3];
	shift  = p[4];
	regbuf = p[5];

	wram_bank  = p[6];
	wram_count = p[7];
}
