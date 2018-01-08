//////////////////////////////////////////////////////////////////////////
// Mapper071  Camerica                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper071::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
//	SetPROM_32K_Bank( 0, 1, 0, 1 );
	rom_type = 0;
	rom_bank = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x5b2b72cb		// Big Nose Freaks Out(U)
	 || crc == 0xccdcbfc6 ) {	// Big Nose Freaks Out(Aladdin)(U)
//		nes->SetRenderMethod( NES::TILE_RENDER );
//		nes->ppu->SetExtNameTableMode( TRUE );
	}

	if( crc == 0x6096F84E		// PEGASUS 5 IN 1 (unl)
	 || crc == 0x57850320 ) {	// PEGASUS 5 IN 1 (unl)(re-link)
		SetPROM_32K_Bank( 0, 1, 30, 31 );
		rom_type = 1;
	}

}

void	Mapper071::WriteLow( WORD addr, BYTE data )
{
	if( (addr&0xE000)==0x6000 ) {
		SetPROM_16K_Bank( 4, rom_bank + data );
	}
}

void	Mapper071::Write( WORD addr, BYTE data )
{
	switch( addr&0xF000 ) {
		case	0x8000:
			if( rom_type && (addr==0x8927) ){
				rom_bank = (data & 7) * 16;
				SetPROM_16K_Bank( 6, rom_bank + 15);
			}
			break;
		case	0x9000:
			if( data&0x10 ) SetVRAM_Mirror( VRAM_MIRROR4H );
			else		SetVRAM_Mirror( VRAM_MIRROR4L );
			break;

		case	0xC000:
		case	0xD000:
		case	0xE000:
		case	0xF000:
			SetPROM_16K_Bank( 4, rom_bank + data );
			break;
	}
}

