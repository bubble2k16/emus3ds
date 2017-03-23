//////////////////////////////////////////////////////////////////////////
// Mapper075  Konami VRC1/Jaleco D65005                                 //
//////////////////////////////////////////////////////////////////////////
void	Mapper075::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	reg[0] = 0;
	reg[1] = 1;
}

void	Mapper075::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;

		case	0x9000:
			if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		  SetVRAM_Mirror( VRAM_VMIRROR );

			reg[0] = (reg[0] & 0x0F) | ((data & 0x02) << 3);
			reg[1] = (reg[1] & 0x0F) | ((data & 0x04) << 2);
			SetVROM_4K_Bank( 0, reg[0] );
			SetVROM_4K_Bank( 4, reg[1] );
			break;

		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xC000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0xE000:
			reg[0] = (reg[0] & 0x10) | (data & 0x0F);
			SetVROM_4K_Bank( 0, reg[0] );
			break;

		case	0xF000:
			reg[1] = (reg[1] & 0x10) | (data & 0x0F);
			SetVROM_4K_Bank( 4, reg[1] );
			break;
	}
}

void	Mapper075::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
}

void	Mapper075::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
}
