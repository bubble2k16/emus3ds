//////////////////////////////////////////////////////////////////////////
// BoardMGC002                                                          //
//////////////////////////////////////////////////////////////////////////

void	BoardMGC002::Reset()
{
	SetPROM_32K_Bank( 0, 1, 6, 7 );
	from_mode = 0;
	game_set = 0;
	reg[0] = 0x0C;
	reg[1] = reg[2] = reg[3] = 0;
	shift = regbuf = 0;
}

void	BoardMGC002::WriteLow( WORD addr, BYTE data )
{
	if(addr==0x4120){
		if((data==0x21)&&(from_mode)){
			game_set = 1;	//Operation Wolf
			SetPROM_32K_Bank( 0x1c, 0x1d, 0x1e, 0x1f );
		}
		if((data==0x62)&&(from_mode)){
			game_set = 2;	//Space Shadow
			SetPROM_32K_Bank( 0x0c, 0x0d, 0x0e, 0x0f );
			SetVROM_8K_Bank( 0 );
		}
	}
	if(addr>=0x6000) CPU_MEM_BANK[addr>>13][addr&0x1FFF]=data;
}

void	BoardMGC002::Write( WORD addr, BYTE data )
{
	if((addr==0x8387)&&(data==0x20)){
		from_mode = 1;
		SetPROM_32K_Bank( 4, 5, 6, 7 );
	}
	if(game_set==1){	//Operation Wolf	[MMC1]
		if((addr&0x6000)!=(last_addr&0x6000)) shift=regbuf=0;
		last_addr=addr;
		if(data&0x80){
			shift=regbuf=0;
			reg[0]|=0x0C;
			return;
		}
		if(data&0x01) regbuf|=1<<shift;
		if(++shift<5) return;
		addr=(addr&0x7FFF)>>13;
		reg[addr]=regbuf;
		shift=regbuf=0;
		switch( addr ) {
			case	0:
				if(reg[0]&0x02){
					if(reg[0]&0x01)	SetVRAM_Mirror( VRAM_HMIRROR );
					else			SetVRAM_Mirror( VRAM_VMIRROR );
				}else{
					if(reg[0]&0x01)	SetVRAM_Mirror( VRAM_MIRROR4H );
					else			SetVRAM_Mirror( VRAM_MIRROR4L );
				}
				break;
			case	1:
			case	2:
				if(reg[0]&0x10){
					SetVROM_4K_Bank(0,reg[1]+0x20);
					SetVROM_4K_Bank(4,reg[2]+0x20);
				} else {
					SetVROM_8K_Bank((reg[1]>>1)+0x10);
				}
				break;
			case	3:
				if(!(reg[0]&0x08)){
					SetPROM_32K_Bank((reg[3]>>1)+0x4);
				} else {
					if(reg[0]&0x04){
						SetPROM_16K_Bank(4,reg[3]+0x8);
						SetPROM_16K_Bank(6,0x1f);
					} else {
						SetPROM_16K_Bank(6,reg[3]+0x8);
						SetPROM_16K_Bank(4,+0x8);
					}
				}
				break;
		}
	}
	if(game_set==2){	//Space Shadow	[mapper070]
		SetPROM_16K_Bank(4,(data&0x70)>>4);
		SetVROM_8K_Bank(data&0x0F);
		if(data&0x80)	SetVRAM_Mirror(VRAM_HMIRROR);
		else			SetVRAM_Mirror(VRAM_VMIRROR);
	}
}

void	BoardMGC002::SaveState( LPBYTE p )
{
	//
}

void	BoardMGC002::LoadState( LPBYTE p )
{
	//
}
