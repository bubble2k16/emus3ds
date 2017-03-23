//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      APU Internal                                                    //
//                                                           Norix      //
//                                               written     2002/06/27 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__APU_INTERNAL_INCLUDED__
#define	__APU_INTERNAL_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include "windows.h"
#include "math.h"

#include "typedef.h"
#include "macro.h"

#include "APU_INTERFACE.h"
#include "nes.h"
#include "cpu.h"

class	APU_INTERNAL : public APU_INTERFACE
{
public:
	APU_INTERNAL();
	~APU_INTERNAL();

	void	SetParent( NES* parent ) { nes = parent; }

	void	Reset( FLOAT fClock, INT nRate );
	void	Setup( FLOAT fClock, INT nRate );

	void	ToneTableLoad();

	INT	Process( INT channel );

	void	Write( WORD addr, BYTE data );
	BYTE	Read ( WORD addr );

	void	SyncWrite( WORD addr, BYTE data );
	BYTE	SyncRead ( WORD addr );
	BOOL	Sync( INT cycles );

	INT	GetFreq( INT channel );

	void	GetFrameIRQ( INT& Cycle, BYTE& Count, BYTE& Type, BYTE& IRQ, BYTE& Occur ) {
		Cycle = FrameCycle;
		Count = (BYTE)FrameCount;
		Type  = (BYTE)FrameType;
		IRQ   = FrameIRQ;
		Occur = FrameIRQoccur;
	}

	void	SetFrameIRQ( INT Cycle, BYTE Count, BYTE Type, BYTE IRQ, BYTE Occur ) {
		FrameCycle    = Cycle;
		FrameCount    = (INT)Count;
		FrameType     = (INT)Type;
		FrameIRQ      = IRQ;
		FrameIRQoccur = Occur;
	}

	INT	GetStateSize();
	void	SaveState( LPBYTE p );
	void	LoadState( LPBYTE p );

protected:
	typedef	struct {
		BYTE	reg[4];		// register

		BYTE	enable;		// enable
		BYTE	holdnote;	// holdnote
		BYTE	volume;		// volume
		BYTE	complement;

		// For Render
		INT	phaseacc;
		INT	freq;
		INT	freqlimit;
		INT	adder;
		INT	duty;
		INT	len_count;

		INT	nowvolume;

		// For Envelope
		BYTE	env_fixed;
		BYTE	env_decay;
		BYTE	env_count;
		BYTE	dummy0;
		INT	env_vol;

		// For Sweep
		BYTE	swp_on;
		BYTE	swp_inc;
		BYTE	swp_shift;
		BYTE	swp_decay;
		BYTE	swp_count;
		BYTE	dummy1[3];

		// For sync;
		BYTE	sync_reg[4];
		BYTE	sync_output_enable;
		BYTE	sync_enable;
		BYTE	sync_holdnote;
		BYTE	dummy2;
		INT	sync_len_count;
	} RECTANGLE, *LPRECTANGLE;

	typedef	struct {
		BYTE	reg[4];

		BYTE	enable;
		BYTE	holdnote;
		BYTE	counter_start;
		BYTE	dummy0;

		INT	phaseacc;
		INT	freq;
		INT	len_count;
		INT	lin_count;
		INT	adder;

		INT	nowvolume;

		// For sync;
		BYTE	sync_reg[4];
		BYTE	sync_enable;
		BYTE	sync_holdnote;
		BYTE	sync_counter_start;
//		BYTE	dummy1;
		INT	sync_len_count;
		INT	sync_lin_count;
	} TRIANGLE, *LPTRIANGLE;

	typedef	struct {
		BYTE	reg[4];		// register

		BYTE	enable;		// enable
		BYTE	holdnote;	// holdnote
		BYTE	volume;		// volume
		BYTE	xor_tap;
		INT	shift_reg;

		// For Render
		INT	phaseacc;
		INT	freq;
		INT	len_count;

		INT	nowvolume;
		INT	output;

		// For Envelope
		BYTE	env_fixed;
		BYTE	env_decay;
		BYTE	env_count;
		BYTE	dummy0;
		INT	env_vol;

		// For sync;
		BYTE	sync_reg[4];
		BYTE	sync_output_enable;
		BYTE	sync_enable;
		BYTE	sync_holdnote;
		BYTE	dummy1;
		INT	sync_len_count;
	} NOISE, *LPNOISE;

	typedef	struct {
		BYTE	reg[4];
		BYTE	enable;
		BYTE	looping;
		BYTE	cur_byte;
		BYTE	dpcm_value;

		INT	freq;
		INT	phaseacc;
		INT	output;

		WORD	address, cache_addr;
		INT	dmalength, cache_dmalength;
		INT	dpcm_output_real, dpcm_output_fake, dpcm_output_old, dpcm_output_offset;

		// For sync
		BYTE	sync_reg[4];
		BYTE	sync_enable;
		BYTE	sync_looping;
		BYTE	sync_irq_gen;
		BYTE	sync_irq_enable;
		INT	sync_cycles, sync_cache_cycles;
		INT	sync_dmalength, sync_cache_dmalength;
	} DPCM, *LPDPCM;

	void	SyncWrite4017( BYTE data );
	void	UpdateFrame();

	// Rectangle
	void	WriteRectangle ( INT no, WORD addr, BYTE data );
	void	UpdateRectangle( RECTANGLE& ch, INT type );
	INT	RenderRectangle( RECTANGLE& ch );
	// For Sync
	void	SyncWriteRectangle ( INT no, WORD addr, BYTE data );
	void	SyncUpdateRectangle( RECTANGLE& ch, INT type );

	// Triangle
	void	WriteTriangle ( WORD addr, BYTE data );
	void	UpdateTriangle( INT type );
	INT	RenderTriangle();
	// For Sync
	void	SyncWriteTriangle ( WORD addr, BYTE data );
	void	SyncUpdateTriangle( INT type );

	// Noise
	void	WriteNoise ( WORD addr, BYTE data );
	void	UpdateNoise( INT type );
	BYTE	NoiseShiftreg( BYTE xor_tap );
	INT	RenderNoise();
	// For Sync
	void	SyncWriteNoise ( WORD addr, BYTE data );
	void	SyncUpdateNoise( INT type );

	// DPCM
	void	WriteDPCM ( WORD addr, BYTE data );
//	void	UpdateDPCM();
	INT	RenderDPCM();
	// For Sync
	void	SyncWriteDPCM ( WORD addr, BYTE data );
	BOOL	SyncUpdateDPCM( INT cycles );

	// Frame Counter
	INT	FrameCycle;
	INT	FrameCount;
	INT	FrameType;
	BYTE	FrameIRQ;
	BYTE	FrameIRQoccur;

	// Channels
	RECTANGLE	ch0;
	RECTANGLE	ch1;
	TRIANGLE	ch2;
	NOISE		ch3;
	DPCM		ch4;

	// $4015 Reg
	BYTE	reg4015, sync_reg4015;

	// NES object(ref)
	NES*	nes;

	// Sound
	FLOAT	cpu_clock;
	INT	sampling_rate;
	INT	cycle_rate;

	// Tables
	static	INT	vbl_length[32];
	static	INT	freq_limit[8];
	static	INT	duty_lut[4];
	static	INT	noise_freq[16];
	static	INT	dpcm_cycles[16];
	static	INT	dpcm_cycles_pal[16];
//	static	INT	vol_effect[16];

	// WaveTables
	enum	{ TONEDATA_MAX = 16, TONEDATA_LEN = 32 };
	enum	{ CHANNEL_MAX = 3, TONE_MAX = 4 };

	BOOL	bToneTableEnable[TONEDATA_MAX];
	INT	ToneTable[TONEDATA_MAX][TONEDATA_LEN];
	INT	ChannelTone[CHANNEL_MAX][TONE_MAX];

private:
};

#endif	// !__APU_INTERNAL_INCLUDED__
