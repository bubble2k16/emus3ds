//////////////////////////////////////////////////////////////////////////
// Mapper199  WaiXingTypeG Base ON Nintendo MMC3                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper199::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
		chr[i] = i;
	}
	prg[0] = 0x00;
	prg[1] = 0x01;
	prg[2] = PROM_8K_SIZE-2;
	prg[3] = PROM_8K_SIZE-1;
	SetBank_CPU();
	SetBank_PPU();

	irq_enable=irq_counter=irq_latch=irq_request=0;

	JMaddr = 0;
	JMaddrDAT[0] = JMaddrDAT[1] = JMaddrDAT[2] = 0;	

	we_sram = 0;
	nes->SetSAVERAM_SIZE( 32*1024 );
	nes->SetVideoMode( 1 );
	
//	DWORD	crcP = nes->rom->GetPROM_CRC();	
//	DWORD	crcV = nes->rom->GetVROM_CRC();
/*
	if( (crcP==0xE80D8741)||(crcV==0x3846520D))
	{//���ǰ����Ĵ�½
		nes->SetRenderMethod( NES::POST_ALL_RENDER );
	}
*/
}


BYTE	Mapper199::ReadLow( WORD addr )
{
//	DEBUGOUT( "ReadLow  A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, (Mapper::ReadLow( addr ))&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

	if( addr >= 0x5000 && addr <= 0x5FFF ){
		return	XRAM[addr-0x4000];
	}else if( addr >= 0x6000 && addr <= 0x7FFF ){
		if(JMaddr){
			switch( addr ) {
				case 0x6000: return JMaddrDAT[0];
				case 0x6010: return JMaddrDAT[1];
				case 0x6013: JMaddr=0; return JMaddrDAT[2];
			}
		}

		switch( we_sram ) {
			case 0xE4:
			case 0xEC: return WRAM[(addr&0x1FFF)+0x0000];
			case 0xE5:
			case 0xED: return WRAM[(addr&0x1FFF)+0x2000];
			case 0xE6:
			case 0xEE: return WRAM[(addr&0x1FFF)+0x4000];
			case 0xE7:
			case 0xEF: return WRAM[(addr&0x1FFF)+0x6000];
			default:   return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
		}

	}else{
		return	Mapper::ReadLow( addr );
	}
}

void	Mapper199::WriteLow( WORD addr, BYTE data )
{
//	DEBUGOUT( "WriteLow A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

	if( addr >= 0x5000 && addr <= 0x5FFF ){
		XRAM[addr-0x4000] = data;
		if((we_sram==0xA1)||(we_sram==0xA5)||(we_sram==0xA9)){
			JMaddr = 1;
			switch( addr ) {
				case 0x5000: JMaddrDAT[0] = data; break;
				case 0x5010: JMaddrDAT[1] = data; break;
				case 0x5013: JMaddrDAT[2] = data; break;
			}
		}
	}else if( addr >= 0x6000 && addr <= 0x7FFF ){

		switch( we_sram ) {
			case 0xE4:	//CPU_MEM_BANK
			case 0xEC:	//CPU_MEM_BANK
				WRAM[(addr&0x1FFF)+0x0000] = data;
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
				break;
			case 0xE5:	//SRAM
			case 0xED:	//SRAM
				WRAM[(addr&0x1FFF)+0x2000] = data;
				break;
			case 0xE6:
			case 0xEE:
				WRAM[(addr&0x1FFF)+0x4000] = data;
				break;
			case 0xE7:
			case 0xEF:
				WRAM[(addr&0x1FFF)+0x6000] = data;
				break;
			default:
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
				break;
		}

	}else{
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper199::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0x8001:
			reg[1] = data;

			switch( reg[0] & 0x0f ) {
				case 0x00:chr[0]=data;SetBank_PPU();break;
				case 0x01:chr[2]=data;SetBank_PPU();break;
				case 0x02:
				case 0x03:
				case 0x04:
				case 0x05:chr[(reg[0]&0x07)+2]=data;SetBank_PPU();break;
				case 0x06:
				case 0x07:
				case 0x08:
				case 0x09:prg[(reg[0]&0x0f)-6]=data;SetBank_CPU();break;
				case 0x0A:chr[1]=data;SetBank_PPU();break;
				case 0x0B:chr[3]=data;SetBank_PPU();break;
			}
			break;
		case	0xA000:
			reg[2] = data;
			data &= 0x03;
			if		( data == 0 )	SetVRAM_Mirror( VRAM_VMIRROR );
			else if ( data == 1 )	SetVRAM_Mirror( VRAM_HMIRROR );
			else if ( data == 2 )	SetVRAM_Mirror( VRAM_MIRROR4L );
			else					SetVRAM_Mirror( VRAM_MIRROR4H );
			break;
		case	0xA001:
//			DEBUGOUT( "MPRWR    A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			reg[3] = data;
			we_sram = data;
			break;
		case	0xC000:
			reg[4] = data;
			irq_counter = data;
			irq_request = 0;
			break;
		case	0xC001:
			reg[5] = data;
			irq_latch = data;
			irq_request = 0;
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
			break;
	}	
	
}

void	Mapper199::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable && !irq_request ) {
				if( scanline == 0 ) {
					if( irq_counter ) {
						irq_counter -= 1;
					}
				}
				if(!(irq_counter)){
					irq_request = 0xFF;
					irq_counter = irq_latch;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
				irq_counter--;
			}
		}
	}
}

void	Mapper199::SetBank_CPU()
{
	SetPROM_8K_Bank(4,prg[0 ^(reg[0]>>5&~(0<<1)&2)]);
	SetPROM_8K_Bank(5,prg[1 ^(reg[0]>>5&~(1<<1)&2)]);
	SetPROM_8K_Bank(6,prg[2 ^(reg[0]>>5&~(2<<1)&2)]);
	SetPROM_8K_Bank(7,prg[3 ^(reg[0]>>5&~(3<<1)&2)]);
}

void	Mapper199::SetBank_PPU()
{
	unsigned int bank = (reg[0]&0x80)>>5;
	for(int x=0; x<8; x++)
	{
		if( chr[x]<=7 )
			SetCRAM_1K_Bank( x^bank, chr[x] );
		else
			SetVROM_1K_Bank( x^bank, chr[x] );
	}	
}

void	Mapper199::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i] = reg[i];
	}
	for( INT i = 8; i < 12; i++ ) {
		p[i] = prg[i];
	}
	for( INT i = 8; i < 20; i++ ) {
		p[i] = chr[i];
	}
	p[20] = we_sram;
	p[21] = JMaddr;
	p[22] = JMaddrDAT[0];
	p[23] = JMaddrDAT[1];
	p[24] = JMaddrDAT[2];
	p[25] = irq_enable;
	p[26] = irq_counter;
	p[27] = irq_latch;
	p[28] = irq_request;
}

void	Mapper199::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
	}
	for( INT i = 8; i < 12; i++ ) {
		prg[i] = p[i];
	}
	for( INT i = 8; i < 20; i++ ) {
		chr[i] = p[i];
	}
	we_sram = p[20];
	JMaddr = p[21];
	JMaddrDAT[0] = p[22];
	JMaddrDAT[1] = p[23];
	JMaddrDAT[2] = p[24];
	irq_enable = p[25];
	irq_counter = p[26];
	irq_latch = p[27];
	irq_request = p[28];
}
