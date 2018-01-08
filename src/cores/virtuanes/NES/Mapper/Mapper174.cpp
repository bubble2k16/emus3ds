//////////////////////////////////////////////////////////////////////////
// Mapper174         Game Star - Smart Genius (Unl)                     //
//////////////////////////////////////////////////////////////////////////
void	Mapper174::Reset()
{
	for( INT i = 0; i < 11; i++ ) {
		reg[i] = 0x00;
	}
	prg0 = 60;
	prg1 = 61;
	SetPROM_32K_Bank( prg0, prg1, 62, 63 );
	chr01 = 0;
	chr23 = 2;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	SetBank_PPU();
}

void	Mapper174::WriteLow( WORD addr, BYTE data )
{

	if(addr>=0x4000 && addr<=0x5FFF)
		DEBUGOUT( "WriteLow - addr= %04x ; dat= %03x\n", addr, data );

	switch( addr ) {
		case	0x5010:
			reg[8] = data;
			break;
		case	0x5011:
			reg[9] = data;
			SetBank_CPU_L();
			break;
		case	0x5012:
			reg[10] = data;
			SetBank_CPU_L();
			break;
	}

	if(addr>=0x6000){
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}


void	Mapper174::Write( WORD addr, BYTE data )
{
	DEBUGOUT( "Write    - addr= %04x ; dat= %03x\n", addr, data );

	if(addr==0xA000){
		data &= 0x03;
		if(data==0)			SetVRAM_Mirror( VRAM_VMIRROR );
		else if(data==1)	SetVRAM_Mirror( VRAM_HMIRROR );
		else if(data==2)	SetVRAM_Mirror( VRAM_MIRROR4L );
		else				SetVRAM_Mirror( VRAM_MIRROR4H );
	}

	if((reg[8]&0x07)<0x05){
		switch( addr ) {
			case	0x8000:
				reg[0] = data;
				break;
			case	0x8001:
				reg[1] = data;
				switch( reg[0] & 0x0f ) {
					case	0x00:
						chr01 = data;
						SetBank_PPU();
						break;
					case	0x01:
						chr23 = data;
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
						prg0 = data + ((reg[9]&0x7F)*2) + (reg[10]*2);
						SetBank_CPU();
						break;
					case	0x07:
						prg1 = data + ((reg[9]&0x7F)*2) + (reg[10]*2);
						SetBank_CPU();
						break;
				}
				break;
		}
	}
/*
	if((reg[8]&0x07)==0x04)
		if(addr==0x8000)
			if(data==0x00)
				SetPROM_32K_Bank( 60, 61, 62, 63 );
*/	
	if((reg[8]&0x07)==0x05)
		SetPROM_16K_Bank( 4, (reg[9] & 0x7F) + data + reg[10] );

}

void	Mapper174::SetBank_CPU_L()
{
		switch( reg[8] & 0x07 ) {
			case	0x00:
				SetPROM_16K_Bank( 6, (reg[9]&0x70) + 0 + reg[10] );
				SetPROM_16K_Bank( 6, (reg[9]&0x70) + 31 + reg[10] );
				break;
			case	0x01:
				SetPROM_16K_Bank( 6, (reg[9]&0x70) + 0 + reg[10] );
				SetPROM_16K_Bank( 6, (reg[9]&0x70) + 15 + reg[10] );
				break;
			case	0x02:	//MMC3 mode(???)
				SetPROM_16K_Bank( 4, (reg[9]&0x7F) + 6 + reg[10] );
				SetPROM_16K_Bank( 6, (reg[9]&0x7F) + 7 + reg[10] );
				break;
			case	0x03:	//Mapper0 mode 16K
				SetPROM_16K_Bank( 4, (reg[9]&0x7F) + reg[10] );
				SetPROM_16K_Bank( 6, (reg[9]&0x7F) + reg[10] );
				break;
			case	0x04:	//normal mode 32K
				SetPROM_32K_Bank( ((reg[9]&0x7F)>>1) + (reg[10]>>1) );
				break;
			case	0x05:	//UNROM(mapper2) mode(???)
				SetPROM_16K_Bank( 4, (reg[9] & 0x7F) + 0 + reg[10] );
				SetPROM_16K_Bank( 6, (reg[9] & 0x7F) + 7 + reg[10] );
				break;
		}
}

void	Mapper174::SetBank_CPU()
{
	SetPROM_8K_Bank( 4, prg0 );
	SetPROM_8K_Bank( 5, prg1 );
}

void	Mapper174::SetBank_PPU()
{
	SetCRAM_1K_Bank( 0, (chr01+0)&0x07 );
	SetCRAM_1K_Bank( 1, (chr01+1)&0x07 );
	SetCRAM_1K_Bank( 2, (chr23+0)&0x07 );
	SetCRAM_1K_Bank( 3, (chr23+1)&0x07 );
	SetCRAM_1K_Bank( 4, chr4&0x07 );
	SetCRAM_1K_Bank( 5, chr5&0x07 );
	SetCRAM_1K_Bank( 6, chr6&0x07 );
	SetCRAM_1K_Bank( 7, chr7&0x07 );
}
