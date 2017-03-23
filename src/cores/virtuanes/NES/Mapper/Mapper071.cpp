//////////////////////////////////////////////////////////////////////////
// Mapper071  Camerica                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper071::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper071::WriteLow( WORD addr, BYTE data )
{
	if( (addr&0xE000)==0x6000 ) {
		SetPROM_16K_Bank( 4, data );
	}
}

void	Mapper071::Write( WORD addr, BYTE data )
{
	switch( addr&0xF000 ) {
		case	0x9000:
			if( data&0x10 ) SetVRAM_Mirror( VRAM_MIRROR4H );
			else		SetVRAM_Mirror( VRAM_MIRROR4L );
			break;

		case	0xC000:
		case	0xD000:
		case	0xE000:
		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}

