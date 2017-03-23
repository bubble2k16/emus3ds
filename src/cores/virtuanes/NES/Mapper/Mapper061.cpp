//////////////////////////////////////////////////////////////////////////
// Mapper061                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper061::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper061::Write( WORD addr, BYTE data )
{
	switch( addr & 0x30 ) {
		case	0x00:
		case	0x30:
			SetPROM_32K_Bank( addr & 0x0F );
			break;
		case	0x10:
		case	0x20:
			SetPROM_16K_Bank( 4, ((addr & 0x0F)<<1)|((addr&0x20)>>4) );
			SetPROM_16K_Bank( 6, ((addr & 0x0F)<<1)|((addr&0x20)>>4) );
			break;
	}

	if( addr & 0x80 ) SetVRAM_Mirror( VRAM_HMIRROR );
	else		  SetVRAM_Mirror( VRAM_VMIRROR );
}

