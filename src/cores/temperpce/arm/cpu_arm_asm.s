// Changes that must be made: (* means implemented, + means tested and works)

// Hu6280 CPU emulator.

// Some of the particular ideas have been inspired by PCEAdvance, so major
// credits to FluBBa. A lot of them we both came up with independetly.
// Some of them are unique here. I aim to make a very fast Hu6280
// interpreter, although nothing can make it nearly as fast as a good
// dynarec. Unfortunately, a dynarec can't get accurate cycle timing on its
// own, so if one is ever done for ARM then this will be a great supplement
// to it.

// Be very careful not to overwrite anything important in the temporaries
// since sometimes they have fixed usage. If you need more temporaries
// save + restore them on the stack.

// t0 will almost always be taken. Use t1 initially. Use t2 only with
// extreme care. t1 won't last inbetween function calls because it's
// also the ARM's lr (if that's a problem save/restore it)

#include "../platform_defines.h"

#define reg_t0        r0
#define reg_t1        r14
#define reg_t2        r1

// reg_ea is where effective addresses should be calculated to usually.

#define reg_ea        r0

// reg_op holds the loaded operand; note that this occupies the bottom
// 8 bits of the register, so to apply it against one of the emulated
// registers it'll probably need to be shifted up.

#define reg_op        r0
#define reg_op2       r1

// reg_code_page stores a pointer to the code page last loaded, offset
// by the page the PC is currently in. Use this to translate between
// reg_tr_pc and the real Hu6280 pc.
// Actually, now the memory map stores an ext bit in the msb, so the
// value has to be shifted before being added/subtracted.
// (reg_code_page << 1) + pc = reg_tr_pc
// pc = reg_tr_oc - (reg_code_page << 1)

// This isn't true for compatibility mode, where this has the properly
// shifted code page. In compatibiltiy mode you can get the translated
// PC through [reg_code_page, reg_tr_pc, ror #19]

#define reg_code_page r2

// These registers keep the correspoding Hu6280 registers in the top
// 8 bits, with the rest of the bits 0. This way, any additions or
// subtractions to the top will wrap over without damaging the
// variable later, and will also automatically set c/v flags.

#define reg_a         r3
#define reg_x         r4
#define reg_y         r5

// Like a/x/y, but the bottom bit is set to 1. That way
// [reg_zero_page, reg_s, ror #24] addresses the stack, and it
// can be incremented/decremented by 0x1 << 24 when pushing and
// popping.

#define reg_s         r6

// This register contains the location to get the current code
// from on the host machine, as opposed to the native machine.
// So it's a full 32bit pointer.

// In compatbility mode this stores the normal PC, but rotated to the right
// by 13 (so the upper 13 bits are the low 13bits of the PC, and the lower
// 13 bits are the high 3 bits of the PC).

#define reg_tr_pc     r7

// Points to pointer/function table for absolute address reads/writes.

#define reg_mem_table r8

// Points to RAM directly (for zero page reads/writes)

#define reg_zero_page r9

// Contains a representation of the N flag and Z flag. The N flag
// matches bit 31, and the Z flag is considered high only if all
// bottom of the 8bits match 0. This way this register can store
// both N and Z high (as can happen by the bit/tst instructions and
// manual flags manipulation). The other 16bits don't matter.

// Setting this register to reflect the
// value of a result should be done with:
// mov reg_p_nz, result, asr #24         @ If result is in the upper 8 bits
// orr reg_p_nz, result, result, lsr #24 @ If result is in the lower 8 bits

#define reg_p_nz      r10

// The upper 24 bits of this register contains how many master clock
// cycles should elapse until the next event occurs.

#define reg_cycles    r11

// The bottom 8 bits of this register contain the emulated p register
// (flags), minus the n and z bits. This way the C flag can be
// transferred from the native C flag to this C flag by doing sbc for
// the next cycle decrement (as opposed to sub), after the flag has
// already been ORRed to 1. This idea was taken from FluBBa.

#define reg_p         r11

// Points to the current opcode handler pointer table. At this location
// + 0x1000 should be a tflag set variant of this table. The table
// itself will float between normal and decimal mode versions depending
// on the d bit.

#define reg_op_table  r12

// Just normal ARM variables here. No games are played with sp, but
// important variables are stored on the stack.

#define reg_sp        r13
#define reg_arm_pc    r15


// Positions of variables kept on the stack.

#define sp_break_function       0x0
#define sp_cycle_end_function   0x4
#define sp_irq_check_function   0x8
#define sp_irq_check_status     0xC
#define sp_resume_cycles        0x10
#define sp_cpu_struct           0x14
#define sp_irq_struct           0x18

#define ext_storage_save_r9     0x40
#define ext_storage_txx_length  0x50
#define ext_storage_txx_dest    0x54
#define ext_storage_txx_type    0x58

#define transfer_type_tai       0x00
#define transfer_type_tia       0x01
#define transfer_type_tin       0x02
#define transfer_type_tii       0x03
#define transfer_type_tdd       0x04


// Stuff pulled from header files

#define BRK_VECTOR              0xFFF6
#define CD_VECTOR               0xFFF6
#define VDC_VECTOR              0xFFF8
#define TIMER_VECTOR            0xFFFA
#define NMI_VECTOR              0xFFFC
#define RESET_VECTOR            0xFFFE

#define IRQ_CD                  0x01
#define IRQ_VDC                 0x02
#define IRQ_TIMER               0x04

#define C_FLAG_BIT              0
#define Z_FLAG_BIT              1
#define I_FLAG_BIT              2
#define D_FLAG_BIT              3
#define B_FLAG_BIT              4
#define T_FLAG_BIT              5
#define V_FLAG_BIT              6
#define N_FLAG_BIT              7


// ARM v4 doesn't have a blx instruction, but I've screwed this up enough
// times to want it. If it's ported to v5+ this can be converted to blx.

#ifdef IPHONE_BUILD

#define ext_symbol(symbol) _##symbol

#define bl_indirect(reg)                                                      \
  str r9, [sp, #-4]!;                                                         \
  ldr r9, [reg_mem_table, #ext_storage_save_r9];                              \
  blx reg;                                                                    \
  ldr r9, [sp], #4                                                            \

#define call_c(function)                                                      \
  str r9, [sp, #-4]!;                                                         \
  ldr r9, [reg_mem_table, #ext_storage_save_r9];                              \
  bl _##function;                                                             \
  ldr r9, [sp], #4                                                            \

#define save_r9()                                                             \
  str r9, [reg_mem_table, #ext_storage_save_r9]                               \

#else

#define ext_symbol(symbol) symbol


#ifdef ARM_V5

#define bl_indirect(reg)                                                      \
  blx reg                                                                     \

#define call_c(function)                                                      \
  bl function                                                                 \

#define save_r9()                                                             \

#else

#define bl_indirect(reg)                                                      \
  mov lr, pc;                                                                 \
  bx reg                                                                      \

#define call_c(function)                                                      \
  bl function                                                                 \

#define save_r9()                                                             \

#endif
#endif

// Custom memory handlers for ARM ASM core

// For handlers taking 16bit addresses, store as follows:
// reg_t1: lower 8 bits
// reg_ea: upper 8 bits (will be destroyed)

// The value in address must be clamped to 8bits. Address should occupy the
// lower 8bits.

#define load_mem_zp(dest, address, ld_type)                                   \
  ld_type dest, [reg_zero_page, address]                                      \

// Address should occupy the upper 8 bits.

#define load_mem_zp_sh(dest, address)                                         \
  ldrb dest, [reg_zero_page, address, lsr #24]                                \

// Address should occupy the lower 8 bits. reg_t1 must be free.

#define load_mem_zp_16(dest, address)                                         \
  ldrb reg_t1, [reg_zero_page, address];                                      \
  add address, address, #0x1;                                                 \
  and address, address, #0xFF;                                                \
  ldrb dest, [reg_zero_page, address]                                         \

// Address should occupy the upper 8 bits. reg_t1 must be free.

#define load_mem_zp_16_sh(dest, address)                                      \
  ldrb reg_t1, [reg_zero_page, address, lsr #24];                             \
  add dest, address, #(0x1 << 24);                                            \
  ldrb dest, [reg_zero_page, dest, lsr #24]                                   \

// There's one condition where reg_ea may not be over 0xFFFF, which will
// crash the usual load_ext. In this case, use load_ext_zp16_##dest instead.

#define load_mem(dest, ld_type, ext_function)                                 \
  add reg_ea, reg_t1, reg_ea, lsl #8;                                         \
  and reg_t1, reg_ea, #0xe000;                                                \
  ldr reg_t1, [reg_mem_table, reg_t1, lsr #(13 - 2)];                         \
  movs reg_t2, reg_t1, lsl #1;                                                \
  blcs ext_function##_##dest;                                                 \
  ld_type dest, [reg_t2, reg_ea]                                              \

// TODO: Support ext for this if necessary.
// Destroys reg_t2.

#define load_mem_16(dest)                                                     \
  add reg_ea, reg_t1, reg_ea, lsl #8;                                         \
  and reg_t1, reg_ea, #0xe000;                                                \
  ldr reg_t1, [reg_mem_table, reg_t1, lsr #(13 - 2)];                         \
  ldrb reg_t1, [reg_ea, reg_t1, lsl #1];                                      \
  add reg_ea, reg_ea, #1;                                                     \
  and reg_t2, reg_ea, #0xe000;                                                \
  ldr reg_t2, [reg_mem_table, reg_t2, lsr #(13 - 2)];                         \
  ldrb dest, [reg_ea, reg_t2, lsl #1]                                         \

// This destination must point to RAM or ROM. You can use something aside
// from ldrb/s here if you know the address is correctly aligned.

#define load_mem_safe(dest, address, ld_type)                                 \
  and reg_t1, address, #0xe000;                                               \
  ldr reg_t1, [reg_mem_table, reg_t1, lsr #(13 - 2)];                         \
  ld_type dest, [address, reg_t1, lsl #1]                                     \

#define load_mem_safe_const(dest, address, ld_type)                           \
  ldr reg_t1, [reg_mem_table, #(0x7 * 4)];                                    \
  mov reg_t1, reg_t1, lsl #1;                                                 \
  add reg_dest, reg_t1, #(address >> 8);                                      \
  ld_type dest, [reg_dest, #(address & 0xFF)]                                 \


// The value in address must be clamped to 8bits. Address should occupy the
// lower 8bits.

#define store_mem_zp(src, address)                                            \
  strb src, [reg_zero_page, address]                                          \

// Address should occupy the upper 8 bits.

#define store_mem_zp_sh(src, address)                                         \
  strb src, [reg_zero_page, address, lsr #24]                                 \


#define store_mem_sh_no(src, address_mode, address_range)                     \
  add reg_ea, reg_t1, reg_ea, lsl #8;                                         \
  and reg_t1, reg_ea, #0xe000;                                                \
  add reg_t2, reg_mem_table, #(0x8 * 4);                                      \
  ldr reg_t2, [reg_t2, reg_t1, lsr #(13 - 2)];                                \
  movs reg_t2, reg_t2, lsl #1;                                                \
  blcs store_ext_zero;                                                        \
  strb src, [reg_t2, reg_ea]                                                  \

#define store_mem_sh_yes(src, address_mode, address_range)                    \
  add reg_ea, reg_t1, reg_ea, lsl #8;                                         \
  and reg_t1, reg_ea, #0xe000;                                                \
  add reg_t2, reg_mem_table, #(0x8 * 4);                                      \
  ldr reg_t2, [reg_t2, reg_t1, lsr #(13 - 2)];                                \
  mov reg_t1, src, lsr #24;                                                   \
  movs reg_t2, reg_t2, lsl #1;                                                \
  blcs store_ext_##src;                                                       \
  strb reg_t1, [reg_t2, reg_ea]                                               \



// Address should occupy the lower 16 bits. reg_t1 must be free.
// All of the operations have to be expanded out into functions somewhere
// for ext. And these must know what var is.

// Note: rmw ops must take care not to modify reg_t2 because it's needed
// as a pointer.

// It may be better if this uses the write table, as opposed to the read
// table.

#define rmw_mem(var, address, operation)                                      \
  add reg_ea, reg_t1, reg_ea, lsl #8;                                         \
  and reg_t1, address, #0xe000;                                               \
  ldr reg_t2, [reg_mem_table, reg_t1, lsr #(13 - 2)];                         \
  movs reg_t2, reg_t2, lsl #1;                                                \
  blcs rmw_ext_##operation;                                                   \
  ldrb var, [reg_t2, address];                                                \
  operation(var, na);                                                         \
  strb var, [reg_t2, address]                                                 \



/** Code fetch operations ****************************************************/

// These fetches are not completely accurate. They will fail if crossing
// a page boundary that isn't contiguous in RAM. Checking for this would
// probably make a noticeable difference in performance. At least one game,
// Exile, does depend on this.

// For that reason a compatibility mode is added to handle it.

// For it to work the following is necessary:
// - All RAM and ROM blocks must be 8192 byte aligned. This is not a problem
//   for the version of GCC used to compile for GP2X, but it might be for
//   other versions.
// - reg_tr_pc must be subtracted by some fixed register (reg_mem_table is
//   a good one) and rotated right by 13 bits. The reason for the subtraction
//   is that the unrotation can't be done without an addition of some sort
//   as well. This should be done in retrieve_pc.
// - Since it destroys flags, the sbcs has to happen before it (for
//   op_fetch_next_fixed_c)

#ifdef CPU_ARM_COMPATIBILITY_MODE

#define fetch_8bit_fixed(dest)                                                \
  ldrb dest, [reg_code_page, reg_tr_pc, ror #19]                              \

#define fetch_8bit_ldrb(dest)                                                 \
  ldrb dest, [reg_code_page, reg_tr_pc, ror #19];                             \
  adds reg_tr_pc, reg_tr_pc, #(1 << 19);                                      \
  blcs adjust_pc_plus                                                         \

#define fetch_8bit_ldrsb(dest)                                                \
  add dest, reg_code_page, reg_tr_pc, ror #19;                                \
  ldrsb dest, [dest];                                                         \
  adds reg_tr_pc, reg_tr_pc, #(1 << 19);                                      \
  blcs adjust_pc_plus                                                         \

#define fetch_8bit(dest, ld_type)                                             \
  fetch_8bit_##ld_type(dest)                                                  \

// adjust_pc_16 can be called under two circumstances, if there's a wrap
// around during half or all of the load, so it'll have to check for both.

#define fetch_16bit(dest)                                                     \
  fetch_8bit(reg_t1, ldrb);                                                   \
  fetch_8bit(dest, ldrb)                                                      \

#endif


#ifdef CPU_ARM_FAST_MODE

#define fetch_8bit_fixed(dest)                                                \
  ldrb dest, [reg_tr_pc]                                                      \

#define fetch_8bit(dest, ld_type)                                             \
  ld_type dest, [reg_tr_pc], #1                                               \

#define fetch_16bit(dest)                                                     \
  ldrb reg_t1, [reg_tr_pc], #1;                                               \
  ldrb dest, [reg_tr_pc], #1                                                  \

#endif


/** Effective address generators *********************************************/

#define address_load_abs_direct(address)                                      \
  fetch_16bit(address)                                                        \

// This can give addresses wrapped around over 16bits; this should be taken
// care of by the memory handlers.

#define address_load_abs_direct_x(address)                                    \
  fetch_16bit(address);                                                       \
  add reg_t1, reg_t1, reg_x, lsr #24                                          \

#define address_load_abs_direct_y(address)                                    \
  fetch_16bit(address);                                                       \
  add reg_t1, reg_t1, reg_y, lsr #24                                          \

#define address_load_abs_indirect(address)                                    \
  address_load_abs_direct(address);                                           \
  load_mem_16(address)                                                        \

#define address_load_abs_x_indirect(address)                                  \
  address_load_abs_direct_x(address);                                         \
  load_mem_16(address)                                                        \

#define address_load_zp_direct(address)                                       \
  fetch_8bit(address, ldrb)                                                   \

// This can be done with shift adds but not in conjunction with the ldrsbs,
// still worth looking into.

#define address_load_zp_direct_x(address)                                     \
  fetch_8bit(address, ldrb);                                                  \
  add reg_op, reg_op, reg_x, lsr #24;                                         \
  and reg_op, reg_op, #0xFF                                                   \

#define address_load_zp_direct_y(address)                                     \
  fetch_8bit(address, ldrb);                                                  \
  add reg_op, reg_op, reg_y, lsr #24;                                         \
  and reg_op, reg_op, #0xFF                                                   \

#define address_load_zp_indirect(address)                                     \
  address_load_zp_direct(address);                                            \
  load_mem_zp_16(address, address)                                            \

// Since an add step has to be done here, it's more efficient to keep this
// in the upper 8bits (so the additional add will properly wrap around).

#define address_load_zp_x_indirect(address)                                   \
  fetch_8bit(address, ldrb);                                                  \
  add address, reg_x, address, lsl #24;                                       \
  load_mem_zp_16_sh(address, address)                                         \

#define address_load_zp_indirect_y(address)                                   \
  fetch_8bit(address, ldrb);                                                  \
  load_mem_zp_16(address, address);                                           \
  add reg_t1, reg_t1, reg_y, lsr #24                                          \



/** Operand load/store operations ********************************************/

#define op_load_abs(address_mode, dest, ld_type)                              \
  address_load_abs_##address_mode(reg_ea);                                    \
  load_mem(dest, ld_type, load_ext)                                           \

#define op_load_zp(address_mode, dest, ld_type)                               \
  address_load_zp_##address_mode(reg_ea);                                     \
  load_mem_zp(dest, reg_ea, ld_type)                                          \

// This wouldn't work if any address_mode for zp16 are direct (instead of
// indirect), but fortunately that's not the case.

// Actually, the zp16 type is only needed for indirect y, but at this point
// it's too difficult to separate them all.

#define op_load_zp16(address_mode, dest, ld_type)                             \
  address_load_zp_##address_mode(reg_ea);                                     \
  load_mem(dest, ld_type, load_ext_zp16)                                      \

#define op_load_imm(address_mode, dest, ld_type)                              \
  fetch_8bit(dest, ld_type)                                                   \

// Tries to rearrange the shift into a load stall - this is beneficial
// for direct_x_zp and direct_y_zp

#define op_store_shift_zp(address_mode, var)                                  \
  address_load_zp_##address_mode(reg_ea);                                     \
  mov reg_t1, var, lsr #24;                                                   \
  store_mem_zp(reg_t1, reg_ea)                                                \

#define op_store_shift_zp_direct(var)                                         \
  op_store_shift_zp(direct, var)                                              \

#define op_store_shift_zp_indirect(var)                                       \
  op_store_shift_zp(direct, var)                                              \

#define op_store_shift_zp_x_indirect(var)                                     \
  op_store_shift_zp(direct, var)                                              \

#define op_store_shift_zp_indirect_y(var)                                     \
  op_store_shift_zp(direct, var)                                              \

#define op_store_shift_zp_direct_x(var)                                       \
  fetch_8bit(reg_ea, ldrb);                                                   \
  mov reg_t1, var, lsr #24;                                                   \
  add reg_ea, reg_x, reg_ea, lsl #24;                                         \
  store_mem_zp_sh(reg_t1, reg_ea)                                             \

#define op_store_shift_zp_direct_y(var)                                       \
  fetch_8bit(reg_ea, ldrb);                                                   \
  mov reg_t1, var, lsr #24;                                                   \
  add reg_ea, reg_y, reg_ea, lsl #24;                                         \
  store_mem_zp_sh(reg_t1, reg_ea)                                             \


#define op_store_shift_yes(address_mode, var)                                 \
  op_store_shift_zp_##address_mode(var)                                       \

#define op_store_shift_no(address_mode, var)                                  \
  address_load_zp_##address_mode(reg_ea);                                     \
  store_mem_zp(var, reg_ea)                                                   \

#define op_store_abs(address_mode, src, sh)                                   \
  address_load_abs_##address_mode(reg_ea);                                    \
  store_mem_sh_##sh(src, reg_ea, abs)                                         \

#define op_store_zp(address_mode, src, sh)                                    \
  op_store_shift_##sh(address_mode, src)                                      \

#define op_store_zp16(address_mode, src, sh)                                  \
  address_load_zp_##address_mode(reg_ea);                                     \
  store_mem_sh_##sh(src, reg_ea, zp16)                                        \


// It turns out that the extra parameter is only needed by zp rmw.

#define op_rmw_abs(address_mode, var, operation, extra)                       \
  address_load_abs_##address_mode(reg_ea);                                    \
  rmw_mem(var, reg_ea, operation)                                             \

#define op_rmw_zp(address_mode, var, operation, extra)                        \
  address_load_zp_##address_mode(reg_ea);                                     \
  load_mem_zp(var, reg_ea, ldrb);                                             \
  operation(var, extra);                                                      \
  store_mem_zp(var, reg_ea)                                                   \


/** Flag and PC operations ***************************************************/

#ifdef CPU_ARM_COMPATIBILITY_MODE

// reg has the PC as a normal 16bit value to set

#define update_pc(reg)                                                        \
  and reg_code_page, reg, #0xe000;                                            \
  ldr reg_code_page, [reg_mem_table, reg_code_page, lsr #(13 - 2)];           \
  mov reg_tr_pc, reg, ror #13;                                                \
  mov reg_code_page, reg_code_page, lsl #1                                    \

// the real 16bit PC is to be stored in reg

#define retrieve_pc(reg)                                                      \
  mov reg, reg_tr_pc, ror #19                                                 \

// reg can't be reg_t1, and is also expected to be signed

// The way this works is by using the n flag like an overflow flag - if the
// result takes more bits than are available (13) then PC should be adjusted.
// Normal overflow can't be used because we're doing an unsigned + signed
// addition, and it'll fail to trigger because it works for signed + signed
// (or unsigned + unsigned for carry). These are the possibilities:

// PC + positive or negative number results in PC within same segment, fits
// in 13 bits, n is not set (two's complement arithmetic prevents n bit from
// being set)
// PC + positive number results in PC in next segment, results is 14 bits,
// n is set, c is clear.
// PC + negative number results in previous segment, underflow results in
// 14 bits, n is set, c is set.

#define advance_pc(reg)                                                       \
  mov reg_t1, reg_tr_pc, lsr #1;                                              \
  cmn reg_t1, reg, lsl #18;                                                   \
  add reg_tr_pc, reg_tr_pc, reg, lsl #19;                                     \
  blmi adjust_pc_plus_minus                                                   \

#define sub_pc(imm)                                                           \
  subs reg_tr_pc, reg_tr_pc, #(imm << 19);                                    \
  blcc adjust_pc_minus                                                        \

#endif


#ifdef CPU_ARM_FAST_MODE

// reg has the value to store the PC in

#define update_pc(reg)                                                        \
  and reg_code_page, reg, #0xe000;                                            \
  ldr reg_code_page, [reg_mem_table, reg_code_page, lsr #(13 - 2)];           \
  add reg_tr_pc, reg, reg_code_page, lsl #1                                   \

// reg has the PC as a normal 16bit value to set

#define retrieve_pc(reg)                                                      \
  sub reg, reg_tr_pc, reg_code_page, lsl #1                                   \

#define advance_pc(reg)                                                       \
  add reg_tr_pc, reg_tr_pc, reg                                               \

#define sub_pc(imm)                                                           \
  sub reg_tr_pc, reg_tr_pc, #(imm)                                            \

#endif


#define collapse_flags(dest)                                                  \
  and dest, reg_p, #(0xFF ^ (1 << Z_FLAG_BIT) ^ (1 << N_FLAG_BIT));           \
  movs reg_p_nz, reg_p_nz;                                                    \
  orrmi dest, dest, #(1 << N_FLAG_BIT);                                       \
  tst reg_p_nz, #0xFF;                                                        \
  orreq dest, dest, #(1 << Z_FLAG_BIT)                                        \

#define extract_flags()                                                       \
  mov reg_p_nz, reg_p, lsr #(N_FLAG_BIT);                                     \
  mov reg_p_nz, reg_p_nz, lsl #31;                                            \
  tst reg_p, #(1 << Z_FLAG_BIT);                                              \
  orreq reg_p_nz, reg_p_nz, #0x01                                             \



/** Binary arithmetic operations *********************************************/

// cmn 0x6 << 28 will boost the carry flag high if the result is > 9
// (otherwise keep it high), then adding #0x6 will bump over anything > 9 to
// two digits (where the top digit will be rounded off).

// Since reg_op only occupies the lower 8 bits (0x000000XX) doing
// sub reg_op, reg_op, #0x100 will convert it to 0xFFFFFFXX. Rotating this
// by 8 will yield 0xXXFFFFFF; the bottom FF's will propagate the carry bit
// added with adc to the upper 8 bits. Thanks to FluBBa for this idea.

#define op_adc_d_clear_t_clear(address_mode, address_range)                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #((1 << V_FLAG_BIT) | (1 << C_FLAG_BIT));                 \
  subcs reg_op, reg_op, #0x100;                                               \
  adcs reg_a, reg_a, reg_op, ror #8;                                          \
  bicvc reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  mov reg_p_nz, reg_a, asr #24                                                \

#define op_adc_d_set_t_clear(address_mode, address_range)                     \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
                                                                              \
  /* Setup */                                                                 \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
                                                                              \
  /* Compute low digit */                                                     \
  and reg_p_nz, reg_op, #0xF;                                                 \
  subcs reg_p_nz, reg_p_nz, #0x10;                                            \
  mov reg_p_nz, reg_p_nz, ror #4;                                             \
  adcs reg_p_nz, reg_p_nz, reg_a, lsl #4;                                     \
  cmncc reg_p_nz, #(0x6 << 28);                                               \
  addcs reg_p_nz, reg_p_nz, #(0x6 << 28);                                     \
                                                                              \
  /* Compute high digit */                                                    \
  mov reg_op, reg_op, lsr #4;                                                 \
  adc reg_op, reg_op, reg_a, lsr #28;                                         \
  movs reg_op, reg_op, lsl #28;                                               \
  cmncc reg_op, #(0x6 << 28);                                                 \
  addcs reg_op, reg_op, #(0x6 << 28);                                         \
                                                                              \
  /* Combine digits and finalize */                                           \
  mov reg_p_nz, reg_p_nz, lsr #0x4;                                           \
  orr reg_a, reg_p_nz, reg_op;                                                \
  mov reg_p_nz, reg_a, asr #24                                                \


#define op_adc_d_clear_t_set(address_mode, address_range)                     \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  load_mem_zp_sh(reg_t1, reg_x);                                              \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #((1 << V_FLAG_BIT) | (1 << C_FLAG_BIT));                 \
  mov reg_t1, reg_t1, lsl #24;                                                \
  subcs reg_op, reg_op, #0x100;                                               \
  adcs reg_t1, reg_t1, reg_op, ror #8;                                        \
  bicvc reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  mov reg_p_nz, reg_t1, asr #24;                                              \
  store_mem_zp_sh(reg_p_nz, reg_x)                                            \

#define op_adc_d_set_t_set(address_mode, address_range)                       \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  load_mem_zp_sh(reg_t1, reg_x);                                              \
                                                                              \
  /* Compute low digit */                                                     \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  and reg_p_nz, reg_op, #0xF;                                                 \
  subcs reg_p_nz, reg_p_nz, #0x10;                                            \
  mov reg_p_nz, reg_p_nz, ror #4;                                             \
  adcs reg_p_nz, reg_p_nz, reg_t1, lsl #28;                                   \
  cmncc reg_p_nz, #(0x6 << 28);                                               \
  addcs reg_p_nz, reg_p_nz, #(0x6 << 28);                                     \
                                                                              \
  /* Compute high digit */                                                    \
  mov reg_op, reg_op, lsr #4;                                                 \
  adc reg_op, reg_op, reg_t1, lsr #4;                                         \
  movs reg_op, reg_op, lsl #28;                                               \
  cmncc reg_op, #(0x6 << 28);                                                 \
  addcs reg_op, reg_op, #(0x6 << 28);                                         \
                                                                              \
  /* Combine digits and finalize */                                           \
  orr reg_p_nz, reg_op, reg_p_nz, lsr #4;                                     \
  mov reg_p_nz, reg_p_nz, asr #24;                                            \
                                                                              \
  store_mem_zp_sh(reg_p_nz, reg_x)                                            \




#define op_adc(address_mode, address_range, d_flag, t_flag)                   \
  op_adc_d_##d_flag##_t_##t_flag(address_mode, address_range)                 \

// For this operation the existing 0's will propagate the sbc, but will
// also set the lower bits high, so it'll have to be cleared afterwards.

#define op_sbc_d_clear(address_mode, address_range)                           \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  movs reg_t1, reg_p, lsr #1;                                                 \
  orr reg_p, reg_p, #((1 << C_FLAG_BIT) | (1 << V_FLAG_BIT));                 \
  sbcs reg_a, reg_a, reg_op, lsl #24;                                         \
  and reg_a, reg_a, #(0xFF << 24);                                            \
  bicvc reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  mov reg_p_nz, reg_a, asr #24                                                \


#define op_sbc_d_set(address_mode, address_range)                             \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
                                                                              \
  /* Compute low digit */                                                     \
  movs reg_t1, reg_p, lsr #1;                                                 \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  mov reg_t1, reg_a, lsl #4;                                                  \
  sbcs reg_t1, reg_t1, reg_op, lsl #28;                                       \
  and reg_t1, reg_t1, #(0xF << 28);                                           \
  subcc reg_t1, reg_t1, #(0x6 << 28);                                         \
                                                                              \
  /* Compute high digit */                                                    \
  mov reg_op, reg_op, lsr #4;                                                 \
  rscs reg_op, reg_op, reg_a, lsr #28;                                        \
  subcc reg_op, reg_op, #0x6;                                                 \
                                                                              \
  mov reg_op, reg_op, lsl #28;                                                \
  orr reg_a, reg_op, reg_t1, lsr #0x4;                                        \
  mov reg_p_nz, reg_a, asr #24                                                \


#define op_sbc(address_mode, address_range, d_flag)                           \
  op_sbc_d_##d_flag(address_mode, address_range)                              \



/** Compare/test operations **************************************************/

#define op_cmp_var(address_mode, address_range, var)                          \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  subs reg_t1, var, reg_op, lsl #24;                                          \
  mov reg_p_nz, reg_t1, asr #24                                               \

#define op_cmp(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, reg_a)                              \

#define op_cpx(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, reg_x)                              \

#define op_cpy(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, reg_y)                              \

#define op_bit(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  tst reg_op, #(1 << V_FLAG_BIT);                                             \
  bic reg_p, reg_p, #(1 << V_FLAG_BIT);                                       \
  orrne reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  and reg_p_nz, reg_op, reg_a, lsr #24;                                       \
  orr reg_p_nz, reg_p_nz, reg_op, lsl #24                                     \

#define op_tst(address_mode, address_range)                                   \
  op_load_imm(immediate, reg_p_nz, ldrb);                                     \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  tst reg_op, #(1 << V_FLAG_BIT);                                             \
  bic reg_p, reg_p, #(1 << V_FLAG_BIT);                                       \
  orrne reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  and reg_t1, reg_op, reg_p_nz;                                               \
  orr reg_p_nz, reg_t1, reg_op, lsl #24                                       \


/** Bitwise operations *******************************************************/

#define logic_op_t_flag_clear(address_mode, address_range, operator)          \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  operator reg_a, reg_a, reg_op, lsl #24;                                     \
  mov reg_p_nz, reg_a, asr #24                                                \

#define logic_op_t_flag_set(address_mode, address_range, operator)            \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  load_mem_zp_sh(reg_t1, reg_x);                                              \
  operator reg_t1, reg_t1, reg_op;                                            \
  orr reg_p_nz, reg_t1, reg_t1, lsl #24;                                      \
  store_mem_zp_sh(reg_t1, reg_x)                                              \

#define op_and(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, and)                  \

#define op_eor(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, eor)                  \

#define op_ora(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, orr)                  \

#define op_rmb_reg(var, bit_num)                                              \
  bic var, var, #(1 << (bit_num))                                             \

#define op_smb_reg(var, bit_num)                                              \
  orr var, var, #(1 << (bit_num))                                             \

#define op_rmb(address_mode, address_range, opcode)                           \
  op_rmw_##address_range(address_mode, reg_t1, op_rmb_reg, opcode >> 4)       \

#define op_smb(address_mode, address_range, opcode)                           \
  op_rmw_##address_range(address_mode, reg_t1, op_smb_reg, (opcode >> 4) - 8) \

// TODO: Refactor these with tst into a smaller macro.. perhaps

#define op_tsb_reg(var, extra)                                                \
  bic reg_p, reg_p, #(1 << V_FLAG_BIT);                                       \
  tst var, #(1 << V_FLAG_BIT);                                                \
  orrne reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  orr reg_p_nz, var, reg_a, lsr #24;                                          \
  orr reg_p_nz, reg_p_nz, var, lsl #24;                                       \
  orr var, var, reg_a, lsr #24                                                \

#define op_trb_reg(var, extra)                                                \
  bic reg_p, reg_p, #(1 << V_FLAG_BIT);                                       \
  tst var, #(1 << V_FLAG_BIT);                                                \
  orrne reg_p, reg_p, #(1 << V_FLAG_BIT);                                     \
  bic reg_p_nz, var, reg_a, lsr #24;                                          \
  orr reg_p_nz, reg_p_nz, var, lsl #24;                                       \
  bic var, var, reg_a, lsr #24                                                \

#define op_tsb(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_tsb_reg, na)                \

#define op_trb(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_trb_reg, na)                \



/** Bitshifts ****************************************************************/

// arg must be reg_p_nz for this to make any sense. Be sure to pass that to
// this (must go for the ext builder too)

#define op_shift_lsr(arg, extra)                                              \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  movs arg, arg, lsr #1                                                       \

#define op_shift_asl(arg, extra)                                              \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  mov arg, arg, lsl #1;                                                       \
  orrs reg_p_nz, arg, arg, lsl #24                                            \

#define op_shift_rol(arg, extra)                                              \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  adc arg, arg, arg;                                                          \
  orrs reg_p_nz, arg, arg, lsl #24                                            \

#define op_shift_ror(arg, extra)                                              \
  movs reg_p_nz, reg_p, lsr #1;                                               \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  orrcs arg, arg, #(0x1 << 8);                                                \
  movs arg, arg, lsr #1;                                                      \
  orr reg_p_nz, arg, arg, lsl #24                                             \


// lsr can't set the sign bit true, so shifting all the way down should be
// fine. This will also shift off for the c bit.

#define op_lsra(address_mode, address_range)                                  \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  movs reg_p_nz, reg_a, lsr #25;                                              \
  mov reg_a, reg_p_nz, lsl #24                                                \

#define op_lsr(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_p_nz, op_shift_lsr, na)            \

// This on the other hand must propagate the n bit.

#define op_asla(address_mode, address_range)                                  \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  movs reg_a, reg_a, lsl #1;                                                  \
  orr reg_p_nz, reg_a, reg_a, asr #24                                         \

#define op_asl(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_shift_asl, na)              \

#define op_rola(address_mode, address_range)                                  \
  movs reg_t1, reg_p, lsr #1;                                                 \
  orrcs reg_a, reg_a, #(0x1 << 23);                                           \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  movs reg_a, reg_a, lsl #1;                                                  \
  mov reg_p_nz, reg_a, asr #24                                                \

#define op_rol(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_shift_rol, na)              \

#define op_rora(address_mode, address_range)                                  \
  movs reg_t1, reg_p, lsr #1;                                                 \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT);                                       \
  mov reg_a, reg_a, rrx;                                                      \
  movs reg_t1, reg_a, lsr #24;                                                \
  and reg_a, reg_a, #(0xFF << 24);                                            \
  mov reg_p_nz, reg_a, asr #24                                                \

#define op_ror(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_shift_ror, na)              \



/** Increment/decrement operations *******************************************/

#define op_dec_reg(arg, extra)                                                \
  sub arg, arg, #1;                                                           \
  orr reg_p_nz, arg, arg, lsl #24                                             \

#define op_inc_reg(arg, extra)                                                \
  add arg, arg, #1;                                                           \
  orr reg_p_nz, arg, arg, lsl #24                                             \

#define op_dea(address_mode, address_range)                                   \
  sub reg_a, reg_a, #(1 << 24);                                               \
  mov reg_p_nz, reg_a, asr #24                                                \

#define op_dex(address_mode, address_range)                                   \
  sub reg_x, reg_x, #(1 << 24);                                               \
  mov reg_p_nz, reg_x, asr #24                                                \

#define op_dey(address_mode, address_range)                                   \
  sub reg_y, reg_y, #(1 << 24);                                               \
  mov reg_p_nz, reg_y, asr #24                                                \

#define op_dec(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_dec_reg, na)                \

#define op_ina(address_mode, address_range)                                   \
  add reg_a, reg_a, #(1 << 24);                                               \
  mov reg_p_nz, reg_a, asr #24                                                \

#define op_inx(address_mode, address_range)                                   \
  add reg_x, reg_x, #(1 << 24);                                               \
  mov reg_p_nz, reg_x, asr #24                                                \

#define op_iny(address_mode, address_range)                                   \
  add reg_y, reg_y, #(1 << 24);                                               \
  mov reg_p_nz, reg_y, asr #24                                                \

#define op_inc(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, reg_t1, op_inc_reg, na)                \



/** Near branches ************************************************************/

// Unfortunately, at least one game branches from one region to a totally
// different one (RAM to CD-RAM), requiring a complete update_pc cycle here.

#define conditional_branch_cv(flag_bit, condition)                            \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p, #(1 << flag_bit);                                                \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed(4)                                                      \

#define conditional_branch_n(condition)                                       \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p_nz, #(1 << 31);                                                   \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed(4)                                                      \

#define conditional_branch_z(condition)                                       \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p_nz, #0xFF;                                                        \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed(4)                                                      \

#define op_bcc(address_mode, address_range)                                   \
  conditional_branch_cv(C_FLAG_BIT, ne)                                       \

#define op_bcs(address_mode, address_range)                                   \
  conditional_branch_cv(C_FLAG_BIT, eq)                                       \

#define op_bne(address_mode, address_range)                                   \
  conditional_branch_z(eq)                                                    \

#define op_beq(address_mode, address_range)                                   \
  conditional_branch_z(ne)                                                    \

#define op_bpl(address_mode, address_range)                                   \
  conditional_branch_n(ne)                                                    \

#define op_bmi(address_mode, address_range)                                   \
  conditional_branch_n(eq)                                                    \


#define op_bvc(address_mode, address_range)                                   \
  conditional_branch_cv(V_FLAG_BIT, ne)                                       \

#define op_bvs(address_mode, address_range)                                   \
  conditional_branch_cv(V_FLAG_BIT, eq)                                       \

#define op_bra(address_mode, address_range)                                   \
  fetch_8bit(reg_op, ldrsb);                                                  \
  advance_pc(reg_op)                                                          \

#define idle_conditional_branch_cv(flag_bit, condition)                       \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p, #(1 << flag_bit);                                                \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed_a()                                                     \

#define idle_conditional_branch_n(condition)                                  \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p_nz, #(1 << 31);                                                   \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed_a()                                                     \

#define idle_conditional_branch_z(condition)                                  \
  fetch_8bit(reg_op, ldrsb);                                                  \
  tst reg_p_nz, #0xFF;                                                        \
  b##condition branch_untaken_a;                                              \
  advance_pc(reg_op);                                                         \
  op_fetch_next_fixed_a()                                                     \


#define op_ibcc(address_mode, address_range)                                  \
  idle_conditional_branch_cv(C_FLAG_BIT, ne)                                  \

#define op_ibcs(address_mode, address_range)                                  \
  idle_conditional_branch_cv(C_FLAG_BIT, eq)                                  \

#define op_ibne(address_mode, address_range)                                  \
  idle_conditional_branch_z(eq)                                               \

#define op_ibeq(address_mode, address_range)                                  \
  idle_conditional_branch_z(ne)                                               \

#define op_ibpl(address_mode, address_range)                                  \
  idle_conditional_branch_n(ne)                                               \

#define op_ibmi(address_mode, address_range)                                  \
  idle_conditional_branch_n(eq)                                               \


#define op_ibra(address_mode, address_range)                                  \
  sub_pc(1);                                                                  \
  op_fetch_next_fixed_a()                                                     \

#define op_ijmp(address_mode, address_range)                                  \
  sub_pc(1);                                                                  \
  op_fetch_next_fixed_a()                                                     \

#define conditional_branch_bit(bit, condition)                                \
  fetch_8bit(reg_t2, ldrsb);                                                  \
  tst reg_op, #(1 << (bit));                                                  \
  b##condition branch_untaken_b;                                              \
  advance_pc(reg_t2);                                                         \
  op_fetch_next_fixed(8)                                                      \

// Here t1 is not the offset but is instead the bit number to test against.
// Need to reload the fetched instruction for the sake of check_irq...

#define idle_conditional_branch_bit_self(condition)                           \
  fetch_8bit(reg_t2, ldrb);                                                   \
  mov reg_t1, #1;                                                             \
  tst reg_op, reg_t1, lsl reg_t2;                                             \
  b##condition branch_untaken_b;                                              \
  sub_pc(3);                                                                  \
  op_fetch_next_fixed_a()                                                     \

#define op_bbr(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  conditional_branch_bit(opcode >> 4, ne)                                     \

#define op_bbs(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  conditional_branch_bit((opcode >> 4) - 8, eq)                               \

// These branch to self always for now.

#define op_ibbr(address_mode, address_range)                                  \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  idle_conditional_branch_bit_self(ne)                                        \

#define op_ibbs(address_mode, address_range)                                  \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  idle_conditional_branch_bit_self(eq)                                        \


#define op_bsr(address_mode, address_range)                                   \
  retrieve_pc(reg_t0);                                                        \
  mov reg_t1, reg_t0, lsr #8;                                                 \
  op_push(reg_t1);                                                            \
  op_push(reg_t0);                                                            \
  fetch_8bit(reg_op, ldrsb);                                                  \
  advance_pc(reg_op)                                                          \



/** Far branches *************************************************************/

#define op_jmp(address_mode, address_range)                                   \
  address_load_##address_range##_##address_mode(reg_op);                      \
  add reg_op, reg_t1, reg_ea, lsl #8;                                         \
  update_pc(reg_op)                                                           \

// The reason why this doesn't do the usual address load is so that it doesn't
// have to subtract one back from the PC.

#define op_jsr(address_mode, address_range)                                   \
  fetch_8bit(reg_ea, ldrb);                                                   \
  fetch_8bit_fixed(reg_t1);                                                   \
  retrieve_pc(reg_tr_pc);                                                     \
  add reg_op, reg_ea, reg_t1, lsl #8;                                         \
  mov reg_t1, reg_tr_pc, lsr #8;                                              \
  op_push(reg_t1);                                                            \
  op_push(reg_tr_pc);                                                         \
  update_pc(reg_op)                                                           \

// This can be optimized by having the BRK_VECTOR cached somewhere but
// really, why bother, it's not like this will ever really be used.

#define op_brk(address_mode, address_range)                                   \
  retrieve_pc(reg_t0);                                                        \
  add reg_t0, reg_t0, #0x1;                                                   \
  mov reg_t1, reg_t0, lsr #8;                                                 \
  op_push(reg_t1);                                                            \
  op_push(reg_t0);                                                            \
  load_mem_safe_const(reg_op, BRK_VECTOR, ldrh);                              \
  orr reg_p, reg_p, #(1 << I_FLAG_BIT);                                       \
  bic reg_p, reg_p, #((1 << D_FLAG_BIT) | (1 << T_FLAG_BIT));                 \
  collapse_flags(reg_t1);                                                     \
  orr reg_t1, reg_t1, #(1 << B_FLAG_BIT);                                     \
  op_push(reg_t1);                                                            \
  update_pc(reg_op)                                                           \

// This looks really weird, but by setting the carry flag an add + 1 can be
// done with the adc; this avoids an ldr stall on the second op_pull

#define op_rts(address_mode, address_range)                                   \
  op_pull(reg_tr_pc);                                                         \
  op_pull(reg_t0);                                                            \
  movs reg_t1, reg_s, lsr #1;                                                 \
  adc reg_tr_pc, reg_tr_pc, reg_t0, lsl #8;                                   \
  update_pc(reg_tr_pc)                                                        \

#define op_rti(address_mode, address_range, d_flag)                           \
  op_pull(reg_t1);                                                            \
  bic reg_p, reg_p, #0xFF;                                                    \
  orr reg_p, reg_p, reg_t1;                                                   \
  extract_flags();                                                            \
  op_pull(reg_tr_pc);                                                         \
  op_pull(reg_t0);                                                            \
  add reg_tr_pc, reg_tr_pc, reg_t0, lsl #8;                                   \
  update_pc(reg_tr_pc);                                                       \
  tst reg_p, #(1 << D_FLAG_BIT);                                              \
  correct_table_d_flip_##d_flag()                                             \


/** Set/clear operations *****************************************************/

#define op_sec(address_mode, address_range)                                   \
  orr reg_p, reg_p, #(1 << C_FLAG_BIT)                                        \

#define d_type_set 1                                                          \

#define d_type_clear 0                                                        \

#define correct_table_d_flip_set()                                            \
  subeq reg_op_table, reg_op_table, #2048                                     \

#define correct_table_d_flip_clear()                                          \
  addne reg_op_table, reg_op_table, #2048                                     \

#define correct_table_d_set_from_d_set()                                      \

#define correct_table_d_set_from_d_clear()                                    \
  orr reg_p, reg_p, #(1 << D_FLAG_BIT);                                       \
  add reg_op_table, reg_op_table, #2048                                       \

#define correct_table_d_clear_from_d_set()                                    \
  bic reg_p, reg_p, #(1 << D_FLAG_BIT);                                       \
  sub reg_op_table, reg_op_table, #2048                                       \

#define correct_table_d_clear_from_d_clear()                                  \


#define op_sed(address_mode, address_range, d_flag)                           \
  correct_table_d_set_from_d_##d_flag()                                       \

#define op_sei(address_mode, address_range)                                   \
  orr reg_p, reg_p, #(1 << I_FLAG_BIT)                                        \

#define op_set(address_mode, address_range)                                   \

#define op_cla(address_mode, address_range)                                   \
  mov reg_a, #0                                                               \

#define op_clx(address_mode, address_range)                                   \
  mov reg_x, #0                                                               \

#define op_cly(address_mode, address_range)                                   \
  mov reg_y, #0                                                               \

#define op_clc(address_mode, address_range)                                   \
  bic reg_p, reg_p, #(1 << C_FLAG_BIT)                                        \

#define op_clv(address_mode, address_range)                                   \
  bic reg_p, reg_p, #(1 << V_FLAG_BIT)                                        \

#define op_cld(address_mode, address_range, d_flag)                           \
  correct_table_d_clear_from_d_##d_flag()                                     \


// A few games will perform cli an in interrupt handler before the interrupt
// has been acknowledged. An infinite loop doesn't happen, possibly because
// of the way fetching works on the Hu6280. Because of this, the interrupt
// shouldn't be checked until after the next instruction or so - I'm giving
// it about just one cycle to execute.


#define op_cli(address_mode, address_range)                                   \
  bic reg_p, reg_p, #(1 << I_FLAG_BIT)                                        \



/** Register transfer operations *********************************************/

#define op_transfer_reg(reg_dest, reg_src)                                    \
  mov reg_dest, reg_src;                                                      \
  mov reg_p_nz, reg_dest, asr #24                                             \

#define op_tax(address_mode, address_range)                                   \
  op_transfer_reg(reg_x, reg_a)                                               \

#define op_tay(address_mode, address_range)                                   \
  op_transfer_reg(reg_y, reg_a)                                               \

#define op_txa(address_mode, address_range)                                   \
  op_transfer_reg(reg_a, reg_x)                                               \

#define op_tya(address_mode, address_range)                                   \
  op_transfer_reg(reg_a, reg_y)                                               \

#define op_tsx(address_mode, address_range)                                   \
  bic reg_x, reg_s, #0x1;                                                     \
  mov reg_p_nz, reg_x, asr #24                                                \

#define op_txs(address_mode, address_range)                                   \
  orr reg_s, reg_x, #0x1                                                      \

// TODO: The original version tested to see if the code segment changes. I
// think this is probably unlikely and it's kinda expensive so I left it out.

#define op_tam(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  stmdb sp!, { r0, r2, r3, r12 };                                             \
  mov r1, reg_a, lsr #24;                                                     \
  call_c(mpr_write);                                                          \
  ldmia sp!, { r0, r2, r3, r12 };                                             \
  tst r0, #0x2;                                                               \
  ldrne reg_zero_page, [reg_mem_table, #0x4];                                 \
  movne reg_zero_page, reg_zero_page, lsl #1;                                 \
  addne reg_zero_page, reg_zero_page, #0x2000                                 \

#define op_tma(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  stmdb sp!, { r2, r3, r12 };                                                 \
  call_c(mpr_read);                                                           \
  ldmia sp!, { r2, r3, r12 };                                                 \
  mov reg_a, r0, lsl #24                                                      \



/** Swap operations **********************************************************/

#define op_swap(op_a, op_b)                                                   \
  mov reg_t1, op_a;                                                           \
  mov op_a, op_b;                                                             \
  mov op_b, reg_t1                                                            \

#define op_sax(address_mode, address_range)                                   \
  op_swap(reg_a, reg_x)                                                       \

#define op_say(address_mode, address_range)                                   \
  op_swap(reg_a, reg_y)                                                       \

#define op_sxy(address_mode, address_range)                                   \
  op_swap(reg_x, reg_y)                                                       \



/** Load/store memory operations *********************************************/

// By loading with ldrsb the n flag is set in reg_p_nz.

#define op_ld(address_mode, address_range, reg)                               \
  op_load_##address_range(address_mode, reg_p_nz, ldrsb);                     \
  mov reg, reg_p_nz, lsl #24                                                  \

#define op_lda(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, reg_a)                                   \

#define op_ldx(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, reg_x)                                   \

#define op_ldy(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, reg_y)                                   \

#define op_sta(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, reg_a, yes)                          \

#define op_stx(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, reg_x, yes)                          \

#define op_sty(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, reg_y, yes)                          \

// This relies on the lower 8bits of reg_x being 0.

#define op_stz(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, reg_x, no)                           \



/** Stack operations *********************************************************/

#define op_push(reg)                                                          \
  strb reg, [reg_zero_page, reg_s, ror #24];                                  \
  sub reg_s, reg_s, #(0x1 << 24)                                              \

#define op_pull(reg)                                                          \
  add reg_s, reg_s, #(0x1 << 24);                                             \
  ldrb reg, [reg_zero_page, reg_s, ror #24]                                   \

#define op_pull_reg(reg)                                                      \
  add reg_s, reg_s, #(0x1 << 24);                                             \
  ldrb reg, [reg_zero_page, reg_s, ror #24];                                  \
  orr reg_p_nz, reg, reg, lsl #24;                                            \
  mov reg, reg, lsl #24                                                       \


#define op_push_reg(reg)                                                      \
  mov reg_t1, reg, lsr #24;                                                   \
  op_push(reg_t1)                                                             \

#define op_pha(address_mode, address_range)                                   \
  op_push_reg(reg_a)                                                          \

#define op_php(address_mode, address_range)                                   \
  collapse_flags(reg_t1);                                                     \
  orr reg_t1, reg_t1, #(1 << B_FLAG_BIT);                                     \
  op_push(reg_t1)                                                             \

#define op_phx(address_mode, address_range)                                   \
  op_push_reg(reg_x)                                                          \

#define op_phy(address_mode, address_range)                                   \
  op_push_reg(reg_y)                                                          \

#define op_pla(address_mode, address_range)                                   \
  op_pull_reg(reg_a)                                                          \

#define op_plp(address_mode, address_range, d_flag)                           \
  op_pull(reg_t1);                                                            \
  bic reg_p, reg_p, #0xFF;                                                    \
  orr reg_p, reg_p, reg_t1;                                                   \
  extract_flags();                                                            \
  tst reg_p, #(1 << D_FLAG_BIT);                                              \
  correct_table_d_flip_##d_flag()                                             \

#define op_plx(address_mode, address_range)                                   \
  op_pull_reg(reg_x)                                                          \

#define op_ply(address_mode, address_range)                                   \
  op_pull_reg(reg_y)                                                          \



/** Memory transfer operations ***********************************************/

@ Do not use reg_cycles (r11), reg_tr_pc (r7) or reg_mem_table (r8) - first two
@ are written to inside, third is read from. reg_op_table (r12) may be used but
@ only after a fetch is made. Really only r4-r6, and r9-r10 are viable inside,
@ but r8 is okay in the inner loop. Just save/restore it.

#define reg_t3               r2
#define reg_src              r4
#define reg_dest             r5
#define reg_length           r6
#define reg_src2             r8
#define reg_dest2            r8
#define reg_remaining_length r9
#define reg_saved_ptrs       r10


@ This is the strategy for transfers that cross segment boundaries:
@ 1) Get initial args. Total length is stored to reg_remaining_length, to
@    indicate that all of it is left to go.
@ 2) Boundary cross is detected by:
@     - If the transfer is in the position direction, adding the
@       length << 19 by the address << 19.
@     - If the transfer is in the negative direction, subtracting the
@       length << 19 by the address << 19.
@    If carry is set then a boundary is crossed; if not, goto 5.
@ 3) reg_length is calculated as:
@     - If the transfer is in the positive direction, -(address << 19) >> 19.
@     - If the transfer is in the negative direction, (address << 19) >> 19.
@ 4) reg_src + reg_length and reg_dest + reg_length are stored in the
@    lower and upper halfwords of reg_saved_ptrs respectively.
@ 5) reg_remaining_length is decrement by reg_length.
@ 6) Transfer is done.
@ 7) If reg_remaining_length is non-zero then reg_length is reloaded
@    from reg_remaining_length and reg_src/reg_dest from reg_saved_ptrs,
@    goto step 2.

@ Sometimes this can get broken in the middle of a tia or tai's pair of
@ transfers, meaning that when the next run goes it has to do a half pair
@ first. To determine this, the top bit of reg_remaining_length is used to
@ flag such a thing.

@ Loads src, dest, and length, and corrects length if it's 0.

#define op_transfer_start(name, src, dest, length)                            \
  stmdb sp!, { r2 - r6, r8 - r10, r12 };                                      \
  fetch_16bit(src);                                                           \
  add src, reg_t1, src, lsl #8;                                               \
  fetch_16bit(dest);                                                          \
  add dest, reg_t1, dest, lsl #8;                                             \
  fetch_16bit(length);                                                        \
  sub reg_cycles, reg_cycles, #(51 << 8);                                     \
  adds length, reg_t1, length, lsl #8;                                        \
  moveq length, #0x10000;                                                     \
  mov reg_remaining_length, reg_length;                                       \
                                                                              \
 name##_redo:                                                                 \


#define op_transfer_done(name)                                                \
  bics reg_length, reg_remaining_length, #0x80000000;                         \
  bne name##_recover_remaining_length;                                        \
                                                                              \
  ldmia sp!, { r2 - r6, r8 - r10, r12 };                                      \
  op_fetch_next_fixed(0)                                                      \

#define op_transfer_recover_remaining_length(name)                            \
 name##_recover_remaining_length:;                                            \
  mov reg_src, reg_saved_ptrs, lsl #16;                                       \
  mov reg_src, reg_src, lsr #16;                                              \
  mov reg_dest, reg_saved_ptrs, lsr #16;                                      \
  b name##_redo                                                               \


@ Loads reg_t3 with memory map address. Sets the carry flag if ext, clears if
@ not. The reason why there's a src and dest one is because load and store
@ use different map tables.

#define op_transfer_prepare_address_src(reg, base)                            \
  and base, reg, #0xe000;                                                     \
  ldr base, [reg_mem_table, base, lsr #(13 - 2)];                             \
  movs base, base, lsl #1                                                     \

#define op_transfer_prepare_address_dest(reg, base)                           \
  and base, reg, #0xe000;                                                     \
  add base, base, #0x10000;                                                   \
  ldr base, [reg_mem_table, base, lsr #(13 - 2)];                             \
  movs base, base, lsl #1                                                     \


@ Takes away cycles for a normal transfer.

#define op_transfer_take_cycles(length)                                       \
  mov reg_t0, #18;                                                            \
  mul reg_t0, length, reg_t0;                                                 \
  sub reg_cycles, reg_cycles, reg_t0, lsl #8                                  \

@ Takes away cycles for an ext transfer. The address passed only need have
@ the lower 13 bits intact from the original one.

#define op_transfer_take_cycles_ext(address, length)                          \
  mov reg_t0, #18;                                                            \
  tst address, #0x1800;                                                       \
  addeq reg_t0, reg_t0, #3;                                                   \
  mul reg_t0, length, reg_t0;                                                 \
  sub reg_cycles, reg_cycles, reg_t0, lsl #8                                  \

@ Although this is rare, it's possible for the ext bits to change during
@ the transfer.

#define op_transfer_take_cycles_ext_partial_up(address, length)               \
  mov reg_t0, #18;                                                            \
  mul reg_t0, length, reg_t0;                                                 \
  sub reg_cycles, reg_cycles, reg_t0, lsl #8;                                 \
  tst address, #0x1800;                                                       \
  bic reg_t0, address, #0xe000;                                               \
  bleq transfer_take_cycles_ext_up                                            \


#define op_transfer_check_boundary_up(reg)                                    \
  add r0, reg, reg_length;                                                    \
  eor r0, r0, reg;                                                            \
  tst r0, #0xe000;                                                            \
  blne transfer_fix_length_single_##reg##_up                                  \

#define op_transfer_check_boundary_dual_up(reg)                               \
  add r0, reg, reg_length;                                                    \
  eor r0, r0, reg;                                                            \
  tst r0, #0xe000;                                                            \
  blne transfer_fix_length_dual_##reg##_up                                    \

#define op_transfer_check_boundary_dual_down(reg)                             \
  sub r0, reg, reg_length;                                                    \
  eor r0, r0, reg;                                                            \
  tst r0, #0xe000;                                                            \
  blne transfer_fix_length_dual_##reg##_down                                  \

@ Either one argument may be non-ext, but if the first argument is non-ext
@ to non-ext it assumes the first value is fixed (copied from ROM).

#define op_tai(address_mode, address_range)                                   \
  op_transfer_start(tai, reg_src, reg_dest, reg_length);                      \
  op_transfer_check_boundary_up(reg_dest);                                    \
  sub reg_remaining_length, reg_remaining_length, reg_length;                 \
  op_transfer_prepare_address_src(reg_src, reg_t3);                           \
  bcs tai_ext_src;                                                            \
  add reg_src, reg_t3, reg_src;                                               \
  op_transfer_prepare_address_dest(reg_dest, reg_t2);                         \
  bcs tai_ext_dest;                                                           \
  add reg_dest, reg_t2, reg_dest;                                             \
                                                                              \
  /* Neither argument are ext.                                              */\
  /* Unfortunately, some games have these overlap, so they can't be kept as */\
  /* constant values to be stored.                                          */\
  op_transfer_take_cycles(reg_length);                                        \
  sub reg_length, reg_length, #2;                                             \
                                                                              \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  addne reg_length, reg_length, #1;                                           \
  ldrneb r1, [reg_src, #1];                                                   \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  ldrb r0, [reg_src];                                                         \
  ldrb r1, [reg_src, #1];                                                     \
  strb r0, [reg_dest], #1;                                                    \
2:;                                                                           \
  strb r1, [reg_dest], #1;                                                    \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  strb r0, [reg_dest];                                                        \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  op_transfer_done(tai);                                                      \
                                                                              \
tai_ext_src:                                                                  \
  /* There are two fixed function pointers at work here. The first is to be */\
  /* stored in reg_src2 (which now has to be saved), and the second in      */\
  /* reg_src, which is no longer needed for its original value.             */\
  op_transfer_take_cycles_ext(reg_src, reg_length);                           \
  str reg_src2, [sp, #-4]!;                                                   \
  op_transfer_prepare_address_dest(reg_dest, reg_t2);                         \
  bcs tai_ext_src_dest;                                                       \
  add reg_dest, reg_t2, reg_dest;                                             \
  ldr reg_src2, [reg_t3, reg_src, lsl #2];                                    \
  add reg_src, reg_src, #1;                                                   \
  ldr reg_src, [reg_t3, reg_src, lsl #2];                                     \
  sub reg_length, reg_length, #2;                                             \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  addne reg_length, reg_length, #1;                                           \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  bl_indirect(reg_src2);                                                      \
  strb r0, [reg_dest], #1;                                                    \
2:;                                                                           \
  bl_indirect(reg_src);                                                       \
  strb r0, [reg_dest], #1;                                                    \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  bl_indirect(reg_src2);                                                      \
  strb r0, [reg_dest], #1;                                                    \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  ldr reg_src2, [sp], #4;                                                     \
  op_transfer_done(tai);                                                      \
                                                                              \
tai_ext_dest:                                                                 \
  /* The function pointer changes dynamically here, so it has to be         */\
  /* reloaded, but the base should be kept in reg_dest.                     */\
  op_transfer_take_cycles_ext_partial_up(reg_dest, reg_length);               \
  add reg_dest, reg_t2, reg_dest, lsl #2;                                     \
  sub reg_length, reg_length, #2;                                             \
  str reg_src2, [sp, #-4]!;                                                   \
  ldrb reg_src2, [reg_src], #1;                                               \
  ldrb reg_src, [reg_src];                                                    \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  addne reg_length, reg_length, #1;                                           \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  mov r0, reg_src2;                                                           \
  ldr r1, [reg_dest], #4;                                                     \
  bl_indirect(r1);                                                            \
2:;                                                                           \
  mov r0, reg_src;                                                            \
  ldr r1, [reg_dest], #4;                                                     \
  bl_indirect(r1);                                                            \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  mov r0, reg_src2;                                                           \
  ldr r1, [reg_dest];                                                         \
  bl_indirect(r1);                                                            \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  ldr reg_src2, [sp], #4;                                                     \
  op_transfer_done(tai);                                                      \
                                                                              \
tai_ext_src_dest:                                                             \
  /* Like the above two combined.                                           */\
  add reg_dest, reg_t3, reg_dest, lsl #2;                                     \
  str reg_src2, [sp, #-4]!;                                                   \
  ldr reg_src2, [reg_t3, reg_src, lsl #2];                                    \
  add reg_src, reg_src, #1;                                                   \
  ldr reg_src, [reg_t3, reg_src, lsl #2];                                     \
                                                                              \
  sub reg_length, reg_length, #2;                                             \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  subne reg_length, reg_length, #1;                                           \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  bl_indirect(reg_src2);                                                      \
  ldr r1, [reg_dest], #4;                                                     \
  bl_indirect(r1);                                                            \
2:;                                                                           \
  bl_indirect(reg_src);                                                       \
  ldr r1, [reg_dest], #4;                                                     \
  bl_indirect(r1);                                                            \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  bl_indirect(reg_src2);                                                      \
  ldr r1, [reg_dest], #4;                                                     \
  bl_indirect(r1);                                                            \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  ldr reg_src2, [sp], #4;                                                     \
  op_transfer_done(tai);                                                      \
                                                                              \
  op_transfer_recover_remaining_length(tai)                                   \


@ Dest is assumed to always be ext, because memory makes little sense.

#define op_tia(address_mode, address_range)                                   \
  op_transfer_start(tia, reg_src, reg_dest, reg_length);                      \
  op_transfer_check_boundary_up(reg_src);                                     \
  sub reg_remaining_length, reg_remaining_length, reg_length;                 \
  op_transfer_prepare_address_src(reg_src, reg_t3);                           \
  bcs tia_ext_src;                                                            \
  add reg_src, reg_t3, reg_src;                                               \
  op_transfer_prepare_address_dest(reg_dest, reg_t3);                         \
  op_transfer_take_cycles_ext(reg_dest, reg_length);                          \
  str reg_dest2, [sp, #-4]!;                                                  \
  ldr reg_dest2, [reg_t3, reg_dest, lsl #2];                                  \
  add reg_dest, reg_dest, #1;                                                 \
  ldr reg_dest, [reg_t3, reg_dest, lsl #2];                                   \
  sub reg_length, reg_length, #2;                                             \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  addne reg_length, reg_length, #1;                                           \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  ldrb r0, [reg_src], #1;                                                     \
  bl_indirect(reg_dest2);                                                     \
2:;                                                                           \
  ldrb r0, [reg_src], #1;                                                     \
  bl_indirect(reg_dest);                                                      \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  ldrb r0, [reg_src];                                                         \
  bl_indirect(reg_dest2);                                                     \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  ldr reg_dest2, [sp], #4;                                                    \
  op_transfer_done(tia);                                                      \
                                                                              \
tia_ext_src:;                                                                 \
  add reg_src, reg_t3, reg_src, lsl #2;                                       \
  op_transfer_prepare_address_dest(reg_dest, reg_t2);                         \
  op_transfer_take_cycles_ext(reg_dest, reg_length);                          \
  str reg_dest2, [sp, #-4]!;                                                  \
  ldr reg_dest2, [reg_t2, reg_dest, lsl #2];                                  \
  add reg_dest, reg_dest, #1;                                                 \
  ldr reg_dest, [reg_t2, reg_dest, lsl #2];                                   \
  sub reg_length, reg_length, #2;                                             \
  tst reg_remaining_length, #0x80000000;                                      \
  bic reg_remaining_length, reg_remaining_length, #0x80000000;                \
  addne reg_length, reg_length, #1;                                           \
  bne 2f;                                                                     \
                                                                              \
0:;                                                                           \
  ldr r0, [reg_src], #4;                                                      \
  bl_indirect(r0);                                                            \
  bl_indirect(reg_dest2);                                                     \
2:;                                                                           \
  ldr r0, [reg_src], #4;                                                      \
  bl_indirect(r0);                                                            \
  bl_indirect(reg_dest);                                                      \
  subs reg_length, reg_length, #2;                                            \
  bpl 0b;                                                                     \
                                                                              \
  adds reg_length, reg_length, #1;                                            \
  bne 1f;                                                                     \
                                                                              \
  ldr r0, [reg_src];                                                          \
  bl_indirect(r0);                                                            \
  bl_indirect(reg_dest2);                                                     \
  orr reg_remaining_length, reg_remaining_length, #0x80000000;                \
                                                                              \
1:;                                                                           \
  ldr reg_dest2, [sp], #4;                                                    \
  op_transfer_done(tia);                                                      \
                                                                              \
  op_transfer_recover_remaining_length(tia)                                   \


@ Like tia this assumes it's going to ext.

#define op_tin(address_mode, address_range)                                   \
  op_transfer_start(tin, reg_src, reg_dest, reg_length);                      \
  op_transfer_check_boundary_up(reg_src);                                     \
  sub reg_remaining_length, reg_remaining_length, reg_length;                 \
  op_transfer_prepare_address_src(reg_src, reg_t3);                           \
  bcs tin_ext_src;                                                            \
  add reg_src, reg_t3, reg_src;                                               \
  op_transfer_prepare_address_dest(reg_dest, reg_t3);                         \
  op_transfer_take_cycles_ext(reg_dest, reg_length);                          \
  ldr reg_dest, [reg_t3, reg_dest, lsl #2];                                   \
                                                                              \
0:;                                                                           \
  ldrb r0, [reg_src], #1;                                                     \
  bl_indirect(reg_dest);                                                      \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(tin);                                                      \
                                                                              \
tin_ext_src:;                                                                 \
  add reg_src, reg_t3, reg_src, lsl #2;                                       \
  op_transfer_prepare_address_dest(reg_dest, reg_t2);                         \
  op_transfer_take_cycles_ext(reg_dest, reg_length);                          \
  ldr reg_dest, [reg_t2, reg_dest, lsl #2];                                   \
                                                                              \
0:;                                                                           \
  ldr r0, [reg_src], #4;                                                      \
  bl_indirect(r0);                                                            \
  bl_indirect(reg_dest);                                                      \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(tin);                                                      \
                                                                              \
  op_transfer_recover_remaining_length(tin)                                   \


@ These assume that only src or dest will be ext, not both.

#define op_transfer_copy(direction, direction_b, name)                        \
  op_transfer_start(name, reg_src, reg_dest, reg_length);                     \
  op_transfer_check_boundary_dual_##direction_b(reg_src);                     \
  op_transfer_check_boundary_dual_##direction_b(reg_dest);                    \
  sub reg_remaining_length, reg_remaining_length, reg_length;                 \
  op_transfer_prepare_address_src(reg_src, reg_t3);                           \
  bcs name##_ext_src;                                                         \
  add reg_src, reg_t3, reg_src;                                               \
  op_transfer_prepare_address_dest(reg_dest, reg_t3);                         \
  bcs name##_ext_dest;                                                        \
  add reg_dest, reg_t3, reg_dest;                                             \
                                                                              \
  op_transfer_take_cycles(reg_length);                                        \
                                                                              \
  /* Non-ext to non-ext                                                     */\
0:;                                                                           \
  ldrb reg_t3, [reg_src], #(direction);                                       \
  strb reg_t3, [reg_dest], #(direction);                                      \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(name);                                                     \
                                                                              \
name##_ext_src:;                                                              \
  /* Here reg_src becomes a floating function pointer to the read           */\
  /* function.                                                              */\
  op_transfer_take_cycles_ext(reg_src, reg_length);                           \
  add reg_src, reg_t3, reg_src, lsl #2;                                       \
  op_transfer_prepare_address_dest(reg_dest, reg_t3);                         \
  bcs name##_ext_src_dest;                                                    \
  add reg_dest, reg_t3, reg_dest;                                             \
                                                                              \
0:;                                                                           \
  ldr r0, [reg_src], #(direction * 4);                                        \
  bl_indirect(r0);                                                            \
  strb r0, [reg_dest], #(direction);                                          \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(name);                                                     \
                                                                              \
name##_ext_dest:;                                                             \
  /* Here reg_dest becomes a floating function pointer to the write         */\
  /* function.                                                              */\
  op_transfer_take_cycles_ext(reg_dest, reg_length);                          \
  /* reg_src has already been setup.                                        */\
  add reg_dest, reg_t3, reg_dest, lsl #2;                                     \
                                                                              \
0:;                                                                           \
  ldrb r0, [reg_src], #(direction);                                           \
  ldr r1, [reg_dest], #(direction * 4);                                       \
  bl_indirect(r1);                                                            \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(name);                                                     \
                                                                              \
name##_ext_src_dest:;                                                         \
  add reg_dest, reg_t3, reg_dest, lsl #2;                                     \
                                                                              \
0:;                                                                           \
  ldr r0, [reg_src], #(direction * 4);                                        \
  bl_indirect(r0);                                                            \
  ldr r1, [reg_dest], #(direction * 4);                                       \
  bl_indirect(r1);                                                            \
  subs reg_length, reg_length, #1;                                            \
  bne 0b;                                                                     \
                                                                              \
  op_transfer_done(name);                                                     \
                                                                              \
  op_transfer_recover_remaining_length(name)                                  \


#define op_tii(address_mode, address_range)                                   \
  op_transfer_copy(1, up, tii)                                                \

#define op_tdd(address_mode, address_range)                                   \
  op_transfer_copy(-1, down, tdd)                                             \



/** Null operations **********************************************************/

#define op_nop(address_mode, address_range)                                   \



/** VDC port operations ******************************************************/

#define op_st0(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  stmdb sp!, { r2, r3, r12 };                                                 \
  call_c(vdc_register_select);                                                \
  ldmia sp!, { r2, r3, r12 }                                                  \

#define op_st1(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  stmdb sp!, { r2, r3, r12 };                                                 \
  call_c(vdc_data_write_low);                                                 \
  ldmia sp!, { r2, r3, r12 }                                                  \

#define op_st2(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, reg_op, ldrb);                        \
  stmdb sp!, { r2, r3, r12 };                                                 \
  call_c(vdc_data_write_high);                                                \
  ldmia sp!, { r2, r3, r12 }                                                  \



/** Clockspeed operations ****************************************************/

#define op_csh(address_mode, address_range)                                   \
  ldr reg_t0, [sp, #sp_cpu_struct];        /* t0 = cpu                      */\
  ldrb reg_t1, [reg_t0, #0xD];             /* t1 = cpu.cpu_divider          */\
  cmp reg_t1, #12;                         /* see if mode is already fast   */\
  bne 1f;                                                                     \
                                                                              \
  mov reg_t1, reg_cycles, lsr #8;          /* t1 = cycles                   */\
  add reg_t1, reg_t1, #1;                  /* fix offset                    */\
  ldr reg_t2, [reg_t0, #0x14];             /* t2 = cpu.extra_cycles         */\
  add reg_t2, reg_t2, reg_t1, lsl #2;      /* t2 = extra + (cycles * 4)     */\
  sub reg_t2, reg_t2, #1;                  /* fix offset                    */\
  and reg_cycles, reg_cycles, #0xFF;       /* set cycles                    */\
  orr reg_cycles, reg_cycles, reg_t2, lsl #8;                                 \
  mov reg_t1, #3;                          /* set cpu.cpu_divider = 3       */\
  strb reg_t1, [reg_t0, #0xD];                                                \
  mov reg_t1, #0;                          /* set cpu.extra_cycles = 0      */\
  str reg_t1, [reg_t0, #0x14];                                                \
  op_fetch_next_fixed(9);                                                     \
                                                                              \
 1:;                                                                          \
  op_fetch_next_fixed(3)                                                      \

#define op_csl(address_mode, address_range)                                   \
  ldr reg_t0, [sp, #sp_cpu_struct];        /* t0 = cpu                      */\
  ldrb reg_t1, [reg_t0, #0xD];             /* t1 = cpu.cpu_divider          */\
  cmp reg_t1, #3;                          /* see if mode is already slow   */\
  bne 1f;                                                                     \
                                                                              \
  mov reg_t1, reg_cycles, lsr #8;          /* t1 = cycles                   */\
  sub reg_t1, reg_t1, #17;                 /* t1 = cycles - 18              */\
  and reg_t2, reg_t1, #0x3;                /* t2 = (cycles - 18) & 0x3      */\
  str reg_t2, [reg_t0, #0x14];             /* cpu.extra_cycles = t2         */\
  mov reg_t1, reg_t1, lsr #2;              /* t1 = cycles / 4               */\
  sub reg_t1, reg_t1, #1;                  /* fix offset                    */\
  and reg_cycles, reg_cycles, #0xFF;       /* set cycles                    */\
  orr reg_cycles, reg_cycles, reg_t1, lsl #8;                                 \
  mov reg_t1, #12;                                                            \
  strb reg_t1, [reg_t0, #0xD];             /* cpu.cpu_divider = 12          */\
  op_fetch_next_fixed(0);                                                     \
                                                                              \
 1:;                                                                          \
  op_fetch_next_fixed(3)                                                      \


// Comment this in/out to enable/disable debugging

#ifdef DEBUGGER_ON

#define perform_step_debug()                                                  \
   bl _step_debug                                                             \

#else

#define perform_step_debug()                                                  \

#endif

// Defining op will specify what op_list should expand to when used
// (op_list is defined in op_list.h)

#include "../op_list.h"

#define cycle_check_irq_main_undo_fetch_yes()                                 \
  sub reg_tr_pc, reg_tr_pc, #1                                                \

#define cycle_check_irq_main_undo_fetch_no()                                  \

#define cycle_check_irq_main_use_delayed_status_yes()                         \
  ldr r14, [sp, #sp_irq_check_status]                                         \

#define cycle_check_irq_main_use_delayed_status_no()                          \
  ldr r14, [r14, #0x4]                                                        \

#define cycle_check_irq_main(undo_fetch, use_delayed_status)                  \
  ldr r14, [sp, #sp_irq_struct]           ;/* r14 = irq                     */\
  ldr r1, [r14, #0x0]                     ;/* r1 = irq.enable               */\
  /* Load the IRQ status, either the real status or the delayed one.        */\
  cycle_check_irq_main_use_delayed_status_##use_delayed_status();             \
  bic r14, r14, r1                        ;/* only look at enabled IRQs     */\
  tst r14, #(IRQ_VDC | IRQ_TIMER | IRQ_CD);/* see if an IRQ has been risen  */\
  beq 1f                                  ;/* if not abort                  */\
  tst reg_p, #(1 << I_FLAG_BIT)           ;/* and IRQs must be on           */\
  bne 1f                                  ;/* if not abort                  */\
                                                                              \
  retrieve_pc(reg_tr_pc);                                                     \
  cycle_check_irq_main_undo_fetch_##undo_fetch();                             \
  mov r0, reg_tr_pc, lsr #8               ;/* upper bits of pc              */\
  op_push(r0)                             ;/* push upper bits               */\
  op_push(reg_tr_pc)                      ;/* push lower bits               */\
                                                                              \
  collapse_flags(r0)                      ;/* get flags                     */\
  bic r0, r0, #(1 << B_FLAG_BIT)          ;/* clear B flag                  */\
  op_push(r0)                             ;/* push them                     */\
                                                                              \
  /* Now that an IRQ has been risen we have to pick the correct IRQ vector. */\
  /* Select these from lowest priority to highest priority.                 */\
  mov r0, #0x10000                        ;/* they're near this page offset */\
  tst r14, #(IRQ_CD)                      ;/* is it a timer IRQ?            */\
  subne r1, r0, #0xA                      ;/* exception vector is 0xfff6    */\
  tst r14, #(IRQ_VDC)                     ;/* is it a VDC IRQ?              */\
  subne r1, r0, #0x8                      ;/* exception vector is 0xfff8    */\
  tst r14, #(IRQ_TIMER)                   ;/* is it a timer IRQ?            */\
  subne r1, r0, #0x6                      ;/* exception vector is 0xfffa    */\
                                                                              \
  ldr r14, [reg_mem_table, #0x1c]         ;/* load top page in address space*/\
  orr reg_p, reg_p, #(1 << I_FLAG_BIT)    ;/* disable interrupts            */\
  bic reg_p, reg_p, #(1 << D_FLAG_BIT)    ;/* clear D flag                  */\
  mov r14, r14, lsl #1                    ;/* correct table entry           */\
                                                                              \
  ldrh r0, [r14, r1]                      ;/* load IRQ vector into PC       */\
  update_pc(r0)                           ;/* get new pc                    */\
                                                                              \
  /* Take out cycles for the IRQ                                            */\
  sub reg_cycles, reg_cycles, #((8 * 3) << 8)                                 \



#define op_branch_next()                                                      \
  ldrpl pc, [reg_op_table, reg_t0, lsl #2];                                   \
  ldr pc, [sp, #sp_break_function]                                            \

#define op_fetch_next_fixed(cycles)                                           \
  fetch_8bit(reg_t0, ldrb);                                                   \
  subs reg_cycles, reg_cycles, #(((cycles) * 3) << 8);                        \
  op_branch_next()                                                            \

#ifdef CPU_ARM_FAST_MODE

#define op_fetch_next_fixed_c(cycles)                                         \
  fetch_8bit(reg_t0, ldrb);                                                   \
  sbcs reg_cycles, reg_cycles, #(((cycles) * 3) << 8);                        \
  op_branch_next()                                                            \

// Load IRQ status from here, since that's what will be used in the check.

#define op_fetch_next_fixed_i(cycles)                                         \
  ldr reg_t1, [sp, #sp_irq_struct];                                           \
  subs reg_t0, reg_cycles, #(((cycles * 3) << 8));                            \
  ldr reg_t1, [reg_t1, #0x4];                                                 \
  and reg_cycles, reg_cycles, #0xFF;                                          \
  str reg_t1, [sp, #sp_irq_check_status];                                     \
  ldr reg_t1, [sp, #sp_irq_check_function];                                   \
  mov reg_t0, reg_t0, lsr #8;                                                 \
  str reg_t0, [sp, #sp_resume_cycles];                                        \
  fetch_8bit(reg_t0, ldrb);                                                   \
  str reg_t1, [sp, #sp_break_function];                                       \
  ldr pc, [reg_op_table, reg_t0, lsl #2]                                      \

#endif

#ifdef CPU_ARM_COMPATIBILITY_MODE

#define op_fetch_next_fixed_c(cycles)                                         \
  biccc reg_cycles, reg_cycles, #1;                                           \
  fetch_8bit(reg_t0, ldrb);                                                   \
  subs reg_cycles, reg_cycles, #(((cycles) * 3) << 8);                        \
  op_branch_next()                                                            \

#define op_fetch_next_fixed_i(cycles)                                         \
  subs reg_t0, reg_cycles, #(((cycles * 3) << 8));                            \
  and reg_cycles, reg_cycles, #0xFF;                                          \
  ldr reg_t1, [sp, #sp_irq_check_function];                                   \
  mov reg_t0, reg_t0, lsr #8;                                                 \
  str reg_t0, [sp, #sp_resume_cycles];                                        \
  str reg_t1, [sp, #sp_break_function];                                       \
  fetch_8bit(reg_t0, ldrb);                                                   \
  ldr pc, [reg_op_table, reg_t0, lsl #2]                                      \

#endif

#define op_fetch_next_fixed_t(cycles)                                         \
  fetch_8bit(reg_t0, ldrb);                                                   \
  subs reg_cycles, reg_cycles, #(((cycles) * 3) << 8);                        \
  add reg_t1, reg_op_table, #(256 * 4);                                       \
  ldr pc, [reg_t1, reg_t0, lsl #2]                                            \

#define op_fetch_next_fixed_x(cycles)                                         \
  cycle_check_irq_main(no, no);                                               \
                                                                              \
1:;                                                                           \
  fetch_8bit(reg_t0, ldrb);                                                   \
  subs reg_cycles, reg_cycles, #((cycles * 3) << 8);                          \
  op_branch_next()                                                            \

#define op_fetch_next_fixed_a()                                               \
  mvn reg_t0, #9;                                                             \
  and reg_cycles, reg_cycles, #0xFF;                                          \
  orr reg_cycles, reg_cycles, reg_t0, lsl #8;                                 \
  fetch_8bit(reg_t0, ldrb);                                                   \
  ldr pc, [sp, #sp_break_function]                                            \


#define op_fetch_next_variable(cycles)                                        \


#define op(opc, operation, addr_mode, addr_range, cyc, cyc_usage, d_f, t_f)   \
  op_mode_dispatch_d_##d_f(opc, operation, addr_mode, addr_range, t_f);       \
  op_fetch_next_d_##d_f(cyc_usage, cyc, t_f);                                 \

// Generate opcode handlers (these are labelled blocks that are branched to
// to execute every opcode)

// First, all operations that are neither d nor t flag sensitive (a majority of
// them): these will filter out non na or op for d_flag and non na
// for t_flag. Note that d_op are known to not be t_flag sensitive. So it
// will cover:

// na, na
// d_op, na

// The filtering works because the op_list will pass d_flag and t_flag only
// for ops that are sensitive to it, otherwise it will force na. By having it
// pass through filter we can catch then ignore these.

#define op_sd2(operation, addr_mode, addr_range)                              \
  perform_step_debug();                                                       \
  op_##operation(addr_mode, addr_range)                                       \

#define op_sd3(operation, addr_mode, addr_range, a1)                          \
  perform_step_debug();                                                       \
  op_##operation(addr_mode, addr_range, a1)                                   \

#define op_sd4(operation, addr_mode, addr_range, a1, a2)                      \
  perform_step_debug();                                                       \
  op_##operation(addr_mode, addr_range, a1, a2)                               \


#define op_mode_dispatch_t_na(operation, address_mode, address_range)         \
op_##operation##_##address_mode##_##address_range:;                           \
  op_sd2(operation, address_mode, address_range)                              \

#define op_mode_dispatch_t_filter(opc, operation, addr_mode)
#define op_mode_dispatch_d_filter(opc, operation, addr_mode, addr_range, t_f)
#define op_mode_dispatch_dt_filter(operation, addr_mode, addr_range, d_flag)

#define op_mode_dispatch_d_na(opc, operation, addr_mode, addr_range, t_flag)  \
  op_mode_dispatch_t_##t_flag(operation, addr_mode, addr_range)               \

#define op_mode_dispatch_d_op(opcode, operation, addr_mode, addr_range, t_f)  \
op_##operation##_##addr_mode##_##addr_range##_op_##opcode:;                   \
  op_sd3(operation, addr_mode, addr_range, opcode)                            \


#define op_fetch_next_t_filter(cycle_usage, cycles)
#define op_fetch_next_d_filter(cycle_usage, cycles, t_flag)

#define op_fetch_next_t_na(cycle_usage, cycles)                               \
  op_fetch_next_##cycle_usage(cycles)                                         \

#define op_fetch_next_d_na(cycle_usage, cycles, t_flag)                       \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \

#define op_fetch_next_d_op(cycle_usage, cycles, t_flag)                       \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \


op_list(filter, filter)


// Now, operations that are d/t sensitive.
// First, d flag only, so:

// d_set, na
// d_clear, na

// d_set and d_clear will pass through, but then only dt_na will pass
// afterwards.

#undef op_mode_dispatch_d_na
#undef op_mode_dispatch_d_op
#undef op_fetch_next_t_na
#undef op_fetch_next_d_na
#undef op_fetch_next_d_op

#define op_mode_dispatch_dt_na(operation, addr_mode, addr_range, d_flag)      \
op_##operation##_##addr_mode##_##addr_range##_d_##d_flag:;                    \
  op_sd3(operation, addr_mode, addr_range, d_flag)                            \

#define op_mode_dispatch_d_na(opcode, operation, addr_mode, addr_range, t_f)
#define op_mode_dispatch_d_op(opcode, operation, addr_mode, addr_range, t_f)

#define op_mode_dispatch_d_clear(opc, operation, addr_mode, addr_range, t_f)  \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, clear)          \

#define op_mode_dispatch_d_set(opcode, operation, addr_mode, addr_range, t_f) \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, set)            \


#define op_fetch_next_t_na(cycle_usage, cycles)                               \
  op_fetch_next_##cycle_usage(cycles)                                         \

#define op_fetch_next_d_na(cycle_usage, cycles, t_flag)
#define op_fetch_next_d_op(cycle_usage, cycles, t_flag)

#define op_fetch_next_d_clear(cycle_usage, cycles, t_flag)                    \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \

#define op_fetch_next_d_set(cycle_usage, cycles, t_flag)                      \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \


op_list(set, filter);
op_list(clear, filter);


// Operations that are t flag only, so:

// na, t_set
// na, t_clear

// d_na will pass through, then t_set and t_clear will pass.

#undef op_mode_dispatch_d_na
#undef op_mode_dispatch_t_na

#undef op_fetch_next_d_na
#undef op_fetch_next_t_na

#define op_mode_dispatch_t_na(operation, address_mode, address_range)

#define op_mode_dispatch_t_clear(operation, address_mode, address_range)      \
op_##operation##_##address_mode##_##address_range##_t_clear:;                 \
  op_sd3(operation, address_mode, address_range, clear)                       \

#define op_mode_dispatch_t_set(operation, address_mode, address_range)        \
op_##operation##_##address_mode##_##address_range##_t_set:;                   \
  op_sd3(operation, address_mode, address_range, set)                         \

#define op_mode_dispatch_d_na(opc, operation, addr_mode, addr_range, t_flag)  \
  op_mode_dispatch_t_##t_flag(operation, addr_mode, addr_range)               \

#define op_fetch_next_t_na(cycle_usage, cycles)

#define op_fetch_next_t_clear(cycle_usage, cycles)                            \
  op_fetch_next_##cycle_usage(cycles)                                         \

#define op_fetch_next_t_set(cycle_usage, cycles)                              \
  op_fetch_next_##cycle_usage(cycles + 3)                                     \

#define op_fetch_next_d_na(cycle_usage, cycles, t_flag)                       \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \


op_list(filter, set);
op_list(filter, clear);


// Finally, those that are both d ant t sensitive:

// d_clear, t_clear
// d_clear, t_set
// d_set, t_clear
// d_set, t_set

// So d_set and d_clear will pass through, and t_set and t_clear will pass.

#undef op_mode_dispatch_d_clear
#undef op_mode_dispatch_d_set
#undef op_mode_dispatch_d_na
#undef op_mode_dispatch_dt_set
#undef op_mode_dispatch_dt_clear
#undef op_mode_dispatch_dt_na

#undef op_fetch_next_t_clear
#undef op_fetch_next_t_set
#undef op_fetch_next_t_na
#undef op_fetch_next_d_set
#undef op_fetch_next_d_clear
#undef op_fetch_next_d_na

#define op_mode_dispatch_dt_na(operation, addr_mode, addr_range, d_flag)      \

#define op_mode_dispatch_dt_clear(operation, addr_mode, addr_range, d_flag)   \
op_##operation##_##addr_mode##_##addr_range##_d_##d_flag##_t_clear:;          \
  op_sd4(operation, addr_mode, addr_range, d_flag, clear)                     \

#define op_mode_dispatch_dt_set(operation, addr_mode, addr_range, d_flag)     \
op_##operation##_##addr_mode##_##addr_range##_d_##d_flag##_t_set:;            \
  op_sd4(operation, addr_mode, addr_range, d_flag, set)                       \

#define op_mode_dispatch_d_na(opc, operation, addr_mode, addr_range, t_flag)  \

#define op_mode_dispatch_d_clear(opc, operation, addr_mode, addr_range, t_f)  \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, clear)          \

#define op_mode_dispatch_d_set(opcode, operation, addr_mode, addr_range, t_f) \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, set)            \


#define op_fetch_next_t_na(cycle_usage, cycles)                               \

#define op_fetch_next_t_clear(cycle_usage, cycles)                            \
  op_fetch_next_##cycle_usage(cycles)                                         \

#define op_fetch_next_t_set(cycle_usage, cycles)                              \
  op_fetch_next_##cycle_usage(cycles + 3)                                     \

#define op_fetch_next_d_na(cycle_usage, cycles, t_flag)                       \

#define op_fetch_next_d_set(cycle_usage, cycles, t_flag)                      \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \

#define op_fetch_next_d_clear(cycle_usage, cycles, t_flag)                    \
  op_fetch_next_t_##t_flag(cycle_usage, cycles)                               \



op_list(clear, clear)
op_list(clear, set)
op_list(set, clear)
op_list(set, set)


// Now the op_list macro will be used to expand the four opcode tables, filled
// in with the handlers from above.

// na and op always go straight to the original ones. This time they shouldn't
// be filtered out since every table needs them.

// The following special types are possible (non-na)
// t_clear
// t_set
// d_clear
// d_set
// d_clear, t_clear
// d_clear, t_set
// d_set, t_clear
// d_clear, t_set
// op_*

#define op_mode_table_t_na(operation, address_mode, address_range)            \
  .long op_##operation##_##address_mode##_##address_range                     \

#define op_mode_table_t_clear(operation, address_mode, address_range)         \
  .long op_##operation##_##address_mode##_##address_range##_t_clear           \

#define op_mode_table_t_set(operation, address_mode, address_range)           \
  .long op_##operation##_##address_mode##_##address_range##_t_set             \

#define op_mode_table_dt_na(operation, addr_mode, addr_range, d_flag)         \
  .long op_##operation##_##addr_mode##_##addr_range##_d_##d_flag              \

#define op_mode_table_dt_clear(operation, addr_mode, addr_range, d_flag)      \
  .long op_##operation##_##addr_mode##_##addr_range##_d_##d_flag##_t_clear    \

#define op_mode_table_dt_set(operation, addr_mode, addr_range, d_flag)        \
  .long op_##operation##_##addr_mode##_##addr_range##_d_##d_flag##_t_set      \


#define op_mode_table_d_na(opcode, operation, addr_mode, addr_range, t_f)     \
  op_mode_table_t_##t_f(operation, addr_mode, addr_range)                     \

#define op_mode_table_d_clear(opc, operation, addr_mode, addr_range, t_f)     \
  op_mode_table_dt_##t_f(operation, addr_mode, addr_range, clear)             \

#define op_mode_table_d_set(opcode, operation, addr_mode, addr_range, t_f)    \
  op_mode_table_dt_##t_f(operation, addr_mode, addr_range, set)               \

#define op_mode_table_d_op(opcode, operation, addr_mode, addr_range, t_f)     \
  .long op_##operation##_##addr_mode##_##addr_range##_op_##opcode             \

#define op_mode_table_d_filter(opcode, operation, addr_mode, addr_range, t_f) \
  .long op_##operation##_##addr_mode##_##addr_range                           \


#undef op

#define op(opc, operation, addr_mode, addr_range, cyc, cyc_usage, d_f, t_f)   \
  op_mode_table_d_##d_f(opc, operation, addr_mode, addr_range, t_f);          \


#define op_table(d_flag, t_flag)                                              \
  op_table_d_##d_flag##_t_##t_flag:;                                          \
  op_list(d_flag, t_flag)                                                     \


op_table:

op_table(clear, clear)
op_table(clear, set)
op_table(set, clear)
op_table(set, set)


// Support functions for memory handlers

#define load_ext_dest_copy_r0()                                               \

#define load_ext_dest_copy_r10()                                              \
  orr r10, r0, r0, lsl #24                                                    \

@ Load a value from I/O.
@ r0: address
@ r1: io read function table base

@ The type (dest) can be either r0 or r10 - the latter will copy the value
@ into reg_p_nz appropriately.

#define load_ext_builder(dest)                                                \
load_ext_zp16_##dest:;                                                        \
  bic r0, r0, #0x10000;                                                       \
                                                                              \
load_ext_##dest:;                                                             \
  tst r0, #0x1800;                                                            \
  subeq reg_cycles, reg_cycles, #(3 << 8);                                    \
  ldr r0, [r1, r0, lsl #2];                                                   \
  stmdb sp!, { r2, r3, r12, lr };                                             \
  bl_indirect(r0);                                                            \
  load_ext_dest_copy_##dest();                                                \
  ldr r2, [sp], #4;                                                           \
  check_irq_raised_save_r0(3 * 4);                                            \
  ldmia sp!, { r3, r12, lr };                                                 \
  add pc, lr, #4                                                              \



#define store_ext_src_copy_r3()                                               \
  mov r0, r3, lsr #24                                                         \

#define store_ext_src_copy_r4()                                               \
  mov r0, r4, lsr #24                                                         \

#define store_ext_src_copy_r5()                                               \
  mov r0, r5, lsr #24                                                         \

#define store_ext_src_copy_zero()                                             \
  mov r0, #00                                                                 \

@ Store a value to I/O.
@ r0: address
@ r1: io write function table base

@ The function type (src) determines what value to store. It can either be
@ r3-r5 or "zero" to store zero out.

#define store_ext_builder(src)                                                \
store_ext_##src:;                                                             \
  tst r0, #0x1800;                                                            \
  subeq reg_cycles, reg_cycles, #(3 << 8);                                    \
  ldr r1, [r1, r0, lsl #2];                                                   \
  stmdb sp!, { r2, r3, r12, lr };                                             \
  store_ext_src_copy_##src();                                                 \
  bl_indirect(r1);                                                            \
  ldr r2, [sp], #4;                                                           \
  check_irq_raised_no_save_r0(3 * 4);                                         \
  ldmia sp!, { r3, r12, lr };                                                 \
  add pc, lr, #4                                                              \


// These aren't at all optimized for the job, but I figure they shouldn't
// actually be happening all that often anyway.

#define rmw_ext_src_copy_reg_t1()                                             \
  mov r14, r0                                                                 \

#define rmw_ext_src_copy_reg_p_nz()                                           \
  mov reg_p_nz, r0                                                            \

// Need to get the value to write back into r1 for io_write.

#define rmw_ext_dest_copy_reg_t1()                                            \
  mov r0, r14                                                                 \

#define rmw_ext_dest_copy_reg_p_nz()                                          \
  mov r0, reg_p_nz                                                            \

@ Read a value from I/O, modify it, and write it back.
@ r0: address
@ r1: io read function table base

@ The write pointer function can be obtained by adding 0x4000 to r1.

@ Here r5, reg_y is used as a temp. It's important that the operations don't
@ need reg_y for anything (they shouldn't).

#define rmw_ext_builder(operation, var, return_offset)                        \
rmw_ext_##operation:;                                                         \
  /* The address is still in reg_ea, so extract the relevant I/O portion.   */\
  tst r0, #0x1800;                                                            \
  subeq reg_cycles, reg_cycles, #(6 << 8);                                    \
  stmdb sp!, { r2, r3, r5, r12, lr };                                         \
  /* r5 = io_write base function pointer - 0x8000                           */\
  add r5, r1, r0, lsl #2;                                                     \
  /* r0 = io_read function pointer                                          */\
  ldr r0, [r5];                                                               \
  /* Call read function                                                     */\
  bl_indirect(r0);                                                            \
  /* Value read is in r0, but it needs to be in the correct place for the   */\
  /* rmw operation to work. Really only ones taking reg_p_nz need the var   */\
  /* to be that though.                                                     */\
  /* We also need reg_a back, but we put it on the stack, so retrieve it.   */\
  ldr reg_a, [sp, #0x4];                                                      \
  rmw_ext_src_copy_##var();                                                   \
  operation(var, na);                                                         \
  /* Now we need to get it back into r0 so it can be properly stored.       */\
  rmw_ext_dest_copy_##var();                                                  \
  /* And need to restore the pointer to the write function.                 */\
  add r1, r5, #0x8000;                                                        \
  ldr r1, [r1];                                                               \
  /* Operation also depends on flags not getting screwed, so save those.    */\
  mrs r5, cpsr;                                                               \
  bl_indirect(r1);                                                            \
  /* Need this for raising IRQ                                              */\
  ldr r2, [sp], #4;                                                           \
  check_irq_raised_no_save_r0(4 * 4);                                         \
  msr cpsr_f, r5;                                                             \
  ldmia sp!, { r3, r5, r12, lr };                                             \
  add pc, lr, #((return_offset + 2) * 4)                                      \


// Check cpu.alert, and if cpu.irq_raised is set raise an IRQ, otherwise
// end execution because cpu.vdc_stalled was set.

#define check_irq_raised_save_r0(offset)                                      \
  ldr r1, [sp, #(sp_cpu_struct + offset)];                                    \
  /* Check cpu.alert                                                        */\
  ldrb r14, [r1, #0x18];                                                      \
  cmp r14, #0;                                                                \
  /* No alert, skip all                                                     */\
  beq 2f;                                                                     \
  /* Clear cpu.alert                                                        */\
  mov r14, #0;                                                                \
  strb r14, [r1, #0x18];                                                      \
  /* Check cpu.irq_raised                                                   */\
  ldrb r14, [r1, #0x10];                                                      \
  cmp r14, #0;                                                                \
  /* No IRQ raised, must be VDC                                             */\
  beq 1f;                                                                     \
  /* Raise IRQ                                                              */\
  blne raise_irq_save_r0;                                                     \
  b 2f;                                                                       \
 1:;                                                                          \
  /* Must be cpu.vdc_stalled                                                */\
  /* Clear cpu_cycles_remaining                                             */\
  mvn reg_cycles, #0xFF;                                                      \
 2:                                                                           \


#define check_irq_raised_no_save_r0(offset)                                   \
  ldr r1, [sp, #(sp_cpu_struct + offset)];                                    \
  /* Check cpu.alert                                                        */\
  ldrb r0, [r1, #0x18];                                                       \
  cmp r0, #0;                                                                 \
  /* No alert, skip all                                                     */\
  beq 2f;                                                                     \
  /* Clear cpu.alert                                                        */\
  mov r0, #0;                                                                 \
  strb r0, [r1, #0x18];                                                       \
  /* Check cpu.irq_raised                                                   */\
  ldrb r0, [r1, #0x10];                                                       \
  cmp r0, #0;                                                                 \
  /* No IRQ raised, must be VDC                                             */\
  beq 1f;                                                                     \
  /* Raise IRQ                                                              */\
  blne raise_irq;                                                             \
  b 2f;                                                                       \
 1:;                                                                          \
  /* Must be cpu.vdc_stalled                                                */\
  /* Clear cpu_cycles_remaining                                             */\
  mvn reg_cycles, #0xFF;                                                      \
 2:                                                                           \


// Upon entry r1 should contain a pointer to the irq raised
// It's important that lr isn't clobbered here:
// op_push - doesn't clobber
// collapse_flags - doesn't clobber
// update_pc - doesn't clobber

@ r0: contents of cpu.irq_raised
@ r1: pointer to cpu

0:
  .long ext_symbol(irq)

irq_exception_table:
  .byte 0x00                              @ 000: no IRQ
  .byte 0xf6                              @ 001: CD IRQ
  .byte 0xf8                              @ 010: VDC IRQ
  .byte 0xf8                              @ 011: VDC IRQ wins
  .byte 0xfa                              @ 100: timer IRQ
  .byte 0xfa                              @ 101: timer IRQ wins
  .byte 0xfa                              @ 110: timer IRQ wins
  .byte 0xfa                              @ 111: timer IRQ wins

raise_irq_save_r0:
  stmdb sp!, { r0, lr }
  ldr r0, [r1, #0x8]
  bl raise_irq
  ldmia sp!, { r0, pc }

raise_irq:
  strb r4, [r1, #0x10]                    @ cpu.irq_raised = 0
  ldr r1, 0b                              @ r1 = irq

  tst reg_p, #(1 << I_FLAG_BIT)           @ see if IRQs are enabled
  ldr r1, [r1, #0x0]                      @ r1 = irq.enable
  bxne lr                                 @ irqs not enabled, return
  bics r1, r0, r1                         @ see if any of the IRQs are enabled
  bxeq lr                                 @ if not return

  retrieve_pc(reg_tr_pc)                  @ reg_tr_pc = pc
  mov r1, reg_tr_pc, lsr #8               @ r1 = upper bits of pc
  op_push(r1)                             @ push upper bits
  op_push(reg_tr_pc)                      @ push lower bits

  collapse_flags(r1)                      @ get flags
  bic r1, r1, #(1 << B_FLAG_BIT)          @ clear B flag
  op_push(r1)                             @ push them

  adr r1, irq_exception_table             @ r1 = ptr to irq_exception_table
  ldrb r0, [r1, r0]                       @ get exception ptr

  ldr r1, [reg_mem_table, #0x1C]          @ r1 = top page in address space
  orr reg_p, reg_p, #(1 << I_FLAG_BIT)    @ disable interrupts
  bic reg_p, reg_p, #(1 << D_FLAG_BIT)    @ clear D flag
  orr r0, r0, #0xFF00                     @ fill in upper bits of vector
  mov r1, r1, lsl #1                      @ fix pointer

  ldrh r0, [r1, r0]                       @ get pc vector
  @ Take out cycles for the IRQ
  sub reg_cycles, reg_cycles, #((8 * 3) << 8)
  update_pc(r0)                           @ get new pc

  bx lr                                   @ return


load_ext_builder(r0);
load_ext_builder(r10);

store_ext_builder(r3);
store_ext_builder(r4);
store_ext_builder(r5);

store_ext_builder(zero);

// These offsets are the instruction counts of the operations. Be careful
// with them. It would be a good idea to automate this instead with label
// arithmetic.

rmw_ext_builder(op_tsb_reg, reg_t1, 6);
rmw_ext_builder(op_trb_reg, reg_t1, 6);
rmw_ext_builder(op_shift_asl, reg_t1, 3);
rmw_ext_builder(op_shift_rol, reg_t1, 5);
rmw_ext_builder(op_shift_lsr, reg_p_nz, 2);
rmw_ext_builder(op_shift_ror, reg_t1, 5);
rmw_ext_builder(op_dec_reg, reg_t1, 2);
rmw_ext_builder(op_inc_reg, reg_t1, 2);


#define transfer_fix_length_up(reg)                                           \
  mvn reg_length, reg, lsl #19;                                               \
  mov reg_length, reg_length, lsr #19;                                        \
  add reg_length, reg_length, #1                                              \

#define transfer_fix_length_down(reg)                                         \
  mov reg_length, reg, lsl #19;                                               \
  mov reg_length, reg_length, lsr #19                                         \

#define transfer_adjust_ptr_reg_src_up()                                      \
  add reg_saved_ptrs, reg_src, reg_length;                                    \
  orr reg_saved_ptrs, reg_saved_ptrs, reg_dest, lsl #16                       \

#define transfer_adjust_ptr_reg_src_down()                                    \
  sub reg_saved_ptrs, reg_src, reg_length;                                    \
  orr reg_saved_ptrs, reg_saved_ptrs, reg_dest, lsl #16                       \

#define transfer_adjust_ptr_reg_dest_up()                                     \
  orr reg_saved_ptrs, reg_src, reg_dest, lsl #16;                             \
  add reg_saved_ptrs, reg_saved_ptrs, reg_length, lsl #16                     \

#define transfer_adjust_ptr_reg_dest_down()                                   \
  orr reg_saved_ptrs, reg_src, reg_dest, lsl #16;                             \
  sub reg_saved_ptrs, reg_saved_ptrs, reg_length, lsl #16                     \


#define transfer_adjust_ptr_dual_up()                                         \
  add reg_saved_ptrs, reg_src, reg_length;                                    \
  orr reg_saved_ptrs, reg_saved_ptrs, reg_dest, lsl #16;                      \
  add reg_saved_ptrs, reg_saved_ptrs, reg_length, lsl #16                     \

#define transfer_adjust_ptr_dual_down()                                       \
  sub reg_saved_ptrs, reg_src, reg_length;                                    \
  orr reg_saved_ptrs, reg_saved_ptrs, reg_dest, lsl #16;                      \
  sub reg_saved_ptrs, reg_saved_ptrs, reg_length, lsl #16                     \


#define transfer_adjust_ptr_reg_src_dual_up()                                 \
  transfer_adjust_ptr_dual_up()                                               \

#define transfer_adjust_ptr_reg_dest_dual_up()                                \
  transfer_adjust_ptr_dual_up()                                               \

#define transfer_adjust_ptr_reg_src_dual_down()                               \
  transfer_adjust_ptr_dual_down()                                             \

#define transfer_adjust_ptr_reg_dest_dual_down()                              \
  transfer_adjust_ptr_dual_down()                                             \

#define transfer_fix_length_builder(reg, direction, type, type_b)             \
transfer_fix_length_##type##_##reg##_##direction:                             \
  transfer_fix_length_##direction(reg);                                       \
  transfer_adjust_ptr_##type_b##_##direction();                               \
  bx lr                                                                       \

transfer_fix_length_builder(reg_src, up, single, reg_src);
transfer_fix_length_builder(reg_src, down, single, reg_src);
transfer_fix_length_builder(reg_dest, up, single, reg_dest);
transfer_fix_length_builder(reg_dest, down, single, reg_dest);

transfer_fix_length_builder(reg_src, up, dual, dual);
transfer_fix_length_builder(reg_src, down, dual, dual);
transfer_fix_length_builder(reg_dest, up, dual, dual);
transfer_fix_length_builder(reg_dest, down, dual, dual);

@ Don't feel like doing the down ones yet. Let's see if anything actually
@ uses them (I doubt it!)

@ reg_t0: address

transfer_take_cycles_ext_up:
  add reg_t3, reg_t0, reg_length          @ reg_t3 = address + length - 1
  sub reg_t3, reg_t3, #1
  cmp reg_t3, #0x800                      @ if address + length - 1 > 0x800
  rsbgt reg_t0, reg_t0, #0x800            @ use 0x801 - address instead
  movle reg_t0, reg_length                @ otherwise use length
  add reg_t0, reg_t0, reg_t0, lsl #1      @ t0 = length * 3
  sub reg_cycles, reg_cycles, reg_t0, lsl #8
  bx lr


// Untaken branch handlers
branch_untaken_a:
  op_fetch_next_fixed(2)

branch_untaken_b:
  op_fetch_next_fixed(6)


/*
typedef struct
{
  u64 global_cycles;                        @ offset 0x0/0x4

  u8 a;                                     @ offset 0x8
  u8 x;                                     @ offset 0x9
  u8 y;                                     @ offset 0xA
  u8 s;                                     @ offset 0xB
  u8 p;                                     @ offset 0xC
  u8 cpu_divider;                           @ offset 0xD
  u16 pc;                                   @ offset 0xE

  u32 irq_raised;                           @ offset 0x10

  u32 extra_cycles;                         @ offset 0x14
} cpu_struct;

typedef struct
{
  u32 enable;                               @ offset 0x0
  u32 status;                               @ offset 0x4
} irq_struct;

*/

// void step_debug(int a, int x, int y, int p, int s, int pc, int remaining)

#define DEBUG_RUN                  1
#define DEBUG_COUNTDOWN_BREAKPOINT 4

#define DEBUG_MODE_OFFSET                  (ext_symbol(debug) + 0x20)
#define DEBUG_BREAKPOINT_OFFSET            (ext_symbol(debug) + 0x4)
#define DEBUG_INSTRUCTION_COUNT_OFFSET     (ext_symbol(debug) + 0x0)
#define DEBUG_BREAKPOINT_ORIGINAL_OFFSET   (ext_symbol(debug) + 0x8)


0:
  .long DEBUG_MODE_OFFSET
  .long DEBUG_BREAKPOINT_OFFSET
  .long DEBUG_INSTRUCTION_COUNT_OFFSET
  .long DEBUG_BREAKPOINT_ORIGINAL_OFFSET

_step_debug:
  ldr r0, 0b                                @ r0 = debug_mode pointer
  ldr r1, 0b + 0x4                          @ r1 = breakpoint pointer
  ldr r0, [r0]                              @ r0 = debug_mode
  cmp r0, #(DEBUG_COUNTDOWN_BREAKPOINT)     @ counting down?
  bne _step_debug4                          @ if not, do a real step_debug
  ldr r0, [r1]                              @ r14 = breakpoint
  subs r0, r0, #1                           @ count down breakpoint
  beq _step_debug3                          @ if hit 0, do a real step_debug
  str r0, [r1]                              @ otherwise, store breakpoint
  bx lr                                     @ and return

_step_debug3:                               @ need to add instructions skipped
  ldr r0, 0b + 0xC                          @ r0 = breakpoint_original ptr
  ldr r1, 0b + 0x8                          @ r1 = debug_instruction_count ptr
  ldr r0, [r0]                              @ r0 = breakpoint_original
  ldr r1, [r1]                              @ r1 = debug_instruction_count
  add r1, r0, r1                            @ add skipped instructions
  ldr r0, 0b + 0x8                          @ get this pointer back
  sub r1, r1, #1                            @ actually, it's one less
  str r1, [r0]                              @ store new instruction count
  b _step_debug2                            @ go to real step debug handler

_step_debug4:                               @ see if it's run mode
  cmp r0, #(DEBUG_RUN)                      @ run mode?
  bxeq lr                                   @ if so can return

_step_debug2:
  stmdb sp!, { r2, r3, r12, lr }            @ these will be overwritten, save them
  mov r0, reg_s, lsr #24
  retrieve_pc(r1)                           @ r1 = pc
  sub r1, r1, #1                            @ roll back one to undo fetch
  mov r2, reg_cycles, asr #8                @ overwrites r2 (reg_code_page)
  add r2, r2, #1                            @ fix offset
  stmdb sp!, { r0, r1, r2 }                 @ store last three args on the stack
  mov r0, reg_a, lsr #24                    @ move reg_a into arg0
  mov r1, reg_x, lsr #24                    @ move reg_x into arg1
  mov r2, reg_y, lsr #24                    @ move reg_y into arg2
  collapse_flags(r3)                        @ move p into arg3
  call_c(step_debug)                        @ all function
  add sp, sp, #(0x4 * 3)                    @ take args off the stack
  ldmia sp!, { r2, r3, r12, pc }            @ take other stuff off, return


@ PC is overflowed, real PC is retrieved PC + 0x2000

adjust_pc_plus:
  add reg_tr_pc, reg_tr_pc, #0x1
  retrieve_pc(reg_tr_pc)
  update_pc(reg_tr_pc)
  bx lr

@ PC is underflowed, real PC is retrieved PC - 0x2000

adjust_pc_minus:
  sub reg_tr_pc, reg_tr_pc, #0x1
  retrieve_pc(reg_tr_pc)
  update_pc(reg_tr_pc)
  bx lr

@ PC is overflowed or underflowed: carry flag determines this.

adjust_pc_plus_minus:
  subcs reg_tr_pc, reg_tr_pc, #0x1
  addcc reg_tr_pc, reg_tr_pc, #0x1
  retrieve_pc(reg_tr_pc)
  update_pc(reg_tr_pc)
  bx lr



@ r0 still contains the next fetched instruction, and if cycle_check_irq_main
@ fails then it'll still be valid.

cycle_check_irq:
  cycle_check_irq_main(yes, yes)
  fetch_8bit(r0, ldrb)                      @ reload opcode

1:
  ldr r1, [sp, #sp_cycle_end_function]      @ load cycle end function ptr
  ldr r14, [sp, #sp_resume_cycles]          @ go back to this cycle count
  str r1, [sp, #sp_break_function]          @ restore
  adds reg_cycles, reg_cycles, r14, lsl #8  @ add back to cycle counter
  op_branch_next()                          @ go to next instruction maybe


#define MEMORY_MPR_TRANSLATED_OFFSET        (ext_symbol(memory) + 0x288818)

0:
  .long ext_symbol(cpu)                     @ 0 + 0x00
  .long MEMORY_MPR_TRANSLATED_OFFSET        @ 0 + 0x04
  .long op_table                            @ 0 + 0x08
  .long cycle_end                           @ 0 + 0x0C
  .long cycle_check_irq                     @ 0 + 0x10
  .long ext_symbol(irq)                     @ 0 + 0x14

cycle_end:
  add sp, sp, #0x1C                         @ deallocate stack variables
  ldr r1, 0b                                @ r0 = cpu
  @ The follow regs get packed together for slightly speedier storage
  bic r0, reg_s, #0x1                       @ starting with s
  orr r0, r0, reg_y, lsr #8                 @ orr in y
  orr r0, r0, reg_x, lsr #16                @ orr in x
  orr r0, r0, reg_a, lsr #24                @ orr in a
  str r0, [r1, #0x8]                        @ restore cpu.a/x/y/s
  collapse_flags(r0)                        @ get the flags back in r0
  strb r0, [r1, #0xC]                       @ cpu.p = flags
  retrieve_pc(r0)                           @ r0 = pc
  sub r0, r0, #1                            @ fetched one too many, undo
  strh r0, [r1, #0xE]                       @ cpu.pc = pc

  ldrb r0, [r1, #0xD]                       @ see if slow mode is on
  cmp r0, #12
  beq cycle_end_slow

  mvn r0, reg_cycles, asr #8                @ r0 = -cycles + 1
  str r0, [r1, #0x14]                       @ cpu.extra_cycles = -cycles + 1

  @ time to leave the function
  ldmia sp!, { r4, r5, r6, r7, r8, r9, r10, r11, pc }

cycle_end_slow:
  mov r2, reg_cycles, asr #8                @ r2 = cycles
  add r2, r2, #1                            @ adjust by 1
  mov r2, r2, lsl #2                        @ r2 = cycles * 4

  ldr r0, [r1, #0x14]                       @ r0 = cpu.extra_cycles
  sub r0, r0, r2                            @ r0 = extra_cycles - cycles * 4
  str r0, [r1, #0x14]                       @ cpu.extra_cycles -= cycles * 4

  @ time to leave the function
  ldmia sp!, { r4, r5, r6, r7, r8, r9, r10, r11, pc }



@ r0: s32 cpu cycles to execute

adjust_cycles_remaining_csl:
  and r2, r0, #0x3                          @ r2 = cycles % 4
  str r2, [r1, #0x14]                       @ cpu.extra_cycles = cycles % 4
  mov r0, r0, lsr #2                        @ cycles /= 4
  bx lr


#ifdef CPU_ARM_FAST_MODE
.globl ext_symbol(execute_instructions_fast)
ext_symbol(execute_instructions_fast):
#endif

#ifdef CPU_ARM_COMPATIBILITY_MODE
.globl ext_symbol(execute_instructions_compatible)
ext_symbol(execute_instructions_compatible):
#endif

  ldr r1, 0b                                @ r1 = cpu

  ldr r2, [r1, #0x0]                        @ r2 = cpu.global_cycles low
  ldr r3, [r1, #0x4]                        @ r3 = cpu.global_cycles high

  add r2, r2, r0                            @ cpu.global_cycles += cycles
  adc r3, r3, #0

  str r2, [r1, #0x0]                        @ write back cpu.global_cycles
  str r3, [r1, #0x4]

  ldr r2, [r1, #0x14]                       @ r2 = cpu.extra_cycles
  cmp r0, r2                                @ check extra cycles vs cycles

  suble r2, r2, r0                          @ extra_cycles -= cycles
  strle r2, [r1, #0x14]                     @ write back cpu.extra_cycles
  bxle lr                                   @ return

  mov r3, #0                                @ r3 = 0
  str r3, [r1, #0x18]                       @ cpu.alert = 0

  sub r0, r0, r2                            @ take away extra cycles

  ldr r2, [r1, #0x1C]                       @ r2 = cpu.vdc_stalled
  mov r3, #0                                @ r3 = 0
  str r3, [r1, #0x14]                       @ cpu.extra_cycles = 0
  cmp r2, #1                                @ check cpu.vdc_stalled
  bxeq lr                                   @ exit if it's 1

  stmdb sp!, { r4, r5, r6, r7, r8, r9, r10, r11, r14 }

  ldrb r2, [r1, #0xD]                       @ check cpu.cpu_divider
  cmp r2, #12                               @ see if it's slow speed
  bleq adjust_cycles_remaining_csl

  mov reg_cycles, r0, lsl #8                @ load cycles
  sub reg_cycles, reg_cycles, #(0x1 << 8)   @ plus one more so pl triggers

  ldrb reg_a, [r1, #0x8]                    @ reg_a = cpu.a
  ldrb reg_x, [r1, #0x9]                    @ reg_x = cpu.x
  ldrb reg_y, [r1, #0xA]                    @ reg_y = cpu.y
  ldrb reg_s, [r1, #0xB]                    @ reg_s = cpu.s
  ldrb r2, [r1, #0xC]                       @ r1 = cpu.p
  orr reg_p, reg_p, r2                      @ set flags in reg_p
  extract_flags()                           @ set reg_p_nz
  ldr reg_mem_table, 0b + 0x04              @ reg_mem_table = mpr_translated
  save_r9()                                 @ some platforms need r9 saved
  ldr reg_zero_page, [reg_mem_table, #0x4]  @ setup zero page pointer
  ldr reg_op_table, 0b + 0x08               @ setup op table
  mov reg_zero_page, reg_zero_page, lsl #1  @ convert zero page map ptr
  add reg_zero_page, reg_zero_page, #0x2000 @ adjust to the right offset

  @ adjust registers to most significant byte
  mov reg_a, reg_a, lsl #24
  mov reg_x, reg_x, lsl #24
  mov reg_y, reg_y, lsl #24
  mov reg_s, reg_s, lsl #24
  orr reg_s, reg_s, #0x1

  ldrh r0, [r1, #0xE]                       @ r0 = cpu.pc
  update_pc(r0)                             @ set pc pointer

  // The following things go on the stack (after being pushed):
  // [sp, #0x0]: where to branch to after cycles run out
  // [sp, #0x4]: handler for normal cycle end
  // [sp, #0x8]: handler for delayed IRQ check
  // [sp, #0xC]: irq_status to check on IRQ check handler
  // [sp, #0x10]: how many cycles remaining before IRQ check
  // [sp, #0x14]: pointer to cpu structure
  // [sp, #0x18]: pointer to irq structure

  mov r0, #0                                @ how many cycles to resume
  ldr r14, 0b + 0x14;                       @ irq pointer
  stmdb sp!, { r0, r1, r14 }                @ store on stack

  ldr r0, 0b + 0x0C;                        @ cycle end function
  ldr r14, 0b + 0x10;                       @ irq check end function

  // Third thing is just a filler slot, so r15 works for it.
  stmdb sp!, { r0, r14, r15 }               @ and irq check end function
  str r0, [sp, #-4]!                        @ store cycle end function twice

  ldrb r0, [r1, #0x10]                      @ r0 = cpu.irq_raised
  cmp r0, #0                                @ was an IRQ raised?
  blne raise_irq                            @ if so do it

  fetch_8bit(reg_t0, ldrb)                  @ load first opcode to execute
  tst reg_p, #(1 << D_FLAG_BIT)             @ check if in decimal mode
  addne reg_op_table, reg_op_table, #2048   @ if in dec mode adjust op table
  ldr pc, [reg_op_table, reg_t0, lsl #2]    @ set things off

