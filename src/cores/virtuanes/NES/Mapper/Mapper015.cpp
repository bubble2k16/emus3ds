//////////////////////////////////////////////////////////////////////////
// Mapper015  100-in-1 chip                                             //
//////////////////////////////////////////////////////////////////////////
void	Mapper015::Reset()
{
	PRG[0]=0;
	PRG[1]=1;
	PRG[2]=2;
	PRG[3]=3;
	Mirror = 0;
	SetPROM_32K_Bank( 0, 1, 2, 3 );
}

void	Mapper015::Sync (void)
{
	//EMU->SetPRG_RAM8(0x6, 0);
	SetPROM_8K_Bank(4, PRG[0]);
	SetPROM_8K_Bank(5, PRG[1]);
	SetPROM_8K_Bank(6, PRG[2]);
	SetPROM_8K_Bank(7, PRG[3]);

	SetCRAM_8K_Bank(0);
	if (Mirror)
		SetVRAM_Mirror(VRAM_HMIRROR);
	else	SetVRAM_Mirror(VRAM_VMIRROR);
}

void	Mapper015::Write( WORD addr, BYTE data )
{
	uint8 PRGbank = (data & 0x3F) << 1;
	uint8 PRGflip = (data & 0x80) >> 7;
	Mirror = data & 0x40;
	switch( addr ) {
		case	0x8000:
				PRGbank &= 0x7C;
				PRG[0] = PRGbank | 0 ^ PRGflip;
				PRG[1] = PRGbank | 1 ^ PRGflip;
				PRG[2] = PRGbank | 2 ^ PRGflip;
				PRG[3] = PRGbank | 3 ^ PRGflip;
				break;
		case	0x8001:
				PRG[0] = PRGbank | 0 ^ PRGflip;
				PRG[1] = PRGbank | 1 ^ PRGflip;
				PRG[2] = 0x7E | 0 ^ PRGflip;
				PRG[3] = 0x7F | 1 ^ PRGflip;
				break;
		case	0x8002:
				PRG[0] = PRGbank ^ PRGflip;
				PRG[1] = PRGbank ^ PRGflip;
				PRG[2] = PRGbank ^ PRGflip;
				PRG[3] = PRGbank ^ PRGflip;
				break;
		case	0x8003:
				PRG[0] = PRGbank | 0 ^ PRGflip;
				PRG[1] = PRGbank | 1 ^ PRGflip;
				PRG[2] = PRGbank | 0 ^ PRGflip;
				PRG[3] = PRGbank | 1 ^ PRGflip;
				break;
	}
	Sync();
}

