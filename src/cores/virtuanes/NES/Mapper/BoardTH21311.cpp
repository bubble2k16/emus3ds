//////////////////////////////////////////////////////////////////////////
// BoardTH21311                                                         //
//////////////////////////////////////////////////////////////////////////

void	BoardTH21311::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0;
	}

	irq_enable = 0;
	irq_counter = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

}

void	BoardTH21311::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xA000:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0x9000:
			data &= 0x03;
			if( data == 0 )		 SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else				 SetVRAM_Mirror( VRAM_MIRROR4H );
			break;
		case 0xB000:
			reg[0] = (reg[0] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
		case 0xB001:
			reg[0] = (reg[0] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
		case 0xB002:
			reg[1] = (reg[1] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
		case 0xB003:
			reg[1] = (reg[1] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
		case 0xC000:
			reg[2] = (reg[2] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
		case 0xC001:
			reg[2] = (reg[2] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
		case 0xC002:
			reg[3] = (reg[3] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
		case 0xC003:
			reg[3] = (reg[3] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
		case 0xD000:
			reg[4] = (reg[4] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
		case 0xD001:
			reg[4] = (reg[4] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
		case 0xD002:
			reg[5] = (reg[5] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
		case 0xD003:
			reg[5] = (reg[5] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
		case 0xE000:
			reg[6] = (reg[6] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
		case 0xE001:
			reg[6] = (reg[6] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
		case 0xE002:
			reg[7] = (reg[7] & 0xF0) | (data & 0x0F);
			SetVROM_1K_Bank( 7, reg[7] );
			break;
		case 0xE003:
			reg[7] = (reg[7] & 0x0F) | ((data & 0x0F) << 4);
			SetVROM_1K_Bank( 7, reg[7] );
			break;

		case 0xF000:
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case 0xF001:
			irq_enable = 1;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case 0xF002:
			irq_counter = (irq_counter & 0xFF00) | data;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case 0xF003:
			irq_counter = (irq_counter & 0x00FF) | (data << 8);
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}
/*
void	BoardTH21311::Clock( INT cycles )
{
	if( irq_enable ) {
		irq_counter -= cycles;
		if( irq_counter <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}
*/
void	BoardTH21311::HSync( INT scanline )
{
	if( irq_enable ) {
		irq_counter -= 123;
		if( irq_counter <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
			irq_enable = 0;
			irq_counter = 0xFFFF;
		}
	}
}

void	BoardTH21311::SaveState( LPBYTE p )
{
	//
}

void	BoardTH21311::LoadState( LPBYTE p )
{
	//
}
