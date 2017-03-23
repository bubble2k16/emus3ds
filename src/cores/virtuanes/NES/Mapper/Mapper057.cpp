//////////////////////////////////////////////////////////////////////////
// Mapper057                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper057::Reset()
{
	SetPROM_32K_Bank( 0, 1, 0, 1 );
	SetVROM_8K_Bank( 0 );

	reg = 0;
}

void	Mapper057::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			if( data & 0x40 ) {
				SetVROM_8K_Bank( (data&0x03)+((reg&0x10)>>1)+(reg&0x07) );
			}
			break;
		case	0x8800:
			reg = data;

			if( data & 0x80 ) {
				SetPROM_8K_Bank( 4, ((data & 0x40)>>6)*4+8+0 );
				SetPROM_8K_Bank( 5, ((data & 0x40)>>6)*4+8+1 );
				SetPROM_8K_Bank( 6, ((data & 0x40)>>6)*4+8+2 );
				SetPROM_8K_Bank( 7, ((data & 0x40)>>6)*4+8+3 );
			} else {
				SetPROM_8K_Bank( 4, ((data & 0x60)>>5)*2+0 );
				SetPROM_8K_Bank( 5, ((data & 0x60)>>5)*2+1 );
				SetPROM_8K_Bank( 6, ((data & 0x60)>>5)*2+0 );
				SetPROM_8K_Bank( 7, ((data & 0x60)>>5)*2+1 );
			}

			SetVROM_8K_Bank( (data&0x07)+((data&0x10)>>1) );

			if( data & 0x08 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		  SetVRAM_Mirror( VRAM_VMIRROR );

			break;
	}
}

void	Mapper057::SaveState( LPBYTE p )
{
	p[0] = reg;
}

void	Mapper057::LoadState( LPBYTE p )
{
	reg = p[0];
}
