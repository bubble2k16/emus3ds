//////////////////////////////////////////////////////////////////////////
// BoardDragonFighter                                                   //
//////////////////////////////////////////////////////////////////////////

void	BoardDragonFighter::Reset()
{
	chr0 = chr1 = 0;
	reg[0] = reg[1] = 0;
	irq_enable = 0;
	irq_counter = 0;
	irq_latch = 0;
	irq_request = 0;
	SetPROM_32K_Bank( 0, PROM_8K_SIZE-1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
	SetVROM_8K_Bank( 0 );
}

BYTE	BoardDragonFighter::ReadLow( WORD addr )
{
	switch( addr ) {
		case	0x6000:
			SetVROM_4K_Bank( 4, RAM[0xFF]&0x3F );
			break;
		case	0x6002:
			SetVROM_4K_Bank( 4, RAM[0xFF]&0x3F );
			if(chr0){
				if(RAM[0x4F]==RAM[0x6A])
					SetVROM_2K_Bank( 0, ((reg[1]>>1)^RAM[0x4F]) );
				else
					SetVROM_2K_Bank( 0, ((reg[1]>>1)^RAM[0x4F]) + 0x80 );
			}
			chr0 = 0;
			break;
		case	0x6004:
			if(chr1)
				SetVROM_2K_Bank( 2, (reg[1]>>1) + (RAM[0xFF]&0x40)*2 );
			chr1 = 0;
			break;
	}

	if(addr>=0x6000) return CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	return	Mapper::ReadLow( addr );
}

void	BoardDragonFighter::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			break;
		case	0x8001:
			reg[1] = data;
			switch( reg[0] & 0x07 ) {
				case	0x00:
					chr0 = 1;
					if((addr&0xF000)==0x9000){
						SetVROM_2K_Bank( 0, reg[1]>>1 );
						chr0 = 0;
					}
					break;
				case	0x01:
					chr1 = 1;
					break;
				case	0x06:
					SetPROM_8K_Bank( 4, reg[1] );
					break;
				case	0x07:
					SetPROM_8K_Bank( 5, reg[1] );
					break;
			}
			break;
		case	0xA000:
			if(data&0x01)	SetVRAM_Mirror( VRAM_HMIRROR );
			else			SetVRAM_Mirror( VRAM_VMIRROR );
			break;
		case	0xC000:
			irq_counter = data;
			irq_request = 0;
			break;
		case	0xC001:
			irq_latch = data;
			irq_request = 0;
			break;
		case	0xE000:
			irq_enable = 0;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			irq_enable = 1;
			irq_request = 0;
			break;
	}
}

void	BoardDragonFighter::HSync(int scanline)
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable && !irq_request ) {
				if( scanline == 0 ) {
					if( irq_counter ) {
						irq_counter -= 1;
					}
				}
				if(!(irq_counter)){
					irq_request = 0xFF;
					irq_counter = irq_latch;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
				irq_counter--;
			}
		}
	}
}
