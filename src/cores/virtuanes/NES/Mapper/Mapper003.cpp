//////////////////////////////////////////////////////////////////////////
// Mapper003 CNROM                                                      //
//////////////////////////////////////////////////////////////////////////
void	Mapper003::Reset()
{
	switch( PROM_16K_SIZE ) {
		case	1:	// 16K only
			SetPROM_16K_Bank( 4, 0 );
			SetPROM_16K_Bank( 6, 0 );
			break;
		case	2:	// 32K
			SetPROM_32K_Bank( 0 );
			break;
	}
//	nes->SetRenderMethod( NES::TILE_RENDER );
	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x2b72fe7e ) {	// Ganso Saiyuuki - Super Monkey Dai Bouken(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
		nes->ppu->SetExtNameTableMode( TRUE );
	}

//	if( crc == 0xE44D95B5 ) {	// ‚Ð‚Ý‚Â‚—
//	}
}

#if	0
void	Mapper003::WriteLow( WORD addr, BYTE data )
{
	if( patch ) {
		Mapper::WriteLow( addr, data );
	} else {
		if( nes->rom->IsSAVERAM() ) {
			Mapper::WriteLow( addr, data );
		} else {
			if( addr >= 0x4800 ) {
				SetVROM_8K_Bank( data & 0x03 );
			}
		}
	}
}
#endif

void	Mapper003::Write( WORD addr, BYTE data )
{
	SetVROM_8K_Bank( data );
}
