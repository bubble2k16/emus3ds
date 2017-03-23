//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC7                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_VRC7_INCLUDED__
#define	__APU_VRC7_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"

#include "emu2413.h"

class	APU_VRC7 : public APU_INTERFACE
{
public:
	APU_VRC7();
	~APU_VRC7();

	void	Reset( FLOAT fClock, INT nRate );
	void	Setup( FLOAT fClock, INT nRate );
	void	Write( WORD addr, BYTE data );
	INT	Process( INT channel );

	INT	GetFreq( INT channel );
protected:
	OPLL*	VRC7_OPLL;

	BYTE	address;
private:
};

#endif	// !__APU_VRC7_INCLUDED__
