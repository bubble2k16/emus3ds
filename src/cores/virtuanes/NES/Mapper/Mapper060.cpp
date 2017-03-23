//////////////////////////////////////////////////////////////////////////
// Mapper060                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper060::Reset()
{
	patch = 0;

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0xf9c484a0 ) {	// Reset Based 4-in-1(Unl)
		SetPROM_16K_Bank( 4, game_sel );
		SetPROM_16K_Bank( 6, game_sel );
		SetVROM_8K_Bank( game_sel );
		game_sel++;
		game_sel &= 3;
	} else {
		patch = 1;
		SetPROM_32K_Bank( 0 );
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper060::Write( WORD addr, BYTE data )
{
	if( patch ) {
		if( addr & 0x80 ) {
			SetPROM_16K_Bank( 4, (addr&0x70)>>4 );
			SetPROM_16K_Bank( 6, (addr&0x70)>>4 );
		} else {
			SetPROM_32K_Bank( (addr&0x70)>>5 );
		}

		SetVROM_8K_Bank( addr&0x07 );

		if( data & 0x08 ) SetVRAM_Mirror( VRAM_VMIRROR );
		else		  SetVRAM_Mirror( VRAM_HMIRROR );
	}
}

