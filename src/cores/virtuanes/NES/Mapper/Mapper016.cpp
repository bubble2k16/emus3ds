//////////////////////////////////////////////////////////////////////////
// Mapper016  Bandai Standard                                           //
//////////////////////////////////////////////////////////////////////////
void	Mapper016::Reset()
{
	patch = 0;

	reg[0] = reg[1] = reg[2] = 0;
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;

	irq_type = 0;
	nes->SetIrqType( NES::IRQ_CLOCK );

	eeprom_type = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0x3f15d20d		// Famicom Jump 2(J)
	 || crc == 0xf76aa523 ) {	// Famicom Jump 2(J)(alt)
		patch = 1;
		eeprom_type = 0xFF;

		WRAM[0x0BBC] = 0xFF;	// SRAM‘Îô
	}

	if( crc == 0x1d6f27f7 ) {	// Dragon Ball Z 2(Korean Hack)
		nes->SetIrqType( NES::IRQ_HSYNC );
		eeprom_type = 1;
	}
	if( crc == 0x6f7247c8 ) {	// Dragon Ball Z 3(Korean Hack)
		nes->SetIrqType( NES::IRQ_CLOCK );
		eeprom_type = 1;
	}

	if( crc == 0x7fb799fd ) {	// Dragon Ball 2 - Dai Maou Fukkatsu(J)
	}
	if( crc == 0x6c6c2feb		// Dragon Ball 3 - Gokuu Den(J)
	 || crc == 0x8edeb257 ) {	// Dragon Ball 3 - Gokuu Den(J)(Alt)
	}
	if( crc == 0x31cd9903 ) {	// Dragon Ball Z - Kyoushuu! Saiya Jin(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
	}
	if( crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!!(J)
	 || crc == 0x1582fee0 ) {	// Dragon Ball Z 2 - Gekishin Freeza!!(J) [alt]
		nes->SetIrqType( NES::IRQ_HSYNC );
		eeprom_type = 1;
	}
	if( crc == 0x09499f4d ) {	// Dragon Ball Z 3 - Ressen Jinzou Ningen(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
		eeprom_type = 1;
	}
	if( crc == 0x2e991109 ) {	// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
		nes->SetIrqType( NES::IRQ_HSYNC );
		eeprom_type = 1;
	}
	if( crc == 0x146fb9c3 ) {	// SD Gundam Gaiden - Knight Gundam Monogatari(J)
	}

	if( crc == 0x73ac76db		// SD Gundam Gaiden - Knight Gundam Monogatari 2 - Hikari no Kishi(J)
	 || crc == 0x81a15eb8 ) {	// SD Gundam Gaiden - Knight Gundam Monogatari 3 - Densetsu no Kishi Dan(J)
		eeprom_type = 1;
	}
	if( crc == 0x170250de ) {	// Rokudenashi Blues(J)
		nes->SetRenderMethod( NES::PRE_ALL_RENDER );
		eeprom_type = 1;
	}

	// DATACHŒn
	if( crc == 0x0be0a328 		// Datach - SD Gundam - Gundam Wars(J)
	 || crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai(J)
	 || crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight!(J)
	 || crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi(J)
	 || crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament(J)
	 || crc == 0xbe06853f ) {	// Datach - J League Super Top Players(J)
		eeprom_type = 2;
	}
	if( crc == 0xf51a7f46 ) {	// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai(J)
		nes->SetIrqType( NES::IRQ_HSYNC );
		eeprom_type = 2;
	}

	if( eeprom_type == 0 ) {
		nes->SetSAVERAM_SIZE( 128 );
		x24c01.Reset( WRAM );
	} else 
	if( eeprom_type == 1 ) {
		nes->SetSAVERAM_SIZE( 256 );
		x24c02.Reset( WRAM );
	} else 
	if( eeprom_type == 2 ) {
		nes->SetSAVERAM_SIZE( 384 );
		x24c02.Reset( WRAM );
		x24c01.Reset( WRAM+256 );
	}
}

BYTE	Mapper016::ReadLow( WORD addr )
{
	if( patch ) {
		return	Mapper::ReadLow( addr );
	} else {
		if( (addr & 0x00FF) == 0x0000 ) {
			BYTE	ret = 0;
			if( eeprom_type == 0 ) {
				ret = x24c01.Read();
			} else
			if( eeprom_type == 1 ) {
				ret = x24c02.Read();
			} else
			if( eeprom_type == 2 ) {
				ret = x24c02.Read() & x24c01.Read();
			}
			return	(ret?0x10:0)|(nes->GetBarcodeStatus());
		}
	}
	return	0x00;
}

void	Mapper016::WriteLow( WORD addr, BYTE data )
{
	if( !patch ) {
		WriteSubA( addr, data );
	} else {
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper016::Write( WORD addr, BYTE data )
{
	if( !patch ) {
		WriteSubA( addr, data );
	} else {
		WriteSubB( addr, data );
	}
}

static	BYTE	eeprom_addinc;

// Normal mapper #16
void	Mapper016::WriteSubA( WORD addr, BYTE data )
{
	switch( addr & 0x000F ) {
		case	0x0000:
		case	0x0001:
		case	0x0002:
		case	0x0003:
		case	0x0004:
		case	0x0005:
		case	0x0006:
		case	0x0007:
			if( VROM_1K_SIZE ) {
				SetVROM_1K_Bank( addr&0x0007, data );
			}
			if( eeprom_type == 2 ) {
				reg[0] = data;
				x24c01.Write( (data&0x08)?0xFF:0, (reg[1]&0x40)?0xFF:0 );
			}
			break;

		case	0x0008:
			SetPROM_16K_Bank( 4, data );
			break;

		case	0x0009:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0x000A:
			irq_enable = data & 0x01;
			irq_counter = irq_latch;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x000B:
			irq_latch = (irq_latch & 0xFF00) | data;
			irq_counter = (irq_counter & 0xFF00) | data;
			break;
		case	0x000C:
			irq_latch = ((INT)data << 8) | (irq_latch & 0x00FF);
			irq_counter = ((INT)data << 8) | (irq_counter & 0x00FF);
			break;

		case	0x000D:
			// EEPTYPE0(DragonBallZ)
			if( eeprom_type == 0 ) {
				x24c01.Write( (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			// EEPTYPE1(DragonBallZ2,Z3,Z Gaiden)
			if( eeprom_type == 1 ) {
				x24c02.Write( (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			// EEPTYPE2(DATACH)
			if( eeprom_type == 2 ) {
				reg[1] = data;
				x24c02.Write( (data&0x20)?0xFF:0, (data&0x40)?0xFF:0 );
				x24c01.Write( (reg[0]&0x08)?0xFF:0, (data&0x40)?0xFF:0 );
			}
			break;
	}
}

// Famicom Jump 2
void	Mapper016::WriteSubB( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8000:
		case	0x8001:
		case	0x8002:
		case	0x8003:
			reg[0] = data & 0x01;
			SetPROM_8K_Bank( 4, reg[0]*0x20+reg[2]*2+0 );
			SetPROM_8K_Bank( 5, reg[0]*0x20+reg[2]*2+1 );
			break;
		case	0x8004:
		case	0x8005:
		case	0x8006:
		case	0x8007:
			reg[1] = data & 0x01;
			SetPROM_8K_Bank( 6, reg[1]*0x20+0x1E );
			SetPROM_8K_Bank( 7, reg[1]*0x20+0x1F );
			break;
		case	0x8008:
			reg[2] = data;
			SetPROM_8K_Bank( 4, reg[0]*0x20+reg[2]*2+0 );
			SetPROM_8K_Bank( 5, reg[0]*0x20+reg[2]*2+1 );
			SetPROM_8K_Bank( 6, reg[1]*0x20+0x1E );
			SetPROM_8K_Bank( 7, reg[1]*0x20+0x1F );
			break;

		case	0x8009:
			data &= 0x03;
			if( data == 0 )	     SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else		     SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0x800A:
			irq_enable = data & 0x01;
			irq_counter = irq_latch;

//			if( !irq_enable ) {
//				nes->cpu->ClrIRQ( IRQ_MAPPER );
//			}
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0x800B:
			irq_latch = (irq_latch & 0xFF00) | data;
			break;
		case	0x800C:
			irq_latch = ((INT)data << 8) | (irq_latch & 0x00FF);
			break;

		case	0x800D:
			break;
	}
}

void	Mapper016::HSync( INT scanline )
{
	if( irq_enable && (nes->GetIrqType() == NES::IRQ_HSYNC) ) {
		if( irq_counter <= 113 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
//			nes->cpu->IRQ();
////			nes->cpu->IRQ_NotPending();
//			irq_enable = 0;
//			irq_counter = 0;
			irq_counter &= 0xFFFF;
		} else {
			irq_counter -= 113;
		}
	}
}

void	Mapper016::Clock( INT cycles )
{
	if( irq_enable && (nes->GetIrqType() == NES::IRQ_CLOCK) ) {
		if( (irq_counter-=cycles) <= 0 ) {
			nes->cpu->SetIRQ( IRQ_MAPPER );
//			nes->cpu->IRQ();
////			nes->cpu->IRQ_NotPending();
//			irq_enable = 0;
//			irq_counter = 0;
			irq_counter &= 0xFFFF;
		}
	}
}

void	Mapper016::SaveState( LPBYTE p )
{
	p[0] = reg[0];
	p[1] = reg[1];
	p[2] = reg[2];
	p[3] = irq_enable;
	*(INT*)&p[4] = irq_counter;
	*(INT*)&p[8] = irq_latch;

	if( eeprom_type == 0 ) {
		x24c01.Save( &p[16] );
	} else
	if( eeprom_type == 1 ) {
		x24c02.Save( &p[16] );
	} else
	if( eeprom_type == 2 ) {
		x24c02.Save( &p[16] );
		x24c01.Save( &p[48] );
	}
}

void	Mapper016::LoadState( LPBYTE p )
{
	reg[0] = p[0];
	reg[1] = p[1];
	reg[2] = p[2];
	irq_enable  = p[3];
	irq_counter = *(INT*)&p[4];
	irq_latch   = *(INT*)&p[8];
	if( eeprom_type == 0 ) {
		x24c01.Load( &p[16] );
	} else
	if( eeprom_type == 1 ) {
		x24c02.Load( &p[16] );
	} else
	if( eeprom_type == 2 ) {
		x24c02.Load( &p[16] );
		x24c01.Load( &p[48] );
	}
}
