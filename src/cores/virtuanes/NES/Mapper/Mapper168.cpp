//////////////////////////////////////////////////////////////////////////
// Mapper168          Subor (PPUExtLatch)                               //
//////////////////////////////////////////////////////////////////////////

void	Mapper168::Reset()
{
	reg5000 = 0;
	reg5200 = 0;
	reg5300 = 0;
	PPU_SW = 0;
	NT_data = 0;
	nes->ppu->SetExtLatchMode( TRUE );
	SetPROM_16K_Bank(4,0);
	SetPROM_16K_Bank(6,0);

	Rom_Type = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x0A9808AE )		//[Subor] Karaoke (C)
	{
		Rom_Type = 1;
		SetPROM_32K_Bank( 0 );
		nes->SetVideoMode( 2 );
	}	
	if( crc == 0x12D61CE8 )		//[Subor] Subor V11.0 (C)
	{
		Rom_Type = 2;
	}	
}

BYTE Mapper168::ReadLow( WORD addr )
{
	if(addr==0x5300) return 0x8F;	//返回0x8F，跳过真人语音发声有关的程序段
	return	Mapper::ReadLow( addr );
}

void Mapper168::WriteLow(WORD addr, BYTE data)
{
	if(addr==0x5000){
		reg5000 = data;
		SetBank_CPU();
	}else if(addr==0x5200){
		reg5200 = data & 0x7;
		SetBank_CPU();
	}else if(addr==0x5300){
		reg5300 = data;
	}else if(addr>=0x6000){
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

void Mapper168::Write(WORD addr, BYTE data)
{
	if(Rom_Type==1){	//[Subor] Karaoke (C)
		SetPROM_32K_Bank( data&0x1F );
		if(data&0x40)	SetVRAM_Mirror( VRAM_HMIRROR );
		else			SetVRAM_Mirror( VRAM_VMIRROR );
		if(data&0xC0)	PPU_SW = 1;
		else			PPU_SW = 0;
	}
}

void	Mapper168::SetBank_CPU()
{
	if(reg5200<4)	SetPROM_16K_Bank(4,reg5000);
	else			SetPROM_32K_Bank(reg5000);
	switch( reg5200 ) {
		case	0:
			SetVRAM_Mirror( VRAM_VMIRROR );
			PPU_SW = 0;
			break;
		case	2:
			SetVRAM_Mirror( VRAM_VMIRROR );
			PPU_SW = 1;
			break;
		case	1:
		case	3:
			SetVRAM_Mirror( VRAM_HMIRROR );
			PPU_SW = 0;
			break;
		case	5:
			if(reg5000==4 && Rom_Type==2){	//Special for [Subor] Subor V11.0 (C) - Tank (坦克大战)
				nes->ppu->SetExtLatchMode( FALSE );
				SetVRAM_Mirror( VRAM_HMIRROR );
			}
			break;
	}
}

void	Mapper168::PPU_Latch( WORD addr )
{
	if((addr&0xF000)==0x2000){
		NT_data=(addr>>8)&0x03;
	}
}

void	Mapper168::PPU_ExtLatch( WORD ntbladr, BYTE& chr_l, BYTE& chr_h, BYTE& attr )
{
	INT loopy_v = nes->ppu->GetPPUADDR();
	INT loopy_y = nes->ppu->GetTILEY();
	INT tileofs = (PPUREG[0]&PPU_BGTBL_BIT)<<8;
	INT attradr = 0x23C0+(loopy_v&0x0C00)+((loopy_v&0x0380)>>4);
	INT attrsft = (ntbladr&0x0040)>>4;
	LPBYTE pNTBL = PPU_MEM_BANK[ntbladr>>10];
	INT ntbl_x  = ntbladr&0x001F;
	INT tileadr, ntb;

	ntb = (ntbladr>>10)&3;

	if(ntb==2)
	tileofs |= 0x1000;
	else if(ntb && PPU_SW)
	tileofs |= 0x1000;
	else
	tileofs |= 0x0000;

	attradr &= 0x3FF;
	attr = ((pNTBL[attradr+(ntbl_x>>2)]>>((ntbl_x&2)+attrsft))&3)<<2;
	tileadr = tileofs+pNTBL[ntbladr&0x03FF]*0x10+loopy_y;

	chr_l = PPU_MEM_BANK[tileadr>>10][ tileadr&0x03FF   ];
	chr_h = PPU_MEM_BANK[tileadr>>10][(tileadr&0x03FF)+8];
}

void	Mapper168::SaveState( LPBYTE p )
{
	p[0] = reg5000;
	p[1] = reg5200;
}

void	Mapper168::LoadState( LPBYTE p )
{

	reg5000 = p[0];
	reg5200 = p[1];
}
