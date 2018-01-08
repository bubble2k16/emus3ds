//////////////////////////////////////////////////////////////////////////
// Mapper170         PYRAMID                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper170::Reset()
{
	for(INT i=0;i<8;i++) reg[i]=0;
	for(INT i=0; i<32; i+=2)
	{
		memcpy( &WRAM00[0x400*i+0x000], PROM+0x400*((PROM_8K_SIZE*0x4+0x07)+i*8),0x400);
		memcpy( &WRAM00[0x400*i+0x400], PROM+0x400*((PROM_8K_SIZE*0x4+0x0F)+i*8),0x400);
	}
	SetPROM_Bank0();
	p_mode = 0;
	NT_data = 0;
	ex_slot2 = 0;
	ex_slot3 = 0;
	nes->ppu->SetExtLatchMode( TRUE );

	dip_s++;
	if(dip_s==4) dip_s=0;

	Rom_Type = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0xFE31765B ){	//[PYRAMID] PEC-9588 Computer & Game (C)
		Rom_Type = 1;
		SetPROM_32K_Bank( 0, 1, 0x3E, 0x3F );
	}
	if( crc == 0x428C1C1D		//[FengLi] Zhong Ying Wen Dian Nao Ka (C)
	 || crc == 0x836CDDEF ){	//[BaTong] Zhong Ying Wen Dian Nao Ka (C)
		Rom_Type = 2;
		SetPROM_32K_Bank( 0 );
	}
}

BYTE	Mapper170::ReadLow( WORD addr )
{
//	DEBUGOUT( "ReadLow  - addr= %04x ; dat= %03x\n", addr, Mapper::ReadLow( addr ) );
	if(addr==0x5500) return ((dip_s&3)*0x10);	//[0x00]=XinKe; [0x10]=KingWang; [0x20]=?ABC; [0x30]=KingWangEX

	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return	Mapper::ReadLow( addr );
}

void	Mapper170::WriteLow( WORD addr, BYTE data )
{
//	DEBUGOUT( "WriteLow - addr= %04x ; dat= %03x\n", addr, data );
	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;

	if(addr<0x6000){
		reg[(addr&0x700)>>8] = data;
		p_mode = reg[0]&0x80;
		if((reg[0]&0x18)==0x18)	SetVRAM_Mirror(VRAM_HMIRROR);
		else					SetVRAM_Mirror(VRAM_VMIRROR);

		if( reg[0]&0x10 ){		// [PYRAMID] PEC-586 Computer & Game (C)
			SetPROM_32K_Bank(reg[0]&0x07);
		}else if((reg[0]&0x70)==0x40){
			SetPROM_Bank0();
			SetPROM_8K_Bank(4,(reg[0]&0x0F)|0x20);
		}else if((reg[0]&0x70)==0x60){
			SetPROM_Bank0();
			SetPROM_8K_Bank(4,(reg[0]&0x0F)|0x30);
		}else{
			SetPROM_Bank0();
		}

		if(Rom_Type==1){		// [PYRAMID] PEC-9588 Computer & Game (C)
//			DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			switch( reg[0]&0x70 ) {
				case 0x00:
					SetPROM_16K_Bank( 4, reg[0]&0x0F );
					SetPROM_16K_Bank( 6, 0x1F );
					break;
				case 0x10:
					//
					break;
				case 0x20:
					SetPROM_16K_Bank( 4, (reg[0]&0x0F) | 0x10 );
					SetPROM_16K_Bank( 6, 0x1F );
					break;
				case 0x30:	//С����������2
					ex_slot2 = 1;
//					SetPROM_16K_Bank( 4, (reg[0]&0x0F) | 0x20 );
//					SetPROM_16K_Bank( 6, 0x2F );
//					SetPROM_32K_Bank((reg[0]&0x07)|0x20);
					break;
				case 0x40:
					SetPROM_16K_Bank( 4, reg[0]&0x0F );
					SetPROM_16K_Bank( 6, 0x1E );
					break;
				case 0x50:	//С����������3
					ex_slot3 = 1;
					break;
				case 0x60:
					SetPROM_16K_Bank( 4, (reg[0]&0x0F) | 0x10 );
					SetPROM_16K_Bank( 6, 0x1E );
					if(reg[0]>=0x6C) SetPROM_16K_Bank( 6, 0x1C );
					break;
			}
		}
		if(Rom_Type==2){
//			DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
			SetPROM_32K_Bank(reg[0]&0x0F);
		}

	}
}

void	Mapper170::PPU_Latch( WORD addr )
{
	if((addr&0xF000)==0x2000){
		NT_data=(addr>>8)&0x03;
	}
}
void	Mapper170::PPU_ExtLatchX( INT x )
{
	a3 = (x&1)<<3;
}
void	Mapper170::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	INT loopy_v = nes->ppu->GetPPUADDR();
	INT loopy_y = nes->ppu->GetTILEY();
	INT	tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT	attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT	attrsft = (ntbladr&0x0040)>>4;
	LPBYTE	pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT	ntbl_x  = ntbladr&0x001F;
	INT	tileadr;

	attradr &= 0x3FF;
	attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

	if(p_mode){
		if(NT_data&0x02)	tileadr |=  0x1000;
		else				tileadr &= ~0x1000;
		tileadr = (tileadr&0xfff7)|a3;
		chr_l = chr_h = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	}else{
		chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
		chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
	}
}
BYTE	Mapper170::PPU_ExtLatchSP()	//��������������ɫ���⣬δ��ȫ
{
	if(p_mode)
		return 2;	//��ʱ���˴���������ԭ��δ֪��
	return 0;
}
void	Mapper170::SetPROM_Bank0(void) 	
{
	SetPROM_Bank(4, &WRAM00[0x2000*0], 0);
	SetPROM_Bank(5, &WRAM00[0x2000*1], 0);
	SetPROM_Bank(6, &WRAM00[0x2000*2], 0);
	SetPROM_Bank(7, &WRAM00[0x2000*3], 0);
}