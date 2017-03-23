//////////////////////////////////////////////////////////////////////////
// Mapper040  SMB2J                                                     //
//////////////////////////////////////////////////////////////////////////
void	Mapper040::Reset()
{
	irq_enable = 0;
	irq_line = 0;

	SetPROM_8K_Bank( 3, 6 );
	SetPROM_32K_Bank( 4, 5, 0, 7 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}
}

void	Mapper040::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE000 ) {
		case	0x8000:
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xA000:
			irq_enable = 0xFF;
			irq_line = 37;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xC000:
			break;
		case	0xE000:
			SetPROM_8K_Bank( 6, data&0x07 );
			break;
	}
}

void	Mapper040::HSync( INT scanline )
{
	if( irq_enable ) {
		if( --irq_line <= 0 ) {
//			nes->cpu->IRQ();
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper040::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	*(INT*)&p[1] = irq_line;
}

void	Mapper040::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_line = *(INT*)&p[1];
}
