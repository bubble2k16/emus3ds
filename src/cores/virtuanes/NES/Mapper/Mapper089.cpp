//////////////////////////////////////////////////////////////////////////
// Mapper089  SunSoft (…ŒË‰©–å)                                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper089::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper089::Write( WORD addr, BYTE data )
{
	if( (addr&0xFF00) == 0xC000 ) {
		SetPROM_16K_Bank( 4, (data&0x70)>>4 );

		SetVROM_8K_Bank( ((data&0x80)>>4)|(data&0x07) );

		if( data & 0x08 ) SetVRAM_Mirror( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror( VRAM_MIRROR4L );
	}
}
