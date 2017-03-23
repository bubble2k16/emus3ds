//////////////////////////////////////////////////////////////////////////
// Mapper022  Konami VRC2 type A                                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper022::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper022::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case 0x8000:
			SetPROM_8K_Bank( 4, data );
			break;

		case 0x9000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4H );
			else		     SetVRAM_Mirror( VRAM_MIRROR4L );
			break;

		case 0xA000:
			SetPROM_8K_Bank( 5, data );
			break;

		case 0xB000:
			SetVROM_1K_Bank( 0, data>>1 );
			break;

		case 0xB001:
			SetVROM_1K_Bank( 1, data>>1 );
			break;

		case 0xC000:
			SetVROM_1K_Bank( 2, data>>1 );
			break;

		case 0xC001:
			SetVROM_1K_Bank( 3, data>>1 );
			break;

		case 0xD000:
			SetVROM_1K_Bank( 4, data>>1 );
			break;

		case 0xD001:
			SetVROM_1K_Bank( 5, data>>1 );
			break;

		case 0xE000:
			SetVROM_1K_Bank( 6, data>>1 );
			break;

		case 0xE001:
			SetVROM_1K_Bank( 7, data>>1 );
			break;
	}
}
