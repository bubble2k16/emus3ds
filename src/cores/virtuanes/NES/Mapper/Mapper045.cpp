//////////////////////////////////////////////////////////////////////////
// Mapper045  1000000-in-1                                              //
//////////////////////////////////////////////////////////////////////////
void	Mapper045::Reset()
{
	patch = 0;
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0;
	}

	prg0 = 0;
	prg1 = 1;
	prg2 = PROM_8K_SIZE-2;
	prg3 = PROM_8K_SIZE-1;

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x58bcacf6		// Kunio 8-in-1 (Pirate Cart)
	 || crc == 0x9103cfd6		// HIK 7-in-1 (Pirate Cart)
	 || crc == 0xc082e6d3 ) {	// Super 8-in-1 (Pirate Cart)
		patch = 1;
		prg2 = 62;
		prg3 = 63;
	}
	if( crc == 0xe0dd259d ) {	// Super 3-in-1 (Pirate Cart)
		patch = 2;
	}
	SetPROM_32K_Bank( prg0, prg1, prg2, prg3 );
	p[0] = prg0;
	p[1] = prg1;
	p[2] = prg2;
	p[3] = prg3;

	SetVROM_8K_Bank( 0 );

//	chr0 = c[0] = 0;
//	chr1 = c[1] = 0
//	chr2 = c[2] = 0;
//	chr3 = c[3] = 0;
//	chr4 = c[4] = 0;
//	chr5 = c[5] = 0;
//	chr6 = c[6] = 0;
//	chr7 = c[7] = 0;

	chr0 = c[0] = 0;
	chr1 = c[1] = 1;
	chr2 = c[2] = 2;
	chr3 = c[3] = 3;
	chr4 = c[4] = 4;
	chr5 = c[5] = 5;
	chr6 = c[6] = 6;
	chr7 = c[7] = 7;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_latched = 0;
	irq_reset = 0;
}

void	Mapper045::WriteLow( WORD addr, BYTE data )
{
//	if( addr == 0x6000 ) {
//	if( addr == 0x6000 && !(reg[3]&0x40) ) {
	if( !(reg[3]&0x40) ) {
		reg[reg[5]] = data;
		reg[5] = (reg[5]+1) & 0x03;

		SetBank_CPU_4( prg0 );
		SetBank_CPU_5( prg1 );
		SetBank_CPU_6( prg2 );
		SetBank_CPU_7( prg3 );
		SetBank_PPU();
	}
}

void	Mapper045::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			if( (data&0x40)!=(reg[6]&0x40) ) {
				BYTE	swp;
				swp = prg0; prg0 = prg2; prg2 = swp;
				swp = p[0]; p[0] = p[2]; p[2] = swp;
				SetBank_CPU_4( p[0] );
				SetBank_CPU_5( p[1] );
			}
			if( VROM_1K_SIZE ) {
				if( (data&0x80)!=(reg[6]&0x80) ) {
					INT	swp;
					swp = chr4; chr4 = chr0; chr0 = swp;
					swp = chr5; chr5 = chr1; chr1 = swp;
					swp = chr6; chr6 = chr2; chr2 = swp;
					swp = chr7; chr7 = chr3; chr3 = swp;
					swp = c[4]; c[4] = c[0]; c[0] = swp;
					swp = c[5]; c[5] = c[1]; c[1] = swp;
					swp = c[6]; c[6] = c[2]; c[2] = swp;
					swp = c[7]; c[7] = c[3]; c[3] = swp;
					SetVROM_8K_Bank( c[0],c[1],c[2],c[3],c[4],c[5],c[6],c[7] );
				}
			}
			reg[6] = data;
			break;
		case	0x8001:
			switch( reg[6] & 0x07 ) {
				case	0x00:
					chr0 = (data & 0xFE)+0;
					chr1 = (data & 0xFE)+1;
					SetBank_PPU();
					break;
				case	0x01:
					chr2 = (data & 0xFE)+0;
					chr3 = (data & 0xFE)+1;
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
					if( reg[6] & 0x40 ) {
						prg2 = data & 0x3F;
						SetBank_CPU_6( data );
					} else {
						prg0 = data & 0x3F;
						SetBank_CPU_4( data );
					}
					break;
				case	0x07:
					prg1 = data & 0x3F;
					SetBank_CPU_5( data );
					break;
			}
			break;
		case	0xA000:
			if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		  SetVRAM_Mirror( VRAM_VMIRROR );
			break;
		case	0xC000:
			if( patch == 2 ) {
			      if( data == 0x29 || data == 0x70 )
					data = 0x07;
			}
			irq_latch = data;
			irq_latched = 1;
			if( irq_reset ) {
				irq_counter = data;
				irq_latched = 0;
			}
//			irq_counter = data;
			break;
		case	0xC001:
//			irq_latch = data;
			irq_counter = irq_latch;
			break;
		case	0xE000:
			irq_enable = 0;
			irq_reset = 1;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			irq_enable = 1;
			if( irq_latched ) {
				irq_counter = irq_latch;
			}
			break;
	}
}

void	Mapper045::HSync( INT scanline )
{
	irq_reset = 0;
	if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
		if( irq_counter ) {
			irq_counter--;
			if( irq_counter == 0 ) {
				if( irq_enable ) {
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper045::SetBank_CPU_4( INT data )
{
	data &= (reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= reg[1];
	SetPROM_8K_Bank( 4, data );
	p[0] = data;
}

void	Mapper045::SetBank_CPU_5( INT data )
{
	data &= (reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= reg[1];
	SetPROM_8K_Bank( 5, data );
	p[1] = data;
}

void	Mapper045::SetBank_CPU_6( INT data )
{
	data &= (reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= reg[1];
	SetPROM_8K_Bank( 6, data );
	p[2] = data;
}

void	Mapper045::SetBank_CPU_7( INT data )
{
	data &= (reg[3] & 0x3F) ^ 0xFF;
	data &= 0x3F;
	data |= reg[1];
	SetPROM_8K_Bank( 7, data );
	p[3] = data;
}

void	Mapper045::SetBank_PPU()
{
	BYTE	table[16] = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x01,0x03,0x07,0x0F,0x1F,0x3F,0x7F,0xFF
	};

	c[0] = chr0;
	c[1] = chr1;
	c[2] = chr2;
	c[3] = chr3;
	c[4] = chr4;
	c[5] = chr5;
	c[6] = chr6;
	c[7] = chr7;

	for( INT i = 0; i < 8; i++ ) {
		c[i] &= table[reg[2] & 0x0F];
		c[i] |= reg[0] & ((patch!=1)?0xFF:0xC0);
		c[i] += (reg[2] & ((patch!=1)?0x10:0x30))<<4;
	}

	if( reg[6] & 0x80 ) {
		SetVROM_8K_Bank( c[4], c[5], c[6], c[7], c[0], c[1], c[2], c[3] );
	} else {
		SetVROM_8K_Bank( c[0], c[1], c[2], c[3], c[4], c[5], c[6], c[7] );
	}
}

void	Mapper045::SaveState( LPBYTE ps )
{
	INT	i;
	for( i = 0; i < 8; i++ ) {
		ps[i] = reg[i];
	}
	for( i = 0; i < 4; i++ ) {
		ps[i+8] = p[i];
	}
	for( i = 0; i < 8; i++ ) {
		*(INT*)&ps[i*4+64] = c[i];
	}
	ps[20] = prg0;
	ps[21] = prg1;
	ps[22] = prg2;
	ps[23] = prg3;
	ps[24] = chr0;
	ps[25] = chr1;
	ps[26] = chr2;
	ps[27] = chr3;
	ps[28] = chr4;
	ps[29] = chr5;
	ps[30] = chr6;
	ps[31] = chr7;
	ps[32] = irq_enable;
	ps[33] = irq_counter;
	ps[34] = irq_latch;
}

void	Mapper045::LoadState( LPBYTE ps )
{
	INT	i;
	for( i = 0; i < 8; i++ ) {
		reg[i] = ps[i];
	}
	for( i = 0; i < 4; i++ ) {
		p[i] = ps[i+8];
	}
	for( i = 0; i < 8; i++ ) {
		c[i] = *(INT*)&ps[i*4+64];
	}
	prg0 = ps[20];
	prg1 = ps[21];
	prg2 = ps[22];
	prg3 = ps[23];
	chr0 = ps[24];
	chr1 = ps[25];
	chr2 = ps[26];
	chr3 = ps[27];
	chr4 = ps[28];
	chr5 = ps[29];
	chr6 = ps[30];
	chr7 = ps[31];
	irq_enable  = ps[32];
	irq_counter = ps[33];
	irq_latch   = ps[34];
}

