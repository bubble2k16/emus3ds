//////////////////////////////////////////////////////////////////////////
// Mapper222                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper220::Reset()
{
	//MRAM
	SetPROM_Bank(3,&MRAM[0x0000],BANKTYPE_RAM);
	SetPROM_Bank(4,&MRAM[0x2000],BANKTYPE_RAM);
	for(int i=0; i<8; i++)
		reg[i]=0;
	mirror = 0;
	Sync();
}

void	Mapper220::Sync()
{
  //setprg2(0x6000,reg[4]);
  //setprg2(0x6800,reg[5]);
  //setprg2(0x7000,reg[6]);
  //setprg2(0x7800,reg[7]);
	for(int i=0; i<4; i++)
	{
		memcpy(&MRAM[0x0000+i*0x800],&PROM[ (reg[i+4]&0x3f)*0x800],0x800);
	}

  //setprg2(0x8000,reg[0]);
  //setprg2(0x8800,reg[1]);
  //setprg2(0x9000,reg[2]);
  //setprg2(0x9800,reg[3]);
	for(int i=0; i<4; i++)
	{
		memcpy(&MRAM[0x2000+i*0x800],&PROM[ (reg[i]&0x3f)*0x800],0x800);
	}

  //setprg8(0xA000,0xd);
  setprg8(0xA000,0xD);
  //setprg16(0xC000,7);
  setprg16(0xC000,7);
  //setchr8(0);
  SetVRAM_Mirror(mirror);
}

void	Mapper220::Write( WORD A, BYTE V )
{
	
  switch(A&0xF003) {
   case 0x8000:
   case 0x8001:
   case 0x8002:
   case 0x8003:
   case 0x9000: 
   case 0x9001: 
   case 0x9002: 
   case 0x9003: mirror = V & 1; Sync(); break;
   case 0xB000: reg[0] = (reg[0] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xB001: reg[0] = (reg[0] & 0x0F) | (V << 4); Sync(); break;
   case 0xB002: reg[1] = (reg[1] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xB003: reg[1] = (reg[1] & 0x0F) | (V << 4); Sync(); break;
   case 0xC000: reg[2] = (reg[2] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xC001: reg[2] = (reg[2] & 0x0F) | (V << 4); Sync(); break;
   case 0xC002: reg[3] = (reg[3] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xC003: reg[3] = (reg[3] & 0x0F) | (V << 4); Sync(); break;
   case 0xD000: reg[4] = (reg[4] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xD001: reg[4] = (reg[4] & 0x0F) | (V << 4); Sync(); break;
   case 0xD002: reg[5] = (reg[5] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xD003: reg[5] = (reg[5] & 0x0F) | (V << 4); Sync(); break;
   case 0xE000: reg[6] = (reg[6] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xE001: reg[6] = (reg[6] & 0x0F) | (V << 4); Sync(); break;
   case 0xE002: reg[7] = (reg[7] & 0xF0) | (V & 0x0F); Sync(); break;
   case 0xE003: reg[7] = (reg[7] & 0x0F) | (V << 4); Sync(); break;
  }
}

//////////////////////////////////////////////////////////////////////////
// Mapper222                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper222::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
	SetVRAM_Mirror( VRAM_VMIRROR );
}

void	Mapper222::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF003 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xB000:
			SetVROM_1K_Bank( 0, data );
			break;
		case	0xB002:
			SetVROM_1K_Bank( 1, data );
			break;
		case	0xC000:
			SetVROM_1K_Bank( 2, data );
			break;
		case	0xC002:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0xD000:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0xD002:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0xE000:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0xE002:
			SetVROM_1K_Bank( 7, data );
			break;
	}
}

