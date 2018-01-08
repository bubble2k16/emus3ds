//////////////////////////////////////////////////////////////////////////
// Mapper074  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper074::Reset()
{
	DWORD crc = nes->rom->GetPROM_CRC();

	if(crc == 0x227f8f9f) Decode_PROM();	//Ji Jia Zhan Shi (Ch) [new pcb dump]

	nes->ppu->SetVromWrite(1);
	for( INT j = 0; j < 8; j++ ) reg[j] = 0x00;
	prg0 = 0;
	prg1 = 1;
	prg2 = PROM_8K_SIZE-2;
	prg3 = PROM_8K_SIZE-1;
	SetBank_CPU();

	chr01 = 0;
	chr1  = 1;
	chr23 = 2;
	chr3  = 3;
	chr4  = 4;
	chr5  = 5;
	chr6  = 6;
	chr7  = 7;
	SetBank_PPU();
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;

	reg5000 = 0;
	reg5001 = 0;
	reg5002 = 0;
	reg5003 = 0;
	reg5010 = 0;
	reg5011 = 0;
	reg5012 = 0;
	reg5013 = 0;
	reg5FF3 = 0;

	JMaddr = 0;
	JMaddrDAT[0] = JMaddrDAT[1] = JMaddrDAT[2] = 0;	

	we_sram  = 0;
	nes->SetSAVERAM_SIZE( 32*1024 );

	sp_rom = 0;

	if(crc == 0x84966C88){	//[KT-1005] Feng Shen Bang (C)
		nes->SetSAVERAM_SIZE( 8*1024 );
		sp_rom = 1;
	}
	if(crc == 0x830BCF70){	//[KT-1015] Chu Liu Xiang Xin Zhuan (C)
		nes->SetSAVERAM_SIZE( 8*1024 );
		sp_rom = 2;
		bank = 0;
		prg2 = (PROM_8K_SIZE>>1)-2;
		prg3 = (PROM_8K_SIZE>>1)-1;
		SetBank_CPU();
	}

}


BYTE	Mapper074::ReadLow( WORD addr )
{
	DEBUGOUT("ReadLow : Address=%04X\n", addr&0xFFFF );

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

void	Mapper074::WriteLow( WORD addr, BYTE data )
{
	DEBUGOUT("WriteLow : Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );
/*
	if ((addr&0x4100)==0x4100) {	//ÖÐ¹ú´óºà[Sachen]

		if(data&2){
			SetCRAM_8K_Bank(0);
		}else
			SetVROM_1K_Bank(addr>>10,data);
//		data &= 0x03;
//		if		( data == 0 )	SetCRAM_8K_Bank( 2 );
//		else if ( data == 1 )	SetCRAM_8K_Bank( 3 );
//		else if ( data == 2 )	SetCRAM_8K_Bank( 0 );
//		else					SetCRAM_8K_Bank( 1 );

	}
*/
	if( addr >= 0x5000 && addr <= 0x5FFF ) {
		XRAM[addr-0x4000] = data;
		
		switch( addr ) {
			case	0x5000:
				reg5000 = data;
				if (sp_rom==2){	//[KT-1015] Chu Liu Xiang Xin Zhuan (C)
					bank = ((reg5000<<4)|(reg5003&0xf))>>2;
					SetPROM_32K_Bank( bank );}
				break;
			case	0x5001:
				reg5001 = data;
				break;
			case	0x5002:
				reg5002 = data;
				break;
			case	0x5003:
				reg5003 = data;
				break;
			case	0x5010:
				reg5010 = data;
				break;
			case	0x5011:
				reg5011 = data;
				break;
			case	0x5012:
				reg5012 = data;
				break;
			case	0x5013:
				reg5013 = data;
				break;
			case	0x5FF3:
				reg5FF3 = data;
				if(reg5FF3==2) SetPROM_32K_Bank(0,0,0,0);
				break;
		}

		if((we_sram==0xA5)||(we_sram==0xA9)){
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

void	Mapper074::Write( WORD addr, BYTE data )
{
//	DEBUGOUT("Write : Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );

	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0x8001:
			reg[1] = data;
			switch( reg[0] & 0x0f ) {
				case	0x00:
					chr01 = data;
					chr1 = chr01+1;
					SetBank_PPU();
					break;
				case	0x01:
					chr23 = data;
					chr3 = chr23+1;
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
					prg0 = data;
					SetBank_CPU();
					break;
				case	0x07:
					prg1 = data;
					SetBank_CPU();
					break;
				case	0x08:
					prg2 = data;
					SetBank_CPU();
					break;
				case	0x09:
					prg3 = data;
					SetBank_CPU();
					break;
				case	0x0a:
					chr1 = data;
					SetBank_PPU();
					break;
				case	0x0b:
					chr3 = data;
					SetBank_PPU();
					break;
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
//			DEBUGOUT("Write : Address=%04X Data=%02X\n", addr&0xFFFF, data&0xFF );
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

void	Mapper074::HSync( INT scanline )
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

void	Mapper074::SetBank_CPU()
{
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( prg2, prg1, prg0, prg3 );
	} else {
		SetPROM_32K_Bank( prg0, prg1, prg2, prg3 );
	}
}

void	Mapper074::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		if( reg[0] & 0x80 ) {
			SetVROM_1K_Bank( 4, chr01);
			SetVROM_1K_Bank( 5, chr1 );
			SetVROM_1K_Bank( 6, chr23);
			SetVROM_1K_Bank( 7, chr3 );
			SetVROM_1K_Bank( 0, chr4 );
			SetVROM_1K_Bank( 1, chr5 );
			SetVROM_1K_Bank( 2, chr6 );
			SetVROM_1K_Bank( 3, chr7 );
		} else {
			SetVROM_1K_Bank( 0, chr01);
			SetVROM_1K_Bank( 1, chr1 );
			SetVROM_1K_Bank( 2, chr23);
			SetVROM_1K_Bank( 3, chr3 );
			SetVROM_1K_Bank( 4, chr4 );
			SetVROM_1K_Bank( 5, chr5 );
			SetVROM_1K_Bank( 6, chr6 );
			SetVROM_1K_Bank( 7, chr7 );
			if(sp_rom==1){	//[KT-1005] Feng Shen Bang (C)
				SetVROM_2K_Bank( 0, chr01);
				SetVROM_2K_Bank( 2, chr23);
				SetVROM_2K_Bank( 4, chr4 );
				SetVROM_2K_Bank( 6, chr6 );
			}
		}
	} else {
		if( reg[0] & 0x80 ) {
			SetCRAM_1K_Bank( 4, (chr01+0)&0x07 );
			SetCRAM_1K_Bank( 5, (chr01+1)&0x07 );
			SetCRAM_1K_Bank( 6, (chr23+0)&0x07 );
			SetCRAM_1K_Bank( 7, (chr23+1)&0x07 );
			SetCRAM_1K_Bank( 0, chr4&0x07 );
			SetCRAM_1K_Bank( 1, chr5&0x07 );
			SetCRAM_1K_Bank( 2, chr6&0x07 );
			SetCRAM_1K_Bank( 3, chr7&0x07 );
		} else {
			SetCRAM_1K_Bank( 0, (chr01+0)&0x07 );
			SetCRAM_1K_Bank( 1, (chr01+1)&0x07 );
			SetCRAM_1K_Bank( 2, (chr23+0)&0x07 );
			SetCRAM_1K_Bank( 3, (chr23+1)&0x07 );
			SetCRAM_1K_Bank( 4, chr4&0x07 );
			SetCRAM_1K_Bank( 5, chr5&0x07 );
			SetCRAM_1K_Bank( 6, chr6&0x07 );
			SetCRAM_1K_Bank( 7, chr7&0x07 );
		}
	}
}

void	Mapper074::Decode_PROM()
{
		memcpy(BDRAM,PROM,0x80000);
		for(INT i=0;i<0x80000;i++)
		{
			switch(BDRAM[i]&0x0F){
				case 0x01:BDRAM[i]=(BDRAM[i]&0xF0)|0x08;break;
				case 0x02:BDRAM[i]=(BDRAM[i]&0xF0)|0x04;break;
				case 0x03:BDRAM[i]=(BDRAM[i]&0xF0)|0x0C;break;
				case 0x04:BDRAM[i]=(BDRAM[i]&0xF0)|0x02;break;
				case 0x05:BDRAM[i]=(BDRAM[i]&0xF0)|0x0A;break;
				case 0x07:BDRAM[i]=(BDRAM[i]&0xF0)|0x0E;break;
				case 0x08:BDRAM[i]=(BDRAM[i]&0xF0)|0x01;break;
				case 0x0A:BDRAM[i]=(BDRAM[i]&0xF0)|0x05;break;
				case 0x0B:BDRAM[i]=(BDRAM[i]&0xF0)|0x0D;break;
				case 0x0C:BDRAM[i]=(BDRAM[i]&0xF0)|0x03;break;
				case 0x0D:BDRAM[i]=(BDRAM[i]&0xF0)|0x0B;break;
				case 0x0E:BDRAM[i]=(BDRAM[i]&0xF0)|0x07;break;
			}
		}
		memcpy(PROM,BDRAM,0x80000);
		memcpy(&PROM[0x01*0x4000],&BDRAM[0x04*0x4000],0x4000);
		memcpy(&PROM[0x03*0x4000],&BDRAM[0x06*0x4000],0x4000);
		memcpy(&PROM[0x04*0x4000],&BDRAM[0x01*0x4000],0x4000);
		memcpy(&PROM[0x06*0x4000],&BDRAM[0x03*0x4000],0x4000);
		memcpy(&PROM[0x09*0x4000],&BDRAM[0x0C*0x4000],0x4000);
		memcpy(&PROM[0x0B*0x4000],&BDRAM[0x0E*0x4000],0x4000);
		memcpy(&PROM[0x0C*0x4000],&BDRAM[0x09*0x4000],0x4000);
		memcpy(&PROM[0x0E*0x4000],&BDRAM[0x0B*0x4000],0x4000);
		memcpy(&PROM[0x11*0x4000],&BDRAM[0x14*0x4000],0x4000);
}

void	Mapper074::SaveState( LPBYTE p )
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
	p[20] = prg2;
	p[21] = prg3;
	p[22] = chr1;
	p[23] = chr3;
}

void	Mapper074::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
	}
	prg0  = p[ 8];
	prg1  = p[ 9];
	prg2  = p[20];
	prg3  = p[21];
	chr01 = p[10];
	chr1  = p[22];
	chr23 = p[11];
	chr3  = p[23];
	chr4  = p[12];
	chr5  = p[13];
	chr6  = p[14];
	chr7  = p[15];
	irq_enable  = p[16];
	irq_counter = p[17];
	irq_latch   = p[18];
	irq_request = p[19];
}
