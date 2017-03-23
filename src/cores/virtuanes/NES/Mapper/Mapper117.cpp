//////////////////////////////////////////////////////////////////////////
// Mapper117 San Ko Gu (TW) :                                           //
//////////////////////////////////////////////////////////////////////////

void    Mapper117::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper117::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0x8001:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x8002:
			SetPROM_8K_Bank( 6, data );
			break;
		case	0xA000:
			SetVROM_1K_Bank( 0, data );
			break;
		case	0xA001:
			SetVROM_1K_Bank( 1, data );
			break;
		case	0xA002:
			SetVROM_1K_Bank( 2, data );
			break;
		case	0xA003:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0xA004:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0xA005:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0xA006:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0xA007:
			SetVROM_1K_Bank( 7, data );
			break;
		case	0xC001:
		case	0xC002:
		case	0xC003:
			irq_counter = data;
			break;
		case	0xE000:
			irq_enable = data & 1;
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}
void	Mapper117::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if(irq_counter==scanline) {
					irq_counter = 0;
//					nes->cpu->IRQ();
//					nes->cpu->SetIRQ( IRQ_MAPPER );
					nes->cpu->SetIRQ( IRQ_TRIGGER );
				}
			}
		}
	}
}

void	Mapper117::SaveState( LPBYTE p )
{
	p[0] = irq_counter;
	p[1] = irq_enable;
}

void	Mapper117::LoadState( LPBYTE p )
{
	irq_counter=p[0];
	irq_enable=p[1];
}
