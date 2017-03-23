//////////////////////////////////////////////////////////////////////////
// Mapper018  Jaleco SS8806                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper018::Reset()
{
	for( INT i = 0; i < 11; i++ ) {
		reg[i] = 0;
	}
	reg[2] = PROM_8K_SIZE-2;
	reg[3] = PROM_8K_SIZE-1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	irq_enable  = 0;
	irq_mode    = 0;
	irq_counter = 0xFFFF;
	irq_latch   = 0xFFFF;

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0xefb1df9e ) {	// The Lord of King(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}
	if( crc == 0x3746f951 ) {	// Pizza Pop!(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}

//	nes->SetRenderMethod( NES::PRE_ALL_RENDER );
//	nes->SetRenderMethod( NES::POST_ALL_RENDER );
}

void	Mapper018::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 4, reg[0] );
			break;
		case	0x8001:
			reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 4, reg[0] );
			break;
		case	0x8002:
			reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 5, reg[1] );
			break;
		case	0x8003:
			reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 5, reg[1] );
			break;
		case	0x9000:
			reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
			SetPROM_8K_Bank( 6, reg[2] );
			break;
		case	0x9001:
			reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetPROM_8K_Bank( 6, reg[2] );
			break;

		case	0xA000:
			reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, reg[3] );
			break;
		case	0xA001:
			reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, reg[3] );
			break;
		case	0xA002:
			reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, reg[4] );
			break;
		case	0xA003:
			reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, reg[4] );
			break;

		case	0xB000:
			reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, reg[5] );
			break;
		case	0xB001:
			reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, reg[5] );
			break;
		case	0xB002:
			reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, reg[6] );
			break;
		case	0xB003:
			reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, reg[6] );
			break;

		case	0xC000:
			reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, reg[7] );
			break;
		case	0xC001:
			reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, reg[7] );
			break;
		case	0xC002:
			reg[8] = (reg[8] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, reg[8] );
			break;
		case	0xC003:
			reg[8] = (reg[8] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, reg[8] );
			break;

		case	0xD000:
			reg[9] = (reg[9] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, reg[9] );
			break;
		case	0xD001:
			reg[9] = (reg[9] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, reg[9] );
			break;
		case	0xD002:
			reg[10] = (reg[10] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, reg[10] );
			break;
		case	0xD003:
			reg[10] = (reg[10] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, reg[10] );
			break;

		case	0xE000:
			irq_latch = (irq_latch & 0xFFF0) | (data & 0x0F);
			break;
		case	0xE001:
			irq_latch = (irq_latch & 0xFF0F) | ((data & 0x0F) << 4);
			break;
		case	0xE002:
			irq_latch = (irq_latch & 0xF0FF) | ((data & 0x0F) << 8);
			break;
		case	0xE003:
			irq_latch = (irq_latch & 0x0FFF) | ((data & 0x0F) << 12);
			break;

		case	0xF000:
//			if( data & 0x01 ) {
				irq_counter = irq_latch;
//			} else {
//				irq_counter = 0;
//			}
			break;
		case	0xF001:
			irq_mode = (data>>1) & 0x07;
			irq_enable = (data & 0x01);
//			if( !irq_enable ) {
				nes->cpu->ClrIRQ( IRQ_MAPPER );
//			}
			break;

		case	0xF002:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_VMIRROR );
			else		     SetVRAM_Mirror( VRAM_MIRROR4L );
			break;
	}
}

void	Mapper018::Clock( INT cycles )
{
BOOL	bIRQ = FALSE;
INT	irq_counter_old = irq_counter;

	if( irq_enable && irq_counter ) {
		irq_counter -= cycles;

		switch( irq_mode ) {
			case	0:
				if( irq_counter <= 0 ) {
					bIRQ = TRUE;
				}
				break;
			case	1:
				if( (irq_counter & 0xF000) != (irq_counter_old & 0xF000) ) {
					bIRQ = TRUE;
				}
				break;
			case	2:
			case	3:
				if( (irq_counter & 0xFF00) != (irq_counter_old & 0xFF00) ) {
					bIRQ = TRUE;
				}
				break;
			case	4:
			case	5:
			case	6:
			case	7:
				if( (irq_counter & 0xFFF0) != (irq_counter_old & 0xFFF0) ) {
					bIRQ = TRUE;
				}
				break;
		}

		if( bIRQ ) {
////			irq_enable = 0;
//			irq_counter = irq_latch;
			irq_counter = 0;
			irq_enable = 0;
//			nes->cpu->IRQ_NotPending();
			nes->cpu->SetIRQ( IRQ_MAPPER );
		}
	}
}

void	Mapper018::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 11; i++ ) {
		p[i] = reg[i];
	}
	p[11] = irq_enable;
	p[12] = irq_mode;
	*(INT*)&p[13] = irq_counter;
	*(INT*)&p[17] = irq_latch;
}

void	Mapper018::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 11; i++ ) {
		p[i] = reg[i];
	}
	irq_enable  = p[11];
	irq_mode    = p[12];
	irq_counter = *(INT*)&p[13];
	irq_latch   = *(INT*)&p[17];
}
