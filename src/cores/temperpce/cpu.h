#ifndef CPU_H
#define CPU_H

typedef enum
{
  BRK_VECTOR    = 0xFFF6,
  CD_VECTOR     = 0xFFF6,
  VDC_VECTOR    = 0xFFF8,
  TIMER_VECTOR  = 0xFFFA,
  NMI_VECTOR    = 0xFFFC,
  RESET_VECTOR  = 0xFFFE
} irq_vector_enum;

typedef enum
{
  C_FLAG_BIT = 0,
  Z_FLAG_BIT = 1,
  I_FLAG_BIT = 2,
  D_FLAG_BIT = 3,
  B_FLAG_BIT = 4,
  T_FLAG_BIT = 5,
  V_FLAG_BIT = 6,
  N_FLAG_BIT = 7
} cpu_flag_bits_enum;

// 20B

typedef struct
{
  u64 global_cycles;

  // These are carefully arranged to align well and make load/store
  // efficient. Don't mess with the order.
  u8 a;
  u8 x;
  u8 y;
  u8 s;
  u8 p;
  u8 cpu_divider;
  u16 pc;

  u32 irq_raised;

  u32 extra_cycles;

  // Not saved in savestate.
  u32 alert;
  u32 vdc_stalled;
  u32 cycles_remaining;
} cpu_struct;

extern cpu_struct cpu;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void initialize_cpu();
void reset_cpu();
void execute_instructions(s32 cycles_to_execute);
void execute_instructions_timer(s32 cpu_cycles_remaining);

#define op_push(val)                                                          \
  stack_page[s] = val;                                                        \
  s = (s - 1) & 0xFF                                                          \

#define op_pull(var)                                                          \
  s = (s + 1) & 0xFF;                                                         \
  var = stack_page[s]                                                         \

void patch_idle_loop();

void cpu_load_savestate(savestate_load_type savestate_file);
void cpu_store_savestate(savestate_store_type savestate_file);

void show_profile_stats(void);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
