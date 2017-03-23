//////////////////////////////////////////////////////////////////////////
// Mapper000                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper000::Reset()
{
	switch( PROM_16K_SIZE ) {
		default:
		case	1:	// 16K only
			SetPROM_16K_Bank( 4, 0 );
			SetPROM_16K_Bank( 6, 0 );
			break;
		case	2:	// 32K
			SetPROM_32K_Bank( 0 );
			break;
	}

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0x4e7db5af ) {	// Circus Charlie(J)
		nes->SetRenderMethod( NES::POST_RENDER );
	}
	if( crc == 0x57970078 ) {	// F-1 Race(J)
		nes->SetRenderMethod( NES::POST_RENDER );
	}
	if( crc == 0xaf2bbcbc		// Mach Rider(JU)
	 || crc == 0x3acd4bf1 ) {	// Mach Rider(Alt)(JU)
		nes->SetRenderMethod( NES::POST_RENDER );
	}
}
