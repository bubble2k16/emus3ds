//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      SunSoft FME7                                                    //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_FME7_INCLUDED__
#define	__APU_FME7_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"
#include "nes.h"

class	APU_FME7 : public APU_INTERFACE
{
public:
	APU_FME7();
	~APU_FME7();

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
		BYTE	volume;

		INT	freq;
		INT	phaseacc;
		INT	envadr;

		LPBYTE	envtbl;
		LPSBYTE	envstep;
	} ENVELOPE, *LPENVELOPE;

	typedef	struct {
		INT	freq;
		INT	phaseacc;
		INT	noiserange;
		BYTE	noiseout;
	} NOISE, *LPNOISE;

	typedef	struct {
		BYTE	reg[3];
		BYTE	enable;
		BYTE	env_on;
		BYTE	noise_on;
		BYTE	adder;
		BYTE	volume;

		INT	freq;
		INT	phaseacc;

		INT	output_vol;
	} CHANNEL, *LPCHANNEL;

	void	EnvelopeRender();
	void	NoiseRender();

	INT	ChannelRender( CHANNEL& ch );

	ENVELOPE envelope;
	NOISE	noise;
	CHANNEL	op[3];

	BYTE	address;

	INT	vol_table[0x20];
	INT	cycle_rate;

	FLOAT	cpu_clock;

	// Tables
	static	BYTE	envelope_pulse0[];
	static	BYTE	envelope_pulse1[];
	static	BYTE	envelope_pulse2[];
	static	BYTE	envelope_pulse3[];
	static	SBYTE	envstep_pulse[];

	static	BYTE	envelope_sawtooth0[];
	static	BYTE	envelope_sawtooth1[];
	static	SBYTE	envstep_sawtooth[];

	static	BYTE	envelope_triangle0[];
	static	BYTE	envelope_triangle1[];

	static	SBYTE	envstep_triangle[];

	static	LPBYTE	envelope_table[16];
	static	LPSBYTE	envstep_table[16];
private:
};

#endif	// !__APU_FME7_INCLUDED__
