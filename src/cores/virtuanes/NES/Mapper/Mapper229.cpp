//////////////////////////////////////////////////////////////////////////
// Mapper229  31-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
void	Mapper229::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper229::Write( WORD addr, BYTE data )
{
	if( addr & 0x001E ) {
		BYTE	prg = addr&0x001F;

		SetPROM_8K_Bank( 4, prg*2+0 );
		SetPROM_8K_Bank( 5, prg*2+1 );
		SetPROM_8K_Bank( 6, prg*2+0 );
		SetPROM_8K_Bank( 7, prg*2+1 );

		SetVROM_8K_Bank( addr & 0x0FFF );
	} else {
		SetPROM_32K_Bank( 0 );
		SetVROM_8K_Bank( 0 );
	}

	if( addr & 0x0020 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}
}
