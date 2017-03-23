//////////////////////////////////////////////////////////////////////////
// Mapper072  Jaleco/Type1 lower bank switch                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper072::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper072::Write( WORD addr, BYTE data )
{
	if( data & 0x80 ) {
		SetPROM_16K_Bank( 4, data&0x0F );
	} else if( data & 0x40 ) {
		SetVROM_8K_Bank( data&0x0F );
	} else {
		if( addr >= 0xC100 && addr <= 0xC11F && data == 0x20 ) {
//DEBUGOUT( "ADDR:%04X DATA:%02X\n", addr, data );
DEBUGOUT( "SOUND CODE:%02X\n", addr & 0x1F );

			// OSD�ɂ����ׂ����c
			//if( Config.sound.bExtraSoundEnable ) {
			//	DirectSound.EsfAllStop();
			//	DirectSound.EsfPlay( ESF_MOETENNIS_00+(addr&0x1F) );
			//}
		}
	}
}
