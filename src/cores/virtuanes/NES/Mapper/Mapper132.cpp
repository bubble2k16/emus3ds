//////////////////////////////////////////////////////////////////////////
// Mapper132  TXC(Qi Wang)                                              //
//////////////////////////////////////////////////////////////////////////

void    Mapper132::Reset()
{
	SetPROM_32K_Bank(0);
	SetVROM_8K_Bank(0);
	for( INT i = 0; i < 4; i++ ) {
		regs[i] = 0;
	}
}

BYTE	Mapper132::ReadLow( WORD addr )
{
	BYTE ret;
	ret=0;
	if( addr==0x4100 && regs[3]!=0 ) ret=regs[2];
	return ret;
}

void	Mapper132::WriteLow( WORD addr, BYTE data )
{
	if( addr>=0x4100 && addr<=0x4103 ) regs[addr&3]=data;
}

void	Mapper132::Write( WORD addr, BYTE data )
{
	SetPROM_32K_Bank( (regs[2]>>2)&1 );
	SetVROM_8K_Bank( regs[2]&3 );
}
