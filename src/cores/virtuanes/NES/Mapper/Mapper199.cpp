//////////////////////////////////////////////////////////////////////////
// Mapper199  WaiXingTypeG Base ON Nintendo MMC3                        //
//////////////////////////////////////////////////////////////////////////
void	Mapper199::Reset()
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = 0x00;
		chr[i] = i;
	}
	prg[0] = 0x00;
	prg[1] = 0x01;
	prg[2] = 0x3e;
	prg[3] = 0x3f;
	SetBank_CPU();
	SetBank_PPU();

	we_sram  = 0;	
	irq_enable=irq_counter=irq_latch=irq_request = 0;

	
	DWORD	crcP = nes->rom->GetPROM_CRC();	
	DWORD	crcV = nes->rom->GetVROM_CRC();

	if( (crcP==0xE80D8741)||(crcV==0x3846520D))
	{//外星霸王的大陆
		nes->SetRenderMethod( NES::POST_ALL_RENDER );
	}
}


BYTE	Mapper199::ReadLow( WORD addr )
{
	if( addr >= 0x5000 && addr <= 0x5FFF ) {
		return	XRAM[addr-0x4000];
	}else{
		return	Mapper::ReadLow( addr );
	}
}

void	Mapper199::WriteLow( WORD addr, BYTE data )
{
	if( addr >= 0x5000 && addr <= 0x5FFF ) {
		XRAM[addr-0x4000] = data;
	} else {
		Mapper::WriteLow( addr, data );
	}
}

void	Mapper199::Write( WORD addr, BYTE data )
{
//DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

	switch( addr & 0xE001 ) {
		case	0x8000:
			reg[0] = data;
			SetBank_CPU();
			SetBank_PPU();
			break;
		case	0x8001:
			reg[1] = data;

			switch( reg[0] & 0x0f ) {
				case	0x00:	chr[0] = data ;			SetBank_PPU();				break;
				case	0x01:	chr[2] = data ;			SetBank_PPU();				break;
				case	0x02:
				case	0x03:
				case	0x04:
				case	0x05:	chr[(reg[0] & 0x07)+2] = data;	SetBank_PPU();		break;
				case	0x06:
				case	0x07:
				case	0x08:
				case	0x09:	prg[(reg[0] & 0x0f)-6] = data;	SetBank_CPU();		break;
				case 0xA:		chr[1] = data;				SetBank_PPU();			break;
				case 0xB:		chr[3] = data;				SetBank_PPU();			break;
			}
			break;
		case	0xA000:
			reg[2] = data;
			//if( !nes->rom->Is4SCREEN() ) 
			{
				if(data==0) SetVRAM_Mirror(VRAM_VMIRROR);
				else if(data==1) SetVRAM_Mirror(VRAM_HMIRROR);
				else if(data==2) SetVRAM_Mirror(VRAM_MIRROR4L);
				else SetVRAM_Mirror(VRAM_MIRROR4H);
			}
			break;
		case	0xA001:
			reg[3] = data;
			break;
		case	0xC000:
			reg[4] = data;
			irq_counter = data;
			irq_request = 0;
			break;
		case	0xC001:
			reg[5] = data;
			irq_latch = data;
			irq_request = 0;
			break;
		case	0xE000:
			reg[6] = data;
			irq_enable = 0;
			irq_request = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			reg[7] = data;
			irq_enable = 1;
			irq_request = 0;
			break;
	}	
	
}

void	Mapper199::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) 
	{
		if( nes->ppu->IsDispON() ) 
		{
			if( irq_enable && !irq_request ) 
			{
				if( scanline == 0 ) 
				{
					if( irq_counter )
					{
						irq_counter--;
					}
				}
				if( !(irq_counter--) ) 
				{
					irq_request = 0xFF;
					irq_counter = irq_latch;
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper199::SetBank_CPU()
{
	SetPROM_8K_Bank(4,prg[0 ^(reg[0]>>5&~(0<<1)&2)]);
	SetPROM_8K_Bank(5,prg[1 ^(reg[0]>>5&~(1<<1)&2)]);
	SetPROM_8K_Bank(6,prg[2 ^(reg[0]>>5&~(2<<1)&2)]);
	SetPROM_8K_Bank(7,prg[3 ^(reg[0]>>5&~(3<<1)&2)]);
}

void	Mapper199::SetBank_PPU()
{
	unsigned int bank = (reg[0]&0x80)>>5;
	for(int x=0; x<8; x++)
	{
		if( chr[x]<=7 )
		{
			SetCRAM_1K_Bank( x^bank, chr[x] );
		}else{
			SetVROM_1K_Bank( x^bank, chr[x] );
		}
	}	
}

void	Mapper199::SaveState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		p[i]   = reg[i];
		p[10+i]  = chr[i];
	}

	p[ 8] = prg[0];
	p[ 9] = prg[1];	
	p[18] = irq_enable;
	p[19] = irq_counter;
	p[20] = irq_latch;
	p[21] = irq_request;
	p[22] = prg[2];
	p[23] = prg[3];	
}

void	Mapper199::LoadState( LPBYTE p )
{
	for( INT i = 0; i < 8; i++ ) {
		reg[i] = p[i];
		chr[i] = p[10+i];
	}
	prg[0]  = p[ 8];
	prg[1]  = p[ 9];
	irq_enable  = p[18];
	irq_counter = p[19];
	irq_latch   = p[20];
	irq_request = p[21];
	prg[2]  = p[ 22];
	prg[3]  = p[ 23];
}
