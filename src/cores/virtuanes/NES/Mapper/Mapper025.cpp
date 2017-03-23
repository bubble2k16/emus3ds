//////////////////////////////////////////////////////////////////////////
// Mapper025  Konami VRC4(Normal)                                       //
//////////////////////////////////////////////////////////////////////////
void	Mapper025::Reset()
{
	for( INT i = 0; i < 11; i++ ) {
		reg[i] = 0;
	}
	reg[9] = PROM_8K_SIZE-2;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_occur = 0;
	irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_1K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	}

	DWORD	crc = nes->rom->GetPROM_CRC();
	if( crc == 0xc71d4ce7 ) {	// Gradius II(J)
//		nes->SetRenderMethod( NES::POST_RENDER );
	}
	if( crc == 0xa2e68da8 ) {	// For Racer Mini Yonku - Japan Cup(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0xea74c587 ) {	// For Teenage Mutant Ninja Turtles(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x5f82cb7d ) {	// For Teenage Mutant Ninja Turtles 2(J)
	}
	if( crc == 0x0bbd85ff ) {	// For Bio Miracle Bokutte Upa(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
	}
}

void	Mapper025::Write( WORD addr, BYTE data )
{
//if( addr >= 0xF000 )
//DEBUGOUT( "M25 WR $%04X=$%02X L=%3d\n", addr, data, nes->GetScanline() );

	switch( addr & 0xF000 ) {
		case	0x8000:
			if(reg[10] & 0x02) {
				reg[9] = data;
				SetPROM_8K_Bank( 6, data );
			} else {
				reg[8] = data;
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
	}

	switch( addr & 0xF00F ) {
		case	0x9000:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case 0x9001:
		case 0x9004:
			if((reg[10] & 0x02) != (data & 0x02)) {
				BYTE	swap = reg[8];
				reg[8] = reg[9];
				reg[9] = swap;

				SetPROM_8K_Bank( 4, reg[8] );
				SetPROM_8K_Bank( 6, reg[9] );
			}
			reg[10] = data;
			break;

		case 0xB000:
			reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
		case 0xB002:
		case 0xB008:
			reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, reg[0] );
			break;

		case 0xB001:
		case 0xB004:
			reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
		case 0xB003:
		case 0xB00C:
			reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, reg[1] );
			break;

		case 0xC000:
			reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
		case 0xC002:
		case 0xC008:
			reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, reg[2] );
			break;

		case 0xC001:
		case 0xC004:
			reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
		case 0xC003:
		case 0xC00C:
			reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, reg[3] );
			break;

		case 0xD000:
			reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
		case 0xD002:
		case 0xD008:
			reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, reg[4] );
			break;

		case 0xD001:
		case 0xD004:
			reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
		case 0xD003:
		case 0xD00C:
			reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, reg[5] );
			break;

		case 0xE000:
			reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
		case 0xE002:
		case 0xE008:
			reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, reg[6] );
			break;

		case 0xE001:
		case 0xE004:
			reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, reg[7] );
			break;
		case 0xE003:
		case 0xE00C:
			reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, reg[7] );
			break;

		case 0xF000:
			irq_latch = (irq_latch & 0xF0) | (data & 0x0F);
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case 0xF002:
		case 0xF008:
			irq_latch = (irq_latch & 0x0F) | ((data & 0x0F) << 4);
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case 0xF001:
		case 0xF004:
			irq_enable = data & 0x03;
//			irq_counter = 0x100 - irq_latch;
			irq_counter = irq_latch;
			irq_clock = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case 0xF003:
		case 0xF00C:
			irq_enable = (irq_enable & 0x01)*3;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper025::Clock( INT cycles )
{
	if( irq_enable & 0x02 ) {
		irq_clock += cycles*3;
		while( irq_clock >= 341 ) {
			irq_clock -= 341;
			irq_counter++;
			if( irq_counter == 0 ) {
				irq_counter = irq_latch;
				nes->cpu->SetIRQ( IRQ_MAPPER );
			}
		}
	}
}

void	Mapper025::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 11; i++ ) {
		p[i] = reg[i];
	}
	p[11] = irq_enable;
	p[12] = irq_occur;
	p[13] = irq_latch;
	p[14] = irq_counter;
	*((INT*)&p[15]) = irq_clock;
}

void	Mapper025::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 11; i++ ) {
		reg[i] = p[i];
	}
	irq_enable  = p[11];
	irq_occur   = p[12];
	irq_latch   = p[13];
	irq_counter = p[14];
	irq_clock   = *((INT*)&p[15]);
}
