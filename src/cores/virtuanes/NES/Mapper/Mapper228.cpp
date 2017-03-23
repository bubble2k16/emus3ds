//////////////////////////////////////////////////////////////////////////
// Mapper228  Action 52                                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper228::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper228::Write( WORD addr, BYTE data )
{
	BYTE	prg = (addr&0x0780)>>7;

	switch( (addr&0x1800)>>11 ) {
		case	1:
			prg |= 0x10;
			break;
		case	3:
			prg |= 0x20;
			break;
	}

	if( addr & 0x0020 ) {
		prg <<= 1;
		if( addr & 0x0040 ) {
			prg++;
		}
		SetPROM_8K_Bank( 4, prg*4+0 );
		SetPROM_8K_Bank( 5, prg*4+1 );
		SetPROM_8K_Bank( 6, prg*4+0 );
		SetPROM_8K_Bank( 7, prg*4+1 );
	} else {
		SetPROM_8K_Bank( 4, prg*4+0 );
		SetPROM_8K_Bank( 5, prg*4+1 );
		SetPROM_8K_Bank( 6, prg*4+2 );
		SetPROM_8K_Bank( 7, prg*4+3 );
	}

	SetVROM_8K_Bank( ((addr&0x000F)<<2)|(data&0x03) );

	if( addr & 0x2000 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}
}
