/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapper                                                      */
/*                                                           Norix      */
/*                                               written     2001/02/05 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
#ifndef	__MAPPER_INCLUDED__
#define	__MAPPER_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "DirectSound.h"

#include "typedef.h"
#include "macro.h"

// class prototype
class	NES;

// Mapper base class
class	Mapper
{
public:
	Mapper( NES* parent );
	virtual	~Mapper();

	// For Mapper
	// Reset
	virtual	void	Reset() = 0;

	// $8000-$FFFF Memory write
	virtual	void	Write( WORD addr, BYTE data ) {}

	// $8000-$FFFF Memory read(Dummy)
	virtual	void	Read( WORD addr, BYTE data ) {}

	// $4100-$7FFF Lower Memory read/write
	virtual	BYTE	ReadLow ( WORD addr );
	virtual	void	WriteLow( WORD addr, BYTE data );

	// $4018-$40FF Extention register read/write
	virtual	BYTE	ExRead ( WORD addr )	{ return 0x00; }
	virtual	void	ExWrite( WORD addr, BYTE data ) {}

	// Extension commands
	// For ExCmdRead command
	enum	EXCMDRD {
		EXCMDRD_NONE = 0,
		EXCMDRD_DISKACCESS,
	};
	// For ExCmdWrite command
	enum	EXCMDWR {
		EXCMDWR_NONE = 0,
		EXCMDWR_DISKINSERT,
		EXCMDWR_DISKEJECT,
	};

	virtual	BYTE	ExCmdRead ( EXCMDRD cmd )	{ return 0x00; }
	virtual	void	ExCmdWrite( EXCMDWR cmd, BYTE data ) {}

	// H sync/V sync/Clock sync
	virtual	void	HSync( INT scanline ) {}
	virtual	void	VSync() {}
	virtual	void	Clock( INT cycles ) {}

	// PPU address bus latch
	virtual	void	PPU_Latch( WORD addr ) {}

	// PPU Character latch
	virtual	void	PPU_ChrLatch( WORD addr ) {}

	// PPU Extension character/palette
	virtual	void	PPU_ExtLatchX( INT x ) {}
	virtual	void	PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr ) {}

	// For State save
	virtual	BOOL	IsStateSave() { return FALSE; }
	virtual	void	SaveState( LPBYTE p ) {}
	virtual	void	LoadState( LPBYTE p ) {}

protected:
	NES*	nes;

private:
};

// Create class instance
extern	Mapper*	CreateMapper( NES* parent, INT no );

#endif
