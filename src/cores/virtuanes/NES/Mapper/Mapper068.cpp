//////////////////////////////////////////////////////////////////////////
// Mapper068  SunSoft (After Burner II)                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper068::Reset()
{
	reg[0] = reg[1] = reg[2] = reg[3] = 0;
	coin = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

#if	0
BYTE	Mapper068::ExRead( WORD addr )
{
	if( addr == 0x4020 ) {
DEBUGOUT( "RD $4020:%02X\n", coin );
		return	coin;
	}

	return	addr>>8;
}

void	Mapper068::ExWrite( WORD addr, BYTE data )
{
	if( addr == 0x4020 ) {
DEBUGOUT( "WR $4020:%02X\n", data );
		coin = data;
	}
}
#endif

void	Mapper068::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			SetVROM_2K_Bank( 0, data );
			break;
		case	0x9000:
			SetVROM_2K_Bank( 2, data );
			break;
		case	0xA000:
			SetVROM_2K_Bank( 4, data );
			break;
		case	0xB000:
			SetVROM_2K_Bank( 6, data );
			break;

		case	0xC000:
			reg[2] = data;
			SetBank();
			break;
		case	0xD000:
			reg[3] = data;
			SetBank();
			break;
		case	0xE000:
			reg[0] = (data & 0x10)>>4;
			reg[1] = data & 0x03;
			SetBank();
			break;

		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}

void	Mapper068::SetBank()
{
	if( reg[0] ) {
		switch( reg[1] ) {
			case	0:
				SetVROM_1K_Bank(  8, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)reg[3]+0x80 );
				SetVROM_1K_Bank( 10, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank( 11, (INT)reg[3]+0x80 );
				break;
			case	1:
				SetVROM_1K_Bank(  8, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank( 10, (INT)reg[3]+0x80 );
				SetVROM_1K_Bank( 11, (INT)reg[3]+0x80 );
				break;
			case	2:
				SetVROM_1K_Bank(  8, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank(  9, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank( 10, (INT)reg[2]+0x80 );
				SetVROM_1K_Bank( 11, (INT)reg[2]+0x80 );
				break;
			case	3:
				SetVROM_1K_Bank(  8, (INT)reg[3]+0x80 );
				SetVROM_1K_Bank(  9, (INT)reg[3]+0x80 );
				SetVROM_1K_Bank( 10, (INT)reg[3]+0x80 );
				SetVROM_1K_Bank( 11, (INT)reg[3]+0x80 );
				break;
		}
	} else {
		switch( reg[1] ) {
			case	0:
				SetVRAM_Mirror( VRAM_VMIRROR );
				break;
			case	1:
				SetVRAM_Mirror( VRAM_HMIRROR );
				break;
			case	2:
				SetVRAM_Mirror( VRAM_MIRROR4L );
				break;
			case	3:
				SetVRAM_Mirror( VRAM_MIRROR4H );
				break;
		}
	}
}

void	Mapper068::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = reg[2];
	p[3] = reg[3];
}

void	Mapper068::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	reg[2] = p[2];
	reg[3] = p[3];
}
