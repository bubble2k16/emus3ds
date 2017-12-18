/*----------------------------------------------------------------------*/
/*                                                                      */
/*      NES Mapper                                                      */
/*                                                           Norix      */
/*                                               written     2001/02/05 */
/*                                               last modify ----/--/-- */
/*----------------------------------------------------------------------*/
#ifndef	__MAPPER_INCLUDED__
#define	__MAPPER_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "DirectSound.h"

#include "typedef.h"
#include "macro.h"

// class prototype
class	NES;

// Mapper base class
class	Mapper
{
public:
	Mapper( NES* parent );
	virtual	~Mapper();

	// For Mapper
	// Reset
	virtual	void	Reset() = 0;

	// $8000-$FFFF Memory write
	virtual	void	Write( WORD addr, BYTE data ) {}

	// $8000-$FFFF Memory read(Dummy)
	virtual	void	Read( WORD addr, BYTE data ) {}
	virtual	BOOL	ReadHigh( WORD addr, LPBYTE pdata ) { return FALSE; }

	// $4100-$7FFF Lower Memory read/write
	virtual	BYTE	ReadLow ( WORD addr );
	virtual	void	WriteLow( WORD addr, BYTE data );

	// $4018-$40FF Extention register read/write
	virtual	BYTE	ExRead ( WORD addr )	{ return 0x00; }
	virtual	void	ExWrite( WORD addr, BYTE data ) {}

	// Extension commands
	// For ExCmdRead command
	enum	EXCMDRD {
		EXCMDRD_NONE = 0,
		EXCMDRD_DISKACCESS,
	};
	// For ExCmdWrite command
	enum	EXCMDWR {
		EXCMDWR_NONE = 0,
		EXCMDWR_DISKINSERT,
		EXCMDWR_DISKEJECT,
	};

	virtual	void	WriteExPPU( WORD addr, BYTE data ) {}

	virtual	BYTE	ReadExAPU ( WORD addr )	{ return 0x00; }
	virtual	void	WriteExAPU( WORD addr, BYTE data ) {}

	virtual	BYTE	ExCmdRead ( EXCMDRD cmd )	{ return 0x00; }
	virtual	void	ExCmdWrite( EXCMDWR cmd, BYTE data ) {}

	// H sync/V sync/Clock sync
	virtual	void	HSync( INT scanline ) {}
	virtual	void	VSync() {}
	virtual	void	Clock( INT cycles ) {}

	// PPU address bus latch
	virtual	void	PPU_Latch( WORD addr ) {}

	// PPU Character latch
	virtual	void	PPU_ChrLatch( WORD addr ) {}

	// PPU Extension character/palette
	virtual	void	PPU_ExtLatchX( INT x ) {}
	virtual	void	PPU_ExtLatch( WORD addr, BYTE& chr_l, BYTE& chr_h, BYTE& attr ) {}

	//YuXing
	virtual	BYTE	PPU_ExtLatchSP() { return 0; }

	// For State save
	virtual	BOOL	IsStateSave() { return FALSE; }
	virtual	void	SaveState( LPBYTE p ) {}
	virtual	void	LoadState( LPBYTE p ) {}

protected:
	NES*	nes;

private:
};

// Create class instance
extern	Mapper*	CreateMapper( NES* parent, INT no,BOOL bUnif = false);

#endif

#ifndef _UNIF_H_
#define _UNIF_H_

struct BDINFO {
char boardName[30];
int  boardID;
} ;

typedef struct BDINFO BOARDINFO;

extern const BOARDINFO BoardInfo[];


enum BoardID
{
	// NROM
	STD_NROM =256              ,
	// AxROM
	STD_AMROM                  ,
	STD_ANROM                  ,
	STD_AN1ROM                 ,
	STD_AOROM                  ,
	// BxROM
	STD_BNROM                  ,
	// CxROM
	STD_CNROM                  ,
	STD_CXROM                  ,
	STD_CPROM                  ,
	// DxROM
	STD_DEROM                  ,
	STD_DE1ROM                 ,
	STD_DRROM                  ,
	// ExROM
	STD_ELROM                  ,
	STD_EKROM                  ,
	STD_ETROM                  ,
	STD_EWROM                  ,
	STD_EXROM_0                ,
	STD_EXROM_1                ,
	STD_EXROM_2                ,
	STD_EXROM_3                ,
	STD_EXROM_4                ,
	STD_EXROM_5                ,
	// FxROM
	STD_FJROM                  ,
	STD_FKROM                  ,
	// GxROM
	STD_GNROM                  ,
	// MxROM
	STD_MHROM                  ,
	// HxROM
	STD_HKROM                  ,
	// JxROM
	STD_JLROM                  ,
	STD_JSROM                  ,
	// NxROM
	STD_NTBROM                 ,
	// PxROM
	STD_PNROM                  ,
	STD_PNROM_PC10             ,
	STD_PEEOROM                ,
	// SxROM
	STD_SAROM                  ,
	STD_SBROM                  ,
	STD_SCROM                  ,
	STD_SEROM                  ,
	STD_SFROM                  ,
	STD_SGROM                  ,
	STD_SHROM                  ,
	STD_SJROM                  ,
	STD_SKROM                  ,
	STD_SLROM                  ,
	STD_SNROM                  ,
	STD_SOROM                  ,
	STD_SUROM                  ,
	STD_SXROM                  ,
	// TxROM
	STD_TEROM                  ,
	STD_TBROM                  ,
	STD_TFROM                  ,
	STD_TGROM                  ,
	STD_TKROM                  ,
	STD_TKSROM                 ,
	STD_TLROM                  ,
	STD_TLSROM                 ,
	STD_TNROM                  ,
	STD_TQROM                  ,
	STD_TR1ROM                 ,
	STD_TSROM                  ,
	STD_TVROM                  ,
	// UxROM
	STD_UNROM                  ,
	STD_UN1ROM                 ,
	STD_UOROM                  ,
	STD_UXROM                  ,
	// Discrete Logic
	DISCRETE_74_377            ,
	DISCRETE_74_139_74         ,
	DISCRETE_74_161_138        ,
	DISCRETE_74_161_161_32_A   ,
	DISCRETE_74_161_161_32_B   ,
	// Other
	CUSTOM_B4                  ,
	CUSTOM_BTR                 ,
	CUSTOM_EVENT               ,
	CUSTOM_FFE3                ,
	CUSTOM_FFE4                ,
	CUSTOM_FFE8                ,
	CUSTOM_FB02                ,
	CUSTOM_FB04                ,
	CUSTOM_RUMBLESTATION       ,
	CUSTOM_QJ                  ,
	CUSTOM_VSSYSTEM_0          ,
	CUSTOM_VSSYSTEM_1          ,
	CUSTOM_WH                  ,
	CUSTOM_X79B                ,
	CUSTOM_ZZ                  ,
	// Active Enterprises
	AE_STD                     ,
	// AGCI
	AGCI_50282                 ,
	// AVE
	AVE_NINA001                ,
	AVE_NINA002                ,
	AVE_NINA03                 ,
	AVE_NINA06                 ,
	AVE_NINA07                 ,
	AVE_MB_91                  ,
	AVE_D1012                  ,
	// Bandai
	BANDAI_FCG1                ,
	BANDAI_FCG2                ,
	BANDAI_BAJUMP2             ,
	BANDAI_LZ93D50_24C01       ,
	BANDAI_LZ93D50_24C02       ,
	BANDAI_DATACH              ,
	BANDAI_KARAOKESTUDIO       ,
	BANDAI_AEROBICSSTUDIO      ,
	BANDAI_OEKAKIDS            ,
	// Bensheng
	BENSHENG_BS5               ,
	// Bootleg multicarts
	BMC_110IN1                 ,
	BMC_150IN1                 ,
	BMC_15IN1                  ,
	BMC_1200IN1                ,
	BMC_20IN1                  ,
	BMC_21IN1                  ,
	BMC_22GAMES                ,
	BMC_31IN1                  ,
	BMC_35IN1                  ,
	BMC_36IN1                  ,
	BMC_64IN1                  ,
	BMC_72IN1                  ,
	BMC_76IN1                  ,
	BMC_8157                   ,
	BMC_9999999IN1             ,
	BMC_A65AS                  ,
	BMC_BALLGAMES_11IN1        ,
	BMC_CTC65                  ,
	BMC_DRAGONBOLLPARTY        ,
	BMC_FAMILY_4646B           ,
	BMC_FK23C                 ,
	BMC_GAME_800IN1            ,
	BMC_GOLDEN_190IN1          ,
	BMC_GOLDENGAME_150IN1      ,
	BMC_GOLDENGAME_260IN1      ,
	BMC_GKA                    ,
	BMC_GKB                    ,
	BMC_GOLDENCARD_6IN1        ,
	BMC_HERO                   ,
	BMC_MARIOPARTY_7IN1        ,
	BMC_NOVELDIAMOND           ,
	BMC_CH001                  ,
	BMC_POWERJOY_84IN1         ,
	BMC_RESETBASED_4IN1        ,
	BMC_SUPER_24IN1            ,
	BMC_SUPER_22GAMES          ,
	BMC_SUPER_40IN1            ,
	BMC_SUPER_42IN1            ,
	BMC_SUPER_700IN1           ,
	BMC_SUPERBIG_7IN1          ,
	BMC_SUPERGUN_20IN1         ,
	BMC_SUPERHIK_4IN1          ,
	BMC_SUPERHIK_300IN1        ,
	BMC_SUPERVISION_16IN1      ,
	BMC_T262                   ,
	BMC_VRC4                   ,
	BMC_VT5201                 ,
	BMC_Y2K_64IN1              ,
	// Bootlegs
	BTL_2708                   ,
	BTL_6035052                ,
	BTL_AISENSHINICOL          ,
	BTL_AX5705                 ,
	BTL_DRAGONNINJA            ,
	BTL_GENIUSMERIOBROS        ,
	BTL_MARIOBABY              ,
	BTL_PIKACHUY2K             ,
	BTL_SHUIGUANPIPE           ,
	BTL_SMB2_A                 ,
	BTL_SMB2_B                 ,
	BTL_SMB2_C                 ,
	BTL_SMB3                   ,
	BTL_SUPERBROS11            ,
	BTL_T230                   ,
	BTL_TOBIDASEDAISAKUSEN     ,
	// Camerica
	CAMERICA_BF9093            ,
	CAMERICA_BF9096            ,
	CAMERICA_BF9097            ,
	CAMERICA_BF909X            ,
	CAMERICA_ALGNV11           ,
	CAMERICA_ALGQV11           ,
	CAMERICA_GOLDENFIVE        ,
	// Caltron
	CALTRON_6IN1               ,
	// C&E
	CNE_SHLZ                   ,
	CNE_DECATHLON              ,
	CNE_PSB                    ,
	// Cony
	CONY_STD                   ,
	// Dreamtech
	DREAMTECH_01               ,
	// Fujiya
	FUJIYA_STD                 ,
	// Fukutake
	FUKUTAKE_SBX               ,
	// Future Media
	FUTUREMEDIA_STD            ,
	// Gouder
	GOUDER_37017               ,
	// Henggedianzi
	HENGEDIANZI_STD            ,
	HENGEDIANZI_XJZB           ,
	// HES
	HES_STD                    ,
	// Hosenkan
	HOSENKAN_STD               ,
	// Irem
	IREM_G101A_0               ,
	IREM_G101A_1               ,
	IREM_G101B_0               ,
	IREM_G101B_1               ,
	IREM_H3001                 ,
	IREM_LROG017               ,
	IREM_HOLYDIVER             ,
	IREM_KAIKETSU              ,
	// Jaleco
	JALECO_JF01                ,
	JALECO_JF02                ,
	JALECO_JF03                ,
	JALECO_JF04                ,
	JALECO_JF05                ,
	JALECO_JF06                ,
	JALECO_JF07                ,
	JALECO_JF08                ,
	JALECO_JF09                ,
	JALECO_JF10                ,
	JALECO_JF11                ,
	JALECO_JF12                ,
	JALECO_JF13                ,
	JALECO_JF14                ,
	JALECO_JF15                ,
	JALECO_JF16                ,
	JALECO_JF17                ,
	JALECO_JF18                ,
	JALECO_JF19                ,
	JALECO_JF20                ,
	JALECO_JF21                ,
	JALECO_JF22                ,
	JALECO_JF23                ,
	JALECO_JF24                ,
	JALECO_JF25                ,
	JALECO_JF26                ,
	JALECO_JF27                ,
	JALECO_JF28                ,
	JALECO_JF29                ,
	JALECO_JF30                ,
	JALECO_JF31                ,
	JALECO_JF32                ,
	JALECO_JF33                ,
	JALECO_JF34                ,
	JALECO_JF35                ,
	JALECO_JF36                ,
	JALECO_JF37                ,
	JALECO_JF38                ,
	JALECO_JF39                ,
	JALECO_JF40                ,
	JALECO_JF41                ,
	JALECO_SS88006             ,
	// J.Y.Company
	JYCOMPANY_TYPE_A           ,
	JYCOMPANY_TYPE_B           ,
	JYCOMPANY_TYPE_C           ,
	// Kaiser
	KAISER_KS202               ,
	KAISER_KS7022              ,
	KAISER_KS7032              ,
	KAISER_KS7058              ,
	// Kasing
	KASING_STD                 ,
	// K
	KAY_H2288                  ,
	KAY_PANDAPRINCE            ,
	// Konami
	KONAMI_VRC1                ,
	KONAMI_VRC2                ,
	KONAMI_VRC3                ,
	KONAMI_VRC4_0              ,
	KONAMI_VRC4_1              ,
	KONAMI_VRC4_2              ,
	KONAMI_VRC6_0              ,
	KONAMI_VRC6_1              ,
	KONAMI_VRC7_0              ,
	KONAMI_VRC7_1              ,
	KONAMI_VSSYSTEM            ,
	// Magic Series
	MAGICSERIES_MAGICDRAGON    ,
	// Namcot
	NAMCOT_3433                ,
	NAMCOT_3443                ,
	NAMCOT_3446                ,
	NAMCOT_3425                ,
	NAMCOT_34XX                ,
	NAMCOT_163_0               ,
	NAMCOT_163_1               ,
	NAMCOT_163_S_0             ,
	NAMCOT_163_S_1             ,
	// Nitra
	NITRA_TDA                  ,
	// NTDEC
	NTDEC_N715062              ,
	NTDEC_ASDER_0              ,
	NTDEC_ASDER_1              ,
	NTDEC_FIGHTINGHERO         ,
	// Nanjing
	NANJING_STD                ,
	// Nihon Bussan
	NIHON_UNROM_M5             ,
	// Open Corp
	OPENCORP_DAOU306           ,
	// RCM
	RCM_GS2004                 ,
	RCM_GS2013                 ,
	RCM_GS2015                 ,
	RCM_TETRISFAMILY           ,
	// Rex Soft
	REXSOFT_DBZ5               ,
	REXSOFT_SL1632             ,
	// Sachen
	SACHEN_8259A               ,
	SACHEN_8259B               ,
	SACHEN_8259C               ,
	SACHEN_8259D               ,
	SACHEN_TCA01               ,
	SACHEN_TCU01               ,
	SACHEN_TCU02               ,
	SACHEN_SA0036              ,
	SACHEN_SA0037              ,
	SACHEN_SA0161M             ,
	SACHEN_SA72007             ,
	SACHEN_SA72008             ,
	SACHEN_74_374A             ,
	SACHEN_74_374B             ,
	SACHEN_STREETHEROES        ,
	// Someri Team
	SOMERITEAM_SL12            ,
	// Subor
	SUBOR_TYPE0                ,
	SUBOR_TYPE1                ,
	SUBOR_STUDYNGAME           ,
	// Sunsoft
	SUNSOFT_1                  ,
	SUNSOFT_2A                 ,
	SUNSOFT_2B                 ,
	SUNSOFT_3                  ,
	SUNSOFT_4_0                ,
	SUNSOFT_4_1                ,
	SUNSOFT_5B_0               ,
	SUNSOFT_5B_1               ,
	SUNSOFT_DCS                ,
	SUNSOFT_FME7_0             ,
	SUNSOFT_FME7_1             ,
	// Super Game
	SUPERGAME_LIONKING         ,
	SUPERGAME_BOOGERMAN        ,
	SUPERGAME_MK3E             ,
	SUPERGAME_POCAHONTAS2      ,
	// Taito
	TAITO_TC0190FMC            ,
	TAITO_TC0190FMC_PAL16R4    ,
	TAITO_X1005                ,
	TAITO_X1017                ,
	// Tengen
	TENGEN_800002              ,
	TENGEN_800004              ,
	TENGEN_800008              ,
	TENGEN_800030              ,
	TENGEN_800032              ,
	TENGEN_800037              ,
	TENGEN_800042              ,
	// TXC
	TXC_22211A                 ,
	TXC_22211B                 ,
	TXC_22211C                 ,
	TXC_MXMDHTWO               ,
	TXC_POLICEMAN              ,
	TXC_TW                     ,
	// Unlicensed
	UNL_A9746                  ,
	UNL_CC21                   ,
	UNL_EDU2000                ,
	UNL_KINGOFFIGHTERS96       ,
	UNL_KINGOFFIGHTERS97       ,
	UNL_MORTALKOMBAT2          ,
	UNL_N625092                ,
	UNL_SUPERFIGHTER3          ,
	UNL_TF1201                 ,
	UNL_WORLDHERO              ,
	UNL_AXROM                  ,
	UNL_BXROM                  ,
	UNL_CXROM                  ,
	UNL_GXROM                  ,
	UNL_NROM                   ,
	UNL_UXROM                  ,
	UNL_TRXROM                 ,
	UNL_XZY                    ,
	// Waixing
	WAIXING_PS2_0              ,
	WAIXING_PS2_1              ,
	WAIXING_TYPE_A             ,
	WAIXING_TYPE_B             ,
	WAIXING_TYPE_C             ,
	WAIXING_TYPE_D             ,
	WAIXING_TYPE_E             ,
	WAIXING_TYPE_F             ,
	WAIXING_TYPE_G             ,
	WAIXING_TYPE_H             ,
	WAIXING_TYPE_I             ,
	WAIXING_TYPE_J             ,
	WAIXING_FFV_0              ,
	WAIXING_FFV_1              ,
	WAIXING_SH2_0              ,
	WAIXING_SH2_1              ,
	WAIXING_SGZLZ              ,
	WAIXING_ZS                 ,
	WAIXING_DQVII              ,
	WAIXING_SGZ                ,
	WAIXING_SECURITY_0         ,
	WAIXING_SECURITY_1         ,
	// Whirlwind
	WHIRLWIND_2706             ,
	// Unknown
	NROM_256_CN		   ,
	// ÐÂ¼Ó
	SUBOR_999,
	SMART_GENIUS,
	FK23CA,
	BMC_FK23CA,
	FK23C,
	CHINA_ER_SAN2,
	UNL_SA_9602B,
	City_Fighter_IV,
	COOLBOY,
	Dragon_Fighter,
	UNL_YOKO,
	OneBus,
	UNL_82112C,
	MGC_002,
	FF3_CN,
	UNL_KS7010,
	UNL_158B,
	UNL_KS7030,
	UNL_TH2131_1,
	UNL_831128C,
	BMC_LB12IN1,
	BOARD_MAX
};

  
int NES_ROM_get_unifBoardID(char *unif_board);
#define DECLFW(f) void f (u32 A, u8 V)
#define DECLFR(f) u8 f (u32 A)

#endif
