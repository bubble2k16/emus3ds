//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Emulation core                                              //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__NES_INCLUDED__
#define	__NES_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "macro.h"

#include "state.h"
#include "cheat.h"
#include "VsUnisystem.h"

#include <vector>
using namespace std;

#define	NES_PROFILER	0

// �R���t�B�O
#define	FETCH_CYCLES	8

typedef struct tagNESCONFIG {
	FLOAT	BaseClock;		// NTSC:21477270.0  PAL:21281364.0
	FLOAT	CpuClock;		// NTSC: 1789772.5  PAL: 1773447.0

	INT	TotalScanlines;		// NTSC: 262  PAL: 312

	INT	ScanlineCycles;		// NTSC:1364  PAL:1362

	INT	HDrawCycles;		// NTSC:1024  PAL:1024
	INT	HBlankCycles;		// NTSC: 340  PAL: 338
	INT	ScanlineEndCycles;	// NTSC:   4  PAL:   2

	INT	FrameCycles;		// NTSC:29829.52  PAL:35468.94
	INT	FrameIrqCycles;		// NTSC:29829.52  PAL:35468.94

	INT	FrameRate;		// NTSC:60(59.94) PAL:50
	FLOAT	FramePeriod;		// NTSC:16.683    PAL:20.0
} NESCONFIG, *LPNESCONFIG;

extern	NESCONFIG NESCONFIG_NTSC;
extern	NESCONFIG NESCONFIG_PAL;

// class prototypes
class	CPU;
class	PPU;
class	APU;
class	ROM;
class	PAD;
class	Mapper;

//
// NES ƒNƒ‰ƒX
//
class NES
{
public:
// ƒƒ“ƒo•Ï”
	CPU*	cpu;
	PPU*	ppu;
	APU*	apu;
	ROM*	rom;
	PAD*	pad;
	Mapper*	mapper;

	NESCONFIG* nescfg;
	char	*error;
public:
	NES( const char* fname );
	virtual	~NES();

// �����o�֐�
	// �G�~�����[�V����
	void	Reset();
	void	SoftReset();

	void	Clock( INT cycles );

	BYTE	Read ( WORD addr );
	void	Write( WORD addr, BYTE data );

	void	EmulationCPU( INT basecycles );
	void	EmulationCPU_BeforeNMI( INT cycles );

	void	EmulateFrame( BOOL bDraw );

	// For NSF
	void	EmulateNSF();
	void	SetNsfPlay( INT songno, INT songmode );
	void	SetNsfStop();
	BOOL	IsNsfPlaying() { return m_bNsfPlaying; };

	// IRQ type contorol
	enum IRQMETHOD { 
		IRQ_HSYNC = 0,
		IRQ_CLOCK = 1
	};
	void	SetIrqType( INT nType ) { nIRQtype = nType; };
	INT	GetIrqType() { return (INT)nIRQtype; };

	// Frame-IRQ control (for Paris-Dakar Rally Special)
	void	SetFrameIRQmode( BOOL bMode ) { bFrameIRQ = bMode; }
	BOOL	GetFrameIRQmode() { return bFrameIRQ; }

	// NTSC/PAL
	void	SetVideoMode( BOOL bMode );
	BOOL	GetVideoMode() { return bVideoMode; }

	//
	INT	GetDiskNo();
	void	SoundSetup();

	INT	GetScanline()	{ return NES_scanline; }
	BOOL	GetZapperHit()	{ return bZapper; }
	void	GetZapperPos( LONG&x, LONG&y )	{ x = ZapperX; y = ZapperY; }
	void	SetZapperPos( LONG x, LONG y )	{ ZapperX = x; ZapperY = y; }

	// ƒXƒe[ƒgƒtƒ@ƒCƒ‹
	// 0:ERROR 1:CRC OK -1:CRC ERR
	static	INT	IsStateFile( const char* fname, ROM* rom );
	BOOL	LoadState( const char* fname );
	BOOL	SaveState( const char* fname );

	INT	GetSAVERAM_SIZE()		{ return SAVERAM_SIZE; }
	void	SetSAVERAM_SIZE( INT size )	{ SAVERAM_SIZE = size; }

	// VS-Unisystem
	BYTE	GetVSDipSwitch()		{ return m_VSDipValue; }
	void	SetVSDipSwitch( BYTE v )	{ m_VSDipValue = v; }
	VSDIPSWITCH*	GetVSDipSwitchTable()	{ return m_VSDipTable; }

	// ƒXƒiƒbƒvƒVƒ‡ƒbƒg
	BOOL	Snapshot();

	// For Movie
	// 0:ERROR 1:CRC OK -1:CRC ERR
	static	INT	IsMovieFile( const char* fname, ROM* rom );

	BOOL	IsMoviePlay() { return m_bMoviePlay; }
	BOOL	IsMovieRec() { return m_bMovieRec; }
	BOOL	MoviePlay( const char* fname );
	BOOL	MovieRec( const char* fname );
	BOOL	MovieRecAppend( const char* fname );
	BOOL	MovieStop();

	// ‚»‚Ì‘¼ƒRƒ“ƒgƒ[ƒ‹
	BOOL	IsDiskThrottle() { return m_bDiskThrottle; }
//	BOOL	IsBraking() { return m_bBrake; }	// Debugger

	// •`‰æ•ûŽ®
	enum RENDERMETHOD { 
		POST_ALL_RENDER = 0, // ƒXƒLƒƒƒ“ƒ‰ƒCƒ“•ª‚Ì–½—ßŽÀsŒãCƒŒƒ“ƒ_ƒŠƒ“ƒO
		PRE_ALL_RENDER  = 1, // ƒŒƒ“ƒ_ƒŠƒ“ƒO‚ÌŽÀsŒãCƒXƒLƒƒƒ“ƒ‰ƒCƒ“•ª‚Ì–½—ßŽÀs
		POST_RENDER     = 2, // •\Ž¦ŠúŠÔ•ª‚Ì–½—ßŽÀsŒãCƒŒƒ“ƒ_ƒŠƒ“ƒO
		PRE_RENDER      = 3, // ƒŒƒ“ƒ_ƒŠƒ“ƒOŽÀsŒãC•\Ž¦ŠúŠÔ•ª‚Ì–½—ßŽÀs
		TILE_RENDER     = 4  // ƒ^ƒCƒ‹ƒx[ƒXƒŒƒ“ƒ_ƒŠƒ“ƒO
	};
	void		SetRenderMethod( RENDERMETHOD type )	
	{ 
		if (type == TILE_RENDER) type = POST_RENDER;
		RenderMethod = type; 
	}
	RENDERMETHOD	GetRenderMethod()			{ return RenderMethod; }

	// �R�}���h
	enum NESCOMMAND {
		NESCMD_NONE = 0,
		NESCMD_HWRESET,
		NESCMD_SWRESET,
		NESCMD_EXCONTROLLER,	// Commandparam
		NESCMD_DISK_THROTTLE_ON,
		NESCMD_DISK_THROTTLE_OFF,
		NESCMD_DISK_EJECT,
		NESCMD_DISK_0A,
		NESCMD_DISK_0B,
		NESCMD_DISK_1A,
		NESCMD_DISK_1B,
		NESCMD_DISK_2A,
		NESCMD_DISK_2B,
		NESCMD_DISK_3A,
		NESCMD_DISK_3B,

		NESCMD_SOUND_MUTE,	// CommandParam
	};

	void	Command( NESCOMMAND cmd );
	BOOL	CommandParam( NESCOMMAND cmd, INT param );

	// For Movie
	void	Movie();
	void	GetMovieInfo( WORD& wRecVersion, WORD& wVersion, DWORD& dwRecordFrames, DWORD& dwRecordTimes );

	// For Cheat
	void	CheatInitial();

	BOOL	IsCheatCodeAdd();

	INT	GetCheatCodeNum();
	BOOL	GetCheatCode( INT no, CHEATCODE& code );
	void	SetCheatCodeFlag( INT no, BOOL bEnable );
	void	SetCheatCodeAllFlag( BOOL bEnable, BOOL bKey );

	void	ReplaceCheatCode( INT no, CHEATCODE code );
	void	AddCheatCode( CHEATCODE code );
	void	DelCheatCode( INT no );

	DWORD	CheatRead( INT length, WORD addr );
	void	CheatWrite( INT length, WORD addr, DWORD data );
	void	CheatCodeProcess();

	// For Genie
	void	GenieInitial();
	bool	GenieAdd( bool enabled, char *buf );
	void	GenieSet(int index, bool enabled);
	void	GenieLoad( char* fname );
	void	GenieCodeProcess();

	// TapeDevice
	BOOL	IsTapePlay() { return m_bTapePlay; }
	BOOL	IsTapeRec() { return m_bTapeRec; }
	BOOL	TapePlay( const char* fname );
	BOOL	TapeRec( const char* fname );
	void	TapeStop();
	void	Tape( INT cycles );

	// Barcode battler(Bandai)
	void	SetBarcodeData( LPBYTE code, INT len );
	void	Barcode( INT cycles );
	BOOL	IsBarcodeEnable() { return m_bBarcode; };
	BYTE	GetBarcodeStatus() { return m_BarcodeOut; };

	// Barcode world(Sunsoft/EPOCH)
	void	SetBarcode2Data( LPBYTE code, INT len );
	BYTE	Barcode2( void );
	BOOL	IsBarcode2Enable() { return m_bBarcode2; };

	// TurboFile
	void	SetTurboFileBank( INT bank ) { m_TurboFileBank = bank; }
	INT	GetTurboFileBank() { return m_TurboFileBank; }

#if	NES_PROFILER
	// TEST
	DWORD	GetFrameTotalCycles() {
		return	m_dwTotalCycle;
	}
	DWORD	GetProfileTotalCycles() {
		return	m_dwProfileTotalCycle;
	}
	DWORD	GetProfileTotalCount() {
		return	m_dwProfileTotalCount;
	}
	DWORD	GetProfileCycles() {
		return	m_dwProfileCycle;
	}
#endif

	void	SaveSRAM();

protected:
// ƒƒ“ƒoŠÖ”
	// ƒGƒ~ƒ…ƒŒ[ƒVƒ‡ƒ“
	BYTE	ReadReg ( WORD addr );
	void	WriteReg( WORD addr, BYTE data );

	// ƒXƒe[ƒgƒTƒu
	BOOL	ReadState( FILE* fp );
	void	WriteState( FILE* fp );

	void	LoadSRAM();
	//void	SaveSRAM();

	void	LoadDISK();
	void	SaveDISK();

	void	LoadTurboFile();
	void	SaveTurboFile();

protected:
// ƒƒ“ƒo•Ï”
	INT	nIRQtype;
	BOOL	bVideoMode;
	BOOL	bFrameIRQ;

	BOOL	bZapper;
	LONG	ZapperX, ZapperY;

	BOOL	m_bPadStrobe;

	RENDERMETHOD	RenderMethod;

	BOOL	m_bDiskThrottle;

	SQWORD	base_cycles;
	SQWORD	emul_cycles;

	INT	NES_scanline;

	INT	SAVERAM_SIZE;

	// For VS-Unisystem
	BYTE		m_VSDipValue;
	VSDIPSWITCH*	m_VSDipTable;

	// Snapshot number
	INT	m_nSnapNo;

	// For NSF
	BOOL	m_bNsfPlaying;
	BOOL	m_bNsfInit;
	INT	m_nNsfSongNo;
	INT	m_nNsfSongMode;

	// For Movie
	BOOL	m_bMoviePlay;
	BOOL	m_bMovieRec;
	WORD	m_MovieVersion;

	FILE*	m_fpMovie;
	MOVIEFILEHDR	m_hedMovie;
	DWORD	m_MovieControl;
	LONG	m_MovieStepTotal;
	LONG	m_MovieStep;
	INT	m_CommandRequest;

	// For Tape
	BOOL	m_bTapePlay;
	BOOL	m_bTapeRec;
	FILE*	m_fpTape;
	double	m_TapeCycles;
	BYTE	m_TapeIn;
	BYTE	m_TapeOut;

	// For Barcode
	BOOL	m_bBarcode;
	BYTE	m_BarcodeOut;
	BYTE	m_BarcodePtr;
	INT	m_BarcodeCycles;
	BYTE	m_BarcodeData[256];

	// For Barcode
	BOOL	m_bBarcode2;
	INT	m_Barcode2seq;
	INT	m_Barcode2ptr;
	INT	m_Barcode2cnt;
	BYTE	m_Barcode2bit;
	BYTE	m_Barcode2data[32];

	// For TurboFile
	INT	m_TurboFileBank;

	// gameoption backup
	INT	m_saveRenderMethod;
	INT	m_saveIrqType;
	BOOL	m_saveFrameIRQ;
	BOOL	m_saveVideoMode;

	// For Cheat
	BOOL	m_bCheatCodeAdd;
	vector<CHEATCODE>	m_CheatCode;

	// For Genie
	vector<GENIECODE>	m_GenieCode;

	// For Movie pad display
	void	DrawPad();
	void	DrawBitmap( INT x, INT y, LPBYTE lpBitmap );
	static	BYTE	m_PadImg[];
	static	BYTE	m_KeyImg0[];
	static	BYTE	m_KeyImg1[];
	static	BYTE	m_KeyImg2[];

#if	NES_PROFILER
	// TEST
	BYTE	m_ProfileEnable;
	DWORD	m_dwTotalCycle;
	DWORD	m_dwTotalTempCycle;
	DWORD	m_dwProfileTotalCycle;
	DWORD	m_dwProfileTotalCount;
	DWORD	m_dwProfileCycle;
	DWORD	m_dwProfileTempCycle;
	DWORD	m_dwProfileAveCycle;
	DWORD	m_dwProfileMaxCycle;

//	static	BYTE	Font6x8[];

//	void	DrawFont( INT x, INT y, BYTE chr, BYTE col );
//	void	DrawString( INT x, INT y, LPSTR str, BYTE col );
#endif
	static	BYTE	Font6x8[];

	void	DrawFont( INT x, INT y, BYTE chr, BYTE col );
	void	DrawString( INT x, INT y, LPSTR str, BYTE col );
private:
};

#endif	// !__NES_INCLUDED__

