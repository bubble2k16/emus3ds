//////////////////////////////////////////////////////////////////////////
// Mapper255  110-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper255::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
	SetVRAM_Mirror( VRAM_VMIRROR );

	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 0;
	reg[3] = 0;
}

BYTE	Mapper255::ReadLow( WORD addr, BYTE data )
{
	if( addr >= 0x5800 ) {
		return	reg[addr&0x0003] & 0x0F;
	} else {
		return	addr>>8;
	}
}

void	Mapper255::WriteLow( WORD addr, BYTE data )
{
	if( addr >= 0x5800 ) {
		reg[addr&0x0003] = data & 0x0F;
	}
}

void	Mapper255::Write( WORD addr, BYTE data )
{
	BYTE	prg = (addr & 0x0F80)>>7;
	INT	chr = (addr & 0x003F);
	INT	bank = (addr & 0x4000)>>14;

	if( addr & 0x2000 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}

	if( addr & 0x1000 ) {
		if( addr & 0x0040 ) {
			SetPROM_8K_Bank( 4, 0x80*bank+prg*4+2 );
			SetPROM_8K_Bank( 5, 0x80*bank+prg*4+3 );
			SetPROM_8K_Bank( 6, 0x80*bank+prg*4+2 );
			SetPROM_8K_Bank( 7, 0x80*bank+prg*4+3 );
		} else {
			SetPROM_8K_Bank( 4, 0x80*bank+prg*4+0 );
			SetPROM_8K_Bank( 5, 0x80*bank+prg*4+1 );
			SetPROM_8K_Bank( 6, 0x80*bank+prg*4+0 );
			SetPROM_8K_Bank( 7, 0x80*bank+prg*4+1 );
		}
	} else {
		SetPROM_8K_Bank( 4, 0x80*bank+prg*4+0 );
		SetPROM_8K_Bank( 5, 0x80*bank+prg*4+1 );
		SetPROM_8K_Bank( 6, 0x80*bank+prg*4+2 );
		SetPROM_8K_Bank( 7, 0x80*bank+prg*4+3 );
	}

	SetVROM_1K_Bank( 0, 0x200*bank+chr*8+0 );
	SetVROM_1K_Bank( 1, 0x200*bank+chr*8+1 );
	SetVROM_1K_Bank( 2, 0x200*bank+chr*8+2 );
	SetVROM_1K_Bank( 3, 0x200*bank+chr*8+3 );
	SetVROM_1K_Bank( 4, 0x200*bank+chr*8+4 );
	SetVROM_1K_Bank( 5, 0x200*bank+chr*8+5 );
	SetVROM_1K_Bank( 6, 0x200*bank+chr*8+6 );
	SetVROM_1K_Bank( 7, 0x200*bank+chr*8+7 );
}

void	Mapper255::SaveState( LPBYTE p )
{
	p[ 0] = reg[0];
	p[ 1] = reg[1];
	p[ 2] = reg[2];
	p[ 3] = reg[3];
}

void	Mapper255::LoadState( LPBYTE p )
{
	reg[0] = p[ 0];
	reg[1] = p[ 1];
	reg[2] = p[ 2];
	reg[3] = p[ 3];
}
