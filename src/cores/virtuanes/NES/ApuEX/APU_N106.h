//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Namcot N106                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_N106_INCLUDED__
#define	__APU_N106_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"

class	APU_N106 : public APU_INTERFACE
{
public:
	APU_N106();
	~APU_N106();

	void	Reset( FLOAT fClock, INT nRate );
	void	Setup( FLOAT fClock, INT nRate );
	void	Write( WORD addr, BYTE data );
	BYTE	Read( WORD addr );
	INT	Process( INT channel );

	INT	GetFreq( INT channel );

	INT	GetStateSize();
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
protected:
	typedef	struct {
		INT	phaseacc;

		DWORD	freq;
		DWORD	phase;
		DWORD	tonelen;

		INT	output;

		BYTE	toneadr;
		BYTE	volupdate;

		BYTE	vol;
		BYTE	databuf;
	} CHANNEL;

	CHANNEL	op[8];

	FLOAT	cpu_clock;
	DWORD	cycle_rate;

	BYTE	addrinc;
	BYTE	address;
	BYTE	channel_use;

	BYTE	tone[0x100];

	INT	ChannelRender( CHANNEL& ch );
private:
};

#endif	// !__APU_N106_INCLUDED__
