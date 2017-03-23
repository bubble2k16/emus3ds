//////////////////////////////////////////////////////////////////////////
// Mapper041  Caltron 6-in-1                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper041::Reset()
{
	reg[0] = reg[1] = 0;

	SetPROM_32K_Bank( 0, 1, 2, 3 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper041::WriteLow( WORD addr, BYTE data )
{
	if( addr >= 0x6000 && addr < 0x6800 ) {
		SetPROM_32K_Bank( addr&0x07 );
		reg[0] = addr & 0x04;
		reg[1] &= 0x03;
		reg[1] |= (addr>>1)&0x0C;
		SetVROM_8K_Bank( reg[1] );
		if( addr&0x20 ) SetVRAM_Mirror( VRAM_HMIRROR );
		else		SetVRAM_Mirror( VRAM_VMIRROR );
	}
}

void	Mapper041::Write( WORD addr, BYTE data )
{
	if( reg[0] ) {
		reg[1] &= 0x0C;
		reg[1] |= addr & 0x03;
		SetVROM_8K_Bank( reg[1] );
	}
}

void	Mapper041::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
}

void	Mapper041::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
}
