//////////////////////////////////////////////////////////////////////////
// Mapper067  SunSoft Mapper 3                                          //
//////////////////////////////////////////////////////////////////////////
void	Mapper067::Reset()
{
	irq_enable = 0;
	irq_toggle = 0;
	irq_counter = 0;
	irq_occur = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	SetVROM_4K_Bank( 0, 0 );
	SetVROM_4K_Bank( 4, VROM_4K_SIZE-1 );

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x7f2a04bf ) {	// For Fantasy Zone 2(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}
}

void	Mapper067::Write( WORD addr, BYTE data )
{
	switch( addr & 0xF800 ) {
		case	0x8800:
			SetVROM_2K_Bank( 0, data );
			break;
		case	0x9800:
			SetVROM_2K_Bank( 2, data );
			break;
		case	0xA800:
			SetVROM_2K_Bank( 4, data );
			break;
		case	0xB800:
			SetVROM_2K_Bank( 6, data );
			break;

		case	0xC800:
			if( !irq_toggle ) {
				irq_counter = (irq_counter&0x00FF)|((INT)data<<8);
			} else {
				irq_counter = (irq_counter&0xFF00)|((INT)data&0xFF);
			}
			irq_toggle ^= 1;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xD800:
			irq_enable = data & 0x10;
			irq_toggle = 0;
			irq_occur = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xE800:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0xF800:
			SetPROM_16K_Bank( 4, data );
			break;
	}
}

void	Mapper067::Clock( INT cycles )
{
	if( irq_enable ) {
		if( (irq_counter -= cycles) <= 0 ) {
			irq_enable = 0;
			irq_occur = 0xFF;
			irq_counter = 0xFFFF;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}

//	if( irq_occur ) {
//		nes->cpu->IRQ_NotPending();
//	}
}

void	Mapper067::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_occur;
	p[2] = irq_toggle;
	*((INT*)&p[3]) = irq_counter;
}

void	Mapper067::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_occur   = p[1];
	irq_toggle  = p[2];
	irq_counter = *((INT*)&p[3]);
}
