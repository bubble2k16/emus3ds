//////////////////////////////////////////////////////////////////////////
// Mapper135  SACHEN CHEN                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper135::Reset()
{
	cmd = 0;
	chr0l = chr1l = chr0h = chr1h = chrch = 0;

	SetPROM_32K_Bank( 0 );
	SetBank_PPU();
}

void	Mapper135::WriteLow( WORD addr, BYTE data )
{
	switch( addr & 0x4101 ) {
		case	0x4100:
			cmd = data & 0x07;
			break;
		case	0x4101:
			switch( cmd ) {
				case	0:
					chr0l = data & 0x07;
					SetBank_PPU();
					break;
				case	1:
					chr0h = data & 0x07;
					SetBank_PPU();
					break;
				case	2:
					chr1l = data & 0x07;
					SetBank_PPU();
					break;
				case	3:
					chr1h = data & 0x07;
					SetBank_PPU();
					break;
				case	4:
					chrch = data & 0x07;
					SetBank_PPU();
					break;
				case	5:
					SetPROM_32K_Bank( data&0x07 );
					break;
				case	6:
					break;
				case	7:
					switch( (data>>1)&0x03 ) {
						case	0: SetVRAM_Mirror( VRAM_MIRROR4L ); break;
						case	1: SetVRAM_Mirror( VRAM_HMIRROR  ); break;
						case	2: SetVRAM_Mirror( VRAM_VMIRROR  ); break;
						case	3: SetVRAM_Mirror( VRAM_MIRROR4L ); break;
					}
					break;
			}
			break;
	}

	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

void	Mapper135::SetBank_PPU()
{
	SetVROM_2K_Bank( 0, 0|(chr0l<<1)|(chrch<<4) );
	SetVROM_2K_Bank( 2, 1|(chr0h<<1)|(chrch<<4) );
	SetVROM_2K_Bank( 4, 0|(chr1l<<1)|(chrch<<4) );
	SetVROM_2K_Bank( 6, 1|(chr1h<<1)|(chrch<<4) );
}

void	Mapper135::SaveState( LPBYTE p )
{
	p[ 0] = cmd;
	p[ 1] = chr0l;
	p[ 2] = chr0h;
	p[ 3] = chr1l;
	p[ 4] = chr1h;
	p[ 5] = chrch;
}

void	Mapper135::LoadState( LPBYTE p )
{
	cmd   = p[ 0];
	chr0l = p[ 1];
	chr0h = p[ 2];
	chr0l = p[ 3];
	chr0h = p[ 4];
	chrch = p[ 5];
}
