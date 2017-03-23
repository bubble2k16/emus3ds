//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Pad                                                         //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#ifndef	__PAD_INCLUDED__
#define	__PAD_INCLUDED__

#include "typedef.h"
#include "macro.h"

// class prototypes
class	NES;
class	EXPAD;

class	PAD
{
public:
	PAD( NES* parent );
	virtual	~PAD();

// ÉÅÉìÉoä÷êî
	void	Reset();

	// For movie
	DWORD	GetSyncData();
	void	SetSyncData( DWORD data );

	DWORD	GetSyncExData();
	void	SetSyncExData( DWORD data );

	void	VSync();

	void	Sync();
	BYTE	SyncSub( INT no );

	void	SetStrobe( BOOL bStrb ) { bStrobe = bStrb; }
	BOOL	GetStrobe() { return bStrobe; }

	void	Strobe();
	WORD	StrobeSub( INT no );

	BYTE	Read( WORD addr );
	void	Write( WORD addr, BYTE data );

	void	SetExController( INT type );
	INT	GetExController()	    { return excontroller_select; }

	BOOL	IsZapperMode() { return bZapperMode; };

	// For VS-Unisystem
	void	SetSwapPlayer( BOOL bSwap ) { bSwapPlayer = bSwap; }
	void	SetSwapButton( BOOL bSwap ) { bSwapButton = bSwap; }
	void	SetVSType( INT nType ) { nVSSwapType = nType; }

	enum	{
		VS_TYPE0 = 0,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P No reverse
		VS_TYPE1,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse
		VS_TYPE2,	// SELECT1P=START1P/START1P =START2P 1P/2P No reverse
		VS_TYPE3,	// SELECT1P=START1P/START1P =START2P 1P/2P Reverse
		VS_TYPE4,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P No reverse (Protection)
		VS_TYPE5,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse    (Protection)
		VS_TYPE6,	// SELECT1P=START1P/SELECT2P=START2P 1P/2P Reverse	(For Golf)
		VS_TYPEZ,	// ZAPPER
	};

	enum	EXCONTROLLER {
		EXCONTROLLER_NONE = 0,
		EXCONTROLLER_PADDLE,
		EXCONTROLLER_HYPERSHOT,
		EXCONTROLLER_ZAPPER,
		EXCONTROLLER_KEYBOARD,
		EXCONTROLLER_CRAZYCLIMBER,
		EXCONTROLLER_TOPRIDER,
		EXCONTROLLER_SPACESHADOWGUN,

		EXCONTROLLER_FAMILYTRAINER_A,
		EXCONTROLLER_FAMILYTRAINER_B,
		EXCONTROLLER_EXCITINGBOXING,
		EXCONTROLLER_MAHJANG,
		EXCONTROLLER_OEKAKIDS_TABLET,
		EXCONTROLLER_TURBOFILE,

		EXCONTROLLER_VSUNISYSTEM,
		EXCONTROLLER_VSZAPPER,

		EXCONTROLLER_GYROMITE,
		EXCONTROLLER_STACKUP,

		EXCONTROLLER_SUPOR_KEYBOARD,
	};

	DWORD	pad1bit, pad2bit;
	DWORD	pad3bit, pad4bit;

	// Frame Synchronized
	LONG	zapperx, zappery;
	BYTE	zapperbit;
	BYTE	crazyclimberbit;

	// For NSF Player
	BYTE	GetNsfController() { return nsfbit; }

protected:
	NES*	nes;
	// Extension Devices
	EXPAD*	expad;

	// Frame Synchronized
	BYTE	padbit[4];
	BYTE	micbit;

	BYTE	padbitsync[4];
	BYTE	micbitsync;

	INT	excontroller_select;
	INT	padcnt[4][2];

	BOOL	bStrobe;
	BOOL	bZapperMode;

	// For VS-Unisystem
	INT	nVSSwapType;
	BOOL	bSwapPlayer;
	BOOL	bSwapButton;

	// For BarcodeWorld
	BOOL	bBarcodeWorld;

	// For NSF Player
	void	NsfSub();

	BYTE	nsfbit;
private:
};

#include "EXPAD.h"
#include "EXPAD_Zapper.h"
#include "EXPAD_Paddle.h"
#include "EXPAD_HyperShot.h"
#include "EXPAD_Keyboard.h"
#include "EXPAD_Supor_Keyboard.h"
#include "EXPAD_CrazyClimber.h"
#include "EXPAD_Toprider.h"
#include "EXPAD_SpaceShadowGun.h"

#include "EXPAD_FamlyTrainer.h"
#include "EXPAD_ExcitingBoxing.h"
#include "EXPAD_Mahjang.h"
#include "EXPAD_OekakidsTablet.h"
#include "EXPAD_TurboFile.h"

#include "EXPAD_VSUnisystem.h"
#include "EXPAD_VSZapper.h"

#include "EXPAD_Gyromite.h"

#endif	// !__PAD_INCLUDED__

