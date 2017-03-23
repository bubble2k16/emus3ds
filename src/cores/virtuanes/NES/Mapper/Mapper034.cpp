//////////////////////////////////////////////////////////////////////////
// Mapper034  Nina-1                                                    //
//////////////////////////////////////////////////////////////////////////
void	Mapper034::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper034::WriteLow( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x7FFD:
			SetPROM_32K_Bank( data );
			break;
		case	0x7FFE:
			SetVROM_4K_Bank( 0, data );
			break;
		case	0x7FFF:
			SetVROM_4K_Bank( 4, data );
			break;
	}
}

void	Mapper034::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( data );
}
