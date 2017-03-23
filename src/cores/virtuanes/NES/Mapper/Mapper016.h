//////////////////////////////////////////////////////////////////////////
// Mapper016  Bandai Standard                                           //
//////////////////////////////////////////////////////////////////////////
#include "EEPROM.h"

class	Mapper016 : public Mapper
{
public:
	Mapper016( NES* parent ) : Mapper(parent) {}

	void	Reset();
	BYTE	ReadLow( WORD addr );
	void	WriteLow( WORD addr, BYTE data );
	void	Write( WORD addr, BYTE data );

	void	HSync( INT scanline );
	void	Clock( INT cycles );

	// For Datach Barcode Battler
	void	SetBarcodeData( LPBYTE code, INT len );

	// For state save
	BOOL	IsStateSave() { return TRUE; }
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	BYTE	patch;	// For Famicom Jump 2
	BYTE	eeprom_type;	// EEPROM type

	BYTE	reg[3];

	BYTE	irq_enable;
	INT	irq_counter;
	INT	irq_latch;
	BYTE	irq_type;

	X24C01	x24c01;
	X24C02	x24c02;

private:
	void	WriteSubA(WORD addr, BYTE data);
	void	WriteSubB(WORD addr, BYTE data);
};
