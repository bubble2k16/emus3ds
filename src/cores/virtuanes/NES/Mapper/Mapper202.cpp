//////////////////////////////////////////////////////////////////////////
// Mapper202  150-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper202::Reset()
{
	SetPROM_16K_Bank( 4, 6 );
	SetPROM_16K_Bank( 6, 7 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper202::ExWrite( WORD addr, BYTE data )
{
	if( addr >= 0x4020 ) {
		WriteSub( addr, data );
	}
}

void	Mapper202::WriteLow( WORD addr, BYTE data )
{
	WriteSub( addr, data );
}

void	Mapper202::Write( WORD addr, BYTE data )
{
	WriteSub( addr, data );
}

void	Mapper202::WriteSub( WORD addr, BYTE data )
{
	INT	bank = (addr>>1) & 0x07;

	SetPROM_16K_Bank( 4, bank );
	if( (addr & 0x0C) == 0x0C ) {
		SetPROM_16K_Bank( 6, bank+1 );
	} else {
		SetPROM_16K_Bank( 6, bank );
	}
	SetVROM_8K_Bank( bank );

	if( addr & 0x01 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}
}
