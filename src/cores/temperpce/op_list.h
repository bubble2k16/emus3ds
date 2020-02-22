#ifndef OP_LIST_H
#define OP_LIST_H

// asla, lsra, rora, and rola aren't real mneumonics, but they're used to
// differentiate from the read/modify/write instructions.

// nop 0xEA is the "real" one so it's not filtered. This avoids duplicates.
// Some of the other nops are replaced with special opcodes used internally
// by the emulator, in the hope that games don't use these. Note: 0xEA is
// the nop used by games, so don't overwrite it.

// Special opcodes:
// ibeq - idle loop beq
// ibne - idle loop bne
// ibmi - idle loop bmi
// ibpl - idle loop bpl
// ibcc - idle loop bcc
// ibcs - idle loop bcs
// ibra - idle loop bra to self
// ijmp - idle loop jmp to self
// ibbs - idle loop bbs to self, bit # encoded in upper 4 bits of dest
// ibbr - idle loop bbr to self, bit # encoded in upper 4 bits of dest

// 0) opcode number
// 1) memory address type: abs loads a 16bit value, zp loads an 8bit value
//    from the zero page, zp16 loads a 16bit value from the zero page
// 2) if variable then the termination is not known, and the cycles taken
//    is not known and the op handler should update appropriately. fixed_c
//    and fixed_t specify that the instruction modifies c or t, and the
//    handler epilogue might be able to take advantage of this. fixed_i
//    means that interrupt state is modified and it should check it after
//    the next instruction. fixed_x means that interrupt state is modified
//    and it should check it immediately.
// 3) whether or not the operation is aware of the d_flag (either uses it or
//    modifies it, or "op" to pass through the opcode for this field. filter
//    is used if this opcode should be normally ignored
// 4) whether or not the operation is modified by the t_flag

// 0)    ins   addr. mode  1)    2) cycle use 3)      4)
#define op_list(d_flag, t_flag)                                               \
op(0x00, brk,  immediate,  imm,  8, fixed,    na,     na    )                 \
op(0x01, ora,  x_indirect, zp16, 7, fixed,    na,     t_flag)                 \
op(0x02, sxy,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x03, st0,  immediate,  imm,  5, fixed,    na,     na    )                 \
op(0x04, tsb,  direct,     zp,   6, fixed,    na,     na    )                 \
op(0x05, ora,  direct,     zp,   4, fixed,    na,     t_flag)                 \
op(0x06, asl,  direct,     zp,   6, fixed_c,  na,     na    )                 \
op(0x07, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x08, php,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x09, ora,  immediate,  imm,  2, fixed,    na,     t_flag)                 \
op(0x0A, asla, implicit,   na,   2, fixed_c,  na,     na    )                 \
op(0x0B, ibeq, label,      na,   2, variable, na,     na    )                 \
op(0x0C, tsb,  direct,     abs,  7, fixed,    na,     na    )                 \
op(0x0D, ora,  direct,     abs,  5, fixed,    na,     t_flag)                 \
op(0x0E, asl,  direct,     abs,  7, fixed_c,  na,     na    )                 \
op(0x0F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x10, bpl,  label,      na,   2, variable, na,     na    )                 \
op(0x11, ora,  indirect_y, zp16, 7, fixed,    na,     t_flag)                 \
op(0x12, ora,  indirect,   zp16, 7, fixed,    na,     t_flag)                 \
op(0x13, st1,  immediate,  imm,  5, fixed,    na,     na    )                 \
op(0x14, trb,  direct,     zp,   6, fixed,    na,     na    )                 \
op(0x15, ora,  direct_x,   zp,   4, fixed,    na,     t_flag)                 \
op(0x16, asl,  direct_x,   zp,   6, fixed_c,  na,     na    )                 \
op(0x17, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x18, clc,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x19, ora,  direct_y,   abs,  5, fixed,    na,     t_flag)                 \
op(0x1A, ina,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x1B, ibne, implicit,   na,   2, variable, na,     na    )                 \
op(0x1C, trb,  direct,     abs,  7, fixed,    na,     na    )                 \
op(0x1D, ora,  direct_x,   abs,  5, fixed,    na,     t_flag)                 \
op(0x1E, asl,  direct_x,   abs,  7, fixed_c,  na,     na    )                 \
op(0x1F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x20, jsr,  direct,     abs,  7, fixed,    na,     na    )                 \
op(0x21, and,  x_indirect, zp16, 7, fixed,    na,     t_flag)                 \
op(0x22, sax,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x23, st2,  immediate,  imm,  5, fixed,    na,     na    )                 \
op(0x24, bit,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0x25, and,  direct,     zp,   4, fixed,    na,     t_flag)                 \
op(0x26, rol,  direct,     zp,   6, fixed_c,  na,     na    )                 \
op(0x27, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x28, plp,  implicit,   na,   4, fixed_i,  d_flag, na    )                 \
op(0x29, and,  immediate,  imm,  2, fixed,    na,     t_flag)                 \
op(0x2A, rola, implicit,   na,   2, fixed_c,  na,     na    )                 \
op(0x2B, ibra, label,      na,   4, variable, na,     na    )                 \
op(0x2C, bit,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0x2D, and,  direct,     abs,  5, fixed,    na,     t_flag)                 \
op(0x2E, rol,  direct,     abs,  7, fixed_c,  na,     na    )                 \
op(0x2F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x30, bmi,  label,      na,   2, variable, na,     na    )                 \
op(0x31, and,  indirect_y, zp16, 7, fixed,    na,     t_flag)                 \
op(0x32, and,  indirect,   zp16, 7, fixed,    na,     t_flag)                 \
op(0x33, ijmp, direct,     abs,  4, variable, na,     na    )                 \
op(0x34, bit,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0x35, and,  direct_x,   zp,   4, fixed,    na,     t_flag)                 \
op(0x36, rol,  direct_x,   zp,   6, fixed_c,  na,     na    )                 \
op(0x37, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x38, sec,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x39, and,  direct_y,   abs,  5, fixed,    na,     t_flag)                 \
op(0x3A, dea,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x3B, ibbr, direct,     zp,   8, variable, na,     na    )                 \
op(0x3C, bit,  direct_x,   abs,  5, fixed,    na,     na    )                 \
op(0x3D, and,  direct_x,   abs,  5, fixed,    na,     t_flag)                 \
op(0x3E, rol,  direct_x,   abs,  7, fixed_c,  na,     na    )                 \
op(0x3F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x40, rti,  implicit,   na,   7, fixed_x,  d_flag, na    )                 \
op(0x41, eor,  x_indirect, zp16, 7, fixed,    na,     t_flag)                 \
op(0x42, say,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x43, tma,  immediate,  imm,  4, fixed,    na,     na    )                 \
op(0x44, bsr,  label,      na,   8, fixed,    na,     na    )                 \
op(0x45, eor,  direct,     zp,   4, fixed,    na,     t_flag)                 \
op(0x46, lsr,  direct,     zp,   6, fixed_c,  na,     na    )                 \
op(0x47, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x48, pha,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x49, eor,  immediate,  imm,  2, fixed,    na,     t_flag)                 \
op(0x4A, lsra, implicit,   na,   2, fixed_c,  na,     na    )                 \
op(0x4B, ibbs, direct,     zp,   8, variable, na,     na    )                 \
op(0x4C, jmp,  direct,     abs,  4, fixed,    na,     na    )                 \
op(0x4D, eor,  direct,     abs,  5, fixed,    na,     t_flag)                 \
op(0x4E, lsr,  direct,     abs,  7, fixed_c,  na,     na    )                 \
op(0x4F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x50, bvc,  label,      na,   2, variable, na,     na    )                 \
op(0x51, eor,  indirect_y, zp16, 7, fixed,    na,     t_flag)                 \
op(0x52, eor,  indirect,   zp16, 7, fixed,    na,     t_flag)                 \
op(0x53, tam,  immediate,  imm,  5, fixed,    na,     na    )                 \
op(0x54, csl,  implicit,   na,   3, variable, na,     na    )                 \
op(0x55, eor,  direct_x,   zp,   4, fixed,    na,     t_flag)                 \
op(0x56, lsr,  direct_x,   zp,   6, fixed_c,  na,     na    )                 \
op(0x57, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x58, cli,  implicit,   na,   2, fixed_i,  na,     na    )                 \
op(0x59, eor,  direct_y,   abs,  5, fixed,    na,     t_flag)                 \
op(0x5A, phy,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0x5B, ibmi, label,      na,   2, variable, na,     na    )                 \
op(0x5C, ibpl, label,      na,   2, variable, na,     na    )                 \
op(0x5D, eor,  direct_x,   abs,  5, fixed,    na,     t_flag)                 \
op(0x5E, lsr,  direct_x,   abs,  7, fixed_c,  na,     na    )                 \
op(0x5F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x60, rts,  implicit,   na,   7, fixed,    na,     na    )                 \
op(0x61, adc,  x_indirect, zp16, 7, fixed_c,  d_flag, t_flag)                 \
op(0x62, cla,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x63, ibcc, label,      na,   2, variable, na,     na    )                 \
op(0x64, stz,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0x65, adc,  direct,     zp,   4, fixed_c,  d_flag, t_flag)                 \
op(0x66, ror,  direct,     zp,   6, fixed_c,  na,     na    )                 \
op(0x67, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x68, pla,  implicit,   na,   4, fixed,    na,     na    )                 \
op(0x69, adc,  immediate,  imm,  2, fixed_c,  d_flag, t_flag)                 \
op(0x6A, rora, implicit,   na,   2, fixed_c,  na,     na    )                 \
op(0x6B, ibcs, label,      na,   2, variable, na,     na    )                 \
op(0x6C, jmp,  indirect,   abs,  7, fixed,    na,     na    )                 \
op(0x6D, adc,  direct,     abs,  5, fixed_c,  d_flag, t_flag)                 \
op(0x6E, ror,  direct,     abs,  7, fixed_c,  na,     na    )                 \
op(0x6F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x70, bvs,  label,      na,   2, variable, na,     na    )                 \
op(0x71, adc,  indirect_y, zp16, 7, fixed_c,  d_flag, t_flag)                 \
op(0x72, adc,  indirect,   zp16, 7, fixed_c,  d_flag, t_flag)                 \
op(0x73, tii,  transfer,   na,   *, variable, na,     na    )                 \
op(0x74, stz,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0x75, adc,  direct_x,   zp,   4, fixed_c,  d_flag, t_flag)                 \
op(0x76, ror,  direct_x,   zp,   6, fixed_c,  na,     na    )                 \
op(0x77, rmb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x78, sei,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x79, adc,  direct_y,   abs,  5, fixed_c,  d_flag, t_flag)                 \
op(0x7A, ply,  implicit,   na,   4, fixed,    na,     na    )                 \
op(0x7B, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0x7C, jmp,  x_indirect, abs,  7, fixed,    na,     na    )                 \
op(0x7D, adc,  direct_x,   abs,  5, fixed_c,  d_flag, t_flag)                 \
op(0x7E, ror,  direct_x,   abs,  7, fixed_c,  na,     na    )                 \
op(0x7F, bbr,  direct,     zp,   6, variable, op,     na    )                 \
op(0x80, bra,  label,      na,   4, fixed,    na,     na    )                 \
op(0x81, sta,  x_indirect, zp16, 7, fixed,    na,     na    )                 \
op(0x82, clx,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x83, tst,  direct,     zp,   7, fixed,    na,     na    )                 \
op(0x84, sty,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0x85, sta,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0x86, stx,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0x87, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x88, dey,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x89, bit,  immediate,  imm,  2, fixed,    na,     na    )                 \
op(0x8A, txa,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x8B, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0x8C, sty,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0x8D, sta,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0x8E, stx,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0x8F, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0x90, bcc,  label,      na,   2, variable, na,     na    )                 \
op(0x91, sta,  indirect_y, zp16, 7, fixed,    na,     na    )                 \
op(0x92, sta,  indirect,   zp16, 7, fixed,    na,     na    )                 \
op(0x93, tst,  direct,     abs,  8, fixed,    na,     na    )                 \
op(0x94, sty,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0x95, sta,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0x96, stx,  direct_y,   zp,   4, fixed,    na,     na    )                 \
op(0x97, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0x98, tya,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x99, sta,  direct_y,   abs,  5, fixed,    na,     na    )                 \
op(0x9A, txs,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0x9B, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0x9C, stz,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0x9D, sta,  direct_x,   abs,  5, fixed,    na,     na    )                 \
op(0x9E, stz,  direct_x,   abs,  5, fixed,    na,     na    )                 \
op(0x9F, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xA0, ldy,  immediate,  imm,  2, fixed,    na,     na    )                 \
op(0xA1, lda,  x_indirect, zp16, 7, fixed,    na,     na    )                 \
op(0xA2, ldx,  immediate,  imm,  2, fixed,    na,     na    )                 \
op(0xA3, tst,  direct_x,   zp,   7, fixed,    na,     na    )                 \
op(0xA4, ldy,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0xA5, lda,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0xA6, ldx,  direct,     zp,   4, fixed,    na,     na    )                 \
op(0xA7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xA8, tay,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xA9, lda,  immediate,  imm,  2, fixed,    na,     na    )                 \
op(0xAA, tax,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xAB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xAC, ldy,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0xAD, lda,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0xAE, ldx,  direct,     abs,  5, fixed,    na,     na    )                 \
op(0xAF, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xB0, bcs,  label,      na,   2, variable, na,     na    )                 \
op(0xB1, lda,  indirect_y, zp16, 7, fixed,    na,     na    )                 \
op(0xB2, lda,  indirect,   zp16, 7, fixed,    na,     na    )                 \
op(0xB3, tst,  direct_x,   abs,  8, fixed,    na,     na    )                 \
op(0xB4, ldy,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0xB5, lda,  direct_x,   zp,   4, fixed,    na,     na    )                 \
op(0xB6, ldx,  direct_y,   zp,   4, fixed,    na,     na    )                 \
op(0xB7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xB8, clv,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xB9, lda,  direct_y,   abs,  5, fixed,    na,     na    )                 \
op(0xBA, tsx,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xBB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xBC, ldy,  direct_x,   abs,  5, fixed,    na,     na    )                 \
op(0xBD, lda,  direct_x,   abs,  5, fixed,    na,     na    )                 \
op(0xBE, ldx,  direct_y,   abs,  5, fixed,    na,     na    )                 \
op(0xBF, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xC0, cpy,  immediate,  imm,  2, fixed_c,  na,     na    )                 \
op(0xC1, cmp,  x_indirect, zp16, 7, fixed_c,  na,     na    )                 \
op(0xC2, cly,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xC3, tdd,  transfer,   na,   *, variable, na,     na    )                 \
op(0xC4, cpy,  direct,     zp,   4, fixed_c,  na,     na    )                 \
op(0xC5, cmp,  direct,     zp,   4, fixed_c,  na,     na    )                 \
op(0xC6, dec,  direct,     zp,   6, fixed,    na,     na    )                 \
op(0xC7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xC8, iny,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xC9, cmp,  immediate,  imm,  2, fixed_c,  na,     na    )                 \
op(0xCA, dex,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xCB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xCC, cpy,  direct,     abs,  5, fixed_c,  na,     na    )                 \
op(0xCD, cmp,  direct,     abs,  5, fixed_c,  na,     na    )                 \
op(0xCE, dec,  direct,     abs,  7, fixed,    na,     na    )                 \
op(0xCF, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xD0, bne,  label,      na,   2, variable, na,     na    )                 \
op(0xD1, cmp,  indirect_y, zp16, 7, fixed_c,  na,     na    )                 \
op(0xD2, cmp,  indirect,   zp16, 7, fixed_c,  na,     na    )                 \
op(0xD3, tin,  transfer,   na,   *, variable, na,     na    )                 \
op(0xD4, csh,  implicit,   na,   3, variable, na,     na    )                 \
op(0xD5, cmp,  direct_x,   zp,   4, fixed_c,  na,     na    )                 \
op(0xD6, dec,  direct_x,   zp,   6, fixed,    na,     na    )                 \
op(0xD7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xD8, cld,  implicit,   na,   2, fixed,    d_flag, na    )                 \
op(0xD9, cmp,  direct_y,   abs,  5, fixed_c,  na,     na    )                 \
op(0xDA, phx,  implicit,   na,   3, fixed,    na,     na    )                 \
op(0xDB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xDC, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xDD, cmp,  direct_x,   abs,  5, fixed_c,  na,     na    )                 \
op(0xDE, dec,  direct_x,   abs,  7, fixed,    na,     na    )                 \
op(0xDF, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xE0, cpx,  immediate,  imm,  2, fixed_c,  na,     na    )                 \
op(0xE1, sbc,  x_indirect, zp16, 7, fixed_c,  d_flag, na    )                 \
op(0xE2, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xE3, tia,  transfer,   na,   *, variable, na,     na    )                 \
op(0xE4, cpx,  direct,     zp,   4, fixed_c,  na,     na    )                 \
op(0xE5, sbc,  direct,     zp,   4, fixed_c,  d_flag, na    )                 \
op(0xE6, inc,  direct,     zp,   6, fixed,    na,     na    )                 \
op(0xE7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xE8, inx,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xE9, sbc,  immediate,  imm,  2, fixed_c,  d_flag, na    )                 \
op(0xEA, nop,  implicit,   na,   2, fixed,    na,     na    )                 \
op(0xEB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xEC, cpx,  direct,     abs,  5, fixed_c,  na,     na    )                 \
op(0xED, sbc,  direct,     abs,  5, fixed_c,  d_flag, na    )                 \
op(0xEE, inc,  direct,     abs,  7, fixed,    na,     na    )                 \
op(0xEF, bbs,  direct,     zp,   6, variable, op,     na    )                 \
op(0xF0, beq,  label,      na,   2, variable, na,     na    )                 \
op(0xF1, sbc,  indirect_y, zp16, 7, fixed_c,  d_flag, na    )                 \
op(0xF2, sbc,  indirect,   zp16, 7, fixed_c,  d_flag, na    )                 \
op(0xF3, tai,  transfer,   na,   *, variable, na,     na    )                 \
op(0xF4, set,  implicit,   na,   2, fixed_t,  na,     na    )                 \
op(0xF5, sbc,  direct_x,   zp,   4, fixed_c,  d_flag, na    )                 \
op(0xF6, inc,  direct_x,   zp,   6, fixed,    na,     na    )                 \
op(0xF7, smb,  direct,     zp,   7, fixed,    op,     na    )                 \
op(0xF8, sed,  implicit,   na,   2, fixed,    d_flag, na    )                 \
op(0xF9, sbc,  direct_y,   abs,  5, fixed_c,  d_flag, na    )                 \
op(0xFA, plx,  implicit,   na,   4, fixed,    na,     na    )                 \
op(0xFB, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xFC, nop,  implicit,   na,   2, fixed,    filter, filter)                 \
op(0xFD, sbc,  direct_x,   abs,  5, fixed_c,  d_flag, na    )                 \
op(0xFE, inc,  direct_x,   abs,  7, fixed,    na,     na    )                 \
op(0xFF, bbs,  direct,     zp,   6, variable, op,     na    )                 \

#endif
