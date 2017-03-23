//////////////////////////////////////////////////////////////////////////
// Mapper187  Street Fighter Zero 2 97                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper187::Reset()
{
INT	i;

	for( i = 0; i < 8; i++ ) {
		chr[i] = 0x00;
		bank[i] = 0x00;
	}

	prg[0] = PROM_8K_SIZE-4;
	prg[1] = PROM_8K_SIZE-3;
	prg[2] = PROM_8K_SIZE-2;
	prg[3] = PROM_8K_SIZE-1;
	SetBank_CPU();

	ext_mode = 0;
	chr_mode = 0;
	ext_enable = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;

	last_write = 0;

	nes->SetRenderMethod( NES::POST_ALL_RENDER );
}

BYTE	Mapper187::ReadLow( WORD addr )
{
	switch( last_write&0x03 ) {
		case 0:
			return	0x83;
		case 1:
			return	0x83;
		case 2:
			return	0x42;
		case 3:
			return	0x00;
	}
	return 0;
}

void	Mapper187::WriteLow( WORD addr, BYTE data )
{
	last_write = data;
	if( addr == 0x5000 ) {
		ext_mode = data;
		if( data & 0x80 ) {
			if( data & 0x20 ) {
				prg[0] = ((data&0x1E)<<1)+0;
				prg[1] = ((data&0x1E)<<1)+1;
				prg[2] = ((data&0x1E)<<1)+2;
				prg[3] = ((data&0x1E)<<1)+3;
			} else {
				prg[2] = ((data&0x1F)<<1)+0;
				prg[3] = ((data&0x1F)<<1)+1;
			}
		} else {
			prg[0] = bank[6];
			prg[1] = bank[7];
			prg[2] = PROM_8K_SIZE-2;
			prg[3] = PROM_8K_SIZE-1;
		}
		SetBank_CPU();
	}
}

void	Mapper187::Write( WORD addr, BYTE data )
{
	last_write = data;
	switch( addr ) {
		case	0x8003:
			ext_enable = 0xFF;
//			if( (data&0x80) != (chr_mode&0x80) ) {
//				for( INT i = 0; i < 4; i++ ) {
//					INT temp = chr[i];
//					chr[i] = chr[i+4];
//					chr[i+4] = temp;
//				}
//				SetBank_PPU();
//			}
			chr_mode = data;
			if( (data&0xF0) == 0 ) {
				prg[2] = PROM_8K_SIZE-2;
				SetBank_CPU();
			}
			break;

		case	0x8000:
			ext_enable = 0;
//			if( (data&0x80) != (chr_mode&0x80) ) {
//				for( INT i = 0; i < 4; i++ ) {
//					INT temp = chr[i];
//					chr[i] = chr[i+4];
//					chr[i+4] = temp;
//				}
//				SetBank_PPU();
//			}
			chr_mode = data;
			break;

		case	0x8001:
			if( !ext_enable ) {
				switch( chr_mode & 7 ) {
					case	0:
						data &= 0xFE;
						chr[4] = (INT)data+0x100;
						chr[5] = (INT)data+0x100+1;
//						chr[0+((chr_mode&0x80)?4:0)] = data;
//						chr[1+((chr_mode&0x80)?4:0)] = data+1;
						SetBank_PPU();
						break;
					case	1:
						data &= 0xFE;
						chr[6] = (INT)data+0x100;
						chr[7] = (INT)data+0x100+1;
//						chr[2+((chr_mode&0x80)?4:0)] = data;
//						chr[3+((chr_mode&0x80)?4:0)] = data+1;
						SetBank_PPU();
						break;
					case	2:
						chr[0] = data;
//						chr[0+((chr_mode&0x80)?0:4)] = data;
						SetBank_PPU();
						break;
					case	3:
						chr[1] = data;
//						chr[1+((chr_mode&0x80)?0:4)] = data;
						SetBank_PPU();
						break;
					case	4:
						chr[2] = data;
//						chr[2+((chr_mode&0x80)?0:4)] = data;
						SetBank_PPU();
						break;
					case	5:
						chr[3] = data;
//						chr[3+((chr_mode&0x80)?0:4)] = data;
						SetBank_PPU();
						break;
					case	6:
						if( (ext_mode&0xA0)!=0xA0 ) {
							prg[0] = data;
							SetBank_CPU();
						}
						break;
					case	7:
						if( (ext_mode&0xA0)!=0xA0 ) {
							prg[1] = data;
							SetBank_CPU();
						}
						break;
					default:
						break;
				}
			} else {
				switch( chr_mode ) {
					case	0x2A:
						prg[1] = 0x0F;
						break;
					case	0x28:
						prg[2] = 0x17;
						break;
					case	0x26:
						break;
					default:
						break;
				}
				SetBank_CPU();
			}
			bank[chr_mode&7] = data;
			break;

		case	0xA000:
			if( data & 0x01 ) {
				SetVRAM_Mirror( VRAM_HMIRROR );
			} else {
				SetVRAM_Mirror( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
			break;

		case	0xC000:
			irq_counter = data;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xC001:
			irq_latch = data;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE000:
		case	0xE002:
			irq_enable = 0;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
		case	0xE003:
			irq_enable = 1;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper187::Clock( INT cycles )
{
//	if( irq_occur ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper187::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !irq_counter ) {
					irq_counter--;
					irq_enable = 0;
					irq_occur = 0xFF;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				} else {
					irq_counter--;
				}
			}
		}
	}
}

void	Mapper187::SetBank_CPU()
{
	SetPROM_32K_Bank( prg[0], prg[1], prg[2], prg[3] );
}

void	Mapper187::SetBank_PPU()
{
	SetVROM_8K_Bank( chr[0], chr[1], chr[2], chr[3],
			 chr[4], chr[5], chr[6], chr[7] );
}

void	Mapper187::SaveState( LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		p[i] = prg[i];
	}
	for( i = 0; i < 8; i++ ) {
		p[4+i] = bank[i];
	}
	for( i = 0; i < 8; i++ ) {
		*((INT*)&p[12+i*sizeof(INT)]) = chr[i];
	}

	p[44] = ext_mode;
	p[45] = chr_mode;
	p[46] = ext_enable;
	p[47] = irq_enable;
	p[48] = irq_counter;
	p[49] = irq_latch;
	p[50] = irq_occur;
	p[51] = last_write;
}

void	Mapper187::LoadState( LPBYTE p )
{
INT	i;

	for( i = 0; i < 4; i++ ) {
		prg[i] = p[i];
	}
	for( i = 0; i < 8; i++ ) {
		bank[i] = p[4+i];
	}
	for( i = 0; i < 8; i++ ) {
		chr[i] = *((INT*)&p[12+i*sizeof(INT)]);
	}
	ext_mode    = p[44];
	chr_mode    = p[45];
	ext_enable  = p[46];
	irq_enable  = p[47];
	irq_counter = p[48];
	irq_latch   = p[49];
	irq_occur   = p[50];
	last_write  = p[51];
}
