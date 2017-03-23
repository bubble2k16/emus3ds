//////////////////////////////////////////////////////////////////////////
// Mapper201  21-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
void	Mapper201::Reset()
{
//	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetPROM_16K_Bank( 4, 0 );
	SetPROM_16K_Bank( 6, 0 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper201::Write( WORD addr, BYTE data )
{
	BYTE	bank = (BYTE)addr & 0x03;
	if( !(addr&0x08) )
		bank = 0;
	SetPROM_32K_Bank( bank );
	SetVROM_8K_Bank( bank );
}
