//////////////////////////////////////////////////////////////////////////
// Mapper052  Konami VRC2 type B                                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper052::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = i;
	}
	reg[8] = 0;

	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_clock = 0;

	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );

	nes->SetRenderMethod( NES::PRE_ALL_RENDER );
//	nes->SetRenderMethod( NES::POST_RENDER );
//	nes->SetRenderMethod( NES::POST_ALL_RENDER );

}

void	Mapper052::Write( WORD addr, BYTE data )
{
	if(addr>=0xf000) DEBUGOUT( "MPRWR A=%04X   D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	if(addr>=0xf000) DEBUGOUT( "MPRWR A=%04X RAM=%02X L=%3d CYC=%d\n", addr&0xFFFF, RAM[0x1c0]&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );
	switch( addr & 0xFFFF ) {
		case 0x8000:
			if(reg[8])	SetPROM_8K_Bank( 6, data );
			else		SetPROM_8K_Bank( 4, data );
			break;
		case 0x9002:
			reg[8] = data & 0x02;
			break;

		case 0x9004:
			data &= 0x03;
			if(data==0)		 SetVRAM_Mirror( VRAM_VMIRROR );
			else if(data==1) SetVRAM_Mirror( VRAM_HMIRROR );
			else if(data==2) SetVRAM_Mirror( VRAM_MIRROR4L);
			else			 SetVRAM_Mirror( VRAM_MIRROR4H);
			break;

		case 0xA000:
			SetPROM_8K_Bank( 5, data );
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
			break;
		case 0xF004:
		case 0xFF04:
			RAM[0x7f8] = 1;
			break;
		case 0xF008:
		case 0xFF08:
			irq_enable = 1;
//			irq_latch = ((RAM[0x7f8]*2)+0x11)^0xFF;	//Akumajou Special - Boku Dracula-kun
			irq_latch = ((RAM[0x1c0]*2)+0x11)^0xFF;	//Teenage Mutant Ninja Turtles
			irq_counter = irq_latch;
			irq_clock = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case 0xF00C:
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
	}
}

void	Mapper052::HSync( INT scanline )
{
	//
}

void	Mapper052::Clock( INT cycles )
{
	if( irq_enable ) {
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

void	Mapper052::SaveState( LPBYTE p )
{
	//
}
void	Mapper052::LoadState( LPBYTE p )
{
	//
}
