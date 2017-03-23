//////////////////////////////////////////////////////////////////////////
// Mapper097  Irem 74161                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper097::Reset()
{
	SetPROM_32K_Bank( PROM_8K_SIZE-2, PROM_8K_SIZE-1, 0, 1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper097::Write( WORD addr, BYTE data )
{
	if( addr < 0xC000 ) {
		SetPROM_16K_Bank( 6, data & 0x0F );

		if( data & 0x80 ) SetVRAM_Mirror( VRAM_VMIRROR );
		else		  SetVRAM_Mirror( VRAM_HMIRROR );
	}
}
