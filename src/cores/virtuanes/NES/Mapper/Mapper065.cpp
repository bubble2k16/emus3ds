//////////////////////////////////////////////////////////////////////////
// Mapper065  Irem H3001                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper065::Reset()
{
	patch = 0;

	// Kaiketsu Yanchamaru 3(J)
	if( nes->rom->GetPROM_CRC() == 0xe30b7f64 ) {
		patch = 1;
	}

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	irq_enable = 0;
	irq_counter = 0;
}

void	Mapper065::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;

		case	0x9000:
			if( !patch ) {
				if( data & 0x40 ) SetVRAM_Mirror( VRAM_VMIRROR );
				else		  SetVRAM_Mirror( VRAM_HMIRROR );
			}
			break;

		case	0x9001:
			if( patch ) {
				if( data & 0x80 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
			}
			break;

		case	0x9003:
			if( !patch ) {
				irq_enable = data & 0x80;
				nes->cpu->ClrIRQ( IRQ_MAPPER );
			}
			break;
		case	0x9004:
			if( !patch ) {
				irq_counter = irq_latch;
			}
			break;
		case	0x9005:
			if( patch ) {
				irq_counter = (BYTE)(data<<1);
				irq_enable = data;
				nes->cpu->ClrIRQ( IRQ_MAPPER );
			} else {
				irq_latch = (irq_latch & 0x00FF)|((INT)data<<8);
			}
			break;

		case	0x9006:
			if( patch ) {
				irq_enable = 1;
			} else {
				irq_latch = (irq_latch & 0xFF00)|data;
			}
			break;

		case	0xB000:
		case	0xB001:
		case	0xB002:
		case	0xB003:
		case	0xB004:
		case	0xB005:
		case	0xB006:
		case	0xB007:
			SetVROM_1K_Bank( addr & 0x0007, data );
			break;

		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xC000:
			SetPROM_8K_Bank( 6, data );
			break;
	}
}

void	Mapper065::HSync( INT scanline )
{
	if( patch ) {
		if( irq_enable ) {
			if( irq_counter == 0 ) {
//				nes->cpu->IRQ_NotPending();
				nes->cpu->SetIRQ( IRQ_MAPPER );
			} else {
				irq_counter--;
			}
		}
	}
}

void	Mapper065::Clock( INT cycles )
{
	if( !patch ) {
		if( irq_enable ) {
			if( irq_counter <= 0 ) {
//				nes->cpu->IRQ_NotPending();
				nes->cpu->SetIRQ( IRQ_MAPPER );
			} else {
				irq_counter -= cycles;
			}
		}
	}
}

void	Mapper065::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	*(INT*)&p[1] = irq_counter;
	*(INT*)&p[5] = irq_latch;
}

void	Mapper065::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = *(INT*)&p[1];
	irq_latch   = *(INT*)&p[5];
}
