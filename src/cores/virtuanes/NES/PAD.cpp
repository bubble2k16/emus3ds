//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Pad                                                         //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "typedef.h"
#include "macro.h"

#include "DebugOut.h"
#include "Config.h"

//#include "DirectDraw.h"
//#include "DirectInput.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "pad.h"
#include "rom.h"

PAD::PAD( NES* parent ) : nes( parent )
{
	excontroller_select = 0;
	expad = NULL;
	bStrobe = FALSE;
	bSwapButton = FALSE;
	bSwapPlayer = FALSE;
	bZapperMode = FALSE;
	nVSSwapType = VS_TYPE0;

	padbit[0] = padbit[1] = padbit[2] = padbit[3] = 0;
	micbit = 0;

	padbitsync[0] = padbitsync[1] = padbitsync[2] = padbitsync[3] = 0;
	micbitsync = 0;
}

PAD::~PAD()
{
	//DirectDraw.SetZapperMode( FALSE );
	//DirectDraw.SetZapperDrawMode( FALSE );

	DELETEPTR( expad );
}

void	PAD::Reset()
{
	pad1bit = pad2bit = 0;
	bStrobe = FALSE;

//	if( !bZapperMode ) {
//		bZapperMode = FALSE;
//	}

	bBarcodeWorld = FALSE;

	ZEROMEMORY( padcnt, sizeof(padcnt) );

	// Select Extension Devices
	DWORD	crc = nes->rom->GetPROM_CRC();

	if( crc == 0xfbfc6a6c		// Adventures of Bayou Billy, The(E)
	 || crc == 0xcb275051		// Adventures of Bayou Billy, The(U)
	 || crc == 0xfb69c131		// Baby Boomer(Unl)(U)
	 || crc == 0xf2641ad0		// Barker Bill's Trick Shooting(U)
	 || crc == 0xbc1dce96		// Chiller (Unl)(U)
	 || crc == 0x90ca616d		// Duck Hunt(JUE)
	 || crc == 0x59e3343f		// Freedom Force(U)
	 || crc == 0x242a270c		// Gotcha!(U)
	 || crc == 0x7b5bd2de		// Gumshoe(UE)
	 || crc == 0x255b129c		// Gun Sight(J)
	 || crc == 0x8963ae6e		// Hogan's Alley(JU)
	 || crc == 0x51d2112f		// Laser Invasion(U)
	 || crc == 0x0a866c94		// Lone Ranger, The(U)
//	 || crc == 0xe4c04eea		// Mad City(J)
	 || crc == 0x9eef47aa		// Mechanized Attack(U)
	 || crc == 0xc2db7551		// Shooting Range(U)
	 || crc == 0x163e86c0		// To The Earth(U)
	 || crc == 0x42d893e4		// Operation Wolf(J)
	 || crc == 0x1388aeb9		// Operation Wolf(U)
	 || crc == 0x0d3cf705		// Wild Gunman(J)
	 || crc == 0x389960db ) {	// Wild Gunman(JUE)
		SetExController( EXCONTROLLER_ZAPPER );
	}
	if( crc == 0x35893b67		// Arkanoid(J)
	 || crc == 0x6267fbd1 ) {	// Arkanoid 2(J)
		SetExController( EXCONTROLLER_PADDLE );
	}
	if( crc == 0xff6621ce		// Hyper Olympic(J)
	 || crc == 0xdb9418e8		// Hyper Olympic(Tonosama Ban)(J)
	 || crc == 0xac98cd70 ) {	// Hyper Sports(J)
		SetExController( EXCONTROLLER_HYPERSHOT );
	}
	if( crc == 0xf9def527		// Family BASIC(Ver2.0)
	 || crc == 0xde34526e		// Family BASIC(Ver2.1a)
	 || crc == 0xf050b611		// Family BASIC(Ver3)
	 || crc == 0x3aaeed3f		// Family BASIC(Ver3)(Alt)
	 || crc == 0x868FCD89		// Family BASIC(Ver1.0)
	 || crc == 0x2D6B7E5A		// PLAYBOX BASIC(J) (Prototype_v0.0)
	 || crc == 0xDA03D908 ) {	// PLAYBOX BASIC (J)
		SetExController( EXCONTROLLER_KEYBOARD );
	}
	if( crc == 0x589b6b0d		// Supor Computer V3.0
	 || crc == 0x8b265862		// Supor English
	 || crc == 0x41401c6d		// Supor Computer V4.0
	 || crc == 0x82F1Fb96		// Supor Computer(Russia) V1.0
	 || crc == 0xd5d6eac4 ) {	// EDU(C) Computer
		SetExController( EXCONTROLLER_SUPOR_KEYBOARD );
		nes->SetVideoMode( TRUE );
	}
	if( crc == 0xc68363f6		// Crazy Climber(J)
	 || crc == 0x2989ead6		// Smash TV(U) [!]
	 || crc == 0x0b8f8128 ) {	// Smash TV(E) [!]
		SetExController( EXCONTROLLER_CRAZYCLIMBER );
	}
	if( crc == 0x20d22251 ) {	// Top rider(J)
		SetExController( EXCONTROLLER_TOPRIDER );
	}
	if( crc == 0x0cd00488 ) {	// Space Shadow(J)
		SetExController( EXCONTROLLER_SPACESHADOWGUN );
	}

	if( crc == 0x8c8fa83b		// Family Trainer - Athletic World (J)
	 || crc == 0x7e704a14		// Family Trainer - Jogging Race (J)
	 || crc == 0x2330a5d3 ) {	// Family Trainer - Rairai Kyonshiizu (J)
		SetExController( EXCONTROLLER_FAMILYTRAINER_A );
	}
	if( crc == 0xf8da2506		// Family Trainer - Aerobics Studio (J)
	 || crc == 0xca26a0f1		// Family Trainer - Dai Undoukai (J)
	 || crc == 0x28068b8c		// Family Trainer - Fuuun Takeshi Jou 2 (J)
	 || crc == 0x10bb8f9a		// Family Trainer - Manhattan Police (J)
	 || crc == 0xad3df455		// Family Trainer - Meiro Dai Sakusen (J)
	 || crc == 0x8a5b72c0		// Family Trainer - Running Stadium (J)
	 || crc == 0x59794f2d ) {	// Family Trainer - Totsugeki Fuuun Takeshi Jou (J)
		SetExController( EXCONTROLLER_FAMILYTRAINER_B );
	}
	if( crc == 0x9fae4d46		// Ide Yousuke Meijin no Jissen Mahjong (J)
	 || crc == 0x7b44fb2a ) {	// Ide Yousuke Meijin no Jissen Mahjong 2 (J)
		SetExController( EXCONTROLLER_MAHJANG );
	}
	if( crc == 0x786148b6 ) {	// Exciting Boxing (J)
		SetExController( EXCONTROLLER_EXCITINGBOXING );
	}
	if( crc == 0xc3c0811d		// Oeka Kids - Anpanman no Hiragana Daisuki (J)
	 || crc == 0x9d048ea4 ) {	// Oeka Kids - Anpanman to Oekaki Shiyou!! (J)
		SetExController( EXCONTROLLER_OEKAKIDS_TABLET );
	}
#if	0
	if( crc == 0xe792de94		// Best Play - Pro Yakyuu (New) (J)
	 || crc == 0xf79d684a		// Best Play - Pro Yakyuu (Old) (J)
	 || crc == 0xc2ef3422		// Best Play - Pro Yakyuu 2 (J)
	 || crc == 0x974e8840		// Best Play - Pro Yakyuu '90 (J)
	 || crc == 0xb8747abf		// Best Play - Pro Yakyuu Special (J)
	 || crc == 0x9fa1c11f		// Castle Excellent (J)
	 || crc == 0x0b0d4d1b		// Derby Stallion - Zenkoku Ban (J)
	 || crc == 0x728c3d98		// Downtown - Nekketsu Monogatari (J)
	 || crc == 0xd68a6f33		// Dungeon Kid (J)
	 || crc == 0x3a51eb04		// Fleet Commander (J)
	 || crc == 0x7c46998b		// Haja no Fuuin (J)
	 || crc == 0x7e5d2f1a		// Itadaki Street - Watashi no Mise ni Yottette (J)
	 || crc == 0xcee5857b		// Ninjara Hoi! (J)
	 || crc == 0x50ec5e8b		// Wizardry - Legacy of Llylgamyn (J)
	 || crc == 0x343e9146		// Wizardry - Proving Grounds of the Mad Overlord (J)
	 || crc == 0x33d07e45 ) {	// Wizardry - The Knight of Diamonds (J)
		SetExController( EXCONTROLLER_TURBOFILE );
	}
#endif

	if( crc == 0x67898319 ) {	// Barcode World (J)
		bBarcodeWorld = TRUE;
	}

	// VS-Unisystem
	if( nes->rom->IsVSUNISYSTEM() ) {
		if( crc == 0xff5135a3		// VS Hogan's Alley
		 || crc == 0xed588f00		// VS Duck Hunt
		 || crc == 0x17ae56be ) {	// VS Freedom Force
			SetExController( EXCONTROLLER_VSZAPPER );
		} else {
			SetExController( EXCONTROLLER_VSUNISYSTEM );
		}
	}

	if( crc == 0x21b099f3 ) {	// Gyromite (JUE)
		SetExController( EXCONTROLLER_GYROMITE );
	}

//	if( crc == 0x4ee735c1 ) {	// Stack-Up (JU)
//		SetExController( EXCONTROLLER_STACKUP );
//	}
}

void	PAD::SetExController( INT type )
{
	excontroller_select = type;

	DELETEPTR( expad );

	bZapperMode = FALSE;
	//DirectDraw.SetZapperMode( FALSE );
	//DirectDraw.SetZapperDrawMode( FALSE );

	// ExPad Instance create
	switch( type ) {
		case	EXCONTROLLER_ZAPPER:
			expad = new EXPAD_Zapper( nes );
			bZapperMode = TRUE;
			//DirectDraw.SetZapperMode( TRUE );
			//DirectDraw.SetZapperDrawMode( TRUE );
			break;
		case	EXCONTROLLER_PADDLE:
			expad = new EXPAD_Paddle( nes );
			//DirectDraw.SetZapperMode( TRUE );
			break;
		case	EXCONTROLLER_HYPERSHOT:
			expad = new EXPAD_HyperShot( nes );
			break;
		case	EXCONTROLLER_KEYBOARD:
			expad = new EXPAD_Keyboard( nes );
			break;
		case	EXCONTROLLER_SUPOR_KEYBOARD:
			expad = new EXPAD_Supor_Keyboard( nes );
			break;
		case	EXCONTROLLER_CRAZYCLIMBER:
			expad = new EXPAD_CrazyClimber( nes );
			break;
		case	EXCONTROLLER_TOPRIDER:
			expad = new EXPAD_Toprider( nes );
			break;
		case	EXCONTROLLER_SPACESHADOWGUN:
			expad = new EXPAD_SpaceShadowGun( nes );
			bZapperMode = TRUE;
			//DirectDraw.SetZapperMode( TRUE );
			//DirectDraw.SetZapperDrawMode( TRUE );
			break;

		case	EXCONTROLLER_FAMILYTRAINER_A:
		case	EXCONTROLLER_FAMILYTRAINER_B:
			expad = new EXPAD_FamlyTrainer( nes );
			break;
		case	EXCONTROLLER_EXCITINGBOXING:
			expad = new EXPAD_ExcitingBoxing( nes );
			break;
		case	EXCONTROLLER_MAHJANG:
			expad = new EXPAD_Mahjang( nes );
			break;
		case	EXCONTROLLER_OEKAKIDS_TABLET:
			expad = new EXPAD_OekakidsTablet( nes );
			//DirectDraw.SetZapperMode( TRUE );
			//DirectDraw.SetZapperDrawMode( FALSE );
			break;
		case	EXCONTROLLER_TURBOFILE:
			expad = new EXPAD_TurboFile( nes );
			break;

		case	EXCONTROLLER_VSUNISYSTEM:
			expad = new EXPAD_VSUnisystem( nes );
			break;
		case	EXCONTROLLER_VSZAPPER:
			expad = new EXPAD_VSZapper( nes );
			bZapperMode = TRUE;
			//DirectDraw.SetZapperMode( TRUE );
			//DirectDraw.SetZapperDrawMode( TRUE );
			break;

		case	EXCONTROLLER_GYROMITE:
			expad = new EXPAD_Gyromite( nes );
			break;

// Buggy:(
//		case	EXCONTROLLER_STACKUP:
//			expad = new EXPAD_StackUp( nes );
//			break;

		default:
			break;
	}

	if( expad ) {
		expad->Reset();
	}
}

DWORD	PAD::GetSyncData()
{
DWORD	ret;

	ret = (DWORD)padbit[0]|((DWORD)padbit[1]<<8)|((DWORD)padbit[2]<<16)|((DWORD)padbit[3]<<24);
	ret |= (DWORD)micbit<<8;

	return	ret;
}

void	PAD::SetSyncData( DWORD data )
{
	micbit = (BYTE)((data&0x00000400)>>8);
	padbit[0] = (BYTE) data;
	padbit[1] = (BYTE)(data>> 8);
	padbit[2] = (BYTE)(data>>16);
	padbit[3] = (BYTE)(data>>24);
}

DWORD	PAD::GetSyncExData()
{
DWORD	data = 0;

	switch( excontroller_select ) {
		case	EXCONTROLLER_ZAPPER:
		case	EXCONTROLLER_PADDLE:
		case	EXCONTROLLER_SPACESHADOWGUN:
		case	EXCONTROLLER_OEKAKIDS_TABLET:
		case	EXCONTROLLER_VSZAPPER:
			{
			LONG	x, y;
				x = expad->GetSyncData( 0 );
				y = expad->GetSyncData( 1 );
				if( x == -1 || y == -1 ) {
					data = 0x80000000;
				} else {
					data = (x&0xFF)|((y&0xFF)<<8);
				}
			}
			if( excontroller_select != EXCONTROLLER_SPACESHADOWGUN ) {
				if( expad->GetSyncData( 2 ) )
					data |= 0x0010000;
			} else {
				data |= (DWORD)expad->GetSyncData( 2 )<<16;
			}
			break;
		case	EXCONTROLLER_CRAZYCLIMBER:
			data = (DWORD)expad->GetSyncData( 0 );
			break;
		case	EXCONTROLLER_TOPRIDER:
			data = (DWORD)expad->GetSyncData( 0 );
			break;
		case	EXCONTROLLER_FAMILYTRAINER_A:
		case	EXCONTROLLER_FAMILYTRAINER_B:
			data = (DWORD)expad->GetSyncData( 0 );
			break;
		case	EXCONTROLLER_EXCITINGBOXING:
			data = (DWORD)expad->GetSyncData( 0 );
			break;
		case	EXCONTROLLER_MAHJANG:
			data = (DWORD)expad->GetSyncData( 0 );
			break;

		default:
			break;
	}
	return	data;
}

void	PAD::SetSyncExData( DWORD data )
{
//DEBUGOUT( "PAD::SetSyncExData\n" );
	switch( excontroller_select ) {
		case	EXCONTROLLER_ZAPPER:
		case	EXCONTROLLER_PADDLE:
		case	EXCONTROLLER_SPACESHADOWGUN:
		case	EXCONTROLLER_OEKAKIDS_TABLET:
		case	EXCONTROLLER_VSZAPPER:
			{
			LONG	x, y;
				if( data & 0x80000000 ) {
					x = -1;
					y = -1;
				} else {
					x = data & 0xFF;
					y = (data&0xFF00)>>8;
				}
				expad->SetSyncData( 0, x );
				expad->SetSyncData( 1, y );
				nes->SetZapperPos( x, y );
				//DirectDraw.SetZapperPos( x, y );
			}
			if( excontroller_select != EXCONTROLLER_SPACESHADOWGUN ) {
				if( data & 0x0010000 )
					expad->SetSyncData( 2, 1 );
				else
					expad->SetSyncData( 2, 0 );
			} else {
				expad->SetSyncData( 2, (BYTE)(data>>16) );
			}
			break;
		case	EXCONTROLLER_CRAZYCLIMBER:
			expad->SetSyncData( 0, (LONG)data );
			break;
		case	EXCONTROLLER_TOPRIDER:
			expad->SetSyncData( 0, (LONG)data );
			break;
		case	EXCONTROLLER_FAMILYTRAINER_A:
		case	EXCONTROLLER_FAMILYTRAINER_B:
			expad->SetSyncData( 0, (LONG)data );
			break;
		case	EXCONTROLLER_EXCITINGBOXING:
			expad->SetSyncData( 0, (LONG)data );
			break;
		case	EXCONTROLLER_MAHJANG:
			expad->SetSyncData( 0, (LONG)data );
			break;
		default:
			break;
	}
}

void	PAD::Sync()
{
	padbit[0] = SyncSub( 0 );
	padbit[1] = SyncSub( 1 );
	padbit[2] = SyncSub( 2 );
	padbit[3] = SyncSub( 3 );

	// Mic
	micbit = 0;
	if( Config.ButtonCheck( 1, 10 ) ) micbit |= 4;

	// For Excontroller
	if( expad ) {
		expad->Sync();
	}

	// For NSF
	NsfSub();
}

void	PAD::VSync()
{
	padbitsync[0] = padbit[0];
	padbitsync[1] = padbit[1];
	padbitsync[2] = padbit[2];
	padbitsync[3] = padbit[3];
	micbitsync = micbit;
}

static	INT	ren30fps[] = {
	1, 0
};
static	INT	ren20fps[] = {
	1, 1, 0
};
static	INT	ren15fps[] = {
	1, 1, 0, 0
};
static	INT	ren10fps[] = {
	1, 1, 1, 0, 0, 0
};

static	INT	renmask[] = {
	6, 4, 3, 2,
};
static	INT*	rentbl[] = {
	ren10fps, ren15fps, ren20fps, ren30fps
};

BYTE	PAD::SyncSub( INT no )
{
WORD	bit = 0;

	// Up
	if( Config.ButtonCheck( no, 0 ) ) bit |= 1<<4;
	// Down
	if( Config.ButtonCheck( no, 1 ) ) bit |= 1<<5;
	// Left
	if( Config.ButtonCheck( no, 2 ) ) bit |= 1<<6;
	// Right
	if( Config.ButtonCheck( no, 3 ) ) bit |= 1<<7;

	// �������͂��֎~����
//	if( (bit&((1<<4)|(1<<5))) == ((1<<4)|(1<<5)) )
//		bit &= ~((1<<4)|(1<<5));
	if( (bit&((1<<6)|(1<<7))) == ((1<<6)|(1<<7)) )
		bit &= ~((1<<6)|(1<<7));

	// A
	if( Config.ButtonCheck( no, 4 ) ) bit |= 1<<0;
	// B
	if( Config.ButtonCheck( no, 5 ) ) bit |= 1<<1;

	// A,B Rapid
	if( Config.ButtonCheck( no, 6 ) ) bit |= 1<<8;
	if( Config.ButtonCheck( no, 7 ) ) bit |= 1<<9;

	// Select
	if( Config.ButtonCheck( no, 8 ) ) bit |= 1<<2;
	// Start
	if( Config.ButtonCheck( no, 9 ) ) bit |= 1<<3;

	// A rapid setup
	if( bit&(1<<8) ) {
		INT	spd = Config.controller.nRapid[no][0];
		if( spd >= 3 ) spd = 3;
		INT*	tbl = rentbl[spd];

		if( padcnt[no][0] >= renmask[spd] )
			padcnt[no][0] = 0;

		if( tbl[padcnt[no][0]] )
			bit |= (1<<0);
		else
			bit &= ~(1<<0);

		padcnt[no][0]++;
	} else {
		padcnt[no][0] = 0;
	}
	// B rapid setup
	if( bit&(1<<9) ) {
		INT	spd = Config.controller.nRapid[no][1];
		if( spd >= 3 ) spd = 3;
		INT*	tbl = rentbl[spd];

		if( padcnt[no][1] >= renmask[spd] )
			padcnt[no][1] = 0;

		if( tbl[padcnt[no][1]] )
			bit |= (1<<1);
		else
			bit &= ~(1<<1);

		padcnt[no][1]++;
	} else {
		padcnt[no][1] = 0;
	}

	return	(BYTE)(bit&0xFF);
}

void	PAD::Strobe()
{
	// For VS-Unisystem
	if( nes->rom->IsVSUNISYSTEM() ) {
		DWORD	pad1 = (DWORD)padbitsync[0] & 0xF3;
		DWORD	pad2 = (DWORD)padbitsync[1] & 0xF3;
		DWORD	st1  = ((DWORD)padbitsync[0] & 0x08)>>3;
		DWORD	st2  = ((DWORD)padbitsync[1] & 0x08)>>3;

		switch( nVSSwapType ) {
			case	VS_TYPE0:
				pad1bit = pad1 | (st1<<2);
				pad2bit = pad2 | (st2<<2);
				break;
			case	VS_TYPE1:
				pad1bit = pad2 | (st1<<2);
				pad2bit = pad1 | (st2<<2);
				break;
			case	VS_TYPE2:
				pad1bit = pad1 | (st1<<2) | (st2<<3);
				pad2bit = pad2;
				break;
			case	VS_TYPE3:
				pad1bit = pad2 | (st1<<2) | (st2<<3);
				pad2bit = pad1;
				break;
			case	VS_TYPE4:
				pad1bit = pad1 | (st1<<2) | 0x08;	// 0x08=Start Protect
				pad2bit = pad2 | (st2<<2) | 0x08;	// 0x08=Start Protect
				break;
			case	VS_TYPE5:
				pad1bit = pad2 | (st1<<2) | 0x08;	// 0x08=Start Protect
				pad2bit = pad1 | (st2<<2) | 0x08;	// 0x08=Start Protect
				break;
			case	VS_TYPE6:
				pad1bit = pad1 | (st1<<2) | (((DWORD)padbitsync[0] & 0x04)<<1);
				pad2bit = pad2 | (st2<<2) | (((DWORD)padbitsync[1] & 0x04)<<1);
				break;
			case	VS_TYPEZ:
				pad1bit = 0;
				pad2bit = 0;
				break;
		}

		// Coin 2�Ɣ����ׂɏ���
		micbit  = 0;
	} else {
		if( Config.emulator.bFourPlayer ) {
		// NES type
			pad1bit = (DWORD)padbitsync[0] | ((DWORD)padbitsync[2]<<8) | 0x00080000;
			pad2bit = (DWORD)padbitsync[1] | ((DWORD)padbitsync[3]<<8) | 0x00040000;
		} else {
		// Famicom type
			pad1bit = (DWORD)padbitsync[0];
			pad2bit = (DWORD)padbitsync[1];
		}
	}
	pad3bit = (DWORD)padbitsync[2];
	pad4bit = (DWORD)padbitsync[3];
}

BYTE	PAD::Read( WORD addr )
{
BYTE	data = 0x00;

	if( addr == 0x4016 ) {
		data = (BYTE)pad1bit&1;
		pad1bit>>=1;
		data |= ((BYTE)pad3bit&1)<<1;
		pad3bit>>=1;
		// Mic
		if( !nes->rom->IsVSUNISYSTEM() ) {
			data |= micbitsync;
		}
		if( expad ) {
			data |= expad->Read4016();
		}
	}
	if( addr == 0x4017 ) {
		data = (BYTE)pad2bit&1;
		pad2bit>>=1;
		data |= ((BYTE)pad4bit&1)<<1;
		pad4bit>>=1;

		if( expad ) {
			data |= expad->Read4017();
//DEBUGOUT( "4017:%02X\n", data );
		}

		if( bBarcodeWorld ) {
			data |= nes->Barcode2();
		}
	}

	return	data;
}

void	PAD::Write( WORD addr, BYTE data )
{
	if( addr == 0x4016 ) {
		if( data&0x01 ) {
			bStrobe = TRUE;
		} else if( bStrobe ) {
			bStrobe = FALSE;

			Strobe();
			if( expad ) {
				expad->Strobe();
			}
		}

		if( expad ) {
			expad->Write4016( data );
		}
	}
	if( addr == 0x4017 ) {
		if( expad ) {
			expad->Write4017( data );
		}
	}
}

void	PAD::NsfSub()
{
	nsfbit = 0;

	// Play
	if( Config.ButtonCheck( 0, Config.controller.nNsfButton ) ) nsfbit |= 1<<0;
	// Stop
	if( Config.ButtonCheck( 1, Config.controller.nNsfButton ) ) nsfbit |= 1<<1;

	// Number -1
	if( Config.ButtonCheck( 2, Config.controller.nNsfButton ) ) nsfbit |= 1<<2;
	// Number +1
	if( Config.ButtonCheck( 3, Config.controller.nNsfButton ) ) nsfbit |= 1<<3;
	// Number -16
	if( Config.ButtonCheck( 4, Config.controller.nNsfButton ) ) nsfbit |= 1<<4;
	// Number +16
	if( Config.ButtonCheck( 5, Config.controller.nNsfButton ) ) nsfbit |= 1<<5;

	// �������͂��֎~����
	if( (nsfbit&((1<<2)|(1<<3))) == ((1<<2)|(1<<3)) )
		nsfbit &= ~((1<<2)|(1<<3));
	if( (nsfbit&((1<<4)|(1<<5))) == ((1<<4)|(1<<5)) )
		nsfbit &= ~((1<<4)|(1<<5));
}

#include "EXPAD_Zapper.cpp"
#include "EXPAD_Paddle.cpp"
#include "EXPAD_HyperShot.cpp"
#include "EXPAD_Keyboard.cpp"
#include "EXPAD_Supor_Keyboard.cpp"
#include "EXPAD_CrazyClimber.cpp"
#include "EXPAD_Toprider.cpp"
#include "EXPAD_SpaceShadowGun.cpp"

#include "EXPAD_FamlyTrainer.cpp"
#include "EXPAD_ExcitingBoxing.cpp"
#include "EXPAD_Mahjang.cpp"
#include "EXPAD_OekakidsTablet.cpp"
#include "EXPAD_TurboFile.cpp"

#include "EXPAD_VSUnisystem.cpp"
#include "EXPAD_VSZapper.cpp"

#include "EXPAD_Gyromite.cpp"
