//////////////////////////////////////////////////////////////////////////
// BoardBS5 数独四合一                                                 //
//////////////////////////////////////////////////////////////////////////
void	BoardBS5::Reset()
{
	dip_s++;
	if(dip_s==4) dip_s=0;
	reg_prg[0] = reg_prg[1] = reg_prg[2] = reg_prg[3] = 0x0f;
	reg_chr[0] = reg_chr[1] = reg_chr[2] = reg_chr[3] = 0x00;
	SetVRAM_Mirror( VRAM_VMIRROR );
	SetBank();
}

void	BoardBS5::Write( WORD addr, BYTE data )
{
	int bank = (addr & 0xC00) >> 10;
	switch (addr & 0xF000) {
		case 0x8000:
			reg_chr[bank] = addr & 0x1F;
			break;
		case 0xA000:
			if (addr & (1 << ((dip_s&3)|4)))
				reg_prg[bank] = addr & 0x0F;
			break;
	}
	SetBank();
}

void	BoardBS5::SetBank()
{
	SetPROM_8K_Bank(4, reg_prg[0]);
	SetPROM_8K_Bank(5, reg_prg[1]);
	SetPROM_8K_Bank(6, reg_prg[2]);
	SetPROM_8K_Bank(7, reg_prg[3]);
	SetVROM_2K_Bank(0, reg_chr[0]);
	SetVROM_2K_Bank(2, reg_chr[1]);
	SetVROM_2K_Bank(4, reg_chr[2]);
	SetVROM_2K_Bank(6, reg_chr[3]);
}

void	BoardBS5::SaveState( LPBYTE p )
{
	//
}

void	BoardBS5::LoadState( LPBYTE p )
{
	//
}
