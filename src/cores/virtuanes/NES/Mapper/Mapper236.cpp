//////////////////////////////////////////////////////////////////////////
// Mapper236  800-in-1                                                  //
//////////////////////////////////////////////////////////////////////////
void	Mapper236::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	bank = mode = 0;
}

void	Mapper236::Write( WORD addr, BYTE data )
{
	if( addr >= 0x8000 && addr <= 0xBFFF ) {
		bank = ((addr&0x03)<<4)|(bank&0x07);
	} else {
		bank = (addr&0x07)|(bank&0x30);
		mode = addr&0x30;
	}

	if( addr & 0x20 ) {
		SetVRAM_Mirror( VRAM_HMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_VMIRROR );
	}

	switch( mode ) {
		case	0x00:
			bank |= 0x08;
			SetPROM_8K_Bank( 4, bank*2+0 );
			SetPROM_8K_Bank( 5, bank*2+1 );
			SetPROM_8K_Bank( 6, (bank|0x07)*2+0 );
			SetPROM_8K_Bank( 7, (bank|0x07)*2+1 );
			break;
		case	0x10:
			bank |= 0x37;
			SetPROM_8K_Bank( 4, bank*2+0 );
			SetPROM_8K_Bank( 5, bank*2+1 );
			SetPROM_8K_Bank( 6, (bank|0x07)*2+0 );
			SetPROM_8K_Bank( 7, (bank|0x07)*2+1 );
			break;
		case	0x20:
			bank |= 0x08;
			SetPROM_8K_Bank( 4, (bank&0xFE)*2+0 );
			SetPROM_8K_Bank( 5, (bank&0xFE)*2+1 );
			SetPROM_8K_Bank( 6, (bank&0xFE)*2+2 );
			SetPROM_8K_Bank( 7, (bank&0xFE)*2+3 );
			break;
		case	0x30:
			bank |= 0x08;
			SetPROM_8K_Bank( 4, bank*2+0 );
			SetPROM_8K_Bank( 5, bank*2+1 );
			SetPROM_8K_Bank( 6, bank*2+0 );
			SetPROM_8K_Bank( 7, bank*2+1 );
			break;
	}
}

void	Mapper236::SaveState( LPBYTE p )
{
	p[ 0] = bank;
	p[ 1] = mode;
}

void	Mapper236::LoadState( LPBYTE p )
{
	bank = p[ 0];
	mode = p[ 1];
}
