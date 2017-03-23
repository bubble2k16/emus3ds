//////////////////////////////////////////////////////////////////////////
// Mapper234  Maxi-15                                                   //
//////////////////////////////////////////////////////////////////////////
void	Mapper234::Reset()
{
	SetPROM_32K_Bank( 0, 1, 2, 3 );

	reg[0] = 0;
	reg[1] = 0;
}

void	Mapper234::Read( WORD addr, BYTE data)
{
	if( addr >= 0xFF80 && addr <= 0xFF9F ) {
		if( !reg[0] ) {
			reg[0] = data;
			SetBank();
		}
	}

	if( addr >= 0xFFE8 && addr <= 0xFFF7 ) {
		reg[1] = data;
		SetBank();
	}
}

void	Mapper234::Write( WORD addr, BYTE data )
{
	if( addr >= 0xFF80 && addr <= 0xFF9F ) {
		if( !reg[0] ) {
			reg[0] = data;
			SetBank();
		}
	}

	if( addr >= 0xFFE8 && addr <= 0xFFF7 ) {
		reg[1] = data;
		SetBank();
	}
}

void	Mapper234::SetBank()
{
	if( reg[0] & 0x80 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}
	if( reg[0] & 0x40 ) {
		SetPROM_32K_Bank( (reg[0]&0x0E)|(reg[1]&0x01) );
		SetVROM_8K_Bank( ((reg[0]&0x0E)<<2)|((reg[1]>>4)&0x07) );
	} else {
		SetPROM_32K_Bank( reg[0]&0x0F );
		SetVROM_8K_Bank( ((reg[0]&0x0F)<<2)|((reg[1]>>4)&0x03) );
	}
}

void	Mapper234::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
}

void	Mapper234::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
}
