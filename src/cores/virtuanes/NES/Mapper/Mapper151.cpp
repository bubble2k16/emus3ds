//////////////////////////////////////////////////////////////////////////
// Mapper151 VS-Unisystem                                               //
//////////////////////////////////////////////////////////////////////////
void	Mapper151::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

#if	0
	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x1E438D52 ) {
		DirectDraw.SetVsPalette( 7 );	//VS_Goonies
	}
	if( crc == 0xD99A2087 ) {
		DirectDraw.SetVsPalette( 6 );	//VS_Gradius
	}
#endif
}

void	Mapper151::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case 0x8000:
			SetPROM_8K_Bank( 4,data );
			break;
		case 0xA000:
			SetPROM_8K_Bank( 5,data );
			break;
		case 0xC000:
			SetPROM_8K_Bank( 6,data );
			break;
		case 0xE000:
			SetVROM_4K_Bank( 0, data );
			break;
		case	0xF000:
			SetVROM_4K_Bank( 4, data );
			break;
	}
}












void	Mapper156::Reset()
{
	mirrisused = 0;
	Sync();
}

void	Mapper156::Sync()
{
 // for(int i=0; i<8; i++)
  //{
    //SetChr1r(0,i<<10, chrlo[i]|(chrhi[i] << 8));
 // }
  //setprg8r(0x10,0x6000,0);
  setprg16(0x8000,prg);
  setprg16(0xC000,~0);
  if(mirrisused)
    SetVRAM_Mirror(mirr ^ 1);
  else
    SetVRAM_Mirror(VRAM_MIRROR4L);
}

BYTE	Mapper156::ReadLow ( WORD A )
{
	return MRAM[A];
}
void	Mapper156::WriteLow( WORD A, BYTE V )
{
	MRAM[A]=V;
}
void	Mapper156::Write( WORD A, BYTE V )
{	
  switch(A) {
   case 0xC000:
   case 0xC001:
   case 0xC002:
   case 0xC003: chrlo[A&3] = V;
				//SetChr1r(0,(A&3)*0x1000,V);
				SetVROM_1K_Bank(A&3, chrlo[A&3]|(chrhi[A&3] << 8));
				Sync(); 
				break;
   case 0xC004:
   case 0xC005:
   case 0xC006:
   case 0xC007: chrhi[A&3] = V; 
	   			//SetChr1r(0,0x1000+(A&3)*0x1000,V);
				SetVROM_1K_Bank(A&3, chrlo[A&3]|(chrhi[A&3] << 8));
				Sync(); 
				break;
   case 0xC008:
   case 0xC009:
   case 0xC00A:
   case 0xC00B: chrlo[4+(A&3)] = V; 
				SetVROM_1K_Bank(4 + (A&3), chrlo[4 + (A&3)]|(chrhi[4 + (A&3)] << 8));
   				Sync(); break;
   case 0xC00C:
   case 0xC00D:
   case 0xC00E:
   case 0xC00F: chrhi[4+(A&3)] = V; 
				SetVROM_1K_Bank(4 + (A&3), chrlo[4 + (A&3)]|(chrhi[4 + (A&3)] << 8));
   				Sync(); 
				break;
   case 0xC010: prg = V; Sync(); break;
   case 0xC014: mirr = V; mirrisused = 1; Sync(); break;
  }
}
