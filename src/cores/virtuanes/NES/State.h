#ifndef	__STATE_INCLUDED__
#define	__STATE_INCLUDED__

#pragma pack( push, 1 )

typedef	struct	tagFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES ST"
	WORD	Reserved;
	WORD	BlockVersion;
} FILEHDR, *LPFILEHDR;

// VirtuaNES version0.30以降用
typedef	struct	tagFILEHDR2 {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES ST"
	WORD	Reserved;
	WORD	BlockVersion;	// 0x0200 / 0x0210(v0.60以降)

	DWORD	Ext0;		// ROM:プログラムCRC	FDS:プログラムID
	WORD	Ext1;		// ROM:なし		FDS:メーカーID
	WORD	Ext2;		// ROM:なし		FDS:ディスク枚数
	LONG	MovieStep;	// 追記(取り直し)ムービー時のフレーム数
	LONG	MovieOffset;	// 追記(取り直し)ムービー時のファイルオフセット
} FILEHDR2, *LPFILEHDR2;

typedef	struct	tagBLOCKHDR {
	BYTE	ID[8];
	WORD	Reserved;
	WORD	BlockVersion;
	DWORD	BlockSize;
} BLOCKHDR, *LPBLOCKHDR;

// CPU レジスタ
// version 0x0110まで
typedef	struct	tagCPUSTAT_O {
	WORD	PC;
	BYTE	A;
	BYTE	X;
	BYTE	Y;
	BYTE	S;
	BYTE	P;
	BYTE	I;	// Interrupt pending flag

	BYTE	FrameIRQ;
	BYTE	reserved[3];

	LONG	mod_cycles;	// ムービー等でクロック数の微妙なずれを防ぐ為

	// version 0x0110
	SQWORD	emul_cycles;
	SQWORD	base_cycles;
} CPUSTAT_O, *LPCPUSTAT_O;

// version 0x0210
typedef	struct	tagCPUSTAT {
	WORD	PC;
	BYTE	A;
	BYTE	X;
	BYTE	Y;
	BYTE	S;
	BYTE	P;
	BYTE	I;	// Interrupt pending flag

	BYTE	FrameIRQ;
	BYTE	FrameIRQ_occur;
	BYTE	FrameIRQ_count;
	BYTE	FrameIRQ_type;
	LONG	FrameIRQ_cycles;

	LONG	DMA_cycles;

	SQWORD	emul_cycles;
	SQWORD	base_cycles;
} CPUSTAT, *LPCPUSTAT;

// PPU レジスタ
typedef	struct	tagPPUSTAT {
	BYTE	reg0;
	BYTE	reg1;
	BYTE	reg2;
	BYTE	reg3;
	BYTE	reg7;
	BYTE	toggle56;

	WORD	loopy_t;
	WORD	loopy_v;
	WORD	loopy_x;
} PPUSTAT, *LPPPUSTAT;

// APU レジスタ(拡張サウンド含む)
typedef	struct	tagAPUSTAT_O {
	BYTE	reg[0x0018];
	BYTE	ext[0x0100];
} APUSTAT_O, *LPAPUSTAT_O;

// コントローラレジスタ
typedef	struct	tagCTRREG {
	DWORD	pad1bit;
	DWORD	pad2bit;
	DWORD	pad3bit;
	DWORD	pad4bit;
	BYTE	strobe;
} CTRREG, *LPCTRREG;

//
// レジスタデータ
// ID "REG DATA"
// ver 0x0110まで
typedef	struct	tagREGSTAT_O {
	union	uniCPUREG {
		BYTE	cpudata[32];
		CPUSTAT_O	cpu;
	} cpureg;
	union	uniPPUREG {
		BYTE	ppudata[32];
		PPUSTAT	ppu;
	} ppureg;
	APUSTAT_O	apu;
} REGSTAT_O, *LPREGSTAT_O;

// ver 0x0200以降
typedef	struct	tagREGSTAT {
	union	uniCPUREG {
		BYTE	cpudata[64];
		CPUSTAT	cpu;
	} cpureg;
	union	uniPPUREG {
		BYTE	ppudata[32];
		PPUSTAT	ppu;
	} ppureg;
} REGSTAT, *LPREGSTAT;


//
// 内臓RAMデータ
// ID "RAM DATA"
typedef	struct	tagRAMSTAT {
	BYTE	RAM[2*1024];	// Internal NES RAM
	BYTE	BGPAL[16];	// BG Palette
	BYTE	SPPAL[16];	// SP Palette
	BYTE	SPRAM[256];	// Sprite RAM
} RAMSTAT, *LPRAMSTAT;

//
// MMUデータ
// ID "MMU DATA"
typedef	struct	tagMMUSTAT {
	BYTE	CPU_MEM_TYPE[8];
	WORD	CPU_MEM_PAGE[8];
	BYTE	PPU_MEM_TYPE[12];
	WORD	PPU_MEM_PAGE[12];
	BYTE	CRAM_USED[8];
} MMUSTAT, *LPMMUSTAT;

//
// マッパーデータ
// ID "MMC DATA"
typedef	struct	tagMMCSTAT {
	BYTE	mmcdata[256];
} MMCSTAT, *LPMMCSTAT;

//
// コントローラデータ
// ID "CTR DATA"
typedef	struct	tagCTRSTAT {
	union uniCTRDATA {
		BYTE	ctrdata[32];
		CTRREG	ctr;
	} ctrreg;
} CTRSTAT, *LPCTRSTAT;

//
// SNDデータ
// ID "SND DATA"
typedef	struct	tagSNDSTAT {
	BYTE	snddata[0x800];	// 2KB
} SNDSTAT, *LPSNDSTAT;

//
// ディスクイメージ
// Ver0.24まで
// ID "DSIDE 0A","DSIDE 0B","DSIDE 1A","DSIDE 1B"
typedef	struct	tagDISKSTAT {
	BYTE	DiskTouch[16];
} DISKSTAT, *LPDISKSTAT;

// Ver0.30以降
// ID "DISKDATA"
typedef	struct	tagDISKDATA {
	LONG	DifferentSize;
} DISKDATA, *LPDISKDATA;

// 以下はディスクセーブイメージファイルで使用する
// Ver0.24まで
typedef	struct	tagDISKIMGFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES DI"
	WORD	BlockVersion;
	WORD	DiskNumber;
} DISKIMGFILEHDR, *LPDISKIMGFILEHDR;

typedef	struct	tagDISKIMGHDR {
	BYTE	ID[6];		// ID "SIDE0A","SIDE0B","SIDE1A","SIDE1B"
	BYTE	DiskTouch[16];
} DISKIMGHDR, *LPDISKIMGHDR;

// VirtuaNES version0.30以降用
typedef	struct	tagDISKFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES DI"
	WORD	BlockVersion;	// 0x0200:0.30	0x0210:0.31
	WORD	Reserved;
	DWORD	ProgID;		// プログラムID
	WORD	MakerID;	// メーカーID
	WORD	DiskNo;		// ディスク数
	DWORD	DifferentSize;	// 相違数
} DISKFILEHDR, *LPDISKFILEHDR;

// ID "EXCTRDAT"
typedef	struct	tagEXCTRSTAT {
	DWORD	data;
} EXCTRSTAT, *LPEXCTRSTAT;

//
// ムービーファイル
//
// VirtuaNES version0.60以降用
typedef	struct	tagMOVIEFILEHDR {
	BYTE	ID[12];			// "VirtuaNES MV"
	WORD	BlockVersion;		// Movie version 0x0300
	WORD	RecordVersion;		// Record version
	DWORD	Control;		// コントロールバイト
					// 76543210(Bit)
					// E---4321
					// |   |||+-- 1Pデータ
					// |   ||+--- 2Pデータ
					// |   |+---- 3Pデータ
					// |   +----- 4Pデータ
					// +--------- 追記禁止
					// その他コントロールは1P～4P(どれでも良い)の方向キーが
					// 全てONの時，次の４バイトがコントロール用データになる
	DWORD	Ext0;			// ROM:プログラムCRC	FDS:プログラムID
	WORD	Ext1;			// ROM:なし		FDS:メーカーID
	WORD	Ext2;			// ROM:なし		FDS:ディスク枚数
	DWORD	RecordTimes;		// 記録回数(取り直し回数)

	BYTE	RenderMethod;		// レンダリング方式
	BYTE	IRQtype;		// IRQタイプ
	BYTE	FrameIRQ;		// FrameIRQ禁止
	BYTE	VideoMode;		// NTSC/PAL

	BYTE	reserved2[8];		// 予約

	LONG	StateStOffset;		// Movie start state offset
	LONG	StateEdOffset;		// Movie end state offset
	LONG	MovieOffset;		// Movie data offset
	LONG	MovieStep;		// Movie steps(Frame数)

	DWORD	CRC;			// このデータを除くCRC(インチキ防止)
} MOVIEFILEHDR, *LPMOVIEFILEHDR;

typedef	struct	tagMOVIEIMGFILEHDRx {
	BYTE	ID[12];			// "VirtuaNES MV"
	WORD	BlockVersion;
	WORD	reserved;
	LONG	StateStOffset;		// Movie start state offset
	LONG	StateEdOffset;		// Movie end state offset
	LONG	MovieOffset;		// Movie data offset
	LONG	MovieStep;		// Movie steps
} MOVIEIMGFILEHDRx, *LPMOVIEIMGFILEHDRx;

// Famtasia Movie....
typedef	struct	tagFMVHDR {
	BYTE	ID[4];			// "FMV^Z"
	BYTE	Control1;		// R???????	0:リセット後から記録？ 1:途中から記録
	BYTE	Control2;		// OT??????	O:1P情報 T:2P情報
	DWORD	Unknown1;
	WORD	RecordTimes;		// 記録回数-1
	DWORD	Unknown2;
	BYTE	szEmulators[0x40];	// 記録したエミュレータ
	BYTE	szTitle    [0x40];	// タイトル
} FMVHDR, *LPFMVHDR;

// Nesticle Movie....
typedef	struct	tagNMVHDR {
	BYTE	ExRAM[0x2000];
	BYTE	S_RAM[0x0800];
	WORD	PC;
	BYTE	A;
	BYTE	P;
	BYTE	X;
	BYTE	Y;
	BYTE	SP;
	BYTE	OAM[0x0100];
	BYTE	VRAM[0x4000];
	BYTE	Other[0xC9];
	DWORD	ScanlineCycles;
	DWORD	VblankScanlines;
	DWORD	FrameScanlines;
	DWORD	VirtualFPS;
} NMVHDR, *LPNMVHDR;

// マクロ
#define	SETBYTE(p,v)	{ *p = (v); p++; }
#define	SETWORD(p,v)	{ *(LPWORD)p = (v); p += sizeof(WORD); }
#define	SETDWORD(p,v)	{ *(LPDWORD)p = (v); p += sizeof(DWORD); }
#define	SETINT(p,v)	{ *(LPINT)p = (v); p += sizeof(INT); }
#define	SETBLOCK(p,v,s)	{ memcpy( p, (v), s ); p += s; }

#define	GETBYTE(p,v)	{ (v) = *p; p++; }
#define	GETWORD(p,v)	{ (v) = *(LPWORD)p; p += sizeof(WORD); }
#define	GETDWORD(p,v)	{ (v) = *(LPDWORD)p; p += sizeof(DWORD); }
#define	GETINT(p,v)	{ (v) = *(LPINT)p; p += sizeof(INT); }
#define	GETBLOCK(p,v,s)	{ memcpy( (v), p, s ); p += s; }

#pragma pack( pop )

#endif	// !__STATE_INCLUDED__
