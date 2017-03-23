//////////////////////////////////////////////////////////////////////////
// Mapper083  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper083::Reset()
{
	for( INT i = 0; i < 3; i++ ) {
		reg[i] = 0x00;
	}

	if( PROM_8K_SIZE >= 32 ) {
		SetPROM_32K_Bank( 0, 1, 30, 31 );
		reg[1] = 0x30;
	} else {
		SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	}

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	chr_bank = 0;

	irq_enable = 0;	// Disable
	irq_counter = 0;

	patch = 0;
	if( nes->rom->GetPROM_CRC() == 0x1461D1F8 ) {
		patch = 1;
	}
}

BYTE	Mapper083::ReadLow( WORD addr )
{
	if( (addr&0x5100) == 0x5100 ) {
		return	reg[2];
	} else if( addr >= 0x6000 ) {
		return	Mapper::ReadLow( addr );
	}

	return	(BYTE)(addr>>8);
}

void	Mapper083::WriteLow( WORD addr, BYTE data )
{
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x5101:
		case	0x5102:
		case	0x5103:
			reg[2] = data;
			break;
	}

	if( addr >= 0x6000 ) {
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper083::Write( WORD addr, BYTE data )
{
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	switch( addr ) {
		case	0x8000:
		case	0xB000:
		case	0xB0FF:
		case	0xB1FF:
			reg[0] = data;
			chr_bank = (data&0x30)<<4;
			SetPROM_16K_Bank( 4, data );
			SetPROM_16K_Bank( 6, (data&0x30)|0x0F );
			break;

		case	0x8100:
			reg[1] = data & 0x80;
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0x8200:
			irq_counter = (irq_counter&0xFF00)|(INT)data;
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x8201:
			irq_counter = (irq_counter&0x00FF)|((INT)data<<8);
			irq_enable = reg[1];
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0x8300:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8301:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x8302:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0x8310:
			if( patch ) {
				SetVROM_2K_Bank( 0, chr_bank|data );
			} else {
				SetVROM_1K_Bank( 0, chr_bank|data );
			}
			break;
		case	0x8311:
			if( patch ) {
				SetVROM_2K_Bank( 2, chr_bank|data );
			} else {
				SetVROM_1K_Bank( 1, chr_bank|data );
			}
			break;
		case	0x8312:
			SetVROM_1K_Bank( 2, chr_bank|data );
			break;
		case	0x8313:
			SetVROM_1K_Bank( 3, chr_bank|data );
			break;
		case	0x8314:
			SetVROM_1K_Bank( 4, chr_bank|data );
			break;
		case	0x8315:
			SetVROM_1K_Bank( 5, chr_bank|data );
			break;
		case	0x8316:
			if( patch ) {
				SetVROM_2K_Bank( 4, chr_bank|data );
			} else {
				SetVROM_1K_Bank( 6, chr_bank|data );
			}
			break;
		case	0x8317:
			if( patch ) {
				SetVROM_2K_Bank( 6, chr_bank|data );
			} else {
				SetVROM_1K_Bank( 7, chr_bank|data );
			}
			break;

		case	0x8318:
			SetPROM_16K_Bank( 4, (reg[0]&0x30)|data );
			break;
	}
}

void	Mapper083::HSync( INT scanline )
{
	if( irq_enable ) {
		if( irq_counter <= 113 ) {
//			nes->cpu->IRQ();
			irq_enable = 0;
//			nes->cpu->SetIRQ( IRQ_MAPPER );
			nes->cpu->SetIRQ( IRQ_TRIGGER );
		} else {
			irq_counter -= 113;
		}
	}
}

void	Mapper083::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = reg[2];
	*(INT*)&p[3] = chr_bank;
	p[7] = irq_enable;
	*(INT*)&p[8] = irq_counter;
}

void	Mapper083::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	reg[2] = p[2];
	chr_bank = *(INT*)&p[3];
	irq_enable = p[7];
	irq_counter = *(INT*)&p[8];
}
