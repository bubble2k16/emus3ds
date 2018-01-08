//////////////////////////////////////////////////////////////////////////
// BoardLB12IN1                                                         //
//////////////////////////////////////////////////////////////////////////

void	BoardLB12IN1::Reset()
{
	SetPROM_8K_Bank( 3, 0x0C );
	SetPROM_32K_Bank( 0x0A, 0x0B, 0x06, 0x07 );
	SetVROM_8K_Bank( 0x0C );
}

BYTE	BoardLB12IN1::ReadLow( WORD addr )
{
	if(addr==0x692C)
		if((CPU_MEM_BANK[3][0x92C]==0xFE)&&(CPU_MEM_BANK[3][0x92D]==0xFA))
			SetVROM_8K_Bank( 0x06 );
	return Mapper::ReadLow( addr );
}

void	BoardLB12IN1::ExWrite( WORD addr, BYTE data )
{
	if(addr==0x4025)
		if((RAM[0x602]>0)&&(RAM[0x602]<10)){
			BYTE bank = (RAM[0x602]-4)&0x0F;
			SetPROM_8K_Bank( 3, bank );
			SetVROM_8K_Bank( bank );
		}
}
