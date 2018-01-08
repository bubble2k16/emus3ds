//////////////////////////////////////////////////////////////////////////
// Mapper166                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper166::Reset()
{
	reg[0] = reg[1] = 0;
	SetPROM_32K_Bank( 0 );
//	SetCRAM_4K_Bank( 0, 0x00 );
//	SetCRAM_4K_Bank( 4, 0x00 );
	SetCRAM_8K_Bank( 0 );
	SetVRAM_Mirror( VRAM_MIRROR4H );
}

void	Mapper166::Write( WORD addr, BYTE data )
{
	DEBUGOUT( "Write - addr= %04x ; dat= %03x\n", addr, data );
	if(addr==0xFFFF){
//		reg[0] = (data & 0x04) >> 2;
		reg[0] = data;
//		SetCRAM_4K_Bank( 0, reg[0]*4+reg[1] );
//		SetCRAM_4K_Bank( 4, reg[0]*4+0x03 );

		switch ( data ) {
			case 0:
//				SetCRAM_4K_Bank( 4, 0 );
				break;
			case 1:
//				SetCRAM_4K_Bank( 4, 1 );
				break;
			case 2:
//				SetCRAM_4K_Bank( 4, 2 );
				break;
			case 3:
//				SetCRAM_4K_Bank( 4, 1 );
				break;
		}
	}
}

void	Mapper166::PPU_Latch( WORD addr )
{
//	if((addr&0xF000)==0x2000){
//		NT_data=(addr>>8)&0x03;
//		SetCRAM_4K_Bank( 0, 0 );
//		SetCRAM_4K_Bank( 4, reg[0] );
//	}

	if((addr&0xF000)==0x2000){
		reg[1]=(addr>>8)&0x03;
		SetCRAM_4K_Bank( 0, reg[0]*4+reg[1] );
		SetCRAM_4K_Bank( 4, reg[0] );
	}
/*
	if(DirectInput.m_Sw[DIK_PAUSE]) nes->Dump_CRAM();
*/
}