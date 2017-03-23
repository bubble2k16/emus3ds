//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      6502 CPU core                                                   //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#ifndef	__CPU_INCLUDED__
#define	__CPU_INCLUDED__

#include "typedef.h"
#include "macro.h"

class	NES;
class	APU;
class	Mapper;

// 6502 status flags
#define	C_FLAG		0x01		// 1: Carry
#define	Z_FLAG		0x02		// 1: Zero
#define	I_FLAG		0x04		// 1: Irq disabled
#define	D_FLAG		0x08		// 1: Decimal mode flag (NES unused)
#define	B_FLAG		0x10		// 1: Break
#define	R_FLAG		0x20		// 1: Reserved (Always 1)
#define	V_FLAG		0x40		// 1: Overflow
#define	N_FLAG		0x80		// 1: Negative

// Interrupt
#define	NMI_FLAG	0x01
#define	IRQ_FLAG	0x02

#define	IRQ_FRAMEIRQ	0x04
#define	IRQ_DPCM	0x08
#define	IRQ_MAPPER	0x10
#define	IRQ_MAPPER2	0x20
#define	IRQ_TRIGGER	0x40		// one shot(旧IRQ())
#define	IRQ_TRIGGER2	0x80		// one shot(旧IRQ_NotPending())

#define	IRQ_MASK	(~(NMI_FLAG|IRQ_FLAG))

// Vector
#define	NMI_VECTOR	0xFFFA
#define	RES_VECTOR	0xFFFC
#define	IRQ_VECTOR	0xFFFE

// 6502 context
typedef	struct	{
	WORD	PC;	/* Program counter   */
	BYTE	A;	/* CPU registers     */
	BYTE	P;
	BYTE	X;
	BYTE	Y;
	BYTE	S;

	BYTE	INT_pending;	// 割り込みペンディングフラグ
} R6502;


class	CPU
{
public:
	CPU( NES* parent );
	virtual	~CPU();

	BYTE	RD6502( WORD addr );
	void	WR6502( WORD addr, BYTE data );
	WORD	RD6502W( WORD addr );

	void	Reset();

	void	NMI();
	void	SetIRQ( BYTE mask );
	void	ClrIRQ( BYTE mask );

	void	DMA( INT cycles );

	INT	EXEC( INT request_cycles );

	INT	GetDmaCycles();
	void	SetDmaCycles( INT cycles );

	INT	GetTotalCycles();
	void	SetTotalCycles( INT cycles );

	void	SetContext( R6502 r )	{ R = r; }
	void	GetContext( R6502& r )	{ r = R; }

	void	SetClockProcess( BOOL bEnable ) { m_bClockProcess = bEnable; }
protected:
	NES*	nes;
	APU*	apu;
	Mapper*	mapper;

	R6502	R;

	INT	TOTAL_cycles;	// CPUトータルサイクル数
	INT	DMA_cycles;	// DMAサイクル数

	// PTR
	LPBYTE	STACK;

	// Zero & Negative table
	BYTE	ZN_Table[256];

	// Clock process
	BOOL	m_bClockProcess;
private:
};

#endif	// !__CPU_INCLUDED__
