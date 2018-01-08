//////////////////////////////////////////////////////////////////////////
// Mapper241  Education X-in-1                                          //
//////////////////////////////////////////////////////////////////////////
void    Mapper241::Reset()
{
	SetPROM_32K_Bank( 0 );
	if( VROM_1K_SIZE ) SetVROM_8K_Bank( 0 );

	SBPAL_fix = 0;
	DWORD	crc = nes->rom->GetPROM_CRC();
	if(crc == 0x85068811) SBPAL_fix = 1;	//[Subor] Subor V6.0 (C)
	if(crc == 0xE475D89A) SBPAL_fix = 2;	//[Subor] Subor V9.0 (C)
	if(crc == 0x900D9E00) SBPAL_fix = 2;	//[Subor] Subor V9.1 (C)
}

void	Mapper241::Write( WORD addr, BYTE data )
{

//	if(nes->GetScanline() == 283)
	DEBUGOUT( "MPRWR A=%04X D=%02X L=%3d CYC=%d\n", addr&0xFFFF, data&0xFF, nes->GetScanline(), nes->cpu->GetTotalCycles() );

//	if( addr == 0x8000 )	SetPROM_32K_Bank( data );

//	if( data&0x80 ) 
//		SetPROM_32K_Bank( ((data&7)>>1)+4 );
//	else
//		SetPROM_32K_Bank( (data&7)>>1 );

//	if( data&0x80 ) 
//		SetPROM_32K_Bank( data+8 );
//	else
		SetPROM_32K_Bank( data );

	if(SBPAL_fix==1)	//[Subor] Subor V6.0 (C)
		if((addr==0x8D56)&&(data==3)&&(nes->GetScanline()==283))
		{
			SetPROM_32K_Bank( 0 );
		}
	if(SBPAL_fix==2)	//[Subor] Subor V9.0 (C)
	{
		if((addr==0x8D5C)&&(data==9)&&((nes->GetScanline()>282)&&(nes->GetScanline()<291)))
		{
			SetPROM_32K_Bank( 0 );
		}
		if((addr==0x8D56)&&(data==3)&&(nes->GetScanline()==283))
		{
			SetPROM_32K_Bank( 0 );
		}
	}
}
