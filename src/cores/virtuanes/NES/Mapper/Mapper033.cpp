//////////////////////////////////////////////////////////////////////////
// Mapper033  Taito TC0190                                              //
//////////////////////////////////////////////////////////////////////////
void	Mapper033::Reset()
{
	patch = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch  = 0;

	reg[0] = 0;
	reg[1] = 2;
	reg[2] = 4;
	reg[3] = 5;
	reg[4] = 6;
	reg[5] = 7;
	reg[6] = 1;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	if( VROM_8K_SIZE ) {
		SetBank();
	}

	DWORD	crc = nes->rom->GetPROM_CRC();
	// Check For Old #33 games.... (CRC code by NesToy)
	if( crc == 0x5e9bc161		// Akira(J)
	 || crc == 0xecdbafa4		// Bakushou!! Jinsei Gekijou(J)
	 || crc == 0x59cd0c31		// Don Doko Don(J)
	 || crc == 0x837c1342		// Golf Ko Open(J)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x07ee6d8f		// Power Blazer(J)
	 || crc == 0x5193fb54		// Takeshi no Sengoku Fuuunji(J)
	 || crc == 0xa71c3452 ) {	// Insector X(J)
		patch = 1;
	}

	nes->SetRenderMethod( NES::PRE_RENDER );

	if( crc == 0x202df297 ) {	// Captain Saver(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
	if( crc == 0x63bb86b5 ) {	// The Jetsons(J)
		nes->SetRenderMethod( NES::TILE_RENDER );
	}
}

void	Mapper033::Write( WORD addr, BYTE data )
{
//	LOG( "Mapper033 addr=%04X data=%02X", addr&0xFFFF, data&0xFF );

	switch( addr ) {
		case	0x8000:
			if( patch ) {
				if( data & 0x40 ) SetVRAM_Mirror( VRAM_HMIRROR );
				else		  SetVRAM_Mirror( VRAM_VMIRROR );
				SetPROM_8K_Bank( 4, data & 0x1F );
			} else {
				SetPROM_8K_Bank( 4, data );
			}
			break;
		case	0x8001:
			if( patch ) {
				SetPROM_8K_Bank( 5, data & 0x1F );
			} else {
				SetPROM_8K_Bank( 5, data );
			}
			break;

		case	0x8002:
			reg[0] = data;
			SetBank();
			break;
		case	0x8003:
			reg[1] = data;
			SetBank();
			break;
		case	0xA000:
			reg[2] = data;
			SetBank();
			break;
		case	0xA001:
			reg[3] = data;
			SetBank();
			break;
		case	0xA002:
			reg[4] = data;
			SetBank();
			break;
		case	0xA003:
			reg[5] = data;
			SetBank();
			break;

#if	0
		case	0xC003:
		case	0xE003:
			reg[6] = data;
			SetBank();
			break;

		case	0xC000:
			irq_counter = data;
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;

		case	0xC001:
		case	0xC002:
		case	0xE001:
		case	0xE002:
			irq_enable = data;
//			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
#else
		case	0xC000:
			irq_latch = data;
			irq_counter = irq_latch;
			break;
		case	0xC001:
			irq_counter = irq_latch;
			break;
		case	0xC002:
			irq_enable = 1;
			break;
		case	0xC003:
			irq_enable = 0;
			break;

		case	0xE001:
		case	0xE002:
		case	0xE003:
			break;
#endif

		case	0xE000:
			if( data & 0x40 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else		  SetVRAM_Mirror( VRAM_VMIRROR );
			break;
	}
}

void	Mapper033::HSync( INT scanline )
{
#if	0
//	nes->cpu->ClrIRQ( IRQ_MAPPER );
	if( scanline >= 0 && scanline <= 239 ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( irq_counter == 0xFF ) {
					irq_enable  = 0;
					irq_counter = 0;
//					nes->cpu->SetIRQ( IRQ_MAPPER );
					nes->cpu->SetIRQ( IRQ_TRIGGER );
				} else {
					irq_counter++;
				}
			}
		}
	}
#else
	if( scanline >= 0 && scanline <= 239 && nes->ppu->IsDispON() ) {
		if( irq_enable ) {
			if( ++irq_counter == 0 ) {
				irq_enable  = 0;
				irq_counter = 0;
				nes->cpu->SetIRQ( IRQ_TRIGGER );
			}
		}
	}
#endif
}

void	Mapper033::SetBank()
{
	SetVROM_2K_Bank( 0, reg[0] );
	SetVROM_2K_Bank( 2, reg[1] );

//	if( reg[6] & 0x01 ) {
		SetVROM_1K_Bank( 4, reg[2] );
		SetVROM_1K_Bank( 5, reg[3] );
		SetVROM_1K_Bank( 6, reg[4] );
		SetVROM_1K_Bank( 7, reg[5] );
//	} else {
//		SetVROM_2K_Bank( 4, reg[0] );
//		SetVROM_2K_Bank( 6, reg[1] );
//	}
}

void	Mapper033::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 7; i++ ) {
		p[i] = reg[i];
	}

	p[7] = irq_enable;
	p[8] = irq_counter;
	p[9] = irq_latch;
}

void	Mapper033::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 7; i++ ) {
		reg[i] = p[i];
	}

	irq_enable  = p[7];
	irq_counter = p[8];
	irq_latch   = p[9];
}
