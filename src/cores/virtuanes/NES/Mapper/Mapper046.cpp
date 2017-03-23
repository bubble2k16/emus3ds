//////////////////////////////////////////////////////////////////////////
// Mapper046  Rumble Station                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper046::Reset()
{
	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 0;
	reg[3] = 0;

	SetBank();
	SetVRAM_Mirror( VRAM_VMIRROR );
}

void	Mapper046::WriteLow( WORD addr, BYTE data )
{
	reg[0] = data & 0x0F;
	reg[1] = (data & 0xF0)>>4;
	SetBank();
}

void	Mapper046::Write( WORD addr, BYTE data )
{
	reg[2] = data & 0x01;
	reg[3] = (data & 0x70)>>4;
	SetBank();
}

void	Mapper046::SetBank()
{
	SetPROM_8K_Bank( 4, reg[0]*8+reg[2]*4+0 );
	SetPROM_8K_Bank( 5, reg[0]*8+reg[2]*4+1 );
	SetPROM_8K_Bank( 6, reg[0]*8+reg[2]*4+2 );
	SetPROM_8K_Bank( 7, reg[0]*8+reg[2]*4+3 );

	SetVROM_1K_Bank( 0, reg[1]*64+reg[3]*8+0 );
	SetVROM_1K_Bank( 1, reg[1]*64+reg[3]*8+1 );
	SetVROM_1K_Bank( 2, reg[1]*64+reg[3]*8+2 );
	SetVROM_1K_Bank( 3, reg[1]*64+reg[3]*8+3 );
	SetVROM_1K_Bank( 4, reg[1]*64+reg[3]*8+4 );
	SetVROM_1K_Bank( 5, reg[1]*64+reg[3]*8+5 );
	SetVROM_1K_Bank( 6, reg[1]*64+reg[3]*8+6 );
	SetVROM_1K_Bank( 7, reg[1]*64+reg[3]*8+7 );
}

void	Mapper046::SaveState( LPBYTE p )
{
	p[ 0] = reg[0];
	p[ 1] = reg[1];
	p[ 2] = reg[2];
	p[ 3] = reg[3];
}

void	Mapper046::LoadState( LPBYTE p )
{
	reg[0] = p[ 0];
	reg[1] = p[ 1];
	reg[2] = p[ 2];
	reg[3] = p[ 3];
}
