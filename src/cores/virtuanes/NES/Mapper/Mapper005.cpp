//////////////////////////////////////////////////////////////////////////
// Mapper005  Nintendo MMC5                                             //
//////////////////////////////////////////////////////////////////////////
#define	MMC5_IRQ_METAL		(1<<0)

void	Mapper005::Reset()
{
INT	i;

	prg_size = 3;
	chr_size = 3;

	sram_we_a = 0x00;
	sram_we_b = 0x00;

	graphic_mode = 0;
	nametable_mode = 0;

	for( i = 0; i < 4; i++ ) {
		nametable_type[i] = 0;
	}

	fill_chr = fill_pal = 0;
	split_control = split_scroll = split_page = 0;

	irq_enable = 0;
	irq_status = 0;
	irq_scanline = 0;
	irq_line = 0;
	irq_clear = 0;

	irq_type = 0;

	mult_a = mult_b = 0;

	chr_type = 0;
	chr_mode = 0;
	for( i = 0; i < 8; i++ ) {
		chr_page[0][i] = i;
		chr_page[1][i] = 4+(i&0x03);
	}

	// Bug fix?
	//SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
	SetPROM_32K_Bank( PROM_8K_SIZE-4, PROM_8K_SIZE-3, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

	for( i = 0; i < 8; i++ ) {
		BG_MEM_BANK[i] = VROM+0x0400*i;
		BG_MEM_PAGE[i] = i;
	}

	// SRAM�ݒ�
	SetBank_SRAM( 3, 0 );

	sram_size = 0;
	nes->SetSAVERAM_SIZE( 16*1024 );

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x2b548d75	// Bandit Kings of Ancient China(U)
	 || crc == 0xf4cd4998	// Dai Koukai Jidai(J)
	 || crc == 0x8fa95456	// Ishin no Arashi(J)
	 || crc == 0x98c8e090	// Nobunaga no Yabou - Sengoku Gunyuu Den(J)
	 || crc == 0x8e9a5e2f	// L'Empereur(Alt)(U)
	 || crc == 0x57e3218b	// L'Empereur(U)
	 || crc == 0x2f50bd38	// L'Empereur(J)
	 || crc == 0xb56958d1	// Nobunaga's Ambition 2(U)
	 || crc == 0xe6c28c5f	// Suikoden - Tenmei no Chikai(J)
	 || crc == 0xcd35e2e9 ) {	// Uncharted Waters(U)
		sram_size = 1;
		nes->SetSAVERAM_SIZE( 32*1024 );
	} else
	if( crc == 0xf4120e58	// Aoki Ookami to Shiroki Mejika - Genchou Hishi(J)
	 || crc == 0x286613d8	// Nobunaga no Yabou - Bushou Fuuun Roku(J)
	 || crc == 0x11eaad26	// Romance of the Three Kingdoms 2(U)
	 || crc == 0x95ba5733 ) {	// Sangokushi 2(J)
		sram_size = 2;
		nes->SetSAVERAM_SIZE( 64*1024 );
	}

	if( crc == 0x95ca9ec7 ) { // Castlevania 3 - Dracula's Curse(U)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0xcd9acf43 ) { // Metal Slader Glory(J)
		irq_type = MMC5_IRQ_METAL;
	}

	if( crc == 0xe91548d8 ) { // Shin 4 Nin Uchi Mahjong - Yakuman Tengoku(J)
		chr_type = 1;
	}

	nes->ppu->SetExtLatchMode( TRUE );
	nes->apu->SelectExSound( 8 );
}

BYTE	Mapper005::ReadLow( WORD addr )
{
BYTE	data = (BYTE)(addr>>8);

	switch( addr ) {
		case	0x5015:
			data = nes->apu->ExRead( addr );
			break;

		case	0x5204:
			data = irq_status;
//			irq_status = 0;
			irq_status &= ~0x80;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x5205:
			data = mult_a*mult_b;
			break;
		case	0x5206:
			data = (BYTE)(((WORD)mult_a*(WORD)mult_b)>>8);
			break;
	}

	if( addr >= 0x5C00 && addr <= 0x5FFF ) {
		if( graphic_mode >= 2 ) { // ExRAM mode
			data = VRAM[0x0800+(addr&0x3FF)];
		}
	} else if( addr >= 0x6000 && addr <= 0x7FFF ) {
		data = Mapper::ReadLow( addr );
	}

	return	data;
}

#include "stdio.h"
#include "3dsdbg.h"

void	Mapper005::WriteLow( WORD addr, BYTE data )
{
INT	i;

//#if	0
if( addr >= 0x5000 && addr <=0x5206 ) {
	dbgprintf ("$%04X=%02X C:%10d\n", addr, data, (int) nes->cpu->GetTotalCycles() );
}
//#endif

	switch( addr ) {
		case	0x5100:
			prg_size = data & 0x03;
			break;
		case	0x5101:
			chr_size = data & 0x03;
			break;

		case	0x5102:
			sram_we_a = data & 0x03;
			break;
		case	0x5103:
			sram_we_b = data & 0x03;
			break;

		case	0x5104:
			graphic_mode = data & 0x03;
			break;
		case	0x5105:
			nametable_mode = data;
			for( i = 0; i < 4; i++ ) {
				nametable_type[i] = data&0x03;
				SetVRAM_1K_Bank( 8+i, nametable_type[i] );
				data >>= 2;
			}
			break;

		case	0x5106:
			fill_chr = data;
			break;
		case	0x5107:
			fill_pal = data & 0x03;
			break;

		case	0x5113:
			SetBank_SRAM( 3, data&0x07 );
			break;

		case	0x5114:
		case	0x5115:
		case	0x5116:
		case	0x5117:
			SetBank_CPU( addr, data );
			break;

		case	0x5120:
		case	0x5121:
		case	0x5122:
		case	0x5123:
		case	0x5124:
		case	0x5125:
		case	0x5126:
		case	0x5127:
			chr_mode = 0;
			chr_page[0][addr&0x07] = data | (((INT)chr_bank_upper_bits) << 8);
			SetBank_PPU();
			break;

		case	0x5128:
		case	0x5129:
		case	0x512A:
		case	0x512B:
			chr_mode = 1;
			chr_page[1][(addr&0x03)+0] = data | (((INT)chr_bank_upper_bits) << 8);
			chr_page[1][(addr&0x03)+4] = data | (((INT)chr_bank_upper_bits) << 8);
			SetBank_PPU();
			break;

		// For 3DS: Additional MMC5 logic - handling of $5130 upper bits for 
		// subsequent $5120-$512b writes
		case	0x5130:
			chr_bank_upper_bits = (data & 0x3);
			break;

		case	0x5200:
			split_control = data;
			break;
		case	0x5201:
			split_scroll = data;
			break;
		case	0x5202:
			split_page = data&0x3F;
			break;

		case	0x5203:
			irq_line = data;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x5204:
			irq_enable = data;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0x5205:
			mult_a = data;
			break;
		case	0x5206:
			mult_b = data;
			break;

		default:
			if( addr >= 0x5000 && addr <= 0x5015 ) {
				nes->apu->ExWrite( addr, data );
			} else if( addr >= 0x5C00 && addr <= 0x5FFF ) {
				if( graphic_mode == 2 ) {		// ExRAM
					VRAM[0x0800+(addr&0x3FF)] = data;

					// For 3DS: Some games uses ExRAM to run code
					// So we must ensure this code is readable by CPU.cpp.
					// (CPU_MEM_BANK is already mapped to XRAM)
					//
					XRAM[addr - 0x4000] = data;		
				} else if( graphic_mode != 3 ) {	// Split,ExGraphic
					if( irq_status&0x40 ) {
						VRAM[0x0800+(addr&0x3FF)] = data;
					} else {
						VRAM[0x0800+(addr&0x3FF)] = 0;
					}
				}
			} else if( addr >= 0x6000 && addr <= 0x7FFF ) {
				if( (sram_we_a == 0x02) && (sram_we_b == 0x01) ) {
					if( CPU_MEM_TYPE[3] == BANKTYPE_RAM ) {
						CPU_MEM_BANK[3][addr&0x1FFF] = data;
					}
				}
			}
			break;
	}
}

void	Mapper005::Write( WORD addr, BYTE data )
{
	if( sram_we_a == 0x02 && sram_we_b == 0x01 ) {
		if( addr >= 0x8000 && addr < 0xE000 ) {
			if( CPU_MEM_TYPE[addr>>13] == BANKTYPE_RAM ) {
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
			}
		}
	}
}

void	Mapper005::SetBank_CPU( WORD addr, BYTE data )
{
	if( data & 0x80 ) {
	// PROM Bank
		switch( addr & 7 ) {
			case	4:
				if( prg_size == 3 ) {
					SetPROM_8K_Bank( 4, data&0x7F );
				}
				break;
			case	5:
				if( prg_size == 1 || prg_size == 2 ) {
					SetPROM_16K_Bank( 4, (data&0x7F)>>1 );
				} else if( prg_size == 3 ) {
					SetPROM_8K_Bank( 5, (data&0x7F) );
				}
				break;
			case	6:
				if( prg_size == 2 || prg_size == 3 ) {
					SetPROM_8K_Bank( 6, (data&0x7F) );
				}
				break;
			case	7:
				if( prg_size == 0 ) {
					SetPROM_32K_Bank( (data&0x7F)>>2 );
				} else if( prg_size == 1 ) {
					SetPROM_16K_Bank( 6, (data&0x7F)>>1 );
				} else if( prg_size == 2 || prg_size == 3 ) {
					SetPROM_8K_Bank( 7, (data&0x7F) );
				}
				break;
		}
	} else {
	// WRAM Bank
		switch( addr & 7 ) {
			case	4:
				if( prg_size == 3 ) {
					SetBank_SRAM( 4, data&0x07 );
				}
				break;
			case	5:
				if( prg_size == 1 || prg_size == 2 ) {
					SetBank_SRAM( 4, (data&0x06)+0 );
					SetBank_SRAM( 5, (data&0x06)+1 );
				} else if( prg_size == 3 ) {
					SetBank_SRAM( 5, data&0x07 );
				}
				break;
			case	6:
				if( prg_size == 2 || prg_size == 3 ) {
					SetBank_SRAM( 6, data&0x07 );
				}
				break;
		}
	}
}

void	Mapper005::SetBank_SRAM( BYTE page, BYTE data )
{
	if( sram_size == 0 ) data = (data > 3) ? 8 : 0;
	if( sram_size == 1 ) data = (data > 3) ? 1 : 0;
	if( sram_size == 2 ) data = (data > 3) ? 8 : data;
	if( sram_size == 3 ) data = (data > 3) ? 4 : data;

	if( data != 8 ) {
		SetPROM_Bank( page, &WRAM[0x2000*data], BANKTYPE_RAM );
		CPU_MEM_PAGE[page] = data;
	} else {
		CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	}
}

void	Mapper005::SetBank_PPU()
{
	// Use an 8K CHR-RAM for MMC5 games that require it.
	// (eg: Rockman 4 Minus Infinity)
	if (nes->rom->GetVROM() == NULL)
	{
		if( chr_mode == 0 ) {
			// PPU SP Bank
			switch( chr_size ) {
				case	0:
					SetCRAM_8K_Bank( chr_page[0][7] );
					break;
				case	1:
					SetCRAM_4K_Bank( 0, chr_page[0][3] );
					SetCRAM_4K_Bank( 4, chr_page[0][7] );
					break;
				case	2:
					SetCRAM_2K_Bank( 0, chr_page[0][1] );
					SetCRAM_2K_Bank( 2, chr_page[0][3] );
					SetCRAM_2K_Bank( 4, chr_page[0][5] );
					SetCRAM_2K_Bank( 6, chr_page[0][7] );
					break;
				case	3:
					SetCRAM_1K_Bank( 0, chr_page[0][0] );
					SetCRAM_1K_Bank( 1, chr_page[0][1] );
					SetCRAM_1K_Bank( 2, chr_page[0][2] );
					SetCRAM_1K_Bank( 3, chr_page[0][3] );
					SetCRAM_1K_Bank( 4, chr_page[0][4] );
					SetCRAM_1K_Bank( 5, chr_page[0][5] );
					SetCRAM_1K_Bank( 6, chr_page[0][6] );
					SetCRAM_1K_Bank( 7, chr_page[0][7] );

					break;
			}		
		}
		else
		{
			INT i;
			switch( chr_size ) {
				case	0:
					for( i = 0; i < 8; i++ ) {
						BG_MEM_BANK[i] = CRAM+0x2000*(chr_page[1][7]%1)+0x0400*i;
						BG_MEM_PAGE[i] = (chr_page[1][7]%1)*8+i;
					}
					break;
				case	1:
					for( i = 0; i < 4; i++ ) {
						BG_MEM_BANK[i+0] = CRAM+0x1000*(chr_page[1][3]%2)+0x0400*i;
						BG_MEM_BANK[i+4] = CRAM+0x1000*(chr_page[1][7]%2)+0x0400*i;
						BG_MEM_PAGE[i+0] = (chr_page[1][3]%2)*4+i;
						BG_MEM_PAGE[i+4] = (chr_page[1][7]%2)*4+i;
					}
					break;
				case	2:
					for( i = 0; i < 2; i++ ) {
						BG_MEM_BANK[i+0] = CRAM+0x0800*(chr_page[1][1]%4)+0x0400*i;
						BG_MEM_BANK[i+2] = CRAM+0x0800*(chr_page[1][3]%4)+0x0400*i;
						BG_MEM_BANK[i+4] = CRAM+0x0800*(chr_page[1][5]%4)+0x0400*i;
						BG_MEM_BANK[i+6] = CRAM+0x0800*(chr_page[1][7]%4)+0x0400*i;
						BG_MEM_PAGE[i+0] = (chr_page[1][1]%4)*2+i;
						BG_MEM_PAGE[i+2] = (chr_page[1][3]%4)*2+i;
						BG_MEM_PAGE[i+4] = (chr_page[1][5]%4)*2+i;
						BG_MEM_PAGE[i+6] = (chr_page[1][7]%4)*2+i;
					}
					break;
				case	3:
					for( i = 0; i < 8; i++ ) {
						BG_MEM_BANK[i] = CRAM+0x0400*(chr_page[1][i]%8);
						BG_MEM_PAGE[i] = (chr_page[1][i]%8)+i;
					}
					break;
			}	
		}
		return;
	}

	INT	i;
	if( chr_mode == 0 ) {
	// PPU SP Bank
		switch( chr_size ) {
			case	0:
				SetVROM_8K_Bank( chr_page[0][7] );
				break;
			case	1:
				SetVROM_4K_Bank( 0, chr_page[0][3] );
				SetVROM_4K_Bank( 4, chr_page[0][7] );
				break;
			case	2:
				SetVROM_2K_Bank( 0, chr_page[0][1] );
				SetVROM_2K_Bank( 2, chr_page[0][3] );
				SetVROM_2K_Bank( 4, chr_page[0][5] );
				SetVROM_2K_Bank( 6, chr_page[0][7] );
				break;
			case	3:
				SetVROM_8K_Bank( chr_page[0][0],
						 chr_page[0][1],
						 chr_page[0][2],
						 chr_page[0][3],
						 chr_page[0][4],
						 chr_page[0][5],
						 chr_page[0][6],
						 chr_page[0][7] );
				break;
		}
	} else if( chr_mode == 1 ) {
		// PPU BG Bank
		switch( chr_size ) {
			case	0:
				for( i = 0; i < 8; i++ ) {
					BG_MEM_BANK[i] = VROM+0x2000*(chr_page[1][7]%VROM_8K_SIZE)+0x0400*i;
					BG_MEM_PAGE[i] = (chr_page[1][7]%VROM_8K_SIZE)*8+i;
				}
				break;
			case	1:
				for( i = 0; i < 4; i++ ) {
					BG_MEM_BANK[i+0] = VROM+0x1000*(chr_page[1][3]%VROM_4K_SIZE)+0x0400*i;
					BG_MEM_BANK[i+4] = VROM+0x1000*(chr_page[1][7]%VROM_4K_SIZE)+0x0400*i;
					BG_MEM_PAGE[i+0] = (chr_page[1][3]%VROM_4K_SIZE)*4+i;
					BG_MEM_PAGE[i+4] = (chr_page[1][7]%VROM_4K_SIZE)*4+i;
				}
				break;
			case	2:
				for( i = 0; i < 2; i++ ) {
					BG_MEM_BANK[i+0] = VROM+0x0800*(chr_page[1][1]%VROM_2K_SIZE)+0x0400*i;
					BG_MEM_BANK[i+2] = VROM+0x0800*(chr_page[1][3]%VROM_2K_SIZE)+0x0400*i;
					BG_MEM_BANK[i+4] = VROM+0x0800*(chr_page[1][5]%VROM_2K_SIZE)+0x0400*i;
					BG_MEM_BANK[i+6] = VROM+0x0800*(chr_page[1][7]%VROM_2K_SIZE)+0x0400*i;
					BG_MEM_PAGE[i+0] = (chr_page[1][1]%VROM_2K_SIZE)*2+i;
					BG_MEM_PAGE[i+2] = (chr_page[1][3]%VROM_2K_SIZE)*2+i;
					BG_MEM_PAGE[i+4] = (chr_page[1][5]%VROM_2K_SIZE)*2+i;
					BG_MEM_PAGE[i+6] = (chr_page[1][7]%VROM_2K_SIZE)*2+i;
				}
				break;
			case	3:
				for( i = 0; i < 8; i++ ) {
					BG_MEM_BANK[i] = VROM+0x0400*(chr_page[1][i]%VROM_1K_SIZE);
					BG_MEM_PAGE[i] = (chr_page[1][i]%VROM_1K_SIZE)+i;
				}
				break;
		}
	}
}

void	Mapper005::HSync( INT scanline )
{
	if( irq_type & MMC5_IRQ_METAL ) {
		if( irq_scanline == irq_line ) {
			irq_status |= 0x80;
		}
	}

//	if( nes->ppu->IsDispON() && scanline < 239 ) {
	if( nes->ppu->IsDispON() && scanline < 240 ) {
		irq_scanline++;
		irq_status |= 0x40;
		irq_clear = 0;
	} else if( irq_type & MMC5_IRQ_METAL ) {
		irq_scanline = 0;
		irq_status &= ~0x80;
		irq_status &= ~0x40;
	}

	if( !(irq_type & MMC5_IRQ_METAL) ) {
		if( irq_scanline == irq_line ) {
			irq_status |= 0x80;
		}

		if( ++irq_clear > 2 ) {
			irq_scanline = 0;
			irq_status &= ~0x80;
			irq_status &= ~0x40;

			nes->cpu->ClrIRQ( IRQ_MAPPER );
		}
	}

	if( (irq_enable & 0x80) && (irq_status & 0x80) && (irq_status & 0x40) ) {
		nes->cpu->SetIRQ( IRQ_MAPPER );
///		nes->cpu->IRQ_NotPending();
#if	0
{
LPBYTE	lpScn = nes->ppu->GetScreenPtr();

	lpScn = lpScn+(256+16)*scanline;

	for( INT i = 0; i < 256+16; i++ ) {
		lpScn[i] = 22;
	}
}
#endif
	}

	// For Split mode!
	if( scanline == 0 ) {
		split_yofs = split_scroll&0x07;
		split_addr = ((split_scroll&0xF8)<<2);
	} else if( nes->ppu->IsDispON() ) {
		if( split_yofs == 7 ) {
			split_yofs = 0;
			if( (split_addr & 0x03E0) == 0x03A0 ) {
				split_addr &= 0x001F;
			} else {
				if( (split_addr & 0x03E0) == 0x03E0 ) {
					split_addr &= 0x001F;
				} else {
					split_addr += 0x0020;
				}
			}
		} else {
			split_yofs++;
		}
	}
}

void	Mapper005::PPU_ExtLatchX( INT x )
{
	split_x = x;
}

#include "3dsmain.h"

void	Mapper005::PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
WORD	ntbladr, attradr, tileadr, tileofs;
WORD	tile_yofs;
DWORD	tilebank;
BOOL	bSplit;

	tile_yofs = nes->ppu->GetTILEY();

	bSplit = FALSE;
	if( split_control & 0x80 ) {
		if( !(split_control&0x40) ) {
		// Left side
			if( (split_control&0x1F) > split_x ) {
				bSplit = TRUE;
			}
		} else {
		// Right side
			if( (split_control&0x1F) <= split_x ) {
				bSplit = TRUE;
			}
		}
	}

	if( !bSplit ) {
		if( nametable_type[(addr&0x0C00)>>10] == 3 ) {
		// Fill mode
			if( graphic_mode == 1 ) {
			// ExGraphic mode
				ntbladr = 0x2000+(addr&0x0FFF);
				// Get Nametable
				tileadr = fill_chr*0x10+tile_yofs;
				// Get TileBank
				tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
				// Attribute
				attr = (fill_pal<<2)&0x0C;
				// Get Pattern
				chr_l = VROM[tilebank+tileadr  ];
				chr_h = VROM[tilebank+tileadr+8];
			} else {
			// Normal
				tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
				tileadr = tileofs+fill_chr*0x10+tile_yofs;
				attr = (fill_pal<<2)&0x0C;
				// Get Pattern
				if( chr_type ) {
					chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				} else {
					chr_l = BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
					chr_h = BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
				}
			}
		} else if( graphic_mode == 1 ) {
		// ExGraphic mode
			ntbladr = 0x2000+(addr&0x0FFF);
			// Get Nametable
			tileadr = (WORD)PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get TileBank
			tilebank = 0x1000*((VRAM[0x0800+(ntbladr&0x03FF)]&0x3F)%VROM_4K_SIZE);
			// Get Attribute
			attr = (VRAM[0x0800+(ntbladr&0x03FF)]&0xC0)>>4;
			// Get Pattern
			chr_l = VROM[tilebank+tileadr  ];
			chr_h = VROM[tilebank+tileadr+8];
		} else {
		// Normal or ExVRAM
			tileofs = (PPUREG[0]&PPU_BGTBL_BIT)?0x1000:0x0000;
			ntbladr = 0x2000+(addr&0x0FFF);
			attradr = 0x23C0+(addr&0x0C00)+((addr&0x0380)>>4)+((addr&0x001C)>>2);
			// Get Nametable
			tileadr = tileofs+PPU_MEM_BANK[ntbladr>>10][ntbladr&0x03FF]*0x10+tile_yofs;
			// Get Attribute
			attr = PPU_MEM_BANK[attradr>>10][attradr&0x03FF];
			if( ntbladr & 0x0002 ) attr >>= 2;
			if( ntbladr & 0x0040 ) attr >>= 4;
			attr = (attr&0x03)<<2;


			
			// Get Pattern
			if( chr_type ) {
				chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			} else {
				chr_l = BG_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
				chr_h = BG_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
			}

		}
	} else {
		ntbladr = ((split_addr&0x03E0)|(split_x&0x1F))&0x03FF;
		// Get Split TileBank
		tilebank = 0x1000*((INT)split_page%VROM_4K_SIZE);
		tileadr  = (WORD)VRAM[0x0800+ntbladr]*0x10+split_yofs;
		// Get Attribute
		attradr = 0x03C0+((ntbladr&0x0380)>>4)+((ntbladr&0x001C)>>2);
		attr = VRAM[0x0800+attradr];
		if( ntbladr & 0x0002 ) attr >>= 2;
		if( ntbladr & 0x0040 ) attr >>= 4;
		attr = (attr&0x03)<<2;
		// Get Pattern
		chr_l = VROM[tilebank+tileadr  ];
		chr_h = VROM[tilebank+tileadr+8];
	}
}

void	Mapper005::SaveState( LPBYTE p )
{
	p[ 0] = prg_size;
	p[ 1] = chr_size;
	p[ 2] = sram_we_a;
	p[ 3] = sram_we_b;
	p[ 4] = graphic_mode;
	p[ 5] = nametable_mode;
	p[ 6] = nametable_type[0];
	p[ 7] = nametable_type[1];
	p[ 8] = nametable_type[2];
	p[ 9] = nametable_type[3];
	p[10] = sram_page;
	p[11] = fill_chr;
	p[12] = fill_pal;
	p[13] = split_control;
	p[14] = split_scroll;
	p[15] = split_page;
	p[16] = chr_mode;
	p[17] = irq_status;
	p[18] = irq_enable;
	p[19] = irq_line;
	p[20] = irq_scanline;
	p[21] = irq_clear;
	p[22] = mult_a;
	p[23] = mult_b;

	INT	i, j;
	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			p[24+j*8+i] = chr_page[j][i] & 0xff;
		}
	}

	// For 3DS: Save the additional MMC5 state information.
	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			p[40+j*8+i] = (chr_page[j][i] >> 8) & 0xff;
		}
	}

	p[56] = chr_bank_upper_bits;


//	for( i = 0; i < 8; i++ ) {
//		p[40+i] = BG_MEM_PAGE[i];
//	}
}

void	Mapper005::LoadState( LPBYTE p )
{
	prg_size	  = p[ 0];
	chr_size	  = p[ 1];
	sram_we_a	  = p[ 2];
	sram_we_b	  = p[ 3];
	graphic_mode	  = p[ 4];
	nametable_mode	  = p[ 5];
	nametable_type[0] = p[ 6];
	nametable_type[1] = p[ 7];
	nametable_type[2] = p[ 8];
	nametable_type[3] = p[ 9];
	sram_page	  = p[10];
	fill_chr	  = p[11];
	fill_pal	  = p[12];
	split_control	  = p[13];
	split_scroll	  = p[14];
	split_page	  = p[15];
	chr_mode          = p[16];
	irq_status	  = p[17];
	irq_enable	  = p[18];
	irq_line	  = p[19];
	irq_scanline	  = p[20];
	irq_clear	  = p[21];
	mult_a		  = p[22];
	mult_b		  = p[23];

	INT	i, j;

	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			chr_page[j][i] = p[24+j*8+i];
		}
	}

	// For 3DS: Load the additional MMC5 state information.
	for( j = 0; j < 2; j++ ) {
		for( i = 0; i < 8; i++ ) {
			chr_page[j][i] = chr_page[j][i] | (((INT)p[40+j*8+i]) << 8);
		}
	}

	chr_bank_upper_bits = p[56];
	
//	// BG�o���N�̍Đݒ菈��
//	for( i = 0; i < 8; i++ ) {
//		BG_MEM_PAGE[i] = p[40+i]%VROM_1K_SIZE;
//	}
//	for( i = 0; i < 8; i++ ) {
//		BG_MEM_BANK[i] = VROM+0x0400*BG_MEM_PAGE[i];
//	}

	SetBank_PPU();

}
