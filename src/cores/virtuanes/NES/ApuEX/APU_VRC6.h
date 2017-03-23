//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      Konami VRC6                                                     //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_VRC6_INCLUDED__
#define	__APU_VRC6_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"

class	APU_VRC6 : public APU_INTERFACE
{
public:
	APU_VRC6();
	~APU_VRC6();

	void	Reset( FLOAT fClock, INT nRate );
	void	Setup( FLOAT fClock, INT nRate );
	void	Write( WORD addr, BYTE data );
	INT	Process( INT channel );

	INT	GetFreq( INT channel );

	INT	GetStateSize();
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
protected:
	typedef	struct {
		BYTE	reg[3];

		BYTE	enable;
		BYTE	gate;
		BYTE	volume;

		INT	phaseacc;
		INT	freq;
		INT	output_vol;

		BYTE	adder;
		BYTE	duty_pos;
	} RECTANGLE, *LPRECTANGLE;

	typedef	struct {
		BYTE	reg[3];

		BYTE	enable;
		BYTE	volume;

		INT	phaseacc;
		INT	freq;
		INT	output_vol;

		BYTE	adder;
		BYTE	accum;
		BYTE	phaseaccum;
	} SAWTOOTH, *LPSAWTOOTH;

	INT	RectangleRender( RECTANGLE& ch );
	INT	SawtoothRender( SAWTOOTH& ch );

	RECTANGLE	ch0, ch1;
	SAWTOOTH	ch2;

	INT	cycle_rate;

	FLOAT	cpu_clock;
private:
};

#endif	// !__APU_VRC6_INCLUDED__
