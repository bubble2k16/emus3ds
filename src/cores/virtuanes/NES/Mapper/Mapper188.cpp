//////////////////////////////////////////////////////////////////////////
// Mapper188  Bandai Karaoke Studio                                     //
//////////////////////////////////////////////////////////////////////////
void	Mapper188::Reset()
{
	if( PROM_8K_SIZE > 16 ) {
		SetPROM_32K_Bank( 0, 1, 14, 15 );
	} else {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}
}

void	Mapper188::Write( WORD addr, BYTE data )
{
	if( data ) {
		if( data & 0x10 ) {
			data &= 0x07;
			SetPROM_16K_Bank( 4, data );
		} else {
			SetPROM_16K_Bank( 4, data+8 );
		}
	} else {
		if( PROM_8K_SIZE == 0x10 ) {
			SetPROM_16K_Bank( 4, 7 );
		} else {
			SetPROM_16K_Bank( 4, 8 );
		}
	}
}
