
#ifndef	__MMU_FCEUX_INCLUDED__
#define	__MMU_FCEUX_INCLUDED__

// For compatibiity with FCEUX
extern int	PRGsize[32];

// For compatibiity with FCEUX
#define MI_H VRAM_HMIRROR
#define MI_V VRAM_VMIRROR
#define MI_0 VRAM_MIRROR4L
#define MI_1 VRAM_MIRROR4H

// For compatibility with FCEUX
typedef u32 uint32;
typedef u16 uint16;
typedef u8 uint8;
typedef s32 int32;
typedef s16 int16;
typedef s8 int8;

// For compatibiity with FCEUX
extern	void	setprg8 ( WORD A, WORD bank );
extern	void	setprg16( WORD A, WORD bank );
extern	void	setprg32( WORD A, WORD bank );
extern	void	setchr1( WORD A, INT bank );
extern	void	setchr2( WORD A, INT bank );
extern	void	setchr4( WORD A, INT bank );
extern	void	setchr8( INT bank );
extern	void 	setmirror(int t);
extern	void 	setmirrorw(int a, int b, int c, int d);

// For compatibiity with FCEUX
typedef void (*writefunc)(u32 A, u8 V);
typedef u8 (*readfunc)(u32 A);
extern readfunc ARead[0x10000];
extern writefunc BWrite[0x10000];
extern u8 CartBR(u32 A);
extern void CartBW(u32 A, u8 V);
extern u8 ANull(u32 A);
extern void BNull(u32 A, u8 V);
extern void SetReadHandler(s32 start, s32 end, readfunc func);
extern void SetWriteHandler(s32 start, s32 end, writefunc func);

// For compatibility with FCEUX
struct SFORMAT
{
	//a void* to the data or a void** to the data
	void *v;

	//size, plus flags
	uint32 s;

	//a string description of the element
	char *desc;
};


#endif