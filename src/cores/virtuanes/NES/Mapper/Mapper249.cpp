//////////////////////////////////////////////////////////////////////////
// Mapper249  MMC3                                                      //
//////////////////////////////////////////////////////////////////////////
void	Mapper249::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
	}
	prg0 = 0;
	prg1 = 1;

	SetPROM_32K_Bank( 0,1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;

	SetVROM_8K_Bank( 0 );

	we_sram  = 0;	// Disable
	irq_enable = 0;	// Disable
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;

	// IRQƒ^ƒCƒvÝ’è
	nes->SetIrqType( NES::IRQ_CLOCK );

	spdata = 0;
}

void	Mapper249::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x5000 ) {
		switch( data ) {
			case	0x00:
				spdata = 0;
				break;
			case	0x02:
				spdata = 1;
				break;
		}
	}

	if( addr>=0x6000 && addr<0x8000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

void	Mapper249::Write( WORD addr, BYTE data )
{
	BYTE	m0,m1,m2,m3,m4,m5,m6,m7;

	switch( addr&0xFF01 ) {
		case	0x8000:
		case    0x8800:
			reg[0] = data;
			break;
		case	0x8001:
		case    0x8801:
			switch( reg[0] & 0x07 ) {
				case	0x00:
					if( spdata == 1 ) {
						m0=data&0x1;
						m1=(data&0x02)>>1;
						m2=(data&0x04)>>2;
						m3=(data&0x08)>>3;
						m4=(data&0x10)>>4;
						m5=(data&0x20)>>5;
						m6=(data&0x40)>>6;
						m7=(data&0x80)>>7;
						data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 0, data&0xFE );
					SetVROM_1K_Bank( 1, data|0x01 );
					break;
				case	0x01:
					if( spdata == 1 ) {
						m0=data&0x1;
						m1=(data&0x02)>>1;
						m2=(data&0x04)>>2;
						m3=(data&0x08)>>3;
						m4=(data&0x10)>>4;
						m5=(data&0x20)>>5;
						m6=(data&0x40)>>6;
						m7=(data&0x80)>>7;
						data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 2, data&0xFE );
					SetVROM_1K_Bank( 3, data|0x01 );
					break;
				case	0x02:
					if( spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 4, data );
					break;
				case	0x03:
					if( spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 5, data );
					break;
				case	0x04:
					if( spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 6, data );
					break;
				case	0x05:
					if( spdata == 1 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
					}
					SetVROM_1K_Bank( 7, data );
					break;
				case	0x06:
					if( spdata == 1 ) {
						if( data < 0x20 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=0;
							m6=0;
							m7=0;
							data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
						} else {
							data=data-0x20;
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
						}
					}
					SetPROM_8K_Bank( 4, data );
					break;
				case	0x07:
					if( spdata == 1 ) {
						if( data < 0x20 ) {
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=0;
							m6=0;
							m7=0;
							data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
						} else {
							data=data-0x20;
							m0=data&0x1;
							m1=(data&0x02)>>1;
							m2=(data&0x04)>>2;
							m3=(data&0x08)>>3;
							m4=(data&0x10)>>4;
							m5=(data&0x20)>>5;
							m6=(data&0x40)>>6;
							m7=(data&0x80)>>7;
							data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
						}
					}
					SetPROM_8K_Bank( 5, data );
					break;
			}
			break;
		case	0xA000:
		case    0xA800:
			reg[2] = data;
			if( !nes->rom->Is4SCREEN() ) {
				if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
			}
			break;
		case	0xA001:
		case    0xA801:
			reg[3] = data;
			break;
		case	0xC000:
		case    0xC800:
			reg[4] = data;
			irq_counter = data;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xC001:
		case    0xC801:
			reg[5] = data;
			irq_latch = data;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE000:
		case    0xE800:
			reg[6] = data;
			irq_enable = 0;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
		case    0xE801:
			reg[7] = data;
			irq_enable = 1;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper249::Clock( INT cycles )
{
//	if( irq_request && (nes->GetIrqType() == NES::IRQ_CLOCK) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper249::HSync( INT scanline )
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
//	if( irq_request && (nes->GetIrqType() == NES::IRQ_HSYNC) ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper249::SaveState( LPBYTE p )
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
	p[20] = spdata;
}

void	Mapper249::LoadState( LPBYTE p )
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
	spdata = p[20];
}
