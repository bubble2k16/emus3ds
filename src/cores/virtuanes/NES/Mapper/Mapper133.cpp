//////////////////////////////////////////////////////////////////////////
// Mapper133  SACHEN CHEN                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper133::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper133::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x4120 ) {
		SetPROM_32K_Bank( (data&0x04)>>2 );
		SetVROM_8K_Bank( data & 0x03 );
	}
	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

