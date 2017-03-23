//////////////////////////////////////////////////////////////////////////
// TurboFile                                                            //
//////////////////////////////////////////////////////////////////////////
void	EXPAD_TurboFile::Reset()
{
	tf_address = 0;
	tf_dataold = 0;
	tf_databit = 0;
	tf_data    = 0;
}

BYTE	EXPAD_TurboFile::Read4017()
{
	return	tf_data;
}

void	EXPAD_TurboFile::Write4016( BYTE data )
{
	INT	bank = nes->GetTurboFileBank();

	// Reset
	if( !(data & 0x02) ) {
		tf_address = 0;
		tf_databit = 0x01;
	}
	// Write bit
	if( data & 0x04 ) {
		ERAM[bank*0x2000+tf_address] &= ~tf_databit;
		ERAM[bank*0x2000+tf_address] |= (data&0x01)?tf_databit:0;
	}
	// Address inc/bit shift
	if( (tf_dataold&(~data)) & 0x04 ) {
		if( tf_databit == 0x80 ) {
			tf_address = (tf_address+1) & 0x1FFF;
			tf_databit = 0x01;
		} else {
			tf_databit <<= 1;
		}
	}
	// Read bit
	if( ERAM[bank*0x2000+tf_address] & tf_databit ) {
		tf_data = 0x04;
	} else {
		tf_data = 0x00;
	}
	tf_dataold = data;
}
