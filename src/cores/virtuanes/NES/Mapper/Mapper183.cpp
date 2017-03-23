//////////////////////////////////////////////////////////////////////////
// Mapper183  Gimmick (Bootleg)                                         //
//////////////////////////////////////////////////////////////////////////
void	Mapper183::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

	for( INT i = 0; i < 8; i++ ) {
		reg[i] = i;
	}
	irq_enable = 0;
	irq_counter = 0;
}

void	Mapper183::Write( WORD addr, BYTE data )
{
	switch( addr ) {
		case	0x8800:
			SetPROM_8K_Bank( 4, data );
			break;
		case	0xA800:
			SetPROM_8K_Bank( 5, data );
			break;
		case	0xA000:
			SetPROM_8K_Bank( 6, data );
			break;

		case	0xB000:
			reg[0] = (reg[0]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
		case	0xB004:
			reg[0] = (reg[0]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 0, reg[0] );
			break;
		case	0xB008:
			reg[1] = (reg[1]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 1, reg[1] );
			break;
		case	0xB00C:
			reg[1] = (reg[1]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 1, reg[1] );
			break;

		case	0xC000:
			reg[2] = (reg[2]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
		case	0xC004:
			reg[2] = (reg[2]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 2, reg[2] );
			break;
		case	0xC008:
			reg[3] = (reg[3]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 3, reg[3] );
			break;
		case	0xC00C:
			reg[3] = (reg[3]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 3, reg[3] );
			break;

		case	0xD000:
			reg[4] = (reg[4]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
		case	0xD004:
			reg[4] = (reg[4]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 4, reg[4] );
			break;
		case	0xD008:
			reg[5] = (reg[5]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 5, reg[5] );
			break;
		case	0xD00C:
			reg[5] = (reg[5]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 5, reg[5] );
			break;

		case	0xE000:
			reg[6] = (reg[6]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
		case	0xE004:
			reg[6] = (reg[6]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 6, reg[6] );
			break;
		case	0xE008:
			reg[7] = (reg[3]&0xF0)|(data&0x0F);
			SetVROM_1K_Bank( 7, reg[7] );
			break;
		case	0xE00C:
			reg[7] = (reg[3]&0x0F)|((data&0x0F)<<4);
			SetVROM_1K_Bank( 7, reg[7] );
			break;

		case	0x9008:
			if( data == 1 ) {
				for( INT i = 0; i < 8; i++ ) {
					reg[i] = i;
				}
				SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
				SetVROM_8K_Bank( 0 );
			}
			break;

		case	0x9800:
			if( data == 0 )      SetVRAM_Mirror( VRAM_VMIRROR );
			else if( data == 1 ) SetVRAM_Mirror( VRAM_HMIRROR );
			else if( data == 2 ) SetVRAM_Mirror( VRAM_MIRROR4L );
			else if( data == 3 ) SetVRAM_Mirror( VRAM_MIRROR4H );
			break;

		case	0xF000:
			irq_counter = (irq_counter&0xFF00)|data;
			break;
		case	0xF004:
			irq_counter = (irq_counter&0x00FF)|(data<<8);
			break;
		case	0xF008:
			irq_enable = data;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper183::HSync( INT scanline )
{
	if( irq_enable & 0x02 ) {
		if( irq_counter <= 113 ) {
			irq_counter = 0;
//			nes->cpu->IRQ_NotPending();
			nes->cpu->SetIRQ( IRQ_MAPPER );
		} else {
			irq_counter -= 113;
		}
	}
}

void	Mapper183::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i] = reg[i];
	}
	p[ 8] = irq_enable;
	*((INT*)&p[ 9]) = irq_counter;
}

void	Mapper183::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
	}
	irq_enable  = p[8];
	irq_counter = *((INT*)&p[ 9]);
}
