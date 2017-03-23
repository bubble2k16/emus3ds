//////////////////////////////////////////////////////////////////////////
// Mapper243  PC-Sachen/Hacker                                          //
//////////////////////////////////////////////////////////////////////////
void	Mapper243::Reset()
{
	SetPROM_32K_Bank( 0 );
	if( VROM_8K_SIZE > 4 ) {
		SetVROM_8K_Bank( 4 );
	} else if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	SetVRAM_Mirror( VRAM_HMIRROR );

	reg[0] = 0;
	reg[1] = 0;
	reg[2] = 3;
	reg[3] = 0;
}

void	Mapper243::WriteLow( WORD addr, BYTE data )
{
	if( (addr&0x4101) == 0x4100 ) {
		reg[0] = data;
	} else if( (addr&0x4101) == 0x4101 ) {
		switch( reg[0] & 0x07 ) {
			case	0:
				reg[1] = 0;
				reg[2] = 3;
				break;
			case	4:
				reg[2] = (reg[2]&0x06)|(data&0x01);
				break;
			case	5:
				reg[1] = data&0x01;
				break;
			case	6:
				reg[2] = (reg[2]&0x01)|((data&0x03)<<1);
				break;
			case	7:
				reg[3] = data&0x01;
				break;
			default:
				break;
		}

		SetPROM_32K_Bank( reg[1] );
		SetVROM_8K_Bank( reg[2]*8+0, reg[2]*8+1, reg[2]*8+2, reg[2]*8+3,
				 reg[2]*8+4, reg[2]*8+5, reg[2]*8+6, reg[2]*8+7 );

		if( reg[3] ) {
			SetVRAM_Mirror( VRAM_VMIRROR );
		} else {
			SetVRAM_Mirror( VRAM_HMIRROR );
		}
	}
}

void	Mapper243::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = reg[2];
	p[3] = reg[3];
}

void	Mapper243::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	reg[2] = p[2];
	reg[3] = p[3];
}

