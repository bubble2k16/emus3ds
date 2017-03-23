//////////////////////////////////////////////////////////////////////////
// Mapper096  Bandai 74161                                              //
//////////////////////////////////////////////////////////////////////////
void	Mapper096::Reset()
{
	reg[0] = reg[1] = 0;

	SetPROM_32K_Bank( 0, 1, 2, 3 );
	SetBank();

	SetVRAM_Mirror( VRAM_MIRROR4L );
}

void	Mapper096::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data & 0x03 );

	reg[0] = (data & 0x04) >> 2;
	SetBank();
}

void	Mapper096::PPU_Latch( WORD addr )
{
	if( (addr & 0xF000) == 0x2000 ) {
		reg[1] = (addr>>8)&0x03;
		SetBank();
	}
}

void	Mapper096::SetBank()
{
	SetCRAM_4K_Bank( 0, reg[0]*4+reg[1] );
	SetCRAM_4K_Bank( 4, reg[0]*4+0x03 );
}

void	Mapper096::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
}

void	Mapper096::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
}
