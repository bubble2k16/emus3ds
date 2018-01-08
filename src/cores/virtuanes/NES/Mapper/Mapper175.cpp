//////////////////////////////////////////////////////////////////////////
// Mapper175  15-in-1 (Kaiser)                                          //
//////////////////////////////////////////////////////////////////////////
void	Mapper175::Reset()
{
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 0 );
	reg_dat = 0;

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper175::Read( WORD addr, BYTE data)
{
	if( addr == 0xFFFC ) {
		SetPROM_16K_Bank( 4, reg_dat & 0x0F );
		SetPROM_8K_Bank( 6, (reg_dat & 0x0F)*2 );
	}
}

void	Mapper175::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			if( data & 0x04 ) {
				SetVRAM_Mirror( VRAM_HMIRROR );
			} else {
				SetVRAM_Mirror( VRAM_VMIRROR );
			}
			break;
		case	0xA000:
			reg_dat = data;
			SetPROM_8K_Bank( 7, (reg_dat & 0x0F)*2+1 );
			SetVROM_8K_Bank( reg_dat & 0x0F );
			break;
	}
}


//////////////////////////////////////////////////////////////////////////
// Mapper176                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper176::Reset()
{
	prg=~0;
	chr=0;
	Sync();
}

void Mapper176::Sync()
{
  //setprg8r(0x10,0x6000,0);
  SetPROM_32K_Bank(prg>>1);
  SetVROM_8K_Bank(chr);
}

void	Mapper176::WriteLow( WORD addr, BYTE data )
{
	switch( addr ) {
		case 0x5ff1:
			prg = data;	Sync();
			 break;
		case 0x5ff2:
			chr = data;	Sync();
			 break;
		default:
			 break;
			 }
	if( addr>=0x6000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
	}
}

//////////////////////////////////////////////////////////////////////////
// Mapper177  HengGe                                                    //
//////////////////////////////////////////////////////////////////////////
void    Mapper177::Reset()
{
	reg=0;
	Sync(); 
}

void Mapper177::Sync()
{
  SetVROM_8K_Bank(0);
  //setprg8r(0x10,0x6000,0);
  SetPROM_32K_Bank(reg&0x1f);
  SetVRAM_Mirror(((reg&0x20)>>5)^1);
}

void	Mapper177::Write( WORD addr, BYTE data )
{
  reg=data;
  Sync();
}


//////////////////////////////////////////////////////////////////////////
// Mapper178  Education / WaiXing / HengGe                              //
//////////////////////////////////////////////////////////////////////////
void    Mapper178::Reset()
{
	reg[0]=0;
	reg[1]=0;
	reg[2]=0;
	banknum=0;
	SetBank_CPU();
}

void	Mapper178::WriteLow( WORD addr, BYTE data )
{
		if( addr==0x4800 ){
			if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else			  SetVRAM_Mirror( VRAM_VMIRROR );
		}else if( addr==0x4801 ){
			reg[0]=(data>>1) & 0x0f;
			SetBank_CPU();
		}else if( addr==0x4802 ){
			reg[1]=(data<<2) & 0x0f;
//			SetBank_CPU();
		}else if( addr==0x4803 ){
			//unknown
		}else if( addr>=0x6000 ) {
			CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
		}
}

void	Mapper178::Write( WORD addr, BYTE data )
{
//		SetPROM_32K_Bank( data );
}

void	Mapper178::SetBank_CPU()
{
		banknum=(reg[0]+reg[1]) & 0x0f;
		SetPROM_32K_Bank( banknum );
}