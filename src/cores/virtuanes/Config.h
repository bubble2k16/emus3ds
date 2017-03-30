//
// �ݒ��ۑ��N���X
//
#ifndef	__CCONFIG_INCLUDED__
#define	__CCONFIG_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#define	DIRECTINPUT_VERSION	0x0700
//#include <dinput.h>

#include <string>
using namespace std;

#include "extsoundfile.h"

#include "typedef.h"
#include "macro.h"

class	CCfgGeneral
{
public:
	BOOL	bDoubleExecute;
	BOOL	bStartupLauncher;

	BOOL	bWindowSave;
	BOOL	bWindowZoom;
	RECT	rcWindowPos;

	BOOL	bScreenMode;
	BOOL	nScreenZoom;

	RECT	rcSearchDlgPos;

	RECT	rcPatternViewPos;
	RECT	rcNameTableViewPos;
	RECT	rcPaletteViewPos;
	RECT	rcMemoryViewPos;

	RECT	rcBarcodePos;
	RECT	rcPaletteEditPos;

	BOOL	bNoJoystickID;

	WORD	JoyAxisSetting[16];

	// �W���C�p�b�h�̎��̖�����
	//  0 : All enable
	//  1 : X���ȍ~�S��
	//  2 : Y���ȍ~�S��
	//  3 : Z���ȍ~�S��
	//  4 : RX���ȍ~�S��
	//  5 : RY���ȍ~�S��
	//  6 : RZ���ȍ~�S��
	INT	nJoyAxisDisable;

	void	Default() {
		bWindowSave = FALSE;
		bWindowZoom = FALSE;
		rcWindowPos.left = rcWindowPos.right =
		rcWindowPos.top = rcWindowPos.bottom = 0;

		bDoubleExecute = TRUE;
		bStartupLauncher = FALSE;

		bScreenMode = FALSE;
		nScreenZoom = 0;

		rcSearchDlgPos.left = rcSearchDlgPos.right =
		rcSearchDlgPos.top = rcSearchDlgPos.bottom = 0;

		rcPatternViewPos.left = rcPatternViewPos.right =
		rcPatternViewPos.top = rcPatternViewPos.bottom = 0;
		rcNameTableViewPos.left = rcNameTableViewPos.right =
		rcNameTableViewPos.top = rcNameTableViewPos.bottom = 0;
		rcPaletteViewPos.left = rcPaletteViewPos.right =
		rcPaletteViewPos.top = rcPaletteViewPos.bottom = 0;
		rcMemoryViewPos.left = rcMemoryViewPos.right =
		rcMemoryViewPos.top = rcMemoryViewPos.bottom = 0;

		rcBarcodePos.left = rcBarcodePos.right =
		rcBarcodePos.top = rcBarcodePos.bottom = 0;

		rcPaletteEditPos.left = rcPaletteEditPos.right =
		rcPaletteEditPos.top = rcPaletteEditPos.bottom = 0;

		bNoJoystickID = FALSE;
		nJoyAxisDisable = 0;

		for( INT i = 0; i < 16; i++ ) {
			JoyAxisSetting[i] = 0;
		}
	}
};

class	CCfgPath
{
public:
	BOOL	bRomPath;
	BOOL	bSavePath;
	BOOL	bStatePath;
	BOOL	bSnapshotPath;
	BOOL	bMoviePath;
	BOOL	bWavePath;
	BOOL	bCheatPath;
	BOOL	bIpsPath;

	CHAR	szRomPath     [_MAX_PATH];
	CHAR	szSavePath    [_MAX_PATH];
	CHAR	szStatePath   [_MAX_PATH];
	CHAR	szSnapshotPath[_MAX_PATH];
	CHAR	szMoviePath   [_MAX_PATH];
	CHAR	szWavePath    [_MAX_PATH];
	CHAR	szCheatPath   [_MAX_PATH];
	CHAR	szIpsPath     [_MAX_PATH];

	void	Default() {
		bRomPath = bSavePath =
		bStatePath = bSnapshotPath =
		bMoviePath = bWavePath =
		bCheatPath = bIpsPath = TRUE;

		/*
		::lstrcpy( szRomPath,      ".\\roms\\" );
		::lstrcpy( szSavePath,     ".\\save\\" );
		::lstrcpy( szStatePath,    ".\\state\\" );
		::lstrcpy( szSnapshotPath, ".\\snapshot\\" );
		::lstrcpy( szMoviePath,    ".\\movie\\" );
		::lstrcpy( szWavePath,     ".\\wave\\" );
		::lstrcpy( szCheatPath,    ".\\cheatcode\\" );
		::lstrcpy( szIpsPath,      ".\\ips\\" );
		*/
		strcpy( szRomPath,      ".\\roms\\" );
		strcpy( szSavePath,     ".\\save\\" );
		strcpy( szStatePath,    ".\\state\\" );
		strcpy( szSnapshotPath, ".\\snapshot\\" );
		strcpy( szMoviePath,    ".\\movie\\" );
		strcpy( szWavePath,     ".\\wave\\" );
		strcpy( szCheatPath,    ".\\cheatcode\\" );
		strcpy( szIpsPath,      ".\\ips\\" );
	}
};

class	CCfgEmulator
{
public:
	BOOL	bIllegalOp;
	BOOL	bAutoFrameSkip;
	BOOL	bThrottle;
	INT	nThrottleFPS;
	BOOL	bBackground;
	INT	nPriority;
	BOOL	bFourPlayer;
	BOOL	bCrcCheck;
	BOOL	bDiskThrottle;
	BOOL	bLoadFullscreen;
	BOOL	bPNGsnapshot;
	BOOL	bAutoIPS;

	void	Default() {
		bIllegalOp	= FALSE;
		bAutoFrameSkip	= TRUE;
		bThrottle	= TRUE;
		nThrottleFPS	= 120;		// 120FPS
		bBackground	= FALSE;
		nPriority	= 3;		// Normal
		bFourPlayer	= TRUE;		// TRUE:NES FALSE:Famicom
		bCrcCheck	= TRUE;
		bDiskThrottle	= TRUE;
		bLoadFullscreen	= FALSE;
		bPNGsnapshot	= FALSE;
		bAutoIPS	= FALSE;
	}
};

class	CCfgGraphics
{
public:
	BOOL	bAspect;
	BOOL	bAllSprite;
	BOOL	bAllLine;
	BOOL	bFPSDisp;
	BOOL	bTVFrame;
	BOOL	bScanline;
	INT	nScanlineColor;
	BOOL	bSyncDraw;
	BOOL	bFitZoom;

	BOOL	bLeftClip;

	BOOL	bWindowVSync;

	BOOL	bSyncNoSleep;

	BOOL	bDiskAccessLamp;

	BOOL	bDoubleSize;
	BOOL	bSystemMemory;
	BOOL	bUseHEL;

	BOOL	bNoSquareList;

	INT	nGraphicsFilter;

	DWORD	dwDisplayWidth;
	DWORD	dwDisplayHeight;
	DWORD	dwDisplayDepth;
	DWORD	dwDisplayRate;

	BOOL	bPaletteFile;
	CHAR	szPaletteFile[_MAX_PATH];

	void	Default() {
		bAspect		= FALSE;
		bAllSprite	= TRUE;
		bAllLine	= FALSE;
		bFPSDisp	= FALSE;
		bTVFrame	= FALSE;
		bScanline	= FALSE;
		nScanlineColor	= 75;
		bSyncDraw	= FALSE;
		bFitZoom	= FALSE;

		bLeftClip	= TRUE;

		bWindowVSync	= FALSE;
		bSyncNoSleep	= FALSE;

		bDiskAccessLamp	= FALSE;

		bDoubleSize	= FALSE;
		bSystemMemory	= FALSE;
		bUseHEL		= FALSE;

		bNoSquareList   = FALSE;

		nGraphicsFilter = 0;

		dwDisplayWidth	= 640;
		dwDisplayHeight	= 480;
		dwDisplayDepth	= 16;
		dwDisplayRate	= 0;

		bPaletteFile = FALSE;
		szPaletteFile[0] = 0;
	}
};

class	CCfgSound
{
public:
	BOOL	bEnable;
	INT	nRate;
	INT	nBits;
	INT	nBufferSize;

	INT		nFilterType;

	BOOL	bChangeTone;

	BOOL	bDisableVolumeEffect;
	BOOL	bExtraSoundEnable;

	//  0:Master
	//  1:Rectangle 1
	//  2:Rectangle 2
	//  3:Triangle
	//  4:Noise
	//  5:DPCM
	//  6:VRC6
	//  7:VRC7
	//  8:FDS
	//  9:MMC5
	// 10:N106
	// 11:FME7
	SHORT	nVolume[16];

	void	Default() {
		bEnable		= TRUE;
		nRate		= 22050;
		nBits		= 8;
		nBufferSize	= 4;

		nFilterType	= 0;

		bChangeTone	= FALSE;

		bDisableVolumeEffect = FALSE;
		bExtraSoundEnable = TRUE;

		for( INT i = 0; i < 16; i++ ) {
			nVolume[i] = 100;
		}
	}
};

class	CCfgShortCut
{
public:
	WORD	nShortCut[512];

	enum {
		K_ALT   = 0x8000,
		K_CTRL  = 0x4000,
		K_SHIFT = 0x2000,
	};

	void	Default() {
		for( INT i = 0; i < (sizeof(nShortCut)/sizeof(WORD)); i++ ) {
			nShortCut[i] = 0;
		}

/*
		// Main controls
		nShortCut[  0] = DIK_O+K_CTRL;		// ID_OPEN
		nShortCut[  1] = DIK_C+K_CTRL;		// ID_CLOSE
		nShortCut[  2] = DIK_L+K_CTRL;		// ID_LAUNCHER
		nShortCut[  3] = DIK_N+K_CTRL;		// ID_NETPLAY_CONNECT
		nShortCut[  4] = DIK_D+K_CTRL;		// ID_NETPLAY_DISCONNECT
		nShortCut[  5] = DIK_A+K_CTRL;		// ID_NETPLAY_CHAT
		nShortCut[  8] = DIK_I+K_CTRL;		// ID_ROMINFO
		nShortCut[  9] = DIK_W+K_CTRL;		// ID_WAVERECORD
		nShortCut[ 15] = DIK_X+K_CTRL;		// ID_EXIT

		// Emulation controls
		nShortCut[ 16] = DIK_F1;		// Hardware reset
		nShortCut[ 17] = DIK_F2;		// Software reset
		nShortCut[ 18] = DIK_P;			// Hardware pause
		nShortCut[ 19] = DIK_TAB;		// Throttle(toggled)
		nShortCut[ 20] = DIK_NUMPADENTER;	// Frame skip Auto
		nShortCut[ 21] = DIK_ADD;		// Frame skip +
		nShortCut[ 22] = DIK_SUBTRACT;		// Frame skip -

		nShortCut[ 23] = DIK_SPACE;		// One Frame step
		nShortCut[ 24] = DIK_BACKSPACE;		// Throttle(Not toggle)

		// State controls
		nShortCut[ 32] = DIK_L;			// State Load
		nShortCut[ 33] = DIK_S;			// State Save
		nShortCut[ 34] = DIK_F3;		// State Slot +
		nShortCut[ 35] = DIK_F4;		// State Slot -

		// Disk controls
		nShortCut[ 48] = DIK_5;			// Disk Eject
		nShortCut[ 49] = DIK_1;			// Disk 0 Side A
		nShortCut[ 50] = DIK_2;			// Disk 0 Side B
		nShortCut[ 51] = DIK_3;			// Disk 1 Side A
		nShortCut[ 52] = DIK_4;			// Disk 1 Side B
		nShortCut[120] = DIK_1+K_SHIFT;	// Disk 2 Side A
		nShortCut[121] = DIK_2+K_SHIFT;	// Disk 2 Side B
		nShortCut[122] = DIK_3+K_SHIFT;	// Disk 3 Side A
		nShortCut[123] = DIK_4+K_SHIFT;	// Disk 3 Side B

		// Movie controls
		nShortCut[ 56] = DIK_P+K_ALT;		// Movie Play
		nShortCut[ 57] = DIK_R+K_ALT;		// Movie Rec
		nShortCut[ 58] = DIK_A+K_ALT;		// Movie Rec Append
		nShortCut[ 59] = DIK_S+K_ALT;		// Movie Stop
		nShortCut[ 60] = DIK_M+K_ALT;		// Movie Info

		// Screen controls
		nShortCut[ 64] = DIK_F5;		// Zoom x1
		nShortCut[ 65] = DIK_F6;		// Zoom x2
		nShortCut[ 66] = DIK_F7;		// Zoom x3
		nShortCut[ 67] = DIK_F8;		// Zoom x4
		nShortCut[ 68] = DIK_RETURN+K_ALT;	// Fullscreen

		// Sound controls
		nShortCut[ 72] = DIK_BACK+K_CTRL;	// Mute Master
		nShortCut[ 73] = DIK_1+K_CTRL;		// Mute Rectangle #1
		nShortCut[ 74] = DIK_2+K_CTRL;		// Mute Rectangle #2
		nShortCut[ 75] = DIK_3+K_CTRL;		// Mute Triangle
		nShortCut[ 76] = DIK_4+K_CTRL;		// Mute Noise
		nShortCut[ 77] = DIK_5+K_CTRL;		// Mute Dpcm
		nShortCut[ 78] = DIK_6+K_CTRL;		// Mute External #1
		nShortCut[ 79] = DIK_7+K_CTRL;		// Mute External #2
		nShortCut[ 80] = DIK_8+K_CTRL;		// Mute External #3
		nShortCut[ 81] = DIK_9+K_CTRL;		// Mute External #4
		nShortCut[ 82] = DIK_0+K_CTRL;		// Mute External #5
		nShortCut[ 83] = DIK_MINUS+K_CTRL;	// Mute External #6
		nShortCut[ 84] = DIK_CIRCUMFLEX+K_CTRL;	// Mute External #7
		nShortCut[ 85] = DIK_YEN+K_CTRL;	// Mute External #8

		// Tape controls
		nShortCut[ 90] = 0;			// Tape Play
		nShortCut[ 91] = 0;			// Tape Rec
		nShortCut[ 92] = 0;			// Tape Stop

		// Other controls
		nShortCut[ 96] = DIK_P+K_CTRL;		// Snapshot
		nShortCut[ 97] = DIK_F11;		// FPSDISP
		nShortCut[ 98] = DIK_F12+K_CTRL;	// TV Aspect
		nShortCut[ 99] = DIK_F11+K_CTRL;	// TV frame
		nShortCut[100] = DIK_F12;		// Scanline
		nShortCut[101] = DIK_F9+K_CTRL;		// Show 240 lines
		nShortCut[102] = DIK_F9;		// All sprites
		nShortCut[103] = DIK_F10;		// Sync draw
		nShortCut[104] = DIK_F10+K_CTRL;	// Fit screen
		nShortCut[105] = 0;			// Left clip

		// Cheat
		nShortCut[110] = DIK_HOME+K_CTRL;	// Search
		nShortCut[111] = DIK_END+K_CTRL;	// Cheat
		nShortCut[112] = DIK_INSERT;		// Cheat enable
		nShortCut[113] = DIK_DELETE;		// Cheat disable
		nShortCut[114] = 0;			// GameGenie

		// Tools
		nShortCut[116] = 0;			// Pattern Viewer
		nShortCut[117] = 0;			// NameTable Viewer
		nShortCut[118] = 0;			// Palette Viewer

		nShortCut[119] = 0;			// Memory Viewer

		// Quick Load/Save
		nShortCut[256] = DIK_NUMPAD0+K_CTRL;	// QuickLoad Slot0
		nShortCut[257] = DIK_NUMPAD1+K_CTRL;	// QuickLoad Slot1
		nShortCut[258] = DIK_NUMPAD2+K_CTRL;	// QuickLoad Slot2
		nShortCut[259] = DIK_NUMPAD3+K_CTRL;	// QuickLoad Slot3
		nShortCut[260] = DIK_NUMPAD4+K_CTRL;	// QuickLoad Slot4
		nShortCut[261] = DIK_NUMPAD5+K_CTRL;	// QuickLoad Slot5
		nShortCut[262] = DIK_NUMPAD6+K_CTRL;	// QuickLoad Slot6
		nShortCut[263] = DIK_NUMPAD7+K_CTRL;	// QuickLoad Slot7
		nShortCut[264] = DIK_NUMPAD8+K_CTRL;	// QuickLoad Slot8
		nShortCut[265] = DIK_NUMPAD9+K_CTRL;	// QuickLoad Slot9

		nShortCut[266] = DIK_NUMPAD0+K_ALT;	// QuickSave Slot0
		nShortCut[267] = DIK_NUMPAD1+K_ALT;	// QuickSave Slot1
		nShortCut[268] = DIK_NUMPAD2+K_ALT;	// QuickSave Slot2
		nShortCut[269] = DIK_NUMPAD3+K_ALT;	// QuickSave Slot3
		nShortCut[270] = DIK_NUMPAD4+K_ALT;	// QuickSave Slot4
		nShortCut[271] = DIK_NUMPAD5+K_ALT;	// QuickSave Slot5
		nShortCut[272] = DIK_NUMPAD6+K_ALT;	// QuickSave Slot6
		nShortCut[273] = DIK_NUMPAD7+K_ALT;	// QuickSave Slot7
		nShortCut[274] = DIK_NUMPAD8+K_ALT;	// QuickSave Slot8
		nShortCut[275] = DIK_NUMPAD9+K_ALT;	// QuickSave Slot9*/
	}
};

class	CCfgLanguage
{
public:
	LCID	lcID;
	CHAR	szLanguage[256];

	void	Default() {
		lcID = NULL;
		szLanguage[0] = 0;
	}
};

class	CCfgController
{
public:
	WORD	nButton[4][64];
	WORD	nRapid [4][ 2];
	// 0:Crazy Climber
	// 1:Famly Trainer
	// 2:Exciting Boxing
	// 3:Mahjang
	WORD	nExButton[4][64];
	WORD	nNsfButton[64];
	WORD	nVSUnisystem[64];

	void	Default() {
		DefaultController1();
		DefaultController2();
		DefaultController3();
		DefaultController4();
		DefaultExController0();
		DefaultExController1();
		DefaultExController2();
		DefaultExController3();
		DefaultNsfController();
		DefaultVSUnisystem();
	}

	void	DefaultController1() {
		for( INT i = 0; i < 64; i++ )
			nButton[0][i] = 0;
/*
		nButton[0][ 0] = DIK_UP;
		nButton[0][ 1] = DIK_DOWN;
		nButton[0][ 2] = DIK_LEFT;
		nButton[0][ 3] = DIK_RIGHT;
		nButton[0][ 4] = DIK_X;	// A
		nButton[0][ 5] = DIK_Z;	// B
		nButton[0][ 6] = 0;	// A Rapid
		nButton[0][ 7] = 0;	// B Rapid
		nButton[0][ 8] = DIK_RSHIFT; // SELECT
		nButton[0][ 9] = DIK_RETURN; // START

		nRapid[0][0] = 0;	// A Rapid speed
		nRapid[0][1] = 0;	// B Rapid speed*/
	}

	void	DefaultController2() {
		for( INT i = 0; i < 64; i++ )
			nButton[1][i] = 0;
/*
		nButton[1][ 0] = DIK_NUMPAD8;
		nButton[1][ 1] = DIK_NUMPAD2;
		nButton[1][ 2] = DIK_NUMPAD4;
		nButton[1][ 3] = DIK_NUMPAD6;
		nButton[1][ 4] = DIK_N;	// A
		nButton[1][ 5] = DIK_B;	// B
		nButton[1][ 6] = 0;	// A Rapid
		nButton[1][ 7] = 0;	// B Rapid
		nButton[1][ 8] = 0;	// SELECT
		nButton[1][ 9] = 0;	// START
		nButton[1][10] = DIK_M;	// Mic

		nRapid[1][0] = 0;	// A Rapid speed
		nRapid[1][1] = 0;	// B Rapid speed*/
	}

	void	DefaultController3() {
		for( INT i = 0; i < 64; i++ )
			nButton[2][i] = 0;

		nRapid[2][0] = 0;	// A Rapid speed
		nRapid[2][1] = 0;	// B Rapid speed
	}

	void	DefaultController4() {
		for( INT i = 0; i < 64; i++ )
			nButton[3][i] = 0;

		nRapid[3][0] = 0;	// A Rapid speed
		nRapid[3][1] = 0;	// B Rapid speed
	}

	void	DefaultExController0() {
		// Crazy Climber���۰�(���ۂɂ͑��݂��Ȃ�)
		for( INT i = 0; i < 64; i++ )
			nExButton[0][i] = 0;
//		nExButton[0][ 0] = 0;	// L up
//		nExButton[0][ 1] = 0;	// L down
//		nExButton[0][ 2] = 0;	// L left
//		nExButton[0][ 3] = 0;	// L right
//		nExButton[0][ 4] = 0;	// R up
//		nExButton[0][ 5] = 0;	// R down
//		nExButton[0][ 6] = 0;	// R left
//		nExButton[0][ 7] = 0;	// R right
	}

	void	DefaultExController1() {
		// Famly Trainer���۰�
		for( INT i = 0; i < 64; i++ )
			nExButton[1][i] = 0;
	}

	void	DefaultExController2() {
		// Exciting Boxing���۰�
		for( INT i = 0; i < 64; i++ )
			nExButton[2][i] = 0;
	}

	void	DefaultExController3() {
		// Mahjang���۰�
		for( INT i = 0; i < 64; i++ )
			nExButton[3][i] = 0;
	}

	void	DefaultNsfController() {
		for( INT i = 0; i < 64; i++ )
			nNsfButton[i] = 0;
/*
		nNsfButton[ 0] = DIK_UP;	// Play
		nNsfButton[ 1] = DIK_DOWN;	// Stop
		nNsfButton[ 2] = DIK_LEFT;	// Number -1
		nNsfButton[ 3] = DIK_RIGHT;	// Number +1
		nNsfButton[ 4] = DIK_PRIOR;	// Number +16
		nNsfButton[ 5] = DIK_NEXT;	// Number -16
		*/
	}

	void	DefaultVSUnisystem() {
		for( INT i = 0; i < 64; i++ )
			nVSUnisystem[i] = 0;

		//nVSUnisystem[0] = DIK_C;	// Coin
	}
};

class	CCfgMovie
{
public:
	BYTE	bUsePlayer[4];
	BOOL	bRerecord;
	BOOL	bLoopPlay;
	BOOL	bResetRec;
	BOOL	bPadDisplay;
	BOOL	bTimeDisplay;

	void	Default() {
		bUsePlayer[0] = 0xFF;
		bUsePlayer[1] = 0x00;
		bUsePlayer[2] = 0x00;
		bUsePlayer[3] = 0x00;
		bRerecord = TRUE;
		bLoopPlay = FALSE;
		bResetRec = FALSE;
		bPadDisplay = FALSE;
		bTimeDisplay = FALSE;
	}
};

class	CCfgLauncher
{
public:
	RECT	rcWindowPos;

	BYTE	bHeaderView [16];
	BYTE	nHeaderOrder[16];
	SHORT	nHeaderWidth[16];

	BOOL	bActivePause;

	INT	nListSelect;

	BOOL	bSortDir;
	INT	nSortType;

	BYTE	bFolderUse[16];
	CHAR	szFolder[16][_MAX_PATH];

	CHAR	szLastSelect[_MAX_PATH];

	void	Default() {
		rcWindowPos.left = rcWindowPos.right =
		rcWindowPos.top = rcWindowPos.bottom = 0;

		bActivePause = FALSE;

		nListSelect = 0;

		bSortDir = FALSE;
		nSortType = 0;

		INT	i;
		for( i = 0; i < 16; i++ ) {
			bHeaderView [i] = TRUE;
			nHeaderOrder[i] = i;
			nHeaderWidth[i] = (i==0)?160:48;
		}

		for( i = 0; i < 16; i++ ) {
			bFolderUse[i] = FALSE;
			szFolder[i][0] = '\0';
		}

		szLastSelect[0] = '\0';
	}
};

class	CCfgExtraSound
{
public:
	CHAR	szExtSoundFile[ ESF_FILE_MAX ][ MAX_PATH ];

	void	Default() {
		for( INT i = 0; i < ESF_FILE_MAX; i++ ) {
			szExtSoundFile[i][0] = '\0';
		}
	}
};

class	CCfgNetPlay
{
public:
	RECT	rcChatPos;

	INT	nRecentPort, nRecentHost;
	CHAR	szRecentPort[16][_MAX_PATH+1];
	CHAR	szRecentHost[16][_MAX_PATH+1];
	CHAR	szNick[_MAX_PATH+1];

	void	Default() {
		rcChatPos.left = rcChatPos.right =
		rcChatPos.top = rcChatPos.bottom = 0;

		//::lstrcpy( szNick, "NoName" );
		strcpy( szNick, "NoName" );

		for( INT i = 0; i < 16; i++ ) {
			szRecentPort[i][0] = '\0';
			szRecentHost[i][0] = '\0';
		}
		//::lstrcpy( szRecentPort[0], "10000" );
		//::lstrcpy( szRecentHost[0], "localhost:10000" );
		strcpy( szRecentPort[0], "10000" );
		strcpy( szRecentHost[0], "localhost:10000" );

		nRecentPort = 1;
		nRecentHost = 1;
	}
};

class	CConfig
{
public:
	CConfig() {
		m_bKeyboardDisable = FALSE;
		Default();
	}

	CCfgGeneral	general;
	CCfgPath	path;
	CCfgEmulator	emulator;
	CCfgGraphics	graphics;
	CCfgSound	sound;
	CCfgShortCut	shortcut;
	CCfgLanguage	language;
	CCfgController	controller;
	CCfgMovie	movie;
	CCfgLauncher	launcher;
	CCfgExtraSound	extsound;
	CCfgNetPlay	netplay;

	void	Default() {
		general.Default();
		path.Default();
		emulator.Default();
		graphics.Default();
		sound.Default();
		shortcut.Default();
		language.Default();
		controller.Default();
		movie.Default();
		launcher.Default();
		extsound.Default();
		netplay.Default();
	}

	void	Load();
	void	Save();

// Checker
	void	InputKeyboardDisable( BOOL bEnable ) {
		m_bKeyboardDisable = bEnable;
	}

	BOOL	ButtonCheck( INT nNo, INT nID );
	BOOL	ExButtonCheck( INT nNo, INT nID );
	BOOL	NsfButtonCheck( INT nID );
	BOOL	ButtonCheck( INT nID, WORD* pKey );

// Converter
	string	ShortcutToKeyName( INT nShortcut );

// Table
	static	INT	SamplingRateTable[];
	static	INT	ShortcutKeyID[];
protected:

	BOOL	m_bKeyboardDisable;
private:
};

// �ʃQ�[���I�v�V����
class	CGameOption
{
public:
	// for Cartridge
	void	Load( DWORD crc );
	void	Save( LPCSTR name, DWORD crc );
	// for FDS
	void	Load( DWORD gid, DWORD mid );
	void	Save( LPCSTR name, DWORD gid, DWORD mid );

	// Default�ۑ�
	INT	defRenderMethod;
	INT	defIRQtype;
	BOOL	defFrameIRQ;
	BOOL	defVideoMode;

	// �f�[�^
	INT	nRenderMethod;
	INT	nIRQtype;
	BOOL	bFrameIRQ = TRUE;		// Set to TRUE by default, fixes Qix
	BOOL	bVideoMode;
protected:
private:
};

extern	CConfig		Config;
extern	CGameOption	GameOption;

#endif // !__CCONFIG_INCLUDED__
