//////////////////////////////////////////////////////////////////////////
// Mapper112  ASDER Mapper                                              //
//////////////////////////////////////////////////////////////////////////

void	Mapper112::Reset()
{
	for( INT i = 0; i < 4; i++ ) {
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
}

void	Mapper112::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0xA000:
			reg[1] = data;
			switch( reg[0] & 0x07 ) {
				case	0x00:
					prg0 = (data&(PROM_8K_SIZE-1));
					SetBank_CPU();
					break;
				case	0x01:
					prg1 = (data&(PROM_8K_SIZE-1));
					SetBank_CPU();
					break;
				case	0x02:
					chr01 = data & 0xFE;
					SetBank_PPU();
					break;
				case	0x03:
					chr23 = data & 0xFE;
					SetBank_PPU();
					break;
				case	0x04:
					chr4 = data;
					SetBank_PPU();
					break;
				case	0x05:
					chr5 = data;
					SetBank_PPU();
					break;
				case	0x06:
					chr6 = data;
					SetBank_PPU();
					break;
				case	0x07:
					chr7 = data;
					SetBank_PPU();
					break;
			}
			break;

		case	0xC000:
			reg[3] = data;
			SetBank_PPU();

		case	0xE000:
			reg[2] = data;
			if( !nes->rom->Is4SCREEN() ) {
				if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
			}
			SetBank_PPU();
			break;
	}
}

void	Mapper112::SetBank_CPU()
{
	SetPROM_32K_Bank( prg0, prg1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper112::SetBank_PPU()
{
	if( reg[2] & 0x02 ) {
		SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1, chr4, chr5, chr6, chr7 );
	} else {
		SetVROM_8K_Bank(((reg[3]<<6)&0x100)+chr01, 
				((reg[3]<<6)&0x100)+chr01+1, 
				((reg[3]<<5)&0x100)+chr23, 
				((reg[3]<<5)&0x100)+chr23+1, 
				((reg[3]<<4)&0x100)+chr4, 
				((reg[3]<<3)&0x100)+chr5, 
				((reg[3]<<2)&0x100)+chr6, 
				((reg[3]<<1)&0x100)+chr7 );
	}
}

void	Mapper112::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 4; i++ ) {
		p[i] = reg[i];
	}
	p[ 4] = chr01;
	p[ 5] = chr23;
	p[ 6] = chr4;
	p[ 7] = chr5;
	p[ 8] = chr6;
	p[ 9] = chr7;
}

void	Mapper112::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 4; i++ ) {
		reg[i] = p[i];
	}

	chr01 = p[ 4];
	chr23 = p[ 5];
	chr4  = p[ 6];
	chr5  = p[ 7];
	chr6  = p[ 8];
	chr7  = p[ 9];
}
