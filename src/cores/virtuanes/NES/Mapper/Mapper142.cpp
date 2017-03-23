//////////////////////////////////////////////////////////////////////////
// Mapper142  SMB2j Pirate (KS 202)                                     //
//////////////////////////////////////////////////////////////////////////
void	Mapper142::Reset()
{
	prg_sel = 0;
	irq_enable = 0;
	irq_counter = 0;

	SetPROM_8K_Bank( 3, 0 );
	SetPROM_8K_Bank( 7, 0x0F );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper142::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF000 ) {
		case	0x8000:
			irq_counter = (irq_counter & 0xFFF0) | ((data & 0x0F)<< 0);
			break;
		case	0x9000:
			irq_counter = (irq_counter & 0xFF0F) | ((data & 0x0F)<< 4);
			break;
		case	0xA000:
			irq_counter = (irq_counter & 0xF0FF) | ((data & 0x0F)<< 8);
			break;
		case	0xB000:
			irq_counter = (irq_counter & 0x0FFF) | ((data & 0x0F)<<12);
			break;
		case	0xC000:
			irq_enable = data & 0x0F;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE000:
			prg_sel = data & 0x0F;
			break;
		case	0xF000:
			switch( prg_sel ) {
				case	1: SetPROM_8K_Bank( 4, data & 0x0F ); break;
				case	2: SetPROM_8K_Bank( 5, data & 0x0F ); break;
				case	3: SetPROM_8K_Bank( 6, data & 0x0F ); break;
				case	4: SetPROM_8K_Bank( 3, data & 0x0F ); break;
			}
			break;
	}
}

void	Mapper142::HSync( INT scanline )
{
	if( irq_enable ) {
		if( irq_counter > (0xFFFF - 113) ) {
			irq_counter = 0;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		} else {
			irq_counter += 113;
		}
	}
}

void	Mapper142::SaveState( LPBYTE p )
{
	p[0] = prg_sel;
	p[0] = irq_enable;
	*(INT*)&p[2] = irq_counter;
}

void	Mapper142::LoadState( LPBYTE p )
{
	prg_sel = p[0];
	irq_enable  = p[1];
	irq_counter = *(INT*)&p[2];
}
