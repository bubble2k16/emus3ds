//////////////////////////////////////////////////////////////////////////
// VS-Unisystem                                                         //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_VSUnisystem::Reset()
{
}

BYTE	EXPAD_VSUnisystem::Read4016()
{
BYTE	data = 0x00;

	// Coin 1
	if( Config.ButtonCheck( 0, Config.controller.nVSUnisystem ) )
		data |= 0x20;
	// Coin 2
	if( Config.ButtonCheck( 1, Config.controller.nVSUnisystem ) ) {
		data |= 0x40;
	}
	// Service
	if( Config.ButtonCheck( 2, Config.controller.nVSUnisystem ) ) {
		data |= 0x04;
	}

	// Dip-Switch
	data |= ((nes->GetVSDipSwitch()<<3) & 0x18);

	if( nes->rom->GetPROM_CRC() == 0xC99EC059 ) {	// VS Raid on Bungeling Bay(J)
		data |= 0x80;
	}

	return	data;
}

BYTE	EXPAD_VSUnisystem::Read4017()
{
BYTE	data = 0x00;

	// Dip-Switch
	data = nes->GetVSDipSwitch() & 0xFC;

	return	data;
}

