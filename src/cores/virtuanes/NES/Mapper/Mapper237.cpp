//////////////////////////////////////////////////////////////////////////
// Mapper237                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper237::Reset()
{
	A0=A1=A2=0;
	D0=D1=D3=D5=D6=0;
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 7 );

	dip_s=dip_s&3;
	dip_s++;
	if(dip_s==4) dip_s=0;

}

BYTE	Mapper237::Read( WORD addr )
{
	if( addr == 0xC000 ) {
		CPU_MEM_BANK[addr>>13][addr&0x1FFF] = dip_s+1;
	}
	return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
}

void	Mapper237::Write( WORD addr, BYTE data )
{
	if(A1&0x01){
		SetPROM_16K_Bank( 4, data|(D3<<3)|(A2<<5) );
		SetPROM_16K_Bank( 6, 0x07|(D3<<3)|(A2<<5) );
		return;
	}

	A0 = ((addr&0x07)>>0) & 0x01;
	A1 = ((addr&0x07)>>1) & 0x01;
	A2 = ((addr&0x07)>>2) & 0x01;
	D0 = (data>>0) & 0x07;
	D1 = (data>>1) & 0x03;
	D3 = (data>>3) & 0x03;
	D5 = (data>>5) & 0x01;
	D6 = (data>>6) & 0x03;

	if(D5&0x01)	SetVRAM_Mirror( VRAM_HMIRROR );
	else		SetVRAM_Mirror( VRAM_VMIRROR );

	switch( D6 ) {
		case	0:	//UNROM mode
			SetPROM_16K_Bank( 4, D0|(D3<<3)|(A2<<5) );
			SetPROM_16K_Bank( 6, 0x07|(D3<<3)|(A2<<5) );
			break;
		case	1:	//CNROM mode ??
			SetPROM_16K_Bank( 4, (D0&0x06)|(D3<<3)|(A2<<5) );
			SetPROM_16K_Bank( 6, 0x07|(D3<<3)|(A2<<5) );
			break;
		case	2:	//16K NROM mode
			SetPROM_16K_Bank( 4, D0|(D3<<3)|(A2<<5) );
			SetPROM_16K_Bank( 6, D0|(D3<<3)|(A2<<5) );
			break;
		case	3:	//32K NROM mode
			SetPROM_32K_Bank( D1|(D3<<2)|(A2<<4) );
			break;
	}
}
