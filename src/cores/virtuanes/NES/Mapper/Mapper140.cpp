//////////////////////////////////////////////////////////////////////////
// Mapper140                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper140::Reset()
{
	SetPROM_32K_Bank( 0 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper140::WriteLow( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( (data&0xF0)>>4 );
	SetVROM_8K_Bank( data&0x0F );
}

