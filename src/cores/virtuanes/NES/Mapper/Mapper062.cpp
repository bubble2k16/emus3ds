//////////////////////////////////////////////////////////////////////////
// Mapper062                                                            //
//////////////////////////////////////////////////////////////////////////
void	Mapper062::Reset()
{
	SetPROM_32K_Bank( 0 );
	SetVROM_8K_Bank( 0 );
}

void	Mapper062::Write( WORD A, BYTE V )
{	
	//nintendulator
	/*
	union
	{
		struct
		{
			unsigned CHRhi   : 5;
			unsigned PRGsize : 1;
			unsigned PRGchip : 1;
			unsigned Mir_HV  : 1;
			unsigned PRG     : 6;
			unsigned         : 2;
			unsigned CHRlo   : 2;
			unsigned         : 6;
		};
		struct
		{
			unsigned addr    :16;
			unsigned data    : 8;
		};
	};
	addr = A;
	data = V;

	SetVROM_8K_Bank((CHRhi << 2) | CHRlo);

	if (PRGsize)
	{
		SetPROM_16K_Bank(4, (PRGchip << 6) | PRG);
		SetPROM_16K_Bank(6, (PRGchip << 6) | PRG);
	}
	else	SetPROM_32K_Bank(((PRGchip << 6) | PRG) >> 1);

	if (Mir_HV)
		SetVRAM_Mirror(VRAM_HMIRROR);
	else	SetVRAM_Mirror(VRAM_VMIRROR);
	*/

	//fceu
	SetVROM_8K_Bank(((A&0x1F)<<2)|(V&0x03));

	if(A&0x20) {
		SetPROM_16K_Bank(0x8000>>13,(A&0x40)|((A>>8)&0x3F));
		SetPROM_16K_Bank(0xc000>>13,(A&0x40)|((A>>8)&0x3F));
	}
	else
		SetPROM_32K_Bank(((A&0x40)|((A>>8)&0x3F))>>1);
	SetVRAM_Mirror( ((A&0x80)>>7)^1 );
}

