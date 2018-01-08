//////////////////////////////////////////////////////////////////////////
// Mapper007 AOROM/AMROM                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper007::Reset()
{
	patch = 0;

	SetPROM_32K_Bank( 0 );
	SetVRAM_Mirror( VRAM_MIRROR4L );

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x3c9fe649 ) {	// WWF Wrestlemania Challenge(U)
		SetVRAM_Mirror( VRAM_VMIRROR );
		patch = 1;
	}
	if( crc == 0x09874777 ) {	// Marble Madness(U)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}

	if( crc == 0x279710DC		// Battletoads (U)
	 || crc == 0xCEB65B06 ) {	// Battletoads Double Dragon (U)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
		::memset( WRAM, 0, sizeof(WRAM) );
	}
}

void	Mapper007::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data & 0x07 );

	if( !patch ) {
		if( data & 0x10 ) SetVRAM_Mirror( VRAM_MIRROR4H );
		else		  SetVRAM_Mirror( VRAM_MIRROR4L );
	}
}
