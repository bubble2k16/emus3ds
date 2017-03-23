//
// �ݒ��ۑ��N���X
//
#include "VirtuaNESres.h"
#include "DebugOut.h"

#include "Config.h"
//#include "Registry.h"

//#include "DirectInput.h"
#include "extsoundfile.h"

// Global instance
CConfig	Config;

// Sampling rate table
INT	CConfig::SamplingRateTable[] = {
	11025,  8, 11025, 16, 22050,  8, 22050, 16,
	32000,  8, 32000, 16,
	44100,  8, 44100, 16, 48000,  8, 48000, 16,
	96000,  8, 96000, 16, 192000, 8, 192000, 16,
	0, 0,
};

// Shortcut key IDs table
INT	CConfig::ShortcutKeyID[] = {
	// Main controls
	ID_OPEN,		IDS_CUT_OPEN,			0,
	ID_CLOSE,		IDS_CUT_CLOSE,			1,
	ID_LAUNCHER,		IDS_CUT_LAUNCHER,		2,

	ID_NETPLAY_CONNECT,	IDS_CUT_NETPLAY_CONNECT,	3,
	ID_NETPLAY_DISCONNECT,	IDS_CUT_NETPLAY_DISCONNECT,	4,
	ID_NETPLAY_CHAT,	IDS_CUT_NETPLAY_CHAT,		5,

	ID_ROMINFO,		IDS_CUT_ROMINFO,		8,
	ID_WAVERECORD,		IDS_CUT_WAVERECORD,		9,
	ID_EXIT,		IDS_CUT_EXIT,			15,

	// Emulation controls
	ID_HWRESET,		IDS_CUT_HWRESET,		16,
	ID_SWRESET,		IDS_CUT_SWRESET,		17,
	ID_PAUSE,		IDS_CUT_PAUSE,			18,
	ID_ONEFRAME,		IDS_CUT_ONEFRAME,		23,
	ID_THROTTLE,		IDS_CUT_THROTTLE,		19,
	ID_KEYTHROTTLE,		IDS_CUT_KEYTHROTTLE,		24,

	ID_FRAMESKIP_AUTO,	IDS_CUT_FRAMESKIP_AUTO,		20,
	ID_FRAMESKIP_UP,	IDS_CUT_FRAMESKIP_UP,		21,
	ID_FRAMESKIP_DOWN,	IDS_CUT_FRAMESKIP_DOWN,		22,

	// State controls
	ID_STATE_LOAD,		IDS_CUT_STATE_LOAD,		32,
	ID_STATE_SAVE,		IDS_CUT_STATE_SAVE,		33,
	ID_STATE_UP,		IDS_CUT_STATE_UP,		34,
	ID_STATE_DOWN,		IDS_CUT_STATE_DOWN,		35,
	ID_STATE_SLOT0,		IDS_CUT_STATE_SLOT0,		36,
	ID_STATE_SLOT1,		IDS_CUT_STATE_SLOT1,		37,
	ID_STATE_SLOT2,		IDS_CUT_STATE_SLOT2,		38,
	ID_STATE_SLOT3,		IDS_CUT_STATE_SLOT3,		39,
	ID_STATE_SLOT4,		IDS_CUT_STATE_SLOT4,		40,
	ID_STATE_SLOT5,		IDS_CUT_STATE_SLOT5,		41,
	ID_STATE_SLOT6,		IDS_CUT_STATE_SLOT6,		42,
	ID_STATE_SLOT7,		IDS_CUT_STATE_SLOT7,		43,
	ID_STATE_SLOT8,		IDS_CUT_STATE_SLOT8,		44,
	ID_STATE_SLOT9,		IDS_CUT_STATE_SLOT9,		45,

	// QuickLoad
	ID_QUICKLOAD_SLOT0,	IDS_CUT_QUICKLOAD_SLOT0,	256,
	ID_QUICKLOAD_SLOT1,	IDS_CUT_QUICKLOAD_SLOT1,	257,
	ID_QUICKLOAD_SLOT2,	IDS_CUT_QUICKLOAD_SLOT2,	258,
	ID_QUICKLOAD_SLOT3,	IDS_CUT_QUICKLOAD_SLOT3,	259,
	ID_QUICKLOAD_SLOT4,	IDS_CUT_QUICKLOAD_SLOT4,	260,
	ID_QUICKLOAD_SLOT5,	IDS_CUT_QUICKLOAD_SLOT5,	261,
	ID_QUICKLOAD_SLOT6,	IDS_CUT_QUICKLOAD_SLOT6,	262,
	ID_QUICKLOAD_SLOT7,	IDS_CUT_QUICKLOAD_SLOT7,	263,
	ID_QUICKLOAD_SLOT8,	IDS_CUT_QUICKLOAD_SLOT8,	264,
	ID_QUICKLOAD_SLOT9,	IDS_CUT_QUICKLOAD_SLOT9,	265,
	// QuickSave
	ID_QUICKSAVE_SLOT0,	IDS_CUT_QUICKSAVE_SLOT0,	266,
	ID_QUICKSAVE_SLOT1,	IDS_CUT_QUICKSAVE_SLOT1,	267,
	ID_QUICKSAVE_SLOT2,	IDS_CUT_QUICKSAVE_SLOT2,	268,
	ID_QUICKSAVE_SLOT3,	IDS_CUT_QUICKSAVE_SLOT3,	269,
	ID_QUICKSAVE_SLOT4,	IDS_CUT_QUICKSAVE_SLOT4,	270,
	ID_QUICKSAVE_SLOT5,	IDS_CUT_QUICKSAVE_SLOT5,	271,
	ID_QUICKSAVE_SLOT6,	IDS_CUT_QUICKSAVE_SLOT6,	272,
	ID_QUICKSAVE_SLOT7,	IDS_CUT_QUICKSAVE_SLOT7,	273,
	ID_QUICKSAVE_SLOT8,	IDS_CUT_QUICKSAVE_SLOT8,	274,
	ID_QUICKSAVE_SLOT9,	IDS_CUT_QUICKSAVE_SLOT9,	275,

	// Disk controls
	ID_DISK_EJECT,		IDS_CUT_DISK_EJECT,		48,
	ID_DISK_0A,		IDS_CUT_DISK_0A,		49,
	ID_DISK_0B,		IDS_CUT_DISK_0B,		50,
	ID_DISK_1A,		IDS_CUT_DISK_1A,		51,
	ID_DISK_1B,		IDS_CUT_DISK_1B,		52,
	ID_DISK_2A,		IDS_CUT_DISK_2A,		120,
	ID_DISK_2B,		IDS_CUT_DISK_2B,		121,
	ID_DISK_3A,		IDS_CUT_DISK_3A,		122,
	ID_DISK_3B,		IDS_CUT_DISK_3B,		123,

	// Movie controls
	ID_MOVIE_PLAY,		IDS_CUT_MOVIE_PLAY,		56,
	ID_MOVIE_REC,		IDS_CUT_MOVIE_REC,		57,
	ID_MOVIE_REC_APPEND,	IDS_CUT_MOVIE_REC_APPEND,	58,
	ID_MOVIE_STOP,		IDS_CUT_MOVIE_STOP,		59,
	ID_MOVIE_INFO,		IDS_CUT_MOVIE_INFO,		60,
	ID_MOVIE_CONVERT,	IDS_CUT_MOVIE_CONVERT,		61,

	// Screen controls
	ID_ZOOMx1,		IDS_CUT_ZOOMx1,			64,
	ID_ZOOMx2,		IDS_CUT_ZOOMx2,			65,
	ID_ZOOMx3,		IDS_CUT_ZOOMx3,			66,
	ID_ZOOMx4,		IDS_CUT_ZOOMx4,			67,
	ID_FULLSCREEN,		IDS_CUT_FULLSCREEN,		68,

	// Sound controls
	ID_MUTE_0,		IDS_CUT_MUTE_MASTER,		72,
	ID_MUTE_1,		IDS_CUT_MUTE_RECTANGLE1,	73,
	ID_MUTE_2,		IDS_CUT_MUTE_RECTANGLE2,	74,
	ID_MUTE_3,		IDS_CUT_MUTE_TRIANGLE,		75,
	ID_MUTE_4,		IDS_CUT_MUTE_NOISE,		76,
	ID_MUTE_5,		IDS_CUT_MUTE_DPCM,		77,
	ID_MUTE_6,		IDS_CUT_MUTE_EXTERNAL1,		78,
	ID_MUTE_7,		IDS_CUT_MUTE_EXTERNAL2,		79,
	ID_MUTE_8,		IDS_CUT_MUTE_EXTERNAL3,		80,
	ID_MUTE_9,		IDS_CUT_MUTE_EXTERNAL4,		81,
	ID_MUTE_A,		IDS_CUT_MUTE_EXTERNAL5,		82,
	ID_MUTE_B,		IDS_CUT_MUTE_EXTERNAL6,		83,
	ID_MUTE_C,		IDS_CUT_MUTE_EXTERNAL7,		84,
	ID_MUTE_D,		IDS_CUT_MUTE_EXTERNAL8,		85,

	// Tape controls
	ID_TAPE_PLAY,		IDS_CUT_TAPE_PLAY,		90,
	ID_TAPE_REC,		IDS_CUT_TAPE_REC,		91,
	ID_TAPE_STOP,		IDS_CUT_TAPE_STOP,		92,

	// Other controls
	ID_SNAPSHOT,		IDS_CUT_SNAPSHOT,		96,
	ID_FPSDISP,		IDS_CUT_FPSDISP,		97,
	ID_TVASPECT,		IDS_CUT_TVASPECT,		98,
	ID_TVFRAME,		IDS_CUT_TVFRAME,		99,
	ID_SCANLINE,		IDS_CUT_SCANLINE,		100,
	ID_ALLLINE,		IDS_CUT_ALLLINE,		101,
	ID_ALLSPRITE,		IDS_CUT_ALLSPRITE,		102,
	ID_LEFTCLIP,		IDS_CUT_LEFTCLIP,		105,
	ID_SYNCDRAW,		IDS_CUT_SYNCDRAW,		103,
	ID_FITSCREEN,		IDS_CUT_FITSCREEN,		104,

	// Tool controls
	ID_SEARCH,		IDS_CUT_SEARCH,			110,
	ID_CHEAT,		IDS_CUT_CHEAT,			111,
	ID_CHEAT_ENABLE,	IDS_CUT_CHEAT_ENABLE,		112,
	ID_CHEAT_DISABLE,	IDS_CUT_CHEAT_DISABLE,		113,
	ID_GENIE,		IDS_CUT_GENIE,			114,

	ID_VIEW_PATTERN,	IDS_CUT_VIEW_PATTERN,		116,
	ID_VIEW_NAMETABLE,	IDS_CUT_VIEW_NAMETABLE,		117,
	ID_VIEW_PALETTE,	IDS_CUT_VIEW_PALETTE,		118,

	ID_VIEW_MEMORY,		IDS_CUT_VIEW_MEMORY,		119,

	0, 0, 0
};

void	CConfig::Load()
{
INT	i, j;
string	ret;
string	section;
CHAR	keys[64];
CHAR	szTemp[MAX_PATH];
WORD	szKeyTemp[64];

/*
// General
	section = "General";

	//general.bDoubleExecute = (BOOL)CRegistry::GetProfileInt( section.c_str(), "DoubleExecute", general.bDoubleExecute );
	//general.bStartupLauncher = (BOOL)CRegistry::GetProfileInt( section.c_str(), "StartupLauncher", general.bStartupLauncher );

//	general.bWindowSave = CRegistry::GetProfileInt( section.c_str(), "WindowPosSave",   general.bWindowSave );
	//general.bWindowZoom = CRegistry::GetProfileInt( section.c_str(), "WindowZoom",   general.bWindowZoom );

	RECT	rc;
	if( CRegistry::GetProfileBinary( section.c_str(), "WindowPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcWindowPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "SearchDialogPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcSearchDlgPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "PatternViewPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcPatternViewPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "NameTableViewPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcNameTableViewPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "PaletteViewPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcPaletteViewPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "MemoryViewPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcMemoryViewPos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "BarcodePos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcBarcodePos = rc;
	}
	if( CRegistry::GetProfileBinary( section.c_str(), "PaletteEditPos", (LPBYTE)&rc, sizeof(RECT) ) ) {
		general.rcPaletteEditPos = rc;
	}

	general.nScreenZoom = CRegistry::GetProfileInt( section.c_str(), "ScreenZoom", general.nScreenZoom );

	general.bNoJoystickID = (BOOL)CRegistry::GetProfileInt( section.c_str(), "NoJoystickID", general.bNoJoystickID );

	general.nJoyAxisDisable = CRegistry::GetProfileInt( section.c_str(), "JoyAxisDisable", general.nJoyAxisDisable );

	if( general.nJoyAxisDisable ) {
		// �ȑO�̐ݒ��������p����
		WORD	bits = 0;
		switch( general.nJoyAxisDisable ) {
			case	1:
				bits = (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5);
				break;
			case	2:
				bits = (1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5);
				break;
			case	3:
				bits = (1<<2)|(1<<3)|(1<<4)|(1<<5);
				break;
			case	4:
				bits = (1<<3)|(1<<4)|(1<<5);
				break;
			case	5:
				bits = (1<<4)|(1<<5);
				break;
			case	6:
				bits = (1<<5);
				break;
		}
		for( i = 0; i < 16; i++ ) {
			general.JoyAxisSetting[i] = bits;
		}

		general.nJoyAxisDisable = 0;
	} else {
		if( CRegistry::GetProfileBinary( section.c_str(), "JoyAxisSetting", szKeyTemp, 16*sizeof(WORD) ) ) {
			::memcpy( general.JoyAxisSetting, szKeyTemp, 16*sizeof(WORD) );
		}
	}
// Paths
	section = "Path";

	path.bRomPath      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "RomPathUse",      path.bRomPath );
	path.bSavePath     = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SavePathUse",     path.bSavePath );
	path.bStatePath    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "StatePathUse",    path.bStatePath );
	path.bSnapshotPath = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SnapshotPathUse", path.bSnapshotPath );
	path.bMoviePath    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "MoviePathUse",    path.bMoviePath );
	path.bWavePath     = (BOOL)CRegistry::GetProfileInt( section.c_str(), "WavePathUse",     path.bWavePath );
	path.bCheatPath    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "CheatPathUse",    path.bCheatPath );
	path.bIpsPath      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "IpsPathUse",      path.bIpsPath );

	if( CRegistry::GetProfileString( section.c_str(), "RomPath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szRomPath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "SavePath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szSavePath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "StatePath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szStatePath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "SnapshotPath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szSnapshotPath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "MoviePath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szMoviePath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "WavePath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szWavePath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "CheatPath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szCheatPath, szTemp );
	if( CRegistry::GetProfileString( section.c_str(), "IpsPath", szTemp, sizeof(szTemp) ) )
		::strcpy( path.szIpsPath, szTemp );

// Emulator
	section = "Emulation";

	emulator.bIllegalOp     = (BOOL)CRegistry::GetProfileInt( section.c_str(), "IllegalOp",     emulator.bIllegalOp );
	emulator.bAutoFrameSkip = (BOOL)CRegistry::GetProfileInt( section.c_str(), "AutoFrameSkip", emulator.bAutoFrameSkip );
	emulator.bThrottle      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "Throttle",      emulator.bThrottle );
	emulator.nThrottleFPS   =       CRegistry::GetProfileInt( section.c_str(), "ThrottleFPS",   emulator.nThrottleFPS );
	emulator.bBackground    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "Background",    emulator.bBackground );
	emulator.nPriority      =       CRegistry::GetProfileInt( section.c_str(), "Priority",      emulator.nPriority );
	emulator.bFourPlayer    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "FourPlayer",    emulator.bFourPlayer );
	emulator.bCrcCheck      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "CrcCheck",      emulator.bCrcCheck );
	emulator.bDiskThrottle  = (BOOL)CRegistry::GetProfileInt( section.c_str(), "DiskThrottle",  emulator.bDiskThrottle );
	emulator.bLoadFullscreen= (BOOL)CRegistry::GetProfileInt( section.c_str(), "LoadFullscreen",emulator.bLoadFullscreen );
	emulator.bPNGsnapshot   = (BOOL)CRegistry::GetProfileInt( section.c_str(), "PNGsnapshot",   emulator.bPNGsnapshot );
	emulator.bAutoIPS       = (BOOL)CRegistry::GetProfileInt( section.c_str(), "AutoIPS",       emulator.bAutoIPS );

// Graphic
	section = "Graphics";

	graphics.bAspect        = (BOOL)CRegistry::GetProfileInt( section.c_str(), "Aspect",        graphics.bAspect );
	graphics.bAllSprite     = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SpriteMax",     graphics.bAllSprite );
	graphics.bAllLine       = (BOOL)CRegistry::GetProfileInt( section.c_str(), "AllLine",       graphics.bAllLine );
	graphics.bFPSDisp       = (BOOL)CRegistry::GetProfileInt( section.c_str(), "FPSDisp",       graphics.bFPSDisp );
	graphics.bTVFrame       = (BOOL)CRegistry::GetProfileInt( section.c_str(), "TVFrameMode",   graphics.bTVFrame );
	graphics.bScanline      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "ScanlineMode",  graphics.bScanline );
	graphics.nScanlineColor =       CRegistry::GetProfileInt( section.c_str(), "ScanlineColor", graphics.nScanlineColor );
	graphics.bSyncDraw      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SyncDraw",      graphics.bSyncDraw );
	graphics.bFitZoom       = (BOOL)CRegistry::GetProfileInt( section.c_str(), "MaxZoom",       graphics.bFitZoom );

	graphics.bLeftClip      = (BOOL)CRegistry::GetProfileInt( section.c_str(), "LeftClip",      graphics.bLeftClip );

	graphics.bWindowVSync   = (BOOL)CRegistry::GetProfileInt( section.c_str(), "WindowVSync",   graphics.bWindowVSync );
	graphics.bSyncNoSleep   = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SyncNoSleep",   graphics.bSyncNoSleep );

	graphics.bDiskAccessLamp= (BOOL)CRegistry::GetProfileInt( section.c_str(), "DiskAccessLamp",graphics.bDiskAccessLamp );

	graphics.bDoubleSize    = (BOOL)CRegistry::GetProfileInt( section.c_str(), "DoubleSize",    graphics.bDoubleSize );
	graphics.bSystemMemory  = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SystemMemory",  graphics.bSystemMemory );
	graphics.bUseHEL        = (BOOL)CRegistry::GetProfileInt( section.c_str(), "UseHEL",        graphics.bUseHEL );

	graphics.bNoSquareList  = (BOOL)CRegistry::GetProfileInt( section.c_str(), "NoSquareList",  graphics.bNoSquareList );

	graphics.nGraphicsFilter=       CRegistry::GetProfileInt( section.c_str(), "GraphicsFilter",graphics.nGraphicsFilter );

	graphics.dwDisplayWidth  = (DWORD)CRegistry::GetProfileInt( section.c_str(), "DisplayWidth",  graphics.dwDisplayWidth );
	graphics.dwDisplayHeight = (DWORD)CRegistry::GetProfileInt( section.c_str(), "DisplayHeight", graphics.dwDisplayHeight );
	graphics.dwDisplayDepth  = (DWORD)CRegistry::GetProfileInt( section.c_str(), "DisplayDepth",  graphics.dwDisplayDepth );
	graphics.dwDisplayRate   = (DWORD)CRegistry::GetProfileInt( section.c_str(), "DisplayRate",   graphics.dwDisplayRate );

	graphics.bPaletteFile = (BOOL)CRegistry::GetProfileInt( section.c_str(), "PaletteUse", graphics.bPaletteFile );

	if( CRegistry::GetProfileString( section.c_str(), "PaletteFile", szTemp, sizeof(szTemp) ) )
		::strcpy( graphics.szPaletteFile, szTemp );

// Sound
	section = "Sound";

	sound.bEnable     = (BOOL)CRegistry::GetProfileInt( section.c_str(), "Enable",       sound.bEnable );
	sound.nRate       =       CRegistry::GetProfileInt( section.c_str(), "SamplingRate", sound.nRate );
	sound.nBits       =       CRegistry::GetProfileInt( section.c_str(), "SamplingBits", sound.nBits );
	sound.nBufferSize =       CRegistry::GetProfileInt( section.c_str(), "BufferSize",   sound.nBufferSize );
	sound.nFilterType =       CRegistry::GetProfileInt( section.c_str(), "FilterType",   sound.nFilterType );

	sound.bChangeTone = (BOOL)CRegistry::GetProfileInt( section.c_str(), "ChangeTone",   sound.bChangeTone );

	sound.bDisableVolumeEffect = (BOOL)CRegistry::GetProfileInt( section.c_str(), "DisableVolumeEffect",   sound.bDisableVolumeEffect );
	sound.bExtraSoundEnable = (BOOL)CRegistry::GetProfileInt( section.c_str(), "ExtraSoundEnable",   sound.bExtraSoundEnable );

	if( CRegistry::GetProfileBinary( section.c_str(), "Volume", szTemp, sizeof(sound.nVolume) ) )
		::memcpy( sound.nVolume, szTemp, sizeof(sound.nVolume) );

// ShortCuts
	section = "ShortCut";
	for( i = 0; i < sizeof(shortcut.nShortCut)/(16*sizeof(WORD)); i++ ) {
		::wsprintf( keys, "Tbl%02d", i );
		if( CRegistry::GetProfileBinary( section.c_str(), keys, szTemp, 16*sizeof(WORD) ) )
			::memcpy( &shortcut.nShortCut[i*16], szTemp, 16*sizeof(WORD) );
	}

// Controllers
	for( i = 0; i < 4; i++ ) {
		::wsprintf( keys, "Controller %d", i+1 );
		if( CRegistry::GetProfileBinary( keys, "Keys", szKeyTemp, 64*sizeof(WORD) ) ) {
			::memcpy( controller.nButton[i], szKeyTemp, 64*sizeof(WORD) );
		} else if( CRegistry::GetProfileBinary( keys, "Keys", szKeyTemp, 32*sizeof(WORD) ) ) {
			// �Â��ݒ��������p���ׂ̑[�u
			::memcpy( &controller.nButton[i][ 0], &szKeyTemp[ 0], 16*sizeof(WORD) );
			::memcpy( &controller.nButton[i][32], &szKeyTemp[16], 16*sizeof(WORD) );
			::memcpy( controller.nButton[i], szKeyTemp, 32*sizeof(WORD) );
		} else if( CRegistry::GetProfileBinary( keys, "Keys", szKeyTemp, 20*sizeof(WORD) ) ) {
			// �Â��ݒ��������p���ׂ̑[�u
			::memcpy( &controller.nButton[i][ 0], &szKeyTemp[ 0], 10*sizeof(WORD) );
			::memcpy( &controller.nButton[i][32], &szKeyTemp[10], 10*sizeof(WORD) );
			// Mic�̕ύX
			if( i == 1 ) {
				controller.nButton[i][10] = szKeyTemp[ 8];
				controller.nButton[i][ 8] = 0;
				controller.nButton[i][42] = szKeyTemp[18];
				controller.nButton[i][40] = 0;
			}
		} else if( CRegistry::GetProfileBinary( keys, "Keys", szKeyTemp, 10*sizeof(WORD) ) ) {
			// �Â��ݒ��������p���ׂ̑[�u
			::memcpy( controller.nButton[i], szKeyTemp, 10*sizeof(WORD) );
			// Mic�̕ύX
			if( i == 1 ) {
				controller.nButton[i][10] = szKeyTemp[ 8];
				controller.nButton[i][ 8] = 0;
			}
		}
		if( CRegistry::GetProfileBinary( keys, "Rapid", szTemp, 2*sizeof(WORD) ) )
			::memcpy( controller.nRapid[i], szTemp, 2*sizeof(WORD) );
	}

// ExControllers
	for( i = 0; i < 4; i++ ) {
		if( i == 0 ) section = "Crazy Climber";
		if( i == 1 ) section = "Family Trainer";
		if( i == 2 ) section = "Exciting Boxing";
		if( i == 3 ) section = "Mahjang";

		if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 64*sizeof(WORD) ) ) {
			::memcpy( controller.nExButton[i], szKeyTemp, 64*sizeof(WORD) );
		} else if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 32*sizeof(WORD) ) ) {
			::memcpy( &controller.nExButton[i][ 0], &szKeyTemp[ 0], 16*sizeof(WORD) );
			::memcpy( &controller.nExButton[i][32], &szKeyTemp[16], 16*sizeof(WORD) );
		} else if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 20*sizeof(WORD) ) ) {
			// �Â��ݒ��������p���ׂ̑[�u
			::memcpy( &controller.nExButton[i][ 0], &szKeyTemp[ 0], 10*sizeof(WORD) );
			::memcpy( &controller.nExButton[i][32], &szKeyTemp[10], 10*sizeof(WORD) );
		} else if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 10*sizeof(WORD) ) ) {
			// �Â��ݒ��������p���ׂ̑[�u
			::memcpy( controller.nExButton[i], szKeyTemp, 10*sizeof(WORD) );
		}
	}

// NSF Contoller
	section = "NSF controller";
	if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 64*sizeof(WORD) ) ) {
		::memcpy( controller.nNsfButton, szKeyTemp, 64*sizeof(WORD) );
	} else if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 32*sizeof(WORD) ) ) {
		::memcpy( &controller.nNsfButton[ 0], &szKeyTemp[ 0], 16*sizeof(WORD) );
		::memcpy( &controller.nNsfButton[32], &szKeyTemp[16], 16*sizeof(WORD) );
	} else if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 20*sizeof(WORD) ) ) {
		// �Â��ݒ��������p���ׂ̑[�u
		::memcpy( &controller.nNsfButton[ 0], &szKeyTemp[ 0], 10*sizeof(WORD) );
		::memcpy( &controller.nNsfButton[32], &szKeyTemp[10], 10*sizeof(WORD) );
	}

// VS-Unisystem
	section = "VS-Unisystem";
	if( CRegistry::GetProfileBinary( section.c_str(), "Keys", szKeyTemp, 64*sizeof(WORD) ) ) {
		::memcpy( controller.nVSUnisystem, szKeyTemp, 64*sizeof(WORD) );
	}

// Movie
	section = "Movie";
	if( CRegistry::GetProfileBinary( section.c_str(), "UsePlayer", szTemp, sizeof(movie.bUsePlayer) ) )
		::memcpy( movie.bUsePlayer, szTemp, sizeof(movie.bUsePlayer) );
	movie.bResetRec = (BOOL)CRegistry::GetProfileInt( section.c_str(), "ResetRec", movie.bResetRec );
	movie.bRerecord = (BOOL)CRegistry::GetProfileInt( section.c_str(), "Rerecord", movie.bRerecord );
	movie.bLoopPlay = (BOOL)CRegistry::GetProfileInt( section.c_str(), "LoopPlay", movie.bLoopPlay );
	movie.bPadDisplay = (BOOL)CRegistry::GetProfileInt( section.c_str(), "PadDisplay", movie.bPadDisplay );
	movie.bTimeDisplay = (BOOL)CRegistry::GetProfileInt( section.c_str(), "TimeDisplay", movie.bTimeDisplay );

// Launcher
	section = "Launcher";

	if( CRegistry::GetProfileBinary( section.c_str(), "WindowPos", (LPBYTE)&rc, sizeof(RECT) ) )
		launcher.rcWindowPos = rc;
	if( CRegistry::GetProfileBinary( section.c_str(), "ColumnView", szTemp, sizeof(launcher.bHeaderView) ) )
		::memcpy( launcher.bHeaderView, szTemp, sizeof(launcher.bHeaderView) );
	if( CRegistry::GetProfileBinary( section.c_str(), "ColumnOrder", szTemp, sizeof(launcher.nHeaderOrder) ) )
		::memcpy( launcher.nHeaderOrder, szTemp, sizeof(launcher.nHeaderOrder) );
	if( CRegistry::GetProfileBinary( section.c_str(), "ColumnWidth", szTemp, sizeof(launcher.nHeaderWidth) ) )
		::memcpy( launcher.nHeaderWidth, szTemp, sizeof(launcher.nHeaderWidth) );

	launcher.nListSelect = CRegistry::GetProfileInt( section.c_str(), "ListSelect",  launcher.nListSelect );

	launcher.bSortDir  = (BOOL)CRegistry::GetProfileInt( section.c_str(), "SortDir",  launcher.bSortDir );
	launcher.nSortType =       CRegistry::GetProfileInt( section.c_str(), "SortType", launcher.nSortType );

	if( CRegistry::GetProfileBinary( section.c_str(), "ColumnSort", szTemp, sizeof(launcher.nHeaderWidth) ) )
		::memcpy( launcher.nHeaderWidth, szTemp, sizeof(launcher.nHeaderWidth) );

	if( CRegistry::GetProfileBinary( section.c_str(), "FolderUse", szTemp, sizeof(launcher.bFolderUse) ) )
		::memcpy( launcher.bFolderUse, szTemp, sizeof(launcher.bFolderUse) );

	for( i = 0; i < 16; i++ ) {
		::wsprintf( keys, "Folder%02d", i );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( launcher.szFolder[i], szTemp );
	}

	if( CRegistry::GetProfileString( section.c_str(), "LastSelect", szTemp, sizeof(szTemp) ) )
		::strcpy( launcher.szLastSelect, szTemp );

	launcher.bActivePause = (BOOL)CRegistry::GetProfileInt( section.c_str(), "ActivePause",  launcher.bActivePause );

// ExtraSound
	section = "ExtraSound";

	for( i = ESF_MOEPRO_STRIKE; i <= ESF_MOEPRO_WA; i++ ) {
		::wsprintf( keys, "Moepro%02d", i );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( extsound.szExtSoundFile[i], szTemp );
	}

	for( i = ESF_MOETENNIS_00, j = 0; i <= ESF_MOETENNIS_12; i++, j++ ) {
		::wsprintf( keys, "Moetennis%02d", j );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( extsound.szExtSoundFile[i], szTemp );
	}

	for( i = ESF_DISKSYSTEM_BOOT, j = 0; i <= ESF_DISKSYSTEM_SEEKEND; i++, j++ ) {
		::wsprintf( keys, "DiskSound%02d", j );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( extsound.szExtSoundFile[i], szTemp );
	}

// NetPlay
	section = "Netplay";

	if( CRegistry::GetProfileBinary( section.c_str(), "ChatPos", (LPBYTE)&rc, sizeof(RECT) ) )
		netplay.rcChatPos = rc;

	if( CRegistry::GetProfileString( section.c_str(), "NickName", szTemp, sizeof(szTemp) ) )
		::strcpy( netplay.szNick, szTemp );

	netplay.nRecentPort = CRegistry::GetProfileInt( section.c_str(), "RecnetPortNum", netplay.nRecentPort );
	for( i = 0; i < netplay.nRecentPort; i++ ) {
		::wsprintf( keys, "RecentPort%02d", i );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( netplay.szRecentPort[i], szTemp );
	}

	netplay.nRecentHost = CRegistry::GetProfileInt( section.c_str(), "RecnetHostNum", netplay.nRecentHost );
	for( i = 0; i < netplay.nRecentHost; i++ ) {
		::wsprintf( keys, "RecentHost%02d", i );
		if( CRegistry::GetProfileString( section.c_str(), keys, szTemp, sizeof(szTemp) ) )
			::strcpy( netplay.szRecentHost[i], szTemp );
	}
	*/
}

void	CConfig::Save()
{
INT	i;
string	section;
CHAR	keys[64];

/*
// General
	section = "General";

	CRegistry::WriteProfileInt   ( section.c_str(), "DoubleExecute",   general.bDoubleExecute );
	CRegistry::WriteProfileInt   ( section.c_str(), "StartupLauncher", general.bStartupLauncher );

//	CRegistry::WriteProfileInt( section.c_str(), "WindowPosSave",	general.bWindowSave );
	CRegistry::WriteProfileInt   ( section.c_str(), "WindowZoom",	general.bWindowZoom );
	CRegistry::WriteProfileBinary( section.c_str(), "WindowPos",	(LPBYTE)&general.rcWindowPos, sizeof(RECT) );
	CRegistry::WriteProfileInt   ( section.c_str(), "ScreenZoom",	general.nScreenZoom );

	CRegistry::WriteProfileBinary( section.c_str(), "SearchDialogPos", (LPBYTE)&general.rcSearchDlgPos, sizeof(RECT) );

	CRegistry::WriteProfileBinary( section.c_str(), "PatternViewPos", (LPBYTE)&general.rcPatternViewPos, sizeof(RECT) );
	CRegistry::WriteProfileBinary( section.c_str(), "NameTableViewPos", (LPBYTE)&general.rcNameTableViewPos, sizeof(RECT) );
	CRegistry::WriteProfileBinary( section.c_str(), "PaletteViewPos", (LPBYTE)&general.rcPaletteViewPos, sizeof(RECT) );
	CRegistry::WriteProfileBinary( section.c_str(), "MemoryViewPos", (LPBYTE)&general.rcMemoryViewPos, sizeof(RECT) );

	CRegistry::WriteProfileBinary( section.c_str(), "BarcodePos", (LPBYTE)&general.rcBarcodePos, sizeof(RECT) );

	CRegistry::WriteProfileBinary( section.c_str(), "PaletteEditPos", (LPBYTE)&general.rcPaletteEditPos, sizeof(RECT) );

	CRegistry::WriteProfileInt   ( section.c_str(), "JoyAxisDisable", general.nJoyAxisDisable );
	CRegistry::WriteProfileBinary( section.c_str(), "JoyAxisSetting", (LPBYTE)general.JoyAxisSetting, 16*sizeof(WORD) );

	CRegistry::WriteProfileInt   ( section.c_str(), "NoJoystickID", general.bNoJoystickID );

// Paths
	section = "Path";

	CRegistry::WriteProfileInt( section.c_str(), "RomPathUse",      path.bRomPath );
	CRegistry::WriteProfileInt( section.c_str(), "SavePathUse",     path.bSavePath );
	CRegistry::WriteProfileInt( section.c_str(), "StatePathUse",    path.bStatePath );
	CRegistry::WriteProfileInt( section.c_str(), "SnapshotPathUse", path.bSnapshotPath );
	CRegistry::WriteProfileInt( section.c_str(), "MoviePathUse",    path.bMoviePath );
	CRegistry::WriteProfileInt( section.c_str(), "WavePathUse",     path.bWavePath );
	CRegistry::WriteProfileInt( section.c_str(), "CheatPathUse",    path.bCheatPath );
	CRegistry::WriteProfileInt( section.c_str(), "IpsPathUse",      path.bIpsPath );

	CRegistry::WriteProfileString( section.c_str(), "RomPath",      path.szRomPath );
	CRegistry::WriteProfileString( section.c_str(), "SavePath",     path.szSavePath );
	CRegistry::WriteProfileString( section.c_str(), "StatePath",    path.szStatePath );
	CRegistry::WriteProfileString( section.c_str(), "SnapshotPath", path.szSnapshotPath );
	CRegistry::WriteProfileString( section.c_str(), "MoviePath",    path.szMoviePath );
	CRegistry::WriteProfileString( section.c_str(), "WavePath",     path.szWavePath );
	CRegistry::WriteProfileString( section.c_str(), "CheatPath",    path.szCheatPath );
	CRegistry::WriteProfileString( section.c_str(), "IpsPath",      path.szIpsPath );

// Emulation
	section = "Emulation";

	CRegistry::WriteProfileInt( section.c_str(), "IllegalOp",     emulator.bIllegalOp );
	CRegistry::WriteProfileInt( section.c_str(), "AutoFrameSkip", emulator.bAutoFrameSkip );
	CRegistry::WriteProfileInt( section.c_str(), "Throttle",      emulator.bThrottle );
	CRegistry::WriteProfileInt( section.c_str(), "ThrottleFPS",   emulator.nThrottleFPS );
	CRegistry::WriteProfileInt( section.c_str(), "Background",    emulator.bBackground );
	CRegistry::WriteProfileInt( section.c_str(), "Priority",      emulator.nPriority );
	CRegistry::WriteProfileInt( section.c_str(), "FourPlayer",    emulator.bFourPlayer );
	CRegistry::WriteProfileInt( section.c_str(), "CrcCheck",      emulator.bCrcCheck );
	CRegistry::WriteProfileInt( section.c_str(), "DiskThrottle",  emulator.bDiskThrottle );
	CRegistry::WriteProfileInt( section.c_str(), "LoadFullscreen",emulator.bLoadFullscreen );
	CRegistry::WriteProfileInt( section.c_str(), "PNGsnapshot",   emulator.bPNGsnapshot );
	CRegistry::WriteProfileInt( section.c_str(), "AutoIPS",       emulator.bAutoIPS );

// Graphic
	section = "Graphics";

	CRegistry::WriteProfileInt( section.c_str(), "Aspect",        graphics.bAspect );
	CRegistry::WriteProfileInt( section.c_str(), "SpriteMax",     graphics.bAllSprite );
	CRegistry::WriteProfileInt( section.c_str(), "AllLine",       graphics.bAllLine );
	CRegistry::WriteProfileInt( section.c_str(), "FPSDisp",       graphics.bFPSDisp );
	CRegistry::WriteProfileInt( section.c_str(), "TVFrameMode",   graphics.bTVFrame );
	CRegistry::WriteProfileInt( section.c_str(), "ScanlineMode",  graphics.bScanline );
	CRegistry::WriteProfileInt( section.c_str(), "ScanlineColor", graphics.nScanlineColor );
	CRegistry::WriteProfileInt( section.c_str(), "SyncDraw",      graphics.bSyncDraw );
	CRegistry::WriteProfileInt( section.c_str(), "MaxZoom",       graphics.bFitZoom );

	CRegistry::WriteProfileInt( section.c_str(), "LeftClip",      graphics.bLeftClip );

	CRegistry::WriteProfileInt( section.c_str(), "WindowVSync",   graphics.bWindowVSync );
	CRegistry::WriteProfileInt( section.c_str(), "SyncNoSleep",   graphics.bSyncNoSleep );

	CRegistry::WriteProfileInt( section.c_str(), "DiskAccessLamp",graphics.bDiskAccessLamp );

	CRegistry::WriteProfileInt( section.c_str(), "DoubleSize",    graphics.bDoubleSize );
	CRegistry::WriteProfileInt( section.c_str(), "SystemMemory",  graphics.bSystemMemory );
	CRegistry::WriteProfileInt( section.c_str(), "UseHEL",        graphics.bUseHEL );

	CRegistry::WriteProfileInt( section.c_str(), "NoSquareList",  graphics.bNoSquareList );

	CRegistry::WriteProfileInt( section.c_str(), "GraphicsFilter",graphics.nGraphicsFilter );

	CRegistry::WriteProfileInt( section.c_str(), "DisplayWidth",  graphics.dwDisplayWidth );
	CRegistry::WriteProfileInt( section.c_str(), "DisplayHeight", graphics.dwDisplayHeight );
	CRegistry::WriteProfileInt( section.c_str(), "DisplayDepth",  graphics.dwDisplayDepth );
	CRegistry::WriteProfileInt( section.c_str(), "DisplayRate",   graphics.dwDisplayRate );

	CRegistry::WriteProfileInt( section.c_str(), "PaletteUse",   graphics.bPaletteFile );
	CRegistry::WriteProfileString( section.c_str(), "PaletteFile", graphics.szPaletteFile );

// Sound
	section = "Sound";

	CRegistry::WriteProfileInt( section.c_str(), "Enable",       sound.bEnable );

	CRegistry::WriteProfileInt( section.c_str(), "DisableVolumeEffect", sound.bDisableVolumeEffect );
	CRegistry::WriteProfileInt( section.c_str(), "ExtraSoundEnable",    sound.bExtraSoundEnable );

	CRegistry::WriteProfileInt( section.c_str(), "SamplingRate", sound.nRate );
	CRegistry::WriteProfileInt( section.c_str(), "SamplingBits", sound.nBits );
	CRegistry::WriteProfileInt( section.c_str(), "BufferSize",   sound.nBufferSize );
	CRegistry::WriteProfileInt( section.c_str(), "FilterType",   sound.nFilterType );

	CRegistry::WriteProfileBinary( section.c_str(), "Volume", (LPBYTE)sound.nVolume, sizeof(sound.nVolume) );

// ShortCut
	section = "ShortCut";
	for( i = 0; i < sizeof(shortcut.nShortCut)/(16*sizeof(WORD)); i++ ) {
		::wsprintf( keys, "TBL%02d", i );
		CRegistry::WriteProfileBinary( section.c_str(), keys, (LPBYTE)&shortcut.nShortCut[i*16], 16*sizeof(WORD) );
	}

// Controllers
	for( i = 0; i < 4; i++ ) {
		::wsprintf( keys, "Controller %01d", i+1 );
		CRegistry::WriteProfileBinary( keys, "Keys", (LPBYTE)controller.nButton[i], 64*sizeof(WORD) );
		CRegistry::WriteProfileBinary( keys, "Rapid", (LPBYTE)controller.nRapid[i], 2*sizeof(WORD) );
	}

// ExControllers
	section = "Crazy Climber";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nExButton[0], 64*sizeof(WORD) );
	section = "Family Trainer";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nExButton[1], 64*sizeof(WORD) );
	section = "Exciting Boxing";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nExButton[2], 64*sizeof(WORD) );
	section = "Mahjang";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nExButton[3], 64*sizeof(WORD) );

// NSF Contoller
	section = "NSF controller";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nNsfButton, 64*sizeof(WORD) );

// VS-Unisystem
	section = "VS-Unisystem";
	CRegistry::WriteProfileBinary( section.c_str(), "Keys", (LPBYTE)controller.nVSUnisystem, 64*sizeof(WORD) );

// Movie
	section = "Movie";
	CRegistry::WriteProfileBinary( section.c_str(), "UsePlayer", (LPBYTE)movie.bUsePlayer, sizeof(movie.bUsePlayer) );
	CRegistry::WriteProfileInt( section.c_str(), "ResetRec", movie.bResetRec );
	CRegistry::WriteProfileInt( section.c_str(), "Rerecord", movie.bRerecord );
	CRegistry::WriteProfileInt( section.c_str(), "LoopPlay", movie.bLoopPlay );
	CRegistry::WriteProfileInt( section.c_str(), "PadDisplay", movie.bPadDisplay );
	CRegistry::WriteProfileInt( section.c_str(), "TimeDisplay", movie.bTimeDisplay );

// Launcher
	section = "Launcher";

	CRegistry::WriteProfileBinary( section.c_str(), "WindowPos", (LPBYTE)&launcher.rcWindowPos, sizeof(RECT) );

	CRegistry::WriteProfileBinary( section.c_str(), "ColumnView",  (LPBYTE)launcher.bHeaderView,  sizeof(launcher.bHeaderView) );
	CRegistry::WriteProfileBinary( section.c_str(), "ColumnOrder", (LPBYTE)launcher.nHeaderOrder, sizeof(launcher.nHeaderOrder) );
	CRegistry::WriteProfileBinary( section.c_str(), "ColumnWidth", (LPBYTE)launcher.nHeaderWidth, sizeof(launcher.nHeaderWidth) );

	CRegistry::WriteProfileInt( section.c_str(), "ListSelect", launcher.nListSelect );

	CRegistry::WriteProfileInt( section.c_str(), "SortDir",  launcher.bSortDir );
	CRegistry::WriteProfileInt( section.c_str(), "SortType", launcher.nSortType );

	CRegistry::WriteProfileBinary( section.c_str(), "FolderUse", (LPBYTE)launcher.bFolderUse, sizeof(launcher.bFolderUse) );

	for( i = 0; i < 16; i++ ) {
		::wsprintf( keys, "Folder%02d", i );
		CRegistry::WriteProfileString( section.c_str(), keys, launcher.szFolder[i] );
	}

	CRegistry::WriteProfileString( section.c_str(), "LastSelect", launcher.szLastSelect );

	CRegistry::WriteProfileInt( section.c_str(), "ActivePause",  launcher.bActivePause );

// NetPlay
	section = "Netplay";

	CRegistry::WriteProfileBinary( section.c_str(), "ChatPos", (LPBYTE)&netplay.rcChatPos, sizeof(RECT) );

	CRegistry::WriteProfileString( section.c_str(), "NickName", netplay.szNick );

	CRegistry::WriteProfileInt( section.c_str(), "RecnetPortNum", netplay.nRecentPort );
	for( i = 0; i < 16; i++ ) {
		::wsprintf( keys, "RecentPort%02d", i );
		CRegistry::WriteProfileString( section.c_str(), keys, netplay.szRecentPort[i] );
	}
	CRegistry::WriteProfileInt( section.c_str(), "RecnetHostNum", netplay.nRecentHost );
	for( i = 0; i < 16; i++ ) {
		::wsprintf( keys, "RecentHost%02d", i );
		CRegistry::WriteProfileString( section.c_str(), keys, netplay.szRecentHost[i] );
	}*/
}

BOOL	CConfig::ButtonCheck( INT nNo, INT nID )
{
	/*
	if( m_bKeyboardDisable ) {
		if( (Config.controller.nButton[nNo][nID+ 0] >= 256) && DirectInput.m_Sw[Config.controller.nButton[nNo][nID+ 0]]
		 || (Config.controller.nButton[nNo][nID+32] >= 256) && DirectInput.m_Sw[Config.controller.nButton[nNo][nID+32]] )
			return	TRUE;
	} else {
		if( Config.controller.nButton[nNo][nID+ 0] && DirectInput.m_Sw[Config.controller.nButton[nNo][nID+ 0]]
		 || Config.controller.nButton[nNo][nID+32] && DirectInput.m_Sw[Config.controller.nButton[nNo][nID+32]] )
			return	TRUE;
	}
	*/
	return	FALSE;
}

BOOL	CConfig::ExButtonCheck( INT nNo, INT nID )
{
	/*
	if( Config.controller.nExButton[nNo][nID+ 0] && DirectInput.m_Sw[Config.controller.nExButton[nNo][nID+ 0]]
	 || Config.controller.nExButton[nNo][nID+32] && DirectInput.m_Sw[Config.controller.nExButton[nNo][nID+32]] )
		return	TRUE;*/
	return	FALSE;
}

BOOL	CConfig::NsfButtonCheck( INT nID )
{
	/*
	if( Config.controller.nNsfButton[nID+ 0] && DirectInput.m_Sw[Config.controller.nNsfButton[nID+ 0]]
	 || Config.controller.nNsfButton[nID+32] && DirectInput.m_Sw[Config.controller.nNsfButton[nID+32]] )
		return	TRUE;*/
	return	FALSE;
}

BOOL	CConfig::ButtonCheck( INT nID, WORD* pKey )
{
	/*
	if( pKey[nID+ 0] && DirectInput.m_Sw[pKey[nID+ 0]]
	 || pKey[nID+32] && DirectInput.m_Sw[pKey[nID+32]] )
		return	TRUE;*/
	return	FALSE;
}

string	CConfig::ShortcutToKeyName( INT nShortcutKey )
{
	string	str;
	if( nShortcutKey == 0 ) {
		str = "----";
	} else {
		if( nShortcutKey & CCfgShortCut::K_ALT )
			str = str + "Alt+";
		if( nShortcutKey & CCfgShortCut::K_CTRL )
			str = str + "Ctrl+";
		if( nShortcutKey & CCfgShortCut::K_SHIFT )
			str = str + "Shift+";

		//str = str + DirectInput.SearchKeyName( nShortcutKey & 0x0FFF );
	}
	return	str;
}

/////////////////////////////////
CGameOption	GameOption;

void	CGameOption::Load( DWORD crc )
{
	/*
	CRegistry::SetRegistryKey( "GameOption.ini" );

	CHAR	szSection[256];
	::wsprintf( szSection, "%08X", crc );
	nRenderMethod =       CRegistry::GetProfileInt( szSection, "RenderMethod",    defRenderMethod );
	nIRQtype      =       CRegistry::GetProfileInt( szSection, "IRQtype",         defIRQtype );
	bFrameIRQ     = (BOOL)CRegistry::GetProfileInt( szSection, "FrameIRQ",        defFrameIRQ );
	bVideoMode    = (BOOL)CRegistry::GetProfileInt( szSection, "VideoMode",       defVideoMode );
	*/
}

void	CGameOption::Save( LPCSTR name, DWORD crc )
{
	/*
	CRegistry::SetRegistryKey( "GameOption.ini" );

	CHAR	szSection[256];
	::wsprintf( szSection, "%08X", crc );
	CRegistry::WriteProfileString( szSection, "Title", name );
	CRegistry::WriteProfileInt( szSection, "RenderMethod",    nRenderMethod );
	CRegistry::WriteProfileInt( szSection, "IRQtype",         nIRQtype );
	CRegistry::WriteProfileInt( szSection, "FrameIRQ",        (INT)bFrameIRQ );
	CRegistry::WriteProfileInt( szSection, "VideoMode",       (INT)bVideoMode );*/
}

void	CGameOption::Load( DWORD gid, DWORD mid )
{
	/*
	CRegistry::SetRegistryKey( "GameOption.ini" );

	CHAR	szSection[256];
	::wsprintf( szSection, "%08X%08X", gid, mid );
	nRenderMethod =       CRegistry::GetProfileInt( szSection, "RenderMethod",    defRenderMethod );
	nIRQtype      =       CRegistry::GetProfileInt( szSection, "IRQtype",         defIRQtype );
	bFrameIRQ     = (BOOL)CRegistry::GetProfileInt( szSection, "FrameIRQ",        defFrameIRQ );*/
}

void	CGameOption::Save( LPCSTR name, DWORD gid, DWORD mid )
{
	/*
	CRegistry::SetRegistryKey( "GameOption.ini" );

	CHAR	szSection[256];
	::wsprintf( szSection, "%08X%08X", gid, mid );
	CRegistry::WriteProfileString( szSection, "Title", name );
	CRegistry::WriteProfileInt( szSection, "RenderMethod",    nRenderMethod );
	CRegistry::WriteProfileInt( szSection, "IRQtype",         nIRQtype );
	CRegistry::WriteProfileInt( szSection, "FrameIRQ",        (INT)bFrameIRQ );*/
}

