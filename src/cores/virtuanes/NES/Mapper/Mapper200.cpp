//////////////////////////////////////////////////////////////////////////
// Mapper200  1200-in-1                                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper200::Reset()
{
//	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper200::Write( WORD addr, BYTE data )
{
	SetPROM_16K_Bank( 4, addr & 0x07 );
	SetPROM_16K_Bank( 6, addr & 0x07 );
	SetVROM_8K_Bank( addr & 0x07 );

	if( addr & 0x01 ) {
		SetVRAM_Mirror( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_HMIRROR );
	}
}
