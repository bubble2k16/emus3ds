//////////////////////////////////////////////////////////////////////////
// Mapper073  Konami VRC3                                               //
//////////////////////////////////////////////////////////////////////////
void	Mapper073::Reset()
{
	irq_enable = 0;
	irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper073::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0xF000:
			SetPROM_16K_Bank( 4, data );
			break;

		case	0x8000:
			irq_counter = (irq_counter & 0xFFF0) | (data & 0x0F);
			break;
		case	0x9000:
			irq_counter = (irq_counter & 0xFF0F) | ((data & 0x0F) << 4);
			break;
		case	0xA000:
			irq_counter = (irq_counter & 0xF0FF) | ((data & 0x0F) << 8);
			break;
		case	0xB000:
			irq_counter = (irq_counter & 0x0FFF) | ((data & 0x0F) << 12);
			break;
		case	0xC000:
			irq_enable = data & 0x02;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xD000:
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper073::Clock( INT cycles )
{
	if( irq_enable ) {
		if( (irq_counter+=cycles) >= 0xFFFF ) {
			irq_enable = 0;
			irq_counter &= 0xFFFF;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper073::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	*(INT*)&p[1] = irq_counter;
}

void	Mapper073::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = *(INT*)&p[1];
}
