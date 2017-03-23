//////////////////////////////////////////////////////////////////////////
// Mapper226  76-in-1                                                   //
//////////////////////////////////////////////////////////////////////////
void	Mapper226::Reset()
{
	SetPROM_32K_Bank( 0 );

	reg[0] = 0;
	reg[1] = 0;
}

void	Mapper226::Write( WORD addr, BYTE data )
{
	if( addr & 0x001 ) {
		reg[1] = data;
	} else {
		reg[0] = data;
	}

	if( reg[0] & 0x40 ) {
		SetVRAM_Mirror( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_HMIRROR );
	}

	BYTE	bank = ((reg[0]&0x1E)>>1)|((reg[0]&0x80)>>3)|((reg[1]&0x01)<<5);

	if( reg[0] & 0x20 ) {
		if( reg[0] & 0x01 ) {
			SetPROM_8K_Bank( 4, bank*4+2 );
			SetPROM_8K_Bank( 5, bank*4+3 );
			SetPROM_8K_Bank( 6, bank*4+2 );
			SetPROM_8K_Bank( 7, bank*4+3 );
		} else {
			SetPROM_8K_Bank( 4, bank*4+0 );
			SetPROM_8K_Bank( 5, bank*4+1 );
			SetPROM_8K_Bank( 6, bank*4+0 );
			SetPROM_8K_Bank( 7, bank*4+1 );
		}
	} else {
		SetPROM_8K_Bank( 4, bank*4+0 );
		SetPROM_8K_Bank( 5, bank*4+1 );
		SetPROM_8K_Bank( 6, bank*4+2 );
		SetPROM_8K_Bank( 7, bank*4+3 );
	}
}

void	Mapper226::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
}

void	Mapper226::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
}

