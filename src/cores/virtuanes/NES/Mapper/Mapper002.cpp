//////////////////////////////////////////////////////////////////////////
// Mapper002 UNROM                                                      //
//////////////////////////////////////////////////////////////////////////
void	Mapper002::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	patch = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();
//	if( crc == 0x322c9b09 ) {	// Metal Gear (Alt)(J)
////		nes->SetFrameIRQmode( FALSE );
//	}
//	if( crc == 0xe7a3867b ) {	// Dragon Quest 2(Alt)(J)
//		nes->SetFrameIRQmode( FALSE );
//	}
////	if( crc == 0x9622fbd9 ) {	// Ballblazer(J)
////		patch = 0;
////	}
	if( crc == 0x8c3d54e8		// Ikari(J)
	 || crc == 0x655efeed		// Ikari Warriors(U)
	 || crc == 0x538218b2 ) {	// Ikari Warriors(E)
		patch = 1;
	}

	if( crc == 0xb20c1030 ) {	// Shanghai(J)(original)
		patch = 2;
	}
}


void	Mapper002::WriteLow( WORD addr, BYTE data )
{
	if( !nes->rom->IsSAVERAM() ) {
		if( addr >= 0x5000 && patch == 1 )
			SetPROM_16K_Bank( 4, data );
	} else {
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper002::Write( WORD addr, BYTE data )
{
	if( patch != 2 )
		SetPROM_16K_Bank( 4, data );
	else
		SetPROM_16K_Bank( 4, data>>4 );
}
