//////////////////////////////////////////////////////////////////////////
// Mapper160  PC-Aladdin                                                //
//////////////////////////////////////////////////////////////////////////
void	Mapper160::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	refresh_type = 0;
}

void	Mapper160::Write( WORD addr, BYTE data )
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

		case	0x9000:
			if( data == 0x2B ) {
				refresh_type = 1;
			} else if( data == 0xA8 ) {
				refresh_type = 2;
			} else if( data == 0x1F ) {
				refresh_type = 3;
			} else if( data == 0x7C ) {
				refresh_type = 4;
			} else if( data == 0x18 ) {
				refresh_type = 5;
			} else if( data == 0x60 ) {
				refresh_type = 6;
			} else {
				refresh_type = 0;
			}
			SetVROM_1K_Bank( 0, data );
			break;
		case	0x9001:
			SetVROM_1K_Bank( 1, data );
			break;

		case	0x9002:
			if( refresh_type == 2 && data != 0xE8 ) {
				refresh_type = 0;
			}
			SetVROM_1K_Bank( 2, data );
			break;

		case	0x9003:
			SetVROM_1K_Bank( 3, data );
			break;
		case	0x9004:
			SetVROM_1K_Bank( 4, data );
			break;
		case	0x9005:
			SetVROM_1K_Bank( 5, data );
			break;
		case	0x9006:
			SetVROM_1K_Bank( 6, data );
			break;
		case	0x9007:
			SetVROM_1K_Bank( 7, data );
			break;

		case	0xC000:
			irq_counter = irq_latch;
			irq_enable = irq_latch;
			break;
		case	0xC001:
			irq_latch = data;
			break;
		case	0xC002:
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xC003:
			irq_counter = data;
			break;
	}
}

void	Mapper160::HSync( INT scanline )
{
	if( scanline == 0 || scanline == 239 ) {
		switch( refresh_type ) {
			case	1:
				SetVROM_8K_Bank(0x58,0x59,0x5A,0x5B,0x58,0x59,0x5A,0x5B);
				break;
			case	2:
				SetVROM_8K_Bank(0x78,0x79,0x7A,0x7B,0x78,0x79,0x7A,0x7B);
				break;
			case	3:
				SetVROM_8K_Bank(0x7C,0x7D,0x7E,0x7F,0x7C,0x7D,0x7E,0x7F);
				break;
			case	5:
				SetVROM_8K_Bank(0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77);
				break;
			case	6:
				SetVROM_8K_Bank(0x5C,0x5D,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
				break;
		}
	}
	if( scanline == 64 ) {
		if( refresh_type == 4 ) {
			if( PPU_MEM_BANK[8][32*10+16] == 0x0A ) {
				SetVROM_1K_Bank( 0, 0x68 );
				SetVROM_1K_Bank( 1, 0x69 );
				SetVROM_1K_Bank( 2, 0x6A );
				SetVROM_1K_Bank( 3, 0x6B );
			} else {
				SetVROM_1K_Bank( 0, 0x6C );
				SetVROM_1K_Bank( 1, 0x6D );
				SetVROM_1K_Bank( 2, 0x6E );
				SetVROM_1K_Bank( 3, 0x6F );
			}
		}
	}
	if( scanline == 128 ) {
		if( refresh_type == 4 ) {
			SetVROM_1K_Bank( 0, 0x68 );
			SetVROM_1K_Bank( 1, 0x69 );
			SetVROM_1K_Bank( 2, 0x6A );
			SetVROM_1K_Bank( 3, 0x6B );
		} else if( refresh_type == 5 ) {
			SetVROM_8K_Bank(0x74,0x75,0x76,0x77,0x74,0x75,0x76,0x77);
		}
	}
	if( scanline == 160 ) {
		if( refresh_type == 6 ) {
			SetVROM_8K_Bank(0x60,0x61,0x5E,0x5F,0x7C,0x7D,0x7E,0x7F);
		}
	}

	if( irq_enable ) {
		if( irq_counter == 0xFF ) {
//			nes->cpu->IRQ_NotPending();
			irq_enable  = 0;
			irq_counter = 0;
			nes->cpu->SetIRQ( IRQ_MAPPER );
		} else {
			irq_counter++;
		}
	}
}

void	Mapper160::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
	p[2] = irq_latch;
	p[3] = refresh_type;
}

void	Mapper160::LoadState( LPBYTE p )
{
	irq_enable   = p[0];
	irq_counter  = p[1];
	irq_latch    = p[2];
	refresh_type = p[3];
}
