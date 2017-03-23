//////////////////////////////////////////////////////////////////////////
// Mapper109 SACHEN The Great Wall SA-019                               //
//////////////////////////////////////////////////////////////////////////

void	Mapper109::Reset()
{
	reg = 0;
	SetPROM_32K_Bank( 0 );

	chr0 = 0;
	chr1 = 0;
	chr2 = 0;
	chr3 = 0;
	SetBank_PPU();
	chrmode0 = 0;
	chrmode1 = 0;
}

void	Mapper109::WriteLow( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x4100:
			reg = data;
			break;
		case	0x4101:
			switch( reg ) {
				case	0:
					chr0 = data;
					SetBank_PPU();
					break;
				case	1:
					chr1 = data;
					SetBank_PPU();
					break;
				case	2:
					chr2 = data;
					SetBank_PPU();
					break;
				case	3:
					chr3 = data;
					SetBank_PPU();
					break;
				case	4:
					chrmode0 = data & 0x01;
					SetBank_PPU();
					break;
				case	5:
					SetPROM_32K_Bank( data & 0x07 );
					break;
				case	6:
					chrmode1 = data & 0x07;
					SetBank_PPU();
					break;
				case	7:
					if( data & 0x01 ) SetVRAM_Mirror( VRAM_HMIRROR );
					else		  SetVRAM_Mirror( VRAM_VMIRROR );
					break;
			}
		break;
	}

}

void	Mapper109::SetBank_PPU()
{
	if( VROM_1K_SIZE ) {
		SetVROM_1K_Bank( 0, chr0 );
		SetVROM_1K_Bank( 1, chr1|((chrmode1<<3)&0x8) );
		SetVROM_1K_Bank( 2, chr2|((chrmode1<<2)&0x8) );
		SetVROM_1K_Bank( 3, chr3|((chrmode1<<1)&0x8) |(chrmode0*0x10) );
		SetVROM_1K_Bank( 4, VROM_1K_SIZE-4 );
		SetVROM_1K_Bank( 5, VROM_1K_SIZE-3 );
		SetVROM_1K_Bank( 6, VROM_1K_SIZE-2 );
		SetVROM_1K_Bank( 7, VROM_1K_SIZE-1 );
	}
}

void	Mapper109::SaveState( LPBYTE p )
{
	p[ 0] = reg;
	p[ 1] = chr0;
	p[ 2] = chr1;
	p[ 3] = chr2;
	p[ 4] = chr3;
	p[ 5] = chrmode0;
	p[ 6] = chrmode1;
}

void	Mapper109::LoadState( LPBYTE p )
{
	reg      = p[ 0];
	chr0     = p[ 1];
	chr1     = p[ 2];
	chr2     = p[ 3];
	chr3     = p[ 4];
	chrmode0 = p[ 5];
	chrmode1 = p[ 6];
}
