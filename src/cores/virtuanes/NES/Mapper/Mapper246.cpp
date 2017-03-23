//////////////////////////////////////////////////////////////////////////
// Mapper246  Phone Serm Berm                                           //
//////////////////////////////////////////////////////////////////////////

void    Mapper246::Reset()
{
	SetPROM_32K_Bank( 0, 1, PROM_8K_SIZE-2, PROM_8K_SIZE-1 );
}

void	Mapper246::WriteLow( WORD addr, BYTE data )
{
	if( addr>=0x6000 && addr<0x8000 ) {
		switch( addr ) {
			case 0x6000:
				SetPROM_8K_Bank( 4, data );
				break;
			case 0x6001:
				SetPROM_8K_Bank( 5, data );
				break;
			case 0x6002:
				SetPROM_8K_Bank( 6, data );
				break;
			case 0x6003: 
				SetPROM_8K_Bank( 7, data );
				break;
			case 0x6004:
				SetVROM_2K_Bank(0,data);
				break;
			case 0x6005:
				SetVROM_2K_Bank(2,data);
				break;
			case 0x6006:
				SetVROM_2K_Bank(4,data);
				break;
			case 0x6007:
				SetVROM_2K_Bank(6,data);
				break;
			default:
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = data;
				break;
		}
	}
}
