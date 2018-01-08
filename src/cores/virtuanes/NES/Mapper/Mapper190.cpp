//////////////////////////////////////////////////////////////////////////
// Mapper190  Nintendo MMC3                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper190::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );

//	DWORD	crc = nes->rom->GetPROM_CRC();
//	if( crc == 0x6F3D187A ) {
//		Temp_Buf=0;	//Kof96
//	} else {
//		Temp_Buf=1;	//ST97
//	}

	irq_enable = 0;
	irq_counter = 0;
	cbase = 0;		/* PowerOn OR RESET : cbase=0 */
	mp190_lcchk = 0;	/* PowerOn OR RESET */
	mp190_lcmd = 1;
}

void	Mapper190::WriteLow( WORD addr, BYTE data )
{
	/* For Initial Copy Protect check (KOF'96) */
	if( addr == 0x5000 ) {
		mp190_lcmd = data;
		switch( data ) {
			case	0xE0:
				SetPROM_32K_Bank( 0 );
				break;
			case	0xEE:
				SetPROM_32K_Bank( 3 );
				break;
		}
	}
	if( (addr==0x5001) && (mp190_lcmd==0x00) ) {
		 SetPROM_32K_Bank( 7 );
	}
	if( addr == 0x5080 ) {
		switch( data ) {
			case 0x1:lowoutdata=0x83;break;
			case 0x2:lowoutdata=0x42;break;
			case 0x3:lowoutdata=0x00;break;
		}
	}
	CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
}

BYTE	Mapper190::ReadLow( WORD addr )
{
	switch( addr ) {
		case	0x5000:
			return	lowoutdata;
		default:
			return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}
}

void	Mapper190::Write( WORD addr, BYTE data )
{
	switch( addr & 0xE003 ) {
		case	0x8000:
			mp190_cmd = data;
			if( mp190_cmd & 0x80 )
				cbase = 1;
			else
				cbase = 0;
			break;
		case	0x8003:	/* for Street Fighter Zero 2 '97 */
			mp190_lcchk = data;
			switch( data ) {
				case	0x28:
					SetPROM_8K_Bank( 4, 0x1F );
					SetPROM_8K_Bank( 5, 0x1F );
					SetPROM_8K_Bank( 6, 0x17 );
					SetPROM_8K_Bank( 7, 0x1F );
					break;
				case	0x2A:
					SetPROM_8K_Bank( 4, 0x1F );
					SetPROM_8K_Bank( 5, 0x0F );
					SetPROM_8K_Bank( 6, 0x17 );
					SetPROM_8K_Bank( 7, 0x1F );
					break;
				case 0x06:
					SetPROM_8K_Bank( 4, 0x1E );
					SetPROM_8K_Bank( 5, 0x1F );
					SetPROM_8K_Bank( 6, 0x1F );
					SetPROM_8K_Bank( 7, 0x1F );
					break;	
			}
			break;
		case	0x8001:
			if( (mp190_lcchk==0x6) || (mp190_lcmd==0x0) ) {
				switch( mp190_cmd & 0x07 ) {
					case	0:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(0,data+0x100);
							SetVROM_1K_Bank(1,data+0x101);
						} else {
							SetVROM_1K_Bank(4,data+0x100);
							SetVROM_1K_Bank(5,data+0x101);
						}
						break;
					case	1:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(2,data+0x100);
							SetVROM_1K_Bank(3,data+0x101);
						} else {
							SetVROM_1K_Bank(6,data+0x100);
							SetVROM_1K_Bank(7,data+0x101);
						}
						break;
					case	2:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(4,data);
						} else {
							SetVROM_1K_Bank(0,data);
						}
						break;
					case	3:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(5,data);
						} else {
							SetVROM_1K_Bank(1,data);
						}
						break;
					case	4:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(6,data);
						} else {
							SetVROM_1K_Bank(2,data);
						}
						break;
					case	5:
						if( cbase == 0 ) {
							SetVROM_1K_Bank(7,data);
						} else {
							SetVROM_1K_Bank(3,data);
						}
						break;
					case	6:
						data=data&((PROM_8K_SIZE*2)-1);
						if( mp190_lcmd & 0x40 ) {
							SetPROM_8K_Bank(6,data);
							SetPROM_8K_Bank(4,(PROM_8K_SIZE-1)*2);
						} else {
							SetPROM_8K_Bank(4,data);
							SetPROM_8K_Bank(6,(PROM_8K_SIZE-1)*2);
						}
						break;
					case	7:
						data=data&((PROM_8K_SIZE*2)-1);
						if( mp190_lcmd & 0x40 ) {
							SetPROM_8K_Bank(5,data);
							SetPROM_8K_Bank(4,(PROM_8K_SIZE-1)*2);
						} else {
							SetPROM_8K_Bank(5,data);
							SetPROM_8K_Bank(6,(PROM_8K_SIZE-1)*2);
						}
						break;
				}
			}
			break;
		case	0xA000:
		        if( (data&0x1) == 0x1 )
				SetVRAM_Mirror( VRAM_HMIRROR );
			else
				SetVRAM_Mirror( VRAM_VMIRROR );
		        break;
		case	0xA001:
			break;
		case	0xC000:
			irq_counter = data-1;
			break;
		case	0xC001:
			irq_latch = data-1;
			break;
		case	0xC002:
			irq_counter = data;
			break;
		case	0xC003:
			irq_latch = data;
			break;
		case	0xE000:
			irq_counter = irq_latch;
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE001:
			irq_enable = 1;
			break;
		case	0xE002:
			irq_counter = irq_latch;
			irq_enable = 0;
			nes->cpu->ClrIRQ( IRQ_MAPPER );
			break;
		case	0xE003:
			irq_enable = 1;
			irq_counter = irq_counter;
			break;
	}
}

void	Mapper190::HSync( INT scanline )
{
	if( (scanline >= 0 && scanline <= 239) ) {
		if( nes->ppu->IsDispON() ) {
			if( irq_enable ) {
				if( !(irq_counter--) ) {
//					nes->cpu->IRQ_NotPending();
					nes->cpu->SetIRQ( IRQ_MAPPER );
				}
			}
		}
	}
}

void	Mapper190::SaveState( LPBYTE p )
{
	p[0] = irq_enable;
	p[1] = irq_counter;
	p[2] = irq_latch;

	p[3] = cbase;
	p[4] = mp190_lcchk;
	p[5] = mp190_lcmd;
	p[6] = mp190_cmd;
	p[7] = lowoutdata;
}

void	Mapper190::LoadState( LPBYTE p )
{
	irq_enable  = p[0];
	irq_counter = p[1];
	irq_latch   = p[2];

	cbase       = p[3];
	mp190_lcchk = p[4];
	mp190_lcmd  = p[5];
	mp190_cmd   = p[6];
	lowoutdata  = p[7];
}
