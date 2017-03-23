//////////////////////////////////////////////////////////////////////////
// Mapper114                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper114::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	reg_a = reg_c = reg_m = 0;
	for( INT i = 0; i < 8; i++ ) {
		reg_b[i] = 0;
	}
	irq_counter = 0;
	irq_occur = 0;

	nes->SetRenderMethod( NES::POST_RENDER );
}

void	Mapper114::WriteLow( WORD addr, BYTE data )
{
	reg_m = data;
	SetBank_CPU();
}

void	Mapper114::Write( WORD addr, BYTE data )
{
	if( addr == 0xE003 ) {
		irq_counter = data;
	} else
	if( addr == 0xE002 ) {
		irq_occur = 0;
		nes->cpu->ClrIRQ( IRQ_MAPPER );
	} else {
		switch( addr & 0xE000 ) {
			case	0x8000:
				if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
				break;
			case	0xA000:
				reg_c = 1;
				reg_a = data;
				break;
			case	0xC000:
				if( !reg_c ) {
					break;
				}
				reg_b[reg_a&0x07] = data;
				switch( reg_a & 0x07 ) {
					case	0:
					case	1:
					case	2:
					case	3:
					case	6:
					case	7:
						SetBank_PPU();
						break;
					case	4:
					case	5:
						SetBank_CPU();
						break;
				}
				reg_c = 0;
				break;
		}
	}
}

void	Mapper114::Clock( INT scanline )
{
//	if( irq_occur ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper114::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) && nes->ppu->IsDispON() ) {
		if( irq_counter ) {
			irq_counter--;
			if( !irq_counter ) {
				irq_occur = 0xFF;
				nes->cpu->SetIRQ( IRQ_MAPPER );
			}
		}
	}
}

void	Mapper114::SetBank_CPU()
{
	if( reg_m & 0x80 ) {
		SetPROM_16K_Bank( 4, reg_m & 0x1F );
	} else {
		SetPROM_8K_Bank( 4, reg_b[4] );
		SetPROM_8K_Bank( 5, reg_b[5] );
	}
}

void	Mapper114::SetBank_PPU()
{
	SetVROM_2K_Bank( 0, reg_b[0]>>1 );
	SetVROM_2K_Bank( 2, reg_b[2]>>1 );
	SetVROM_1K_Bank( 4, reg_b[6] );
	SetVROM_1K_Bank( 5, reg_b[1] );
	SetVROM_1K_Bank( 6, reg_b[7] );
	SetVROM_1K_Bank( 7, reg_b[3] );
}

void	Mapper114::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i] = reg_b[i];
	}

	p[ 8] = reg_m;
	p[ 9] = reg_a;
	p[10] = reg_c;
	p[11] = irq_counter;
	p[12] = irq_occur;
}

void	Mapper114::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg_b[i] = p[i];
	}
	reg_m = p[ 8];
	reg_a = p[ 9];
	reg_c = p[10];
	irq_counter = p[11];
	irq_occur   = p[12];
}
