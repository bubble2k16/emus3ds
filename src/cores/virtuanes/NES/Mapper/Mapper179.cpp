//////////////////////////////////////////////////////////////////////////
// Mapper179        for test new dump                                   //
//////////////////////////////////////////////////////////////////////////
void	Mapper179::Reset()
{
	for(INT i=0;i<8;i++) reg[i]=0;

//---------------[for WaiXing F001_XXXXX]------------------------------------[OK]
//	SetPROM_32K_Bank( 0, 1, 0, 1 );
//---------------------------------------------------------------------------[OK]
/*
//-----------[for Dragon Ball Z II - Gekishin Freeza!!_Plus (J)]-------------
	for( i = 0; i < 8; i++ ) reg[i] = i;
	reg[8] = 0;
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
//	nes->SetSAVERAM_SIZE( 256 );
//	x24c02.Reset( WRAM );
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
//---------------------------------------------------------------------------
*/
//	SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
//	SetVROM_8K_Bank( 0 );

	SetPROM_32K_Bank( 0 );

}

BYTE	Mapper179::ReadLow( WORD addr )
{
	DEBUGOUT( "ReadLow  - addr= %04x ; dat= %03x\n", addr, Mapper::ReadLow( addr ) );

//-----------[for Dragon Ball Z II - Gekishin Freeza!!_Plus (J)]-------------
//	if( (addr & 0x00FF) == 0x0000 ) {
//		BYTE	ret = 0;
//		ret = x24c02.Read();
//		return	(ret?0x10:0)|(nes->GetBarcodeStatus());
//	}
//	return	0x00;
//---------------------------------------------------------------------------

	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return	Mapper::ReadLow( addr );
}

void	Mapper179::WriteLow( WORD addr, BYTE data )
{
	DEBUGOUT( "WriteLow - addr= %04x ; dat= %03x\n", addr, data );
	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;

//-----------[for Dragon Ball Z II - Gekishin Freeza!!_Plus (J)]-------------
//	if(addr==0x6000) x24c02.Write( (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
//---------------------------------------------------------------------------

}

void	Mapper179::Write( WORD addr, BYTE data )
{
	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );

	if(addr==0xF000) SetVROM_4K_Bank( 0 , data );
	if(addr==0xF0FF) SetVROM_4K_Bank( 4 , data );

//---------------[for WaiXing F001_XXXXX]------------------------------------[OK]
//	if ((addr>=0x8081)&&(addr<=0x8101)) SetPROM_32K_Bank((addr-0x8081)/8);
//	if ((addr>=0x8179)&&(addr<=0x81f9)) SetPROM_32K_Bank((addr-0x8179)/8+15);
//---------------------------------------------------------------------------[OK]
/*
//-----------[for Dragon Ball Z II - Gekishin Freeza!!_Plus (J)]-------------
	switch( addr ) {
//		case	0x8000:
		case	0xA400:
			SetPROM_16K_Bank( 4, data );
			break;
//		case	0xA000:
//		case	0xA400:
//			SetPROM_8K_Bank( 5, data );
//			break;
//		case	0x9000:
		case	0x9400:
			data &= 0x03;
			if( data == 0 )			SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 )	SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 )	SetVRAM_Mirror( VRAM_MIRROR4L );
			else					SetVRAM_Mirror( VRAM_MIRROR4H );
			break;
		case 0xB000:
			reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
//		case 0xB001:
		case 0xB004:
			reg[0] = (reg[0] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
//		case 0xB002:
		case 0xB008:
			reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
//		case 0xB003:
		case 0xB00C:
			reg[1] = (reg[1] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
		case 0xC000:
			reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
//		case 0xC001:
		case 0xC004:
			reg[2] = (reg[2] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
//		case 0xC002:
		case 0xC008:
			reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
//		case 0xC003:
		case 0xC00C:
			reg[3] = (reg[3] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
		case 0xD000:
			reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
//		case 0xD001:
		case 0xD004:
			reg[4] = (reg[4] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
//		case 0xD002:
		case 0xD008:
			reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
//		case 0xD003:
		case 0xD00C:
			reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
		case 0xE000:
			reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
//		case 0xE001:
		case 0xE004:
			reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
//		case 0xE002:
		case 0xE008:
			reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, reg[7] );
			break;
//		case 0xE003:
		case 0xE00C:
			reg[7] = (reg[7] & 0x0F) | ((data & 0x0F)<< 4);
			SetVROM_1K_Bank( 7, reg[7] );
			break;
//		case 0xF000:
		case 0xF408:
	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
			irq_enable = data & 0x02;
			irq_counter = irq_latch;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
//		case 0xF400:
			//
//			break;
//		case 0xF800:
		case 0xF400:
	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
			irq_latch = (irq_latch & 0xFF00) | data;
			irq_counter = (irq_counter & 0xFF00) | data;
			break;
//		case 0xFC00:
		case 0xF404:
	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
			irq_latch = ((INT)data << 8) | (irq_latch & 0x00FF);
			irq_counter = ((INT)data << 8) | (irq_counter & 0x00FF);
			break;
		case 0xFFE0:
//			x24c02.Write( (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
			break;
		default:
			//DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
			break;
	}
//---------------------------------------------------------------------------
*/
/*	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
	if((addr>=0x8000)&&(addr<=0x8fff)){
		switch( addr&0xF00 ) {
			case	0x0000:
				SetVROM_2K_Bank( 0, addr&0x1F );
				break;
			case	0x0400:
				SetVROM_2K_Bank( 2, addr&0x1F );
				break;
			case	0x0800:
				SetVROM_2K_Bank( 4, addr&0x1F );
				break;
			case	0x0C00:
				SetVROM_2K_Bank( 6, addr&0x1F );
				break;
		}
	}
	if((addr>=0xa000)&&(addr<=0xafff)){
//		if(addr&0xf0==0xf0){
		switch( addr&0xF00 ) {
			case	0x0000:
//				SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
				SetPROM_8K_Bank( 4, (addr&0x0F) + 0x00 );
				break;
			case	0x0400:
//				SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
				SetPROM_8K_Bank( 5, (addr&0x0F) + 0x00 );
				break;
			case	0x0800:
//				SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
				SetPROM_8K_Bank( 6, (addr&0x0F) + 0x00 );
				break;
			case	0x0C00:
//				SetPROM_32K_Bank( PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1, PROM_8K_SIZE-1 );
				SetPROM_8K_Bank( 7, (addr&0x0F) + 0x10);
				break;
		}
//		}
		if(addr==0xa020) SetPROM_8K_Bank( 4, (addr&0x0F) + 0x00 );
	}
*/
}

void	Mapper179::HSync(int scanline)
{
/*
//-----------[for Dragon Ball Z II - Gekishin Freeza!!_Plus (J)]-------------
	if( irq_enable ) {
		if( irq_counter <= 114 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_counter &= 0xFFFF;
		} else {
			irq_counter -= 114;
		}
	}
//---------------------------------------------------------------------------
*/
}

void	Mapper179::Clock( INT cycles )
{
	//
}

void	Mapper179::PPU_Latch( WORD addr )
{
//	if(DirectInput.m_Sw[DIK_PAUSE]) nes->Dump_CPUHMEM();
}

void	Mapper179::PPU_ExtLatchX( INT x )
{
	//
}

void	Mapper179::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	//
}
