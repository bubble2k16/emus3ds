//////////////////////////////////////////////////////////////////////////
// Mapper253                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper253::Reset()
{
	for(int i=0; i<8; i++)
	{
		chrlo[i]=chrhi[i]=0;
	}
	prg[0]=prg[1]=0;
	mirr = vlock = 0;
	IRQa = IRQCount = IRQLatch = IRQClock = 0; 

	Sync();
}

void	Mapper253::Write( WORD A, BYTE V )
{
  if((A>=0xB000)&&(A<=0xE00C))
  {
    uint8 ind=((((A&8)|(A>>8))>>3)+2)&7;
    uint8 sar=A&4;
    chrlo[ind]=(chrlo[ind]&(0xF0>>sar))|((V&0x0F)<<sar);
    if(A&4)
      chrhi[ind]=V>>4;
    Sync();
  }
  else
   switch(A)
    {
      case 0x8010: prg[0]=V; Sync(); break;
      case 0xA010: prg[1]=V; Sync(); break;
      case 0x9400: mirr=V&3; Sync(); break;
      case 0xF000: IRQLatch = (IRQLatch & 0xF0) | (V & 0x0F); break;
      case 0xF004: IRQLatch = (IRQLatch & 0x0F) | (V << 4); break;
      case 0xF008:
        IRQa = V&3;
        if(IRQa&2)
        {
          IRQCount = IRQLatch;
          IRQClock = 0;
        }
		nes->cpu->ClrIRQ( IRQ_MAPPER );
        break;
    }



}

void	Mapper253::Sync(void)
{
  uint8 i;
  //setprg8r(0x10,0x6000,0);
  setprg8(0x8000,prg[0]);
  setprg8(0xa000,prg[1]);
  setprg8(0xc000,PROM_8K_SIZE-2);
  setprg8(0xe000,PROM_8K_SIZE-1);
  for(i=0; i<8; i++)
  {
    uint32 chr = chrlo[i]|(chrhi[i]<<8);
    if(chrlo[i]==0xc8)
    {
      vlock = 0;
      continue;
    }
    else if(chrlo[i]==0x88)
    {
      vlock = 1;
      continue;
    }
    if(((chrlo[i]==4)||(chrlo[i]==5))&&!vlock)
      SetCRAM_1K_Bank(i,chr&1);
    else
      SetVROM_1K_Bank(i,chr);
  }
  switch(mirr)
  {
    case 0: SetVRAM_Mirror(VRAM_VMIRROR); break;
    case 1: SetVRAM_Mirror(VRAM_HMIRROR); break;
    case 2: SetVRAM_Mirror(VRAM_MIRROR4L); break;
    case 3: SetVRAM_Mirror(VRAM_MIRROR4H); break;
  }
}


void	Mapper253::Clock( INT cycles )
{
  if(IRQa&2) 
  {
    if((IRQClock+=cycles)>=0x72) 
    {
      IRQClock -= 0x72;
      if(IRQCount==0xFF)  
      {
        IRQCount = IRQLatch;
        IRQa = IRQa|((IRQa&1)<<1);
		nes->cpu->SetIRQ( IRQ_MAPPER );
      }
      else 
        IRQCount++;
    }
  }
}

void	Mapper253::SaveState( LPBYTE p )
{
}

void	Mapper253::LoadState( LPBYTE p )
{
}
