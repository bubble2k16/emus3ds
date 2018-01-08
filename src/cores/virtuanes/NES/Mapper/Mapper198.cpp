//////////////////////////////////////////////////////////////////////////
// Mapper198  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper198::Reset()
{
	
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
	}

	prg0 = 0;
	prg1 = 1;
	prg2 = PROM_8K_SIZE-2;
	prg3 = PROM_8K_SIZE-1;
	SetBank_CPU();

	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	SetBank_PPU();

	reg6800 = 0;
	reg6803 = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x935F2119 ) {	//[ES-1110] Cheng Ji Si Han (C)
		sp_rom = 1;
		nes->SetSAVERAM_SIZE( 16*1024 );
		wram_bank  = 0;
		wram_count = 0;
	}

}

void	Mapper198::WriteLow( WORD addr, BYTE data )
{

//	if((addr!=0x7FA2)&&(addr!=0x7FA3)&&(addr>=0x6000)) DEBUGOUT("Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );
//	 DEBUGOUT("Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );

	if(nes->rom->GetPROM_CRC()==0x2779bb41){
		switch( addr ) {	//[NJ064] Sudoku (C)
			case	0x6800:
				reg6800 = data;
				break;
			case	0x6803:
				reg6803 = data;
				if((reg6800==0xe0)&&(reg6803==0x97)){
					prg0 = (PROM_8K_SIZE>>1)-4;
					prg1 = (PROM_8K_SIZE>>1)-3;
					prg2 = (PROM_8K_SIZE>>1)-2;
					prg3 = (PROM_8K_SIZE>>1)-1;
					SetBank_CPU();
				}
				if((reg6800==0xe1)&&(reg6803==0x97)){
					prg0 = (PROM_8K_SIZE>>2)-4;
					prg1 = (PROM_8K_SIZE>>2)-3;
					prg2 = (PROM_8K_SIZE>>2)-2;
					prg3 = (PROM_8K_SIZE>>2)-1;
					SetBank_CPU();
				}
				break;
		}
	}

	if((sp_rom==1)&&(addr==0x5226)) {
		if( data ) {
			SetPROM_Bank( 3, &WRAM[0x0000], BANKTYPE_RAM );
		} else {
			SetPROM_Bank( 3, &WRAM[0x2000], BANKTYPE_RAM );
		}
	}

	if( (addr>0x4018 && addr<0x6000)||(sp_rom==1) )
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	else
		adr5000buf[addr&0xFFF] = data;
}
BYTE	Mapper198::ReadLow( WORD addr )
{
	if( (addr>0x4018 && addr<0x6000)||(sp_rom==1) )
		return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	else
		return	adr5000buf[addr&0xFFF];
}

void	Mapper198::Write( WORD addr, BYTE data )
{

//	DEBUGOUT("Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );

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
					if(data>=0x50) data&=0x4F;
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
			break;
		case	0xC001:
			reg[5] = data;
			break;
		case	0xE000:
			reg[6] = data;
			break;
		case	0xE001:
			reg[7] = data;
			break;
	}
}

void	Mapper198::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( prg2, prg1, prg0, prg3 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, prg2, prg3 );
	}
}

void	Mapper198::SetBank_PPU()
{

	if( VROM_1K_SIZE ) {
		if( reg[0] & 0x80 ) {
			SetVROM_8K_Bank( chr4, chr5, chr6, chr7,
					 chr01, chr01+1, chr23, chr23+1 );
		} else {
			SetVROM_8K_Bank( chr01, chr01+1, chr23, chr23+1,
					 chr4, chr5, chr6, chr7 );
		}
	}
}

void	Mapper198::SaveState( LPBYTE p )
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
}

void	Mapper198::LoadState( LPBYTE p )
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
}
