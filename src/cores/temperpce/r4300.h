#ifndef R4300_H
#define R4300_H

typedef struct
{
  u64 gp_regs[32];
  double fp_regs[32];
  u32 cop0_regs[32];

  // Only the even floating point registers should be accessed.
  union
  {
    double d[32];
    u64 i[32];
    float f[64];
  } fp_regs;

  u32 pc;
  u64 hi;
  u64 lo;

  u32 epc;
  u32 ecause;

  u32 fcr0;
  u32 fcr31;

  u32 ll_bit;
} r4300_struct;

#endif

