//////////////////////////////////////////////////////////////////////////
// Mapper058                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper058::Reset()
{
	SetPROM_32K_Bank( 0, 1, 0, 1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper058::Write( WORD addr, BYTE data )
{
	if( addr & 0x40 ) {
		SetPROM_16K_Bank( 4, addr&0x07 );
		SetPROM_16K_Bank( 6, addr&0x07 );
	} else {
		SetPROM_32K_Bank( (addr&0x06)>>1 );
	}

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( (addr&0x38)>>3 );
	}

	if( data & 0x02 ) SetVRAM_Mirror( VRAM_VMIRROR );
	else		  SetVRAM_Mirror( VRAM_HMIRROR );
}

