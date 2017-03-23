//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      FDS plugin                                                      //
//                                                           Norix      //
//                                               written     2001/09/18 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__FDSPLUGIN_INCLUDED__
#define	__FDSPLUGIN_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"

class	APU_FDS : public APU_INTERFACE
{
public:
	APU_FDS();
	~APU_FDS();

	void	Reset( FLOAT fClock, INT nRate );
	void	Setup( FLOAT fClock, INT nRate );
	void	Write( WORD addr, BYTE data );
	BYTE	Read ( WORD addr );
	INT	Process( INT channel );

	void	SyncWrite( WORD addr, BYTE data );
	BYTE	SyncRead ( WORD addr );
	BOOL	Sync( INT cycles );

	INT	GetFreq( INT channel );

	INT	GetStateSize();
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );
protected:
	typedef	struct	tagFDSSOUND {
		BYTE	reg[0x80];

		BYTE	volenv_mode;		// Volume Envelope
		BYTE	volenv_gain;
		BYTE	volenv_decay;
		double	volenv_phaseacc;

		BYTE	swpenv_mode;		// Sweep Envelope
		BYTE	swpenv_gain;
		BYTE	swpenv_decay;
		double	swpenv_phaseacc;

		// For envelope unit
		BYTE	envelope_enable;	// $4083 bit6
		BYTE	envelope_speed;		// $408A

		// For $4089
		BYTE	wave_setup;		// bit7
		INT	master_volume;		// bit1-0

		// For Main unit
		INT	main_wavetable[64];
		BYTE	main_enable;
		INT	main_frequency;
		INT	main_addr;

		// For Effector(LFO) unit
		BYTE	lfo_wavetable[64];
		BYTE	lfo_enable;		// 0:Enable 1:Wavetable setup
		INT	lfo_frequency;
		INT	lfo_addr;
		double	lfo_phaseacc;

		// For Sweep unit
		INT	sweep_bias;

		// Misc
		INT	now_volume;
		INT	now_freq;
		INT	output;

	} FDSSOUND, *LPFDSSOUND;

	FDSSOUND fds;
	FDSSOUND fds_sync;

	INT	sampling_rate;
	INT	output_buf[8];

	// Write Sub
	void	WriteSub( WORD addr, BYTE data, FDSSOUND& ch, double rate );
private:
};

#endif	// !__FDSPLUGIN_INCLUDED__
