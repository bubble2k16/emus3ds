// Hu6280 CPU emulator.

#include "common.h"
#include "op_list.h"

//#define PROFILE ON
//#define PROFILE_MPR_SETUP

#ifdef PROFILE

u32 memory_accesses = 0;
u32 opcodes_processed = 0;

u32 opcode_stats[256];

u32 load_memory_accesses = 0;
u32 load_16_memory_accesses = 0;
u32 store_memory_accesses = 0;
u32 rmw_memory_accesses = 0;

u32 tma_processed = 0;
u32 tam_processed = 0;

u32 load_region_stats[4];
u32 load_16_region_stats[4];
u32 store_region_stats[4];
u32 rmw_region_stats[4];

u32 tam_stats[256];
u32 tma_stats[256];

#ifdef PROFILE_MPR_SETUP

#define MPR_CONFIGURATION_REGIONS 8

u64 region_tags[MPR_CONFIGURATION_REGIONS][1024 * 1024 * 2];
u8 region_counts[1024 * 1024 * 2];

#define profile_mpr_configuration()                                           \
{                                                                             \
  u32 pc_physical = (memory.mpr[pc_high] << 13) | pc_low;                     \
  u64 mpr_tags = *((u64 *)memory.mpr);                                        \
  u32 i;                                                                      \
                                                                              \
  for(i = 0; i < region_counts[pc_physical]; i++)                             \
  {                                                                           \
    if(mpr_tags == region_tags[i][pc_physical])                               \
      break;                                                                  \
  }                                                                           \
                                                                              \
  if((i == region_counts[pc_physical]) &&                                     \
   (region_counts[pc_physical] < MPR_CONFIGURATION_REGIONS))                  \
  {                                                                           \
    /* region miss */                                                         \
    region_counts[pc_physical]++;                                             \
    region_tags[i][pc_physical] = mpr_tags;                                   \
  }                                                                           \
}                                                                             \

#define profile_mpr_configuration_stats()                                     \
{                                                                             \
  u32 mpr_configurations[8];                                                  \
  u32 opcodes_present = 0;                                                    \
  u32 i;                                                                      \
                                                                              \
  for(i = 0; i < 1024 * 1024 * 2; i++)                                        \
  {                                                                           \
    if(region_counts[i] > 0)                                                  \
    {                                                                         \
      opcodes_present++;                                                      \
      mpr_configurations[region_counts[i]]++;                                 \
    }                                                                         \
  }                                                                           \
                                                                              \
  printf("%d instructions were in the ROM and executed at least once.\n",     \
   opcodes_present);                                                          \
                                                                              \
  printf(" %d had %d memory configuration (%llf%%)\n",                        \
   mpr_configurations[1], 1, percent_of(mpr_configurations[1],                \
   opcodes_present));                                                         \
                                                                              \
  for(i = 2; i < (MPR_CONFIGURATION_REGIONS - 1); i++)                        \
  {                                                                           \
    printf(" %d had %d memory configurations (%llf%%)\n",                     \
     mpr_configurations[i], i,                                                \
     percent_of(mpr_configurations[i], opcodes_present));                     \
  }                                                                           \
                                                                              \
  printf(" %d had %d or more memory configurations (%llf%%)\n",               \
   mpr_configurations[7], 7, percent_of(mpr_configurations[i],                \
   opcodes_present));                                                         \
}                                                                             \

#else

#define profile_mpr_configuration()

#endif

#define profile_opcode_type(op)                                               \
{                                                                             \
/* Reports if code is in RAM.                                                 \
  if(memory.mpr[debug.current_pc >> 13] == 0xF8)                              \
  {                                                                           \
    u32 disasm_pc = debug.current_pc;                                         \
    char disasm_buffer[1024];                                                 \
    disasm_instruction(disasm_buffer, &disasm_pc);                            \
    printf("pc is in RAM (%x %s)\n", debug.current_pc, disasm_buffer);        \
  } */                                                                        \
  profile_mpr_configuration();                                                \
  opcodes_processed++;                                                        \
  opcode_stats[op]++;                                                         \
}                                                                             \

typedef enum
{
  MEMORY_REGION_ZP,
  MEMORY_REGION_RAM,
  MEMORY_REGION_ROM,
  MEMORY_REGION_IO
} memory_access_types;

#define profile_memory_region(address, type)                                  \
{                                                                             \
  u32 region_type;                                                            \
  switch(memory.mpr[address >> 13])                                           \
  {                                                                           \
    case 0x00 ... 0x7F:                                                       \
    case 0x80 ... 0xF7:                                                       \
    case 0xFC ... 0xFE:                                                       \
      region_type = MEMORY_REGION_ROM;                                        \
      break;                                                                  \
                                                                              \
    case 0xF8 ... 0xFB:                                                       \
      region_type = MEMORY_REGION_RAM;                                        \
      break;                                                                  \
                                                                              \
    case 0xFF:                                                                \
      region_type = MEMORY_REGION_IO;                                         \
      break;                                                                  \
  }                                                                           \
  type##_region_stats[region_type]++;                                         \
  type##_memory_accesses++;                                                   \
  memory_accesses++;                                                          \
}                                                                             \

#define profile_paging(type, region)                                          \
  type##_processed++;                                                         \
  type##_stats[region]++                                                      \


#define profile_memory_zp(type)                                               \
{                                                                             \
  type##_region_stats[MEMORY_REGION_ZP]++;                                    \
  type##_memory_accesses++;                                                   \
  memory_accesses++;                                                          \
}                                                                             \

#define op(opcode, operation, address_mode, address_range, cycles,            \
 cycle_usage, d_flag, t_flag)                                                 \
{                                                                             \
  sprintf(opcode_names[opcode], "%s_%s_%s", #operation, #address_mode,        \
   #address_range);                                                           \
}

int profile_compare(const void *_a, const void *_b)
{
  const u32 *a = _a;
  const u32 *b = _b;

  return (b[0] - a[0]);
}

char mem_names[4][32] = { "zero page (RAM)", "RAM", "ROM", "I/O" };

#define profile_memory_stats(type)                                            \
{                                                                             \
  u32 type##_region_stats_sorted[8];                                          \
                                                                              \
  for(i = 0; i < 4; i++)                                                      \
  {                                                                           \
    type##_region_stats_sorted[i * 2] = type##_region_stats[i];               \
    type##_region_stats_sorted[(i * 2) + 1] = i;                              \
  }                                                                           \
                                                                              \
  qsort(type##_region_stats_sorted, 4, sizeof(u32) * 2,                       \
   profile_compare);                                                          \
                                                                              \
  printf("memory accesses (%s) total: %d (%lf%%):\n", #type,                  \
   type##_memory_accesses,                                                    \
   percent_of(type##_memory_accesses, memory_accesses));                      \
                                                                              \
  for(i = 0; i < 4; i++)                                                      \
  {                                                                           \
    mem_number = type##_region_stats_sorted[(i * 2) + 1];                     \
    mem_stat = type##_region_stats_sorted[i * 2];                             \
                                                                              \
    printf(" %03d: %-32s (%2.4lf%%, %d)\n", i, mem_names[mem_number],         \
     percent_of(mem_stat, type##_memory_accesses), mem_stat);                 \
  }                                                                           \
  printf("\n");                                                               \
}                                                                             \

#define profile_paging_stats(type)                                            \
{                                                                             \
  u32 type##_stats_sorted[8];                                                 \
                                                                              \
  for(i = 0; i < 256; i++)                                                    \
  {                                                                           \
    type##_stats_sorted[i * 2] = type##_stats[i];                             \
    type##_stats_sorted[(i * 2) + 1] = i;                                     \
  }                                                                           \
                                                                              \
  qsort(type##_stats_sorted, 256, sizeof(u32) * 2, profile_compare);          \
                                                                              \
  printf("%s operations total: %d:\n", #type, type##_processed);              \
                                                                              \
  for(i = 0; i < 256; i++)                                                    \
  {                                                                           \
    op_number = type##_stats_sorted[(i * 2) + 1];                             \
    op_stat = type##_stats_sorted[i * 2];                                     \
                                                                              \
    if(op_stat == 0)                                                          \
    {                                                                         \
      printf("(all other variations were not used)\n");                       \
      break;                                                                  \
    }                                                                         \
                                                                              \
    printf(" %03d: %x (%2.4lf%%, %d)\n", i, op_number,                        \
     percent_of(op_stat, type##_processed), op_stat);                         \
  }                                                                           \
  printf("\n");                                                               \
}                                                                             \


void show_profile_stats(void)
{
  u32 opcode_stats_sorted[512];
  u32 op_number, op_stat;
  u32 mem_number, mem_stat;
  u32 i;

  char opcode_names[256][32];

  for(i = 0; i < 256; i++)
  {
    opcode_stats_sorted[i * 2] = opcode_stats[i];
    opcode_stats_sorted[(i * 2) + 1] = i;
  }

  qsort(opcode_stats_sorted, 256, sizeof(u32) * 2,
   profile_compare);

  printf("opcodes total: %d (%lf per frame, %lf per second)\n",
   opcodes_processed, (double)opcodes_processed / vce.frames_rendered,
   (double)opcodes_processed / ((double)vce.frames_rendered / 60));

  op_list(na, na);

  for(i = 0; i < 256; i++)
  {
    op_number = opcode_stats_sorted[(i * 2) + 1];
    op_stat = opcode_stats_sorted[i * 2];

    if(op_stat == 0)
    {
      printf("(all other opcodes were not used)\n");
      break;
    }

    printf(" %03d: %-32s (%02x) (%2.4lf%%, %d)\n", i,
     opcode_names[op_number], op_number,
     percent_of(op_stat, opcodes_processed), op_stat);
  }

  printf("\n");
  profile_memory_stats(load);
  profile_memory_stats(load_16);
  profile_memory_stats(store);
  profile_memory_stats(rmw);

  profile_paging_stats(tma);
  profile_paging_stats(tam);

#ifdef PROFILE_MPR_SETUP
  profile_mpr_configuration_stats();
#endif
}

#undef op

#else

#define profile_opcode_type(op)
#define profile_memory_region(address, type)
#define profile_paging(type, region)
#define profile_memory_zp(type)

void show_profile_stats()
{
}

#endif

char disasmtxt[300];

/** Code fetch operations ****************************************************/

#define update_zero_segment()                                                 \
{                                                                             \
  zero_page = mpr_translate_offset(memory.mpr_translated[1], 0x2000);         \
  stack_page = zero_page + 0x100;                                             \
}                                                                             \

#define update_code_base()                                                    \
  code_base = mpr_translate_offset(memory.mpr_translated[pc_high],            \
   pc_high << 13)                                                             \

#define retrieve_pc(var)                                                      \
  var = ((pc_high << 13) + pc_low) & 0xFFFF                                   \

#define update_pc()                                                           \
{                                                                             \
  pc_high = pc >> 13;                                                         \
  pc_low = pc & 0x1FFF;                                                       \
  update_code_base();                                                         \
}                                                                             \

// If wrapping around the page boundary reload pc status
#define advance_pc(delta)                                                     \
  pc_low += delta;                                                            \
  if(pc_low >= 0x2000)                                                        \
  {                                                                           \
    retrieve_pc(pc);                                                          \
    update_pc();                                                              \
  }                                                                           \

#define fetch_8bit(dest)                                                      \
  dest = code_base[pc_low];                                                   \
  advance_pc(1)                                                               \

#define fetch_8bit_signed(dest)                                               \
  dest = ((s8 *)code_base)[pc_low];                                           \
  advance_pc(1)                                                               \

// Addresses not close to the page boundary don't run a risk of needing
// reload

#define fetch_16bit(dest)                                                     \
  if(pc_low >= 0x1FFE)                                                        \
  {                                                                           \
    u32 dest_low, dest_high;                                                  \
    fetch_8bit(dest_low);                                                     \
    fetch_8bit(dest_high);                                                    \
    dest = (dest_high << 8) | dest_low;                                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    dest = code_base[pc_low] | (code_base[pc_low + 1] << 8);                  \
    pc_low += 2;                                                              \
  }                                                                           \

#ifdef DEBUGGER_ON

#define check_write_breakpoint(src, address)                                  \
  if((debug.mode == DEBUG_WRITE_BREAKPOINT) && (address == debug.breakpoint)) \
  {                                                                           \
    printf("write breakpoint on %04x triggered (writing %02x) (pc %x)\n",     \
     address, src, debug.current_pc);                                         \
    set_debug_mode(DEBUG_STEP);                                               \
  }                                                                           \

#else

#define check_write_breakpoint(src, address)                                  \

#endif



/** Effective address generators *********************************************/

#define address_load_abs_direct(address)                                      \
  fetch_16bit(address)                                                        \

#define address_load_abs_direct_x(address)                                    \
  fetch_16bit(address);                                                       \
  address = (address + x) & 0xFFFF                                            \

#define address_load_abs_direct_y(address)                                    \
  fetch_16bit(address);                                                       \
  address = (address + y) & 0xFFFF                                            \

#define address_load_abs_indirect(address)                                    \
  address_load_abs_direct(address);                                           \
  profile_memory_region(address, load_16);                                    \
  load_mem_16(address, address);                                              \

#define address_load_abs_x_indirect(address)                                  \
  address_load_abs_direct_x(address);                                         \
  profile_memory_region(address, load_16);                                    \
  load_mem_16(address, address)                                               \

#define address_load_zp_direct(address)                                       \
  fetch_8bit(address)                                                         \

#define address_load_zp_direct_x(address)                                     \
  fetch_8bit(address);                                                        \
  address = (address + x) & 0xFF                                              \

#define address_load_zp_direct_y(address)                                     \
  fetch_8bit(address);                                                        \
  address = (address + y) & 0xFF                                              \

#define address_load_zp_indirect(address)                                     \
  address_load_zp_direct(address);                                            \
  profile_memory_zp(load_16);                                                 \
  load_mem_zp_16(address, address)                                            \

#define address_load_zp_x_indirect(address)                                   \
  address_load_zp_direct_x(address);                                          \
  profile_memory_zp(load_16);                                                 \
  load_mem_zp_16(address, address)                                            \

#define address_load_zp_indirect_y(address)                                   \
  fetch_8bit(address);                                                        \
  load_mem_zp_16(address, address);                                           \
  profile_memory_zp(load_16);                                                 \
  address = (address + y) & 0xFFFF                                            \



/** Operand load/store operations ********************************************/

#define op_load_abs(address_mode, dest)                                       \
  address_load_abs_##address_mode(address);                                   \
  profile_memory_region(address, load);                                       \
  load_mem(dest, address)                                                     \

#define op_load_zp(address_mode, dest)                                        \
  address_load_zp_##address_mode(address);                                    \
  profile_memory_zp(load);                                                    \
  load_mem_zp(dest, address)                                                  \

#define op_load_zp16(address_mode, dest)                                      \
  address_load_zp_##address_mode(address);                                    \
  profile_memory_region(address, load);                                       \
  load_mem(dest, address)                                                     \

#define op_load_imm(address_mode, dest)                                       \
  fetch_8bit(dest)                                                            \


#define op_store_abs(address_mode, src)                                       \
  address_load_abs_##address_mode(address);                                   \
  profile_memory_region(address, store);                                      \
  store_mem(src, address)                                                     \

#define op_store_zp(address_mode, src)                                        \
  address_load_zp_##address_mode(address);                                    \
  profile_memory_zp(store);                                                   \
  store_mem_zp(src, address)                                                  \

#define op_store_zp16(address_mode, src)                                      \
  address_load_zp_##address_mode(address);                                    \
  profile_memory_region(address, store);                                      \
  store_mem(src, address)                                                     \


// TODO: This can be optimized.

#define op_rmw_abs(address_mode, var, operation, extra)                       \
  address_load_abs_##address_mode(address);                                   \
  profile_memory_region(address, rmw);                                        \
  rmw_mem(var, address, operation, extra)                                     \

#define op_rmw_zp(address_mode, var, operation, extra)                        \
  address_load_zp_##address_mode(address);                                    \
  profile_memory_zp(rmw);                                                     \
  load_mem_zp(var, address);                                                  \
  operation(var, extra);                                                      \
  store_mem_zp(var, address)                                                  \


/** Flag operations **********************************************************/

#define cpu_raise_irq(vector)                                                 \
{                                                                             \
  op_take_cycles(8);                                                          \
  collapse_flags();                                                           \
  p &= ~(1 << B_FLAG_BIT);                                                    \
  op_push_pc(0);                                                              \
  op_push(p);                                                                 \
  p = (p | (1 << I_FLAG_BIT)) & ~((1 << D_FLAG_BIT) | (1 << T_FLAG_BIT));     \
  load_mem_safe_16(pc, vector);                                               \
  update_pc();                                                                \
}                                                                             \

#define check_pending_interrupts(check_status)                                \
  if(((check_status & (IRQ_CD | IRQ_VDC | IRQ_TIMER)) & ~irq.enable) &&       \
   ((p & (1 << I_FLAG_BIT)) == 0))                                            \
  {                                                                           \
    u32 irq_status = check_status & ~irq.enable;                              \
    u32 exception_ptr;                                                        \
                                                                              \
    if(irq_status & IRQ_CD)                                                   \
      exception_ptr = CD_VECTOR;                                              \
                                                                              \
    if(irq_status & IRQ_VDC)                                                  \
      exception_ptr = VDC_VECTOR;                                             \
                                                                              \
    if(irq_status & IRQ_TIMER)                                                \
      exception_ptr = TIMER_VECTOR;                                           \
                                                                              \
    cpu_raise_irq(exception_ptr);                                             \
  }                                                                           \


#define check_pending_interrupts_delay()                                      \
  check_interrupt_cycles = cpu_cycles_remaining - 1;                          \
  cpu_cycles_remaining = 1                                                    \


#define collapse_flags()                                                      \
  p = (p & ~((1 << C_FLAG_BIT) | (1 << Z_FLAG_BIT) | (1 << V_FLAG_BIT) |      \
   (1 << N_FLAG_BIT))) | (c_flag << C_FLAG_BIT) | (z_flag << Z_FLAG_BIT) |    \
   (v_flag << V_FLAG_BIT) | (n_flag << N_FLAG_BIT)                            \

#define extract_flags()                                                       \
  c_flag = (p >> C_FLAG_BIT) & 0x1;                                           \
  z_flag = (p >> Z_FLAG_BIT) & 0x1;                                           \
  v_flag = (p >> V_FLAG_BIT) & 0x1;                                           \
  n_flag = (p >> N_FLAG_BIT) & 0x1                                            \

#define logic_flags(var)                                                      \
  z_flag = (var == 0);                                                        \
  n_flag = var >> 7                                                           \



/** Binary arithmetic operations *********************************************/

#define op_adc_core_d_flag_clear(address_mode, address_range, var)            \
  op_load_##address_range(address_mode, operand);                             \
  u32 result = var + operand + c_flag;                                        \
  v_flag = ((~(var ^ operand) & (var ^ result)) >> 7) & 0x1;                  \
  var = result & 0xFF;                                                        \
  logic_flags(var);                                                           \
  c_flag = result >> 8                                                        \

#define op_adc_core_d_flag_set(address_mode, address_range, var)              \
  op_load_##address_range(address_mode, operand);                             \
  u32 digit_low = (var & 0x0F) + (operand & 0x0F) + c_flag;                   \
  u32 digit_high = (var >> 4) + (operand >> 4);                               \
                                                                              \
  if(digit_low > 0x9)                                                         \
  {                                                                           \
    digit_low -= 0xA;                                                         \
    digit_high++;                                                             \
  }                                                                           \
                                                                              \
  c_flag = 0;                                                                 \
  if(digit_high > 0x9)                                                        \
  {                                                                           \
    digit_high = (digit_high + 0x6) & 0xF;                                    \
    c_flag = 1;                                                               \
  }                                                                           \
                                                                              \
  var = digit_low | (digit_high << 4);                                        \
                                                                              \
  logic_flags(var)                                                            \

#define op_adc_t_flag_clear(address_mode, address_range, d_flag)              \
  op_adc_core_d_flag_##d_flag(address_mode, address_range, a)                 \

#define op_adc_t_flag_set(address_mode, address_range, d_flag)                \
  u32 operand_b;                                                              \
  load_mem_zp(operand_b, x);                                                  \
  op_adc_core_d_flag_##d_flag(address_mode, address_range, operand_b);        \
  store_mem_zp(operand_b, x)                                                  \


#define op_adc(address_mode, address_range, d_flag, t_flag)                   \
  op_adc_t_flag_##t_flag(address_mode, address_range, d_flag)                 \


#define op_sbc_d_flag_clear(address_mode, address_range)                      \
  op_load_##address_range(address_mode, operand);                             \
  u32 result = a - (operand + (c_flag ^ 1));                                  \
  v_flag = ((a ^ operand) & (a ^ result)) >> 7;                               \
  a = result & 0xFF;                                                          \
  logic_flags(a);                                                             \
  c_flag = (result >> 31) ^ 1                                                 \

#define op_sbc_d_flag_set(address_mode, address_range)                        \
  op_load_##address_range(address_mode, operand);                             \
  operand += (c_flag ^ 1);                                                    \
  u32 digit_low = (a & 0x0F) - (operand & 0x0F);                              \
  u32 digit_high = (a >> 4) - (operand >> 4);                                 \
                                                                              \
  if(digit_low >> 31)                                                         \
  {                                                                           \
    digit_low -= 0x6;                                                         \
    digit_high--;                                                             \
  }                                                                           \
                                                                              \
  c_flag = (digit_high >> 31) ^ 1;                                            \
                                                                              \
  if(c_flag == 0)                                                             \
    digit_high -= 0x6;                                                        \
                                                                              \
  a = ((digit_high & 0x0F) << 4) | (digit_low & 0x0F);                        \
  logic_flags(a)                                                              \

#define op_sbc(address_mode, address_range, d_flag)                           \
  op_sbc_d_flag_##d_flag(address_mode, address_range)                         \



/** Compare/test operations **************************************************/

#define bit_flags(nv_op, z_op)                                                \
  z_flag = (z_op == 0);                                                       \
  n_flag = nv_op >> 7;                                                        \
  v_flag = (nv_op >> 6) & 0x1                                                 \

#define op_cmp_var(address_mode, address_range, var)                          \
  op_load_##address_range(address_mode, operand);                             \
  u32 result = var - operand;                                                 \
  c_flag = (result >> 31) ^ 1;                                                \
  result &= 0xFF;                                                             \
  logic_flags(result)                                                         \

#define op_cmp(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, a)                                  \

#define op_cpx(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, x)                                  \

#define op_cpy(address_mode, address_range)                                   \
  op_cmp_var(address_mode, address_range, y)                                  \

#define op_bit_var(address_mode, address_range, var)                          \
  op_load_##address_range(address_mode, operand);                             \
  u32 result = var & operand;                                                 \
  bit_flags(operand, result)                                                  \

#define op_bit(address_mode, address_range)                                   \
  op_bit_var(address_mode, address_range, a)                                  \

#define op_tst(address_mode, address_range)                                   \
  u32 operand_imm;                                                            \
  op_load_imm(immediate, operand_imm);                                        \
  op_bit_var(address_mode, address_range, operand_imm)                        \



/** Bitwise operations *******************************************************/

#define logic_op_core(address_mode, address_range, operator, var)             \
  op_load_##address_range(address_mode, operand);                             \
  var operator##= operand;                                                    \
  logic_flags(var)                                                            \

#define logic_op_t_flag_clear(address_mode, address_range, operator)          \
  logic_op_core(address_mode, address_range, operator, a)                     \

#define logic_op_t_flag_set(address_mode, address_range, operator)            \
  u32 operand_b;                                                              \
  load_mem_zp(operand_b, x);                                                  \
  logic_op_core(address_mode, address_range, operator, operand_b);            \
  store_mem_zp(operand_b, x)                                                  \

#define op_and(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, &)                    \

#define op_eor(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, ^)                    \

#define op_ora(address_mode, address_range, t_flag)                           \
  logic_op_t_flag_##t_flag(address_mode, address_range, |)                    \

#define op_rmb_reg(var, bit_num)                                              \
  var &= ~(1 << bit_num)                                                      \

#define op_smb_reg(var, bit_num)                                              \
  var |= (1 << bit_num)                                                       \

#define op_rmb(address_mode, address_range, opcode)                           \
  op_rmw_##address_range(address_mode, operand, op_rmb_reg, (opcode >> 4))    \

#define op_smb(address_mode, address_range, opcode)                           \
  op_rmw_##address_range(address_mode, operand, op_smb_reg,                   \
   ((opcode >> 4) - 8))                                                       \

#define op_tsb_reg(var, extra)                                                \
  u32 result = (var) | a;                                                     \
  bit_flags(var, result);                                                     \
  var |= a                                                                    \

#define op_trb_reg(var, extra)                                                \
  u32 result = (var) & ~a;                                                    \
  bit_flags(var, result);                                                     \
  var &= ~a                                                                   \

#define op_tsb(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_tsb_reg, 0)                \

#define op_trb(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_trb_reg, 0)                \



/** Bitshifts ****************************************************************/

#define op_shift_lsr(arg, extra)                                              \
  c_flag = arg & 0x1;                                                         \
  arg = arg >> 1;                                                             \
  logic_flags(arg)                                                            \

#define op_shift_asl(arg, extra)                                              \
  c_flag = arg >> 7;                                                          \
  arg = (arg << 1) & 0xFF;                                                    \
  logic_flags(arg)                                                            \

#define op_shift_rol(arg, extra)                                              \
{                                                                             \
  u32 old_c_flag = c_flag;                                                    \
  c_flag = arg >> 7;                                                          \
  arg = ((arg << 1) & 0xFF) | old_c_flag;                                     \
  logic_flags(arg);                                                           \
}                                                                             \

#define op_shift_ror(arg, extra)                                              \
{                                                                             \
  u32 old_c_flag = c_flag;                                                    \
  c_flag = arg & 0x1;                                                         \
  arg = (arg >> 1) | (old_c_flag << 7);                                       \
  logic_flags(arg);                                                           \
}                                                                             \

#define op_lsra(address_mode, address_range)                                  \
  op_shift_lsr(a, 0)                                                          \

#define op_lsr(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_shift_lsr, 0)              \

#define op_asla(address_mode, address_range)                                  \
  op_shift_asl(a, 0)                                                          \

#define op_asl(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_shift_asl, 0)              \

#define op_rola(address_mode, address_range)                                  \
  op_shift_rol(a, 0)                                                          \

#define op_rol(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_shift_rol, 0)              \

#define op_rora(address_mode, address_range)                                  \
  op_shift_ror(a, 0)                                                          \

#define op_ror(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_shift_ror, 0)              \



/** Increment/decrement operations *******************************************/

#define op_dec_reg(arg, extra)                                                \
  arg = (arg - 1) & 0xFF;                                                     \
  logic_flags(arg)                                                            \

#define op_inc_reg(arg, extra)                                                \
  arg = (arg + 1) & 0xFF;                                                     \
  logic_flags(arg)                                                            \

#define op_dea(address_mode, address_range)                                   \
  op_dec_reg(a, 0)                                                            \

#define op_dex(address_mode, address_range)                                   \
  op_dec_reg(x, 0)                                                            \

#define op_dey(address_mode, address_range)                                   \
  op_dec_reg(y, 0)                                                            \

#define op_dec(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_dec_reg, 0)                \

#define op_ina(address_mode, address_range)                                   \
  op_inc_reg(a, 0)                                                            \

#define op_inx(address_mode, address_range)                                   \
  op_inc_reg(x, 0)                                                            \

#define op_iny(address_mode, address_range)                                   \
  op_inc_reg(y, 0)                                                            \

#define op_inc(address_mode, address_range)                                   \
  op_rmw_##address_range(address_mode, operand, op_inc_reg, 0)                \




/** Near branches ************************************************************/

#define op_branch_offset()                                                    \
  s32 offset;                                                                 \
  fetch_8bit_signed(offset);                                                  \
  advance_pc(offset)                                                          \

#define conditional_branch(condition, base_cycles)                            \
if(condition)                                                                 \
{                                                                             \
  op_branch_offset();                                                         \
  op_take_cycles(base_cycles + 2);                                            \
}                                                                             \
else                                                                          \
{                                                                             \
  advance_pc(1);                                                              \
  op_take_cycles(base_cycles);                                                \
}                                                                             \

#define op_bcc(address_mode, address_range)                                   \
  conditional_branch(c_flag == 0, 2)                                          \

#define op_bcs(address_mode, address_range)                                   \
  conditional_branch(c_flag != 0, 2)                                          \

#define op_bne(address_mode, address_range)                                   \
  conditional_branch(z_flag == 0, 2)                                          \

#define op_beq(address_mode, address_range)                                   \
  conditional_branch(z_flag != 0, 2)                                          \

#define op_bpl(address_mode, address_range)                                   \
  conditional_branch(n_flag == 0, 2)                                          \

#define op_bmi(address_mode, address_range)                                   \
  conditional_branch(n_flag != 0, 2)                                          \

#define op_bvc(address_mode, address_range)                                   \
  conditional_branch(v_flag == 0, 2)                                          \

#define op_bvs(address_mode, address_range)                                   \
  conditional_branch(v_flag != 0, 2)                                          \

#define op_bra(address_mode, address_range)                                   \
  op_branch_offset()                                                          \


#define op_ibra(address_mode, address_range)                                  \
  advance_pc(-1);                                                             \
  cpu_cycles_remaining = -9                                                   \


// For the following two macros, d_flag is overloaded to specify bit number.

#define op_bbr(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, operand);                             \
  conditional_branch((operand & (1 << (opcode >> 4))) == 0, 6)                \

#define op_bbs(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, operand);                             \
  conditional_branch(operand & (1 << ((opcode >> 4) - 8)), 6)                 \

#define op_bsr(address_mode, address_range)                                   \
  op_push_pc(0);                                                              \
  op_branch_offset()                                                          \


// Idle loops, created by the emulator

#define conditional_branch(condition, base_cycles)                            \
if(condition)                                                                 \
{                                                                             \
  op_branch_offset();                                                         \
  op_take_cycles(base_cycles + 2);                                            \
}                                                                             \
else                                                                          \
{                                                                             \
  advance_pc(1);                                                              \
  op_take_cycles(base_cycles);                                                \
}                                                                             \

#define idle_conditional_branch(condition, base_cycles)                       \
if(condition)                                                                 \
{                                                                             \
  op_branch_offset();                                                         \
  cpu_cycles_remaining = 0;                                                   \
  op_take_cycles(3);                                                          \
}                                                                             \
else                                                                          \
{                                                                             \
  advance_pc(1);                                                              \
  op_take_cycles(base_cycles);                                                \
}                                                                             \

#define idle_conditional_branch_self(condition, base_cycles)                  \
if(condition)                                                                 \
{                                                                             \
  advance_pc(-3);                                                             \
  cpu_cycles_remaining = 0;                                                   \
  op_take_cycles(3);                                                          \
}                                                                             \
else                                                                          \
{                                                                             \
  op_take_cycles(base_cycles);                                                \
}                                                                             \


#define op_ibne(address_mode, address_range)                                  \
  idle_conditional_branch(z_flag == 0, 2)                                     \

#define op_ibeq(address_mode, address_range)                                  \
  idle_conditional_branch(z_flag != 0, 2)                                     \

#define op_ibcc(address_mode, address_range)                                  \
  idle_conditional_branch(c_flag == 0, 2)                                     \

#define op_ibcs(address_mode, address_range)                                  \
  idle_conditional_branch(c_flag != 0, 2)                                     \

#define op_ibpl(address_mode, address_range)                                  \
  idle_conditional_branch(n_flag == 0, 2)                                     \

#define op_ibmi(address_mode, address_range)                                  \
  idle_conditional_branch(n_flag != 0, 2)                                     \



#define op_bbr(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, operand);                             \
  conditional_branch((operand & (1 << (opcode >> 4))) == 0, 6)                \

#define op_bbs(address_mode, address_range, opcode)                           \
  op_load_##address_range(address_mode, operand);                             \
  conditional_branch(operand & (1 << ((opcode >> 4) - 8)), 6)                 \

// These currently only work as bbr/bbs to self.

#define op_ibbr(address_mode, address_range)                                  \
{                                                                             \
  u32 bit_num;                                                                \
  op_load_##address_range(address_mode, operand);                             \
  fetch_8bit(bit_num);                                                        \
  idle_conditional_branch_self((operand & (1 << bit_num)) == 0, 6);           \
}                                                                             \

#define op_ibbs(address_mode, address_range)                                  \
{                                                                             \
  u32 bit_num;                                                                \
  op_load_##address_range(address_mode, operand);                             \
  fetch_8bit(bit_num);                                                        \
  idle_conditional_branch_self(operand & (1 << bit_num), 6);                  \
}                                                                             \



/** Far branches *************************************************************/

#define op_jmp(address_mode, address_range)                                   \
  address_load_##address_range##_##address_mode(pc);                          \
  update_pc()                                                                 \

#define op_ijmp(address_mode, address_range)                                  \
  advance_pc(-1);                                                             \
  cpu_cycles_remaining = -9                                                   \


#define op_jsr(address_mode, address_range)                                   \
  op_push_pc(1);                                                              \
  address_load_##address_range##_##address_mode(pc);                          \
  update_pc()                                                                 \

#define op_brk(address_mode, address_range)                                   \
{                                                                             \
  collapse_flags();                                                           \
  op_push_pc(1);                                                              \
  p |= (1 << B_FLAG_BIT);                                                     \
  op_push(p);                                                                 \
                                                                              \
  p = (p | (1 << I_FLAG_BIT)) & ~((1 << D_FLAG_BIT) | (1 << T_FLAG_BIT));     \
  load_mem_16(pc, BRK_VECTOR);                                                \
  update_pc();                                                                \
}                                                                             \

#define op_rts(address_mode, address_range)                                   \
  op_pull_pc(1)                                                               \


#define op_rti(address_mode, address_range, d_flag)                           \
  op_pull(p);                                                                 \
  op_pull_pc(0);                                                              \
  extract_flags();                                                            \
                                                                              \
  if(((p >> D_FLAG_BIT) & 0x1) != d_type_##d_flag)                            \
    op_d_flip_##d_flag();                                                     \
                                                                              \
  if((p & (1 << I_FLAG_BIT)) == 0)                                            \
  {                                                                           \
    check_pending_interrupts(irq.status);                                     \
  }                                                                           \


/** Set/clear operations *****************************************************/

#define op_sec(address_mode, address_range)                                   \
  c_flag = 1                                                                  \

#define d_type_set 1                                                          \

#define d_type_clear 0                                                        \

#define op_d_flip_set()                                                       \
  goto op_loop_d_clear                                                        \

#define op_d_flip_clear()                                                     \
  goto op_loop_d_set                                                          \

#define op_d_set_from_d_set()                                                 \

#define op_d_set_from_d_clear()                                               \
  goto op_loop_d_set                                                          \

#define op_sed(address_mode, address_range, d_flag)                           \
  p |= (1 << D_FLAG_BIT);                                                     \
  op_d_set_from_d_##d_flag()                                                  \

#define op_sei(address_mode, address_range)                                   \
  p |= (1 << I_FLAG_BIT)                                                      \

#define op_set(address_mode, address_range)                                   \
  goto execute_instruction_t_set                                              \

#define op_cla(address_mode, address_range)                                   \
  a = 0                                                                       \

#define op_clx(address_mode, address_range)                                   \
  x = 0                                                                       \

#define op_cly(address_mode, address_range)                                   \
  y = 0                                                                       \

#define op_clc(address_mode, address_range)                                   \
  c_flag = 0                                                                  \

#define op_clv(address_mode, address_range)                                   \
  v_flag = 0                                                                  \

#define op_d_clear_from_d_set()                                               \
  goto op_loop_d_clear                                                        \

#define op_d_clear_from_d_clear()                                             \

#define op_cld(address_mode, address_range, d_flag)                           \
  p = p & ~(1 << D_FLAG_BIT);                                                 \
  op_d_clear_from_d_##d_flag()                                                \


// A few games will perform cli an in interrupt handler before the interrupt
// has been acknowledged. An infinite loop doesn't happen, possibly because
// of the way fetching works on the Hu6280. Because of this, the interrupt
// shouldn't be checked until after the next instruction or so - I'm giving
// it about just one cycle to execute.


#define op_cli(address_mode, address_range)                                   \
  p = p & ~(1 << I_FLAG_BIT);                                                 \
  cli_captured_irq_status = irq.status;                                       \
  check_pending_interrupts_delay()                                            \



/** Register transfer operations *********************************************/

#define op_transfer_reg(reg_dest, reg_src)                                    \
  reg_dest = reg_src;                                                         \
  logic_flags(reg_dest)                                                       \

#define op_tax(address_mode, address_range)                                   \
  op_transfer_reg(x, a)                                                       \

#define op_tay(address_mode, address_range)                                   \
  op_transfer_reg(y, a)                                                       \

#define op_txa(address_mode, address_range)                                   \
  op_transfer_reg(a, x)                                                       \

#define op_tya(address_mode, address_range)                                   \
  op_transfer_reg(a, y)                                                       \

#define op_tsx(address_mode, address_range)                                   \
  op_transfer_reg(x, s)                                                       \

#define op_txs(address_mode, address_range)                                   \
  s = x                                                                       \

#define op_tam(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, operand);                             \
  profile_paging(tam, operand);                                               \
  mpr_write(operand, a);                                                      \
  if(operand & 2)                                                             \
    update_zero_segment();                                                    \
  if(operand & (1 << pc_high))                                                \
    update_code_base()                                                        \

#define op_tma(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, operand);                             \
  profile_paging(tma, operand);                                               \
  a = mpr_read(operand)                                                       \



/** Swap operations **********************************************************/

#define op_swap(op_a, op_b)                                                   \
{                                                                             \
  u32 temp = op_a;                                                            \
  op_a = op_b;                                                                \
  op_b = temp;                                                                \
}                                                                             \

#define op_sax(address_mode, address_range)                                   \
  op_swap(a, x)                                                               \

#define op_say(address_mode, address_range)                                   \
  op_swap(a, y)                                                               \

#define op_sxy(address_mode, address_range)                                   \
  op_swap(x, y)                                                               \



/** Load/store memory operations *********************************************/

#define op_ld(address_mode, address_range, reg)                               \
  op_load_##address_range(address_mode, reg);                                 \
  logic_flags(reg)                                                            \

#define op_lda(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, a)                                       \

#define op_ldx(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, x)                                       \

#define op_ldy(address_mode, address_range)                                   \
  op_ld(address_mode, address_range, y)                                       \

#define op_sta(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, a)                                   \

#define op_stx(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, x)                                   \

#define op_sty(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, y)                                   \

#define op_stz(address_mode, address_range)                                   \
  op_store_##address_range(address_mode, 0)                                   \

/** Stack operations *********************************************************/

#define op_push_pc(offset)                                                    \
  retrieve_pc(operand);                                                       \
  operand += offset;                                                          \
  op_push(operand >> 8);                                                      \
  op_push(operand)                                                            \

#define op_pull_pc(offset)                                                    \
  u32 pc_low_byte, pc_high_byte;                                              \
  op_pull(pc_low_byte);                                                       \
  op_pull(pc_high_byte);                                                      \
  pc = (pc_low_byte | (pc_high_byte << 8)) + offset;                          \
  update_pc()                                                                 \

#define op_pull_reg(reg)                                                      \
  op_pull(reg);                                                               \
  logic_flags(reg)                                                            \


#define op_pha(address_mode, address_range)                                   \
  op_push(a)                                                                  \

#define op_php(address_mode, address_range)                                   \
  collapse_flags();                                                           \
  p |= (1 << B_FLAG_BIT);                                                     \
  op_push(p)                                                                  \

#define op_phx(address_mode, address_range)                                   \
  op_push(x)                                                                  \

#define op_phy(address_mode, address_range)                                   \
  op_push(y)                                                                  \

#define op_pla(address_mode, address_range)                                   \
  op_pull_reg(a)                                                              \

#define op_plp(address_mode, address_range, d_flag)                           \
{                                                                             \
  u32 old_p = p;                                                              \
  op_pull(p);                                                                 \
  extract_flags();                                                            \
  if(((p & (1 << I_FLAG_BIT)) == 0) && ((old_p & (1 << I_FLAG_BIT))))         \
  {                                                                           \
    check_pending_interrupts_delay();                                         \
  }                                                                           \
  if(((p >> D_FLAG_BIT) & 0x1) != d_type_##d_flag)                            \
    op_d_flip_##d_flag();                                                     \
}                                                                             \

#define op_plx(address_mode, address_range)                                   \
  op_pull_reg(x)                                                              \

#define op_ply(address_mode, address_range)                                   \
  op_pull_reg(y)                                                              \



/** Memory transfer operations ***********************************************/

// If these look weird it's probably because they're ported from ARM ASM
// at their core, rather than the other way around.

#define op_transfer_start()                                                   \
  u32 source, dest;                                                           \
  s32 length;                                                                 \
  u32 saved_source, saved_dest, remaining_length;                             \
  u8 *source_ptr, *dest_ptr;                                                  \
  fetch_16bit(source);                                                        \
  fetch_16bit(dest);                                                          \
  fetch_16bit(length);                                                        \
                                                                              \
  if(length == 0)                                                             \
    length = 0x10000;                                                         \
                                                                              \
  op_take_cycles(17);                                                         \
  remaining_length = length                                                   \

#define op_transfer_take_cycles()                                             \
  op_take_cycles(length * 6)                                                  \

#define op_transfer_take_cycles_ext(var)                                      \
  if((var & 0x1800) == 0)                                                     \
    op_take_cycles(length * 7);                                               \
  else                                                                        \
    op_take_cycles(length * 6)                                                \

#define op_transfer_take_cycles_ext_partial_up(var)                           \
  op_take_cycles(length * 6);                                                 \
  if((var & 0x1800) == 0)                                                     \
  {                                                                           \
    if(((var & 0xe000) + length - 1) > 0x800)                                 \
      op_take_cycles(0x801 - (var & 0xe000));                                 \
    else                                                                      \
      op_take_cycles(length);                                                 \
  }                                                                           \


#define op_transfer_adjust_ptrs_source()                                      \
  saved_source = source + length;                                             \
  saved_dest = dest                                                           \

#define op_transfer_adjust_ptrs_dest()                                        \
  saved_source = source;                                                      \
  saved_dest = dest + length                                                  \

#define op_transfer_check_boundary_up(var)                                    \
  if(((var + length) ^ var) & 0xe000)                                         \
  {                                                                           \
    length = (~(var << 19) >> 19) + 1;                                        \
    op_transfer_adjust_ptrs_##var();                                          \
  }                                                                           \

#define op_transfer_check_boundary_dual_up(var)                               \
  if(((var + length) ^ var) & 0xe000)                                         \
  {                                                                           \
    length = (~(var << 19) >> 19) + 1;                                        \
    saved_source = source + length;                                           \
    saved_dest = dest + length;                                               \
  }                                                                           \

#define op_transfer_check_boundary_dual_down(var)                             \
  if(((var - length) ^ var) & 0xe000)                                         \
  {                                                                           \
    length = var & 0x1fff;                                                    \
    saved_source = source - length;                                           \
    saved_dest = dest - length;                                               \
  }                                                                           \


#define op_transfer_prologue_tai()                                            \
  op_transfer_check_boundary_up(dest)                                         \

#define op_transfer_prologue_tia()                                            \
  op_transfer_check_boundary_up(source)                                       \

#define op_transfer_prologue_tin()                                            \
  op_transfer_check_boundary_up(source)                                       \

#define op_transfer_prologue_tii()                                            \
  op_transfer_check_boundary_dual_up(source);                                 \
  op_transfer_check_boundary_dual_up(dest)                                    \

#define op_transfer_prologue_tdd()                                            \
  op_transfer_check_boundary_dual_down(source);                               \
  op_transfer_check_boundary_dual_down(dest)                                  \

#define op_transfer_loop_tai()                                                \
  if(mpr_check_ext(source_ptr))                                               \
  {                                                                           \
    io_read_function_ptr *source_functions =                                  \
     (io_read_function_ptr *)mpr_translate_ext_offset(source_ptr, source);    \
                                                                              \
    op_transfer_take_cycles_ext(source);                                      \
    if(mpr_check_ext(dest_ptr))                                               \
    {                                                                         \
      /* Ext source and dest */                                               \
      io_write_function_ptr *dest_functions =                                 \
       (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);     \
                                                                              \
      if(offset_index)                                                        \
      {                                                                       \
        offset_index = 0;                                                     \
        dest_functions[0](source_functions[1]());                             \
        dest_functions++;                                                     \
        length--;                                                             \
      }                                                                       \
                                                                              \
      length -= 2;                                                            \
      while(length >= 0)                                                      \
      {                                                                       \
        dest_functions[0](source_functions[0]());                             \
        dest_functions[1](source_functions[1]());                             \
        dest_functions += 2;                                                  \
        length -= 2;                                                          \
      }                                                                       \
                                                                              \
      if((length + 1) == 0)                                                   \
      {                                                                       \
        dest_functions[0](source_functions[0]());                             \
        offset_index = 1;                                                     \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      /* Ext source, normal dest */                                           \
      dest_ptr = mpr_translate_offset(dest_ptr, dest);                        \
                                                                              \
      if(offset_index)                                                        \
      {                                                                       \
        offset_index = 0;                                                     \
        dest_ptr[0] = source_functions[1]();                                  \
        dest_ptr++;                                                           \
        length--;                                                             \
      }                                                                       \
                                                                              \
      length -= 2;                                                            \
      while(length >= 0)                                                      \
      {                                                                       \
        dest_ptr[0] = source_functions[0]();                                  \
        dest_ptr[1] = source_functions[1]();                                  \
        dest_ptr += 2;                                                        \
        length -= 2;                                                          \
      }                                                                       \
                                                                              \
      if((length + 1) == 0)                                                   \
      {                                                                       \
        dest_ptr[0] = source_functions[0]();                                  \
        offset_index = 1;                                                     \
      }                                                                       \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    source_ptr = mpr_translate_offset(source_ptr, source);                    \
    if(mpr_check_ext(dest_ptr))                                               \
    {                                                                         \
      /* Normal source, ext dest */                                           \
      io_write_function_ptr *dest_functions =                                 \
       (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);     \
      op_transfer_take_cycles_ext_partial_up(dest);                           \
                                                                              \
      if(offset_index)                                                        \
      {                                                                       \
        offset_index = 0;                                                     \
        dest_functions[0](source_ptr[1]);                                     \
        dest_functions++;                                                     \
        length--;                                                             \
      }                                                                       \
                                                                              \
      length -= 2;                                                            \
      while(length >= 0)                                                      \
      {                                                                       \
        dest_functions[0](source_ptr[0]);                                     \
        dest_functions[1](source_ptr[1]);                                     \
        dest_functions += 2;                                                  \
        length -= 2;                                                          \
      }                                                                       \
                                                                              \
      if((length + 1) == 0)                                                   \
      {                                                                       \
        dest_functions[0](source_ptr[0]);                                     \
        offset_index = 1;                                                     \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      /* Normal source and dest */                                            \
      op_transfer_take_cycles();                                              \
      dest_ptr = mpr_translate_offset(dest_ptr, dest);                        \
                                                                              \
      if(offset_index)                                                        \
      {                                                                       \
        offset_index = 0;                                                     \
        dest_ptr[1] = source_ptr[1];                                          \
        length--;                                                             \
      }                                                                       \
                                                                              \
      length -= 2;                                                            \
      while(length >= 0)                                                      \
      {                                                                       \
        dest_ptr[0] = source_ptr[0];                                          \
        dest_ptr[1] = source_ptr[1];                                          \
        dest_ptr += 2;                                                        \
        length -= 2;                                                          \
      }                                                                       \
                                                                              \
      if((length + 1) == 0)                                                   \
      {                                                                       \
        dest_ptr[0] = source_ptr[0];                                          \
        offset_index = 1;                                                     \
      }                                                                       \
    }                                                                         \
  }                                                                           \

// For tia and tin we assumes that dest will always be ext, because the other
// version doesn't really make sense. But it can be added if necessary.

#define op_transfer_loop_tia()                                                \
  io_write_function_ptr *dest_functions =                                     \
   (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);         \
  op_transfer_take_cycles_ext(dest);                                          \
  length -= 2;                                                                \
                                                                              \
  if(mpr_check_ext(source_ptr))                                               \
  {                                                                           \
    io_read_function_ptr *source_functions =                                  \
     (io_read_function_ptr *)mpr_translate_ext_offset(source_ptr, source);    \
                                                                              \
    /* Ext source and dest */                                                 \
    if(offset_index)                                                          \
    {                                                                         \
      offset_index = 0;                                                       \
      dest_functions[1](source_functions[0]());                               \
      source_functions++;                                                     \
      length--;                                                               \
    }                                                                         \
                                                                              \
    while(length >= 0)                                                        \
    {                                                                         \
      dest_functions[0](source_functions[0]());                               \
      dest_functions[1](source_functions[1]());                               \
      source_functions += 2;                                                  \
      length -= 2;                                                            \
    }                                                                         \
                                                                              \
    if((length + 1) == 0)                                                     \
    {                                                                         \
      dest_functions[0](source_functions[0]());                               \
      offset_index = 1;                                                       \
    }                                                                         \
                                                                              \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    source_ptr = mpr_translate_offset(source_ptr, source);                    \
    /* Normal source, ext dest */                                             \
    if(offset_index)                                                          \
    {                                                                         \
      offset_index = 0;                                                       \
      dest_functions[1](source_ptr[0]);                                       \
      source_ptr++;                                                           \
      length--;                                                               \
    }                                                                         \
                                                                              \
    while(length >= 0)                                                        \
    {                                                                         \
      dest_functions[0](source_ptr[0]);                                       \
      dest_functions[1](source_ptr[1]);                                       \
      source_ptr += 2;                                                        \
      length -= 2;                                                            \
    }                                                                         \
                                                                              \
    if((length + 1) == 0)                                                     \
    {                                                                         \
      dest_functions[0](source_ptr[0]);                                       \
      offset_index = 1;                                                       \
    }                                                                         \
  }                                                                           \

#define op_transfer_loop_tin()                                                \
  io_write_function_ptr *dest_function =                                      \
   (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);         \
  op_transfer_take_cycles_ext(dest);                                          \
                                                                              \
  if(mpr_check_ext(source_ptr))                                               \
  {                                                                           \
    io_read_function_ptr *source_functions =                                  \
     (io_read_function_ptr *)mpr_translate_ext_offset(source_ptr, source);    \
                                                                              \
    /* Ext source and dest */                                                 \
    while(length > 0)                                                         \
    {                                                                         \
      dest_function[0](source_functions[0]());                                \
      source_functions++;                                                     \
      length--;                                                               \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    source_ptr = mpr_translate_offset(source_ptr, source);                    \
    /* Normal source, ext dest */                                             \
    while(length > 0)                                                         \
    {                                                                         \
      dest_function[0](source_ptr[0]);                                        \
      source_ptr++;                                                           \
      length--;                                                               \
    }                                                                         \
  }                                                                           \

#define op_transfer_loop_copy(increment)                                      \
  if(mpr_check_ext(source_ptr))                                               \
  {                                                                           \
    io_read_function_ptr *source_functions =                                  \
     (io_read_function_ptr *)mpr_translate_ext_offset(source_ptr, source);    \
    op_transfer_take_cycles_ext(source);                                      \
                                                                              \
    if(mpr_check_ext(dest_ptr))                                               \
    {                                                                         \
      /* Ext source and dest */                                               \
      io_write_function_ptr *dest_functions =                                 \
       (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);     \
                                                                              \
      while(length > 0)                                                       \
      {                                                                       \
        dest_functions[0](source_functions[0]());                             \
        source_functions += increment;                                        \
        dest_functions += increment;                                          \
        length--;                                                             \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      /* Ext source, normal dest */                                           \
      dest_ptr = mpr_translate_offset(dest_ptr, dest);                        \
                                                                              \
      while(length > 0)                                                       \
      {                                                                       \
        dest_ptr[0] = source_functions[0]();                                  \
        source_functions += increment;                                        \
        dest_ptr += increment;                                                \
        length--;                                                             \
      }                                                                       \
    }                                                                         \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    source_ptr = mpr_translate_offset(source_ptr, source);                    \
    if(mpr_check_ext(dest_ptr))                                               \
    {                                                                         \
      /* Normal source, ext dest */                                           \
      io_write_function_ptr *dest_functions =                                 \
       (io_write_function_ptr *)mpr_translate_ext_offset(dest_ptr, dest);     \
      op_transfer_take_cycles_ext(dest);                                      \
                                                                              \
      while(length > 0)                                                       \
      {                                                                       \
        dest_functions[0](source_ptr[0]);                                     \
        source_ptr += increment;                                              \
        dest_functions += increment;                                          \
        length--;                                                             \
      }                                                                       \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      /* Normal source and dest */                                            \
      dest_ptr = mpr_translate_offset(dest_ptr, dest);                        \
      op_transfer_take_cycles();                                              \
                                                                              \
      while(length > 0)                                                       \
      {                                                                       \
        dest_ptr[0] = source_ptr[0];                                          \
        source_ptr += increment;                                              \
        dest_ptr += increment;                                                \
        length--;                                                             \
      }                                                                       \
    }                                                                         \
  }                                                                           \

#define op_transfer_loop_tii()                                                \
  op_transfer_loop_copy(1)                                                    \

#define op_transfer_loop_tdd()                                                \
  op_transfer_loop_copy(-1)                                                   \

#define op_transfer_loop(type)                                                \
{                                                                             \
  op_transfer_start();                                                        \
                                                                              \
  do                                                                          \
  {                                                                           \
    source_ptr = memory.mpr_translated[source >> 13];                         \
    dest_ptr = memory.mpr_translated[0x8 + (dest >> 13)];                     \
                                                                              \
    op_transfer_prologue_##type();                                            \
    remaining_length -= length;                                               \
    op_transfer_loop_##type();                                                \
                                                                              \
    if(remaining_length != 0)                                                 \
    {                                                                         \
      length = remaining_length;                                              \
      source = saved_source & 0xFFFF;                                         \
      dest = saved_dest & 0xFFFF;                                             \
    }                                                                         \
  } while(remaining_length != 0);                                             \
}                                                                             \

#define op_tai(address_mode, address_range)                                   \
  u32 offset_index = 0;                                                       \
  op_transfer_loop(tai)                                                       \

#define op_tia(address_mode, address_range)                                   \
  u32 offset_index = 0;                                                       \
  op_transfer_loop(tia)                                                       \

#define op_tin(address_mode, address_range)                                   \
  op_transfer_loop(tin)                                                       \

#define op_tii(address_mode, address_range)                                   \
  op_transfer_loop(tii)                                                       \

#define op_tdd(address_mode, address_range)                                   \
  op_transfer_loop(tdd)                                                       \


// Older, slower versions. Try these if something goes wrong.

#define transfer_adjust_ptr_increment(ptr)                                    \
  ptr = (ptr + 1) & 0xFFFF                                                    \

#define transfer_adjust_ptr_decrement(ptr)                                    \
  ptr = (ptr - 1) & 0xFFFF                                                    \

#define transfer_adjust_ptr_alternate(ptr)                                    \
  ptr = (ptr + direction) & 0xFFFF;                                           \
  direction = -direction                                                      \

#define transfer_adjust_ptr_fix(ptr)                                          \

#define op_transfer(src_direction, dest_direction)                            \
{                                                                             \
  u32 source, dest, length;                                                   \
  u32 value;                                                                  \
  s32 direction = 1;                                                          \
  u64 old_cycles = cpu.global_cycles;                                         \
                                                                              \
  fetch_16bit(source);                                                        \
  fetch_16bit(dest);                                                          \
  fetch_16bit(length);                                                        \
                                                                              \
  if(length == 0)                                                             \
    length = 0x10000;                                                         \
                                                                              \
  op_take_cycles(17 + (length * 6));                                          \
  do                                                                          \
  {                                                                           \
    load_mem(value, source);                                                  \
    store_mem(value, dest);                                                   \
    transfer_adjust_ptr_##src_direction(source);                              \
    transfer_adjust_ptr_##dest_direction(dest);                               \
    length--;                                                                 \
  } while(length);                                                            \
}                                                                             \

/*

#define op_tai(address_mode, address_range)                                   \
  op_transfer(alternate, increment)                                           \

#define op_tdd(address_mode, address_range)                                   \
  op_transfer(decrement, decrement)                                           \

#define op_tia(address_mode, address_range)                                   \
  op_transfer(increment, alternate)                                           \

#define op_tii(address_mode, address_range)                                   \
  op_transfer(increment, increment)                                           \

#define op_tin(address_mode, address_range)                                   \
  op_transfer(increment, fix)                                                 \

*/

/** Null operations **********************************************************/

#define op_nop(address_mode, address_range)                                   \


/** VDC port operations ******************************************************/

#define op_st0(address_mode, address_range)                                   \
  op_load_##address_range(address_mode, operand);                             \
  vdc_register_select(operand)                                                \

#define op_st1(address_mode, address_range)                                   \
  cpu.cycles_remaining = cpu_cycles_remaining;                                \
  op_load_##address_range(address_mode, operand);                             \
  vdc_data_write_low(operand)                                                 \

#define op_st2(address_mode, address_range)                                   \
  cpu.cycles_remaining = cpu_cycles_remaining;                                \
  op_load_##address_range(address_mode, operand);                             \
  vdc_data_write_high(operand)                                                \



/** Clockspeed operations ****************************************************/

#define op_csh(address_mode, address_range)                                   \
  if(cpu.cpu_divider == 12)                                                   \
  {                                                                           \
    cpu_cycles_remaining = ((cpu_cycles_remaining * 4) + cpu.extra_cycles) -  \
     27;                                                                      \
    cpu.extra_cycles = 0;                                                     \
    cpu.cpu_divider = 3;                                                      \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    op_take_cycles(3);                                                        \
  }                                                                           \


#define op_csl(address_mode, address_range)                                   \
  if(cpu.cpu_divider == 3)                                                    \
  {                                                                           \
    cpu_cycles_remaining -= 18;                                               \
    cpu.extra_cycles = (u32)cpu_cycles_remaining % 4;                         \
    cpu_cycles_remaining /= 4;                                                \
    cpu.cpu_divider = 12;                                                     \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    op_take_cycles(3);                                                        \
  }                                                                           \

#define op_take_cycles(cycles)                                                \
  cpu_cycles_remaining -= ((cycles) * 3)                                      \


// Defining op will specify what op_list should expand to when used.

#define op_update_cycles_fixed_t_filter(cycles)                               \
  op_take_cycles(cycles)                                                      \

#define op_update_cycles_fixed_t_na(cycles)                                   \
  op_take_cycles(cycles)                                                      \

#define op_update_cycles_fixed_t_clear(cycles)                                \
  op_take_cycles(cycles)                                                      \

#define op_update_cycles_fixed_t_set(cycles)                                  \
  op_take_cycles(cycles + 3)                                                  \


#define op_update_cycles_fixed(cycles, t_flag)                                \
  op_update_cycles_fixed_t_##t_flag(cycles)                                   \

#define op_update_cycles_fixed_t(cycles, t_flag)                              \
  op_update_cycles_fixed_t_##t_flag(cycles)                                   \

#define op_update_cycles_fixed_c(cycles, t_flag)                              \
  op_update_cycles_fixed_t_##t_flag(cycles)                                   \

#define op_update_cycles_fixed_i(cycles, t_flag)                              \
  op_update_cycles_fixed_t_##t_flag(cycles)                                   \

#define op_update_cycles_fixed_x(cycles, t_flag)                              \
  op_update_cycles_fixed_t_##t_flag(cycles)                                   \

#define op_update_cycles_variable(cycles, t_flag)                             \

#define op_mode_dispatch_t_filter(operation, address_mode, address_range)     \
  op_##operation(address_mode, address_range)                                 \

#define op_mode_dispatch_t_na(operation, address_mode, address_range)         \
  op_##operation(address_mode, address_range)                                 \

#define op_mode_dispatch_t_set(operation, address_mode, address_range)        \
  op_##operation(address_mode, address_range, set)                            \

#define op_mode_dispatch_t_clear(operation, address_mode, address_range)      \
  op_##operation(address_mode, address_range, clear)                          \

#define op_mode_dispatch_dt_na(operation, addr_mode, addr_range, d_flag)      \
  op_##operation(addr_mode, addr_range, d_flag)                               \

#define op_mode_dispatch_dt_filter(operation, addr_mode, addr_range, d_f)     \
  op_##operation(addr_mode, addr_range, d_f)                                  \

#define op_mode_dispatch_dt_set(operation, address_mode, address_range, d_f)  \
  op_##operation(address_mode, address_range, d_f, set)                       \

#define op_mode_dispatch_dt_clear(operation, addr_mode, addr_range, d_f)      \
  op_##operation(addr_mode, addr_range, d_f, clear)                           \



#define op_mode_dispatch_d_na(opc, operation, addr_mode, addr_range, t_f)     \
  op_mode_dispatch_t_##t_f(operation, addr_mode, addr_range)                  \

#define op_mode_dispatch_d_filter(opc, operation, addr_mode, addr_range, t_f) \
  op_mode_dispatch_t_##t_f(operation, addr_mode, addr_range)                  \

#define op_mode_dispatch_d_set(opc, operation, addr_mode, addr_range, t_f)    \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, set)            \

#define op_mode_dispatch_d_clear(opc, operation, addr_mode, addr_range, t_f)  \
  op_mode_dispatch_dt_##t_f(operation, addr_mode, addr_range, clear)          \

#define op_mode_dispatch_d_op(opc, operation, addr_mode, addr_range, t_flag)  \
  op_mode_dispatch_dt_##t_flag(operation, addr_mode, addr_range, opc)         \


#define op(opcode, operation, address_mode, address_range, cycles,            \
 cycle_usage, d_flag, t_flag)                                                 \
  case opcode:                                                                \
  {                                                                           \
    op_update_cycles_##cycle_usage(cycles, t_flag);                           \
    op_mode_dispatch_d_##d_flag(opcode, operation, address_mode,              \
     address_range, t_flag);                                                  \
    break;                                                                    \
  }

cpu_struct cpu;

#ifdef DEBUGGER_ON

#define perform_step_debug()                                                  \
  collapse_flags();                                                           \
  retrieve_pc(pc);                                                            \
                                                                              \
  if((a > 0xFF) | (x > 0xFF) | (y > 0xFF) | (s > 0xFF) | (pc > 0xFFFF) |      \
   (p > 0xFF) | (c_flag > 1) | (v_flag > 1) | (z_flag > 1) | (n_flag > 1))    \
  {                                                                           \
    printf("Register range error detected. %d\n", c_flag);                    \
    set_debug_mode(DEBUG_STEP);                                               \
  }                                                                           \
                                                                              \
  collapse_flags();                                                           \
  retrieve_pc(cpu.pc);                                                        \
                                                                              \
  cpu.a = a;                                                                  \
  cpu.x = x;                                                                  \
  cpu.y = y;                                                                  \
  cpu.s = s;                                                                  \
  cpu.p = p;                                                                  \
                                                                              \
  step_debug(a, x, y, p, s, pc, cpu_cycles_remaining);                        \
                                                                              \
  a = cpu.a;                                                                  \
  x = cpu.x;                                                                  \
  y = cpu.y;                                                                  \
  p = cpu.p;                                                                  \
  s = cpu.s;                                                                  \
  pc = cpu.pc;                                                                \
  update_pc();                                                                \
  update_zero_segment();                                                      \
  extract_flags()                                                             \

#else

#define perform_step_debug()

#endif


#include "3dsemu.h"

typedef struct
{
    bool                isReal3DS;
    bool                enableDebug;
    int                 emulatorState;
} SEmulator;

extern SEmulator emulator;

u32 disasm_pc;
u32 disasm_pc2;
u8 disasm_output[500];


/*
#define disasm_opcode \
  if (emulator.enableDebug) \
  { \
    disasm_pc2 = disasm_pc = ((pc_high << 13) + pc_low) & 0xFFFF; \
    disasm_instruction(disasm_output, &disasm_pc); \
    printf ("%4x: %-15s A%02X X%02X Y%02X S%02X\n", disasm_pc2, disasm_output, a, x, y, s); \
    { DEBUG_WAIT_L_KEY } \
  } \
*/
#define disasm_opcode


#define op_switch(d_flag, t_flag)       \
  disasm_opcode                                                               \
  fetch_8bit(current_instruction);                                            \
  profile_opcode_type(current_instruction);                                   \
  switch(current_instruction)                                                 \
  {                                                                           \
    op_list(d_flag, t_flag);                                                  \
  }                                                                           \


#define op_loop(d_flag)                                                       \
  do                                                                          \
  {                                                                           \
    __label__ execute_instruction_t_set;                                      \
                                                                              \
    op_switch(d_flag, clear);                                                 \
                                                                              \
    op_loop_d_##d_flag:                                                       \
    continue;                                                                 \
                                                                              \
    execute_instruction_t_set:                                                \
                                                                              \
    op_switch(d_flag, set);                                                   \
                                                                              \
  } while(cpu_cycles_remaining > 0)                                           \

#ifndef ARM_ARCH

void execute_instructions(s32 cpu_cycles_remaining)
{
  register u32 a = cpu.a;
  register u32 x = cpu.x;
  register u32 y = cpu.y;
  register u32 pc_low;
  register u32 z_flag;
  register u32 n_flag;
  register u8 *code_base;
  register u8 *zero_page;

  u32 current_instruction, operand, address;
  u32 pc_high;
  u32 pc = cpu.pc;
  u32 p = cpu.p;
  u32 c_flag, v_flag;
  u32 cli_captured_irq_status;

  s32 check_interrupt_cycles = 0xFFFFFFF;

  u8 *stack_page;
  u32 s = cpu.s;

  // To get the current cycle count from the middle of
  // execute_instructions decrement cpu_cycles_remaining from it.
  cpu.global_cycles += cpu_cycles_remaining;

  if(cpu_cycles_remaining <= cpu.extra_cycles)
  {
    cpu.extra_cycles -= cpu_cycles_remaining;
    return;
  }

  cpu_cycles_remaining -= cpu.extra_cycles;
  cpu.extra_cycles = 0;

  if(cpu.vdc_stalled)
    return;

  if(cpu.cpu_divider == 12)
  {
    cpu.extra_cycles = (u32)cpu_cycles_remaining % 4;
    cpu_cycles_remaining /= 4;
  }

  update_pc();
  update_zero_segment();
  extract_flags();

  cpu.alert = 0;

  if(cpu.irq_raised)
  {
    check_pending_interrupts(irq.status);
    cpu.irq_raised = 0;
  }

  reenter_execute_loop:

  if(p & (1 << D_FLAG_BIT))
  {
    op_loop(set);
  }
  else
  {
    op_loop(clear);
  }

  // Special hack to make IRQ check happen after cli
  if(check_interrupt_cycles != 0xFFFFFFF)
  {
    cpu_cycles_remaining += check_interrupt_cycles;
    check_interrupt_cycles = 0xFFFFFFF;

    check_pending_interrupts(cli_captured_irq_status);

    if(cpu_cycles_remaining > 0)
      goto reenter_execute_loop;
  }

  cpu_execution_done:

  collapse_flags();
  retrieve_pc(cpu.pc);

  cpu.a = a;
  cpu.x = x;
  cpu.y = y;
  cpu.s = s;
  cpu.p = p;

  if(cpu.cpu_divider == 12)
    cpu.extra_cycles -= cpu_cycles_remaining * 4;
  else
    cpu.extra_cycles = -cpu_cycles_remaining;
}

#endif

void initialize_cpu()
{
}

void reset_cpu()
{
  cpu.global_cycles = 0;
  cpu.extra_cycles = 0;
  cpu.irq_raised = 0;

  cpu.a = 0x0;
  cpu.s = 0xFF;
  cpu.x = 0x0;
  cpu.y = 0x0;

  cpu.p = (1 << I_FLAG_BIT) | (1 << Z_FLAG_BIT) | (1 << B_FLAG_BIT);
  load_mem_safe_16(cpu.pc, RESET_VECTOR);

  cpu.cpu_divider = 12;
}

// Detects most genuine idle loops, typically of the form:

// idle:
//  lda/ldx/ldy mem
//  adc/and/ora/eor/bit/cmp (loaded value)
//  bne/beq idle

// idle:

// idle:
//  bra/jmp idle


#define analyze_idle_branch(patch)                                            \
  fetch_8bit_signed(offset);                                                  \
                                                                              \
  if((offset >= -8) && (offset <= -4))                                        \
  {                                                                           \
    retrieve_pc(pc);                                                          \
    idle_branch_pc = pc - 2;                                                  \
    advance_pc(offset);                                                       \
    idle_loop_base_pc = pc + offset;                                          \
  }                                                                           \
                                                                              \
  idle_loop_patch = patch                                                     \


// TODO: Support more idle loop types

void patch_idle_loop()
{
  if(config.patch_idle_loops == 0)
    return;

  u32 pc_low;
  u8 *code_base;

  u32 current_instruction;
  s32 offset;
  u32 pc_high;
  u32 idle_position_state;
  u32 idle_branch_pc = 0;
  u32 idle_loop_base_pc = 0;
  u32 idle_loop_patch = 0;

  u32 pc = cpu.pc;
  u32 base_pc = pc;

  typedef enum
  {
    LOAD_VALUE_NONE,
    LOAD_VALUE_A,
    LOAD_VALUE_X,
    LOAD_VALUE_Y
  } load_value_enum;

  load_value_enum load_value = LOAD_VALUE_NONE;

  update_pc();

  u32 pc_scanned[4] = { 0 };

  for(idle_position_state = 0; idle_position_state < 4; idle_position_state++)
  {
    pc_scanned[idle_position_state] = pc;
    fetch_8bit(current_instruction);

    switch(current_instruction)
    {
      // bpl
      case 0x10:
      {
        analyze_idle_branch(0x5C);
        break;
      }

      // bmi
      case 0x30:
      {
        analyze_idle_branch(0x5B);
        break;
      }

      // bcc
      case 0x90:
      {
        analyze_idle_branch(0x63);
        break;
      }

      // bcs
      case 0xB0:
      {
        analyze_idle_branch(0x6C);
        break;
      }

      // bne
      case 0xD0:
      {
        analyze_idle_branch(0x1B);
        break;
      }

      // beq
      case 0xF0:
      {
        analyze_idle_branch(0x0B);
        break;
      }

      // bbr
      case 0x0F:
      case 0x1F:
      case 0x2F:
      case 0x3F:
      case 0x4F:
      case 0x5F:
      case 0x6F:
      case 0x7F:
      // bbs
      case 0x8F:
      case 0x9F:
      case 0xAF:
      case 0xCF:
      case 0xDF:
      case 0xEF:
      case 0xFF:
      {
        u32 bit_num = (current_instruction >> 4) & 0x7;
        u32 memory_location;

        fetch_8bit(memory_location);
        fetch_8bit_signed(offset);
        if(offset != -3)
          return;

        retrieve_pc(idle_branch_pc);
        idle_branch_pc -= 3;
        idle_loop_base_pc = idle_branch_pc;

        if(current_instruction & 0x80)
          idle_loop_patch = 0x4B;
        else
          idle_loop_patch = 0x3B;

        store_mem_safe(bit_num, idle_branch_pc + 2);

        advance_pc(offset);
        break;
      }

      // bra
      case 0x80:
      {
        fetch_8bit_signed(offset);
        if(offset != -2)
          return;

        retrieve_pc(idle_branch_pc);
        idle_branch_pc -= 2;
        idle_loop_base_pc = idle_branch_pc;
        idle_loop_patch = 0x2B;
        advance_pc(offset);
        break;
      }

      // jmp
      case 0x4C:
      {
        fetch_16bit(offset);
        retrieve_pc(pc);
        if(offset != (pc - 3))
          return;

        idle_branch_pc = pc - 3;
        idle_loop_base_pc = idle_branch_pc;
        idle_loop_patch = 0x33;
        pc = offset;
        update_pc();
        break;
      }

      // lda with 1 byte operand
      case 0xA5:
      case 0xA9:
      case 0xB5:
        load_value = LOAD_VALUE_A;
        advance_pc(1);
        break;

      // ldx with 1 byte operand
      case 0xA2:
      case 0xA6:
      case 0xB6:
        load_value = LOAD_VALUE_X;
        advance_pc(1);
        break;

      // ldy with 1 byte operand
      case 0xA0:
      case 0xA1:
      case 0xA4:
      case 0xB1:
      case 0xB4:
        load_value = LOAD_VALUE_Y;
        advance_pc(1);
        break;

      // lda with 2 byte mem operand
      case 0xAD:
      case 0xBD:
        load_value = LOAD_VALUE_A;
        advance_pc(2);
        break;

      // ldx with 2 byte mem operand
      case 0xAE:
      case 0xBE:
        load_value = LOAD_VALUE_X;
        advance_pc(2);
        break;

      // ldy with 2 byte mem operand
      case 0xAC:
      case 0xBC:
        load_value = LOAD_VALUE_Y;
        advance_pc(2);
        break;

      // Modifiers after load a w/1 byte operand:
      // bit/and/ora
      case 0x24:
      case 0x34:
      case 0x89:
      case 0x25:
      case 0x29:
      case 0x35:
      case 0x05:
      case 0x09:
      case 0x15:
        if(load_value != LOAD_VALUE_A)
          return;
        advance_pc(1);
        load_value = LOAD_VALUE_NONE;
        break;

      // Modifiers after load a w/2 byte operand:
      // bit/and/ora
      case 0x2C:
      case 0x3C:
      case 0x2D:
      case 0x39:
      case 0x3D:
      case 0x0D:
      case 0x19:
      case 0x1D:
        if(load_value != LOAD_VALUE_A)
          return;
        advance_pc(2);
        load_value = LOAD_VALUE_NONE;
        break;


      // cmp, 1 byte operand
      case 0xC1:
      case 0xC5:
      case 0xC9:
      case 0xD1:
      case 0xD2:
      case 0xD5:
        if(load_value != LOAD_VALUE_A)
          return;
        advance_pc(1);
        break;

      // cmp, 2 byte operand
      case 0xCD:
      case 0xD9:
      case 0xDD:
        if(load_value != LOAD_VALUE_A)
          return;
        advance_pc(2);
        break;

      // cpx, 1 byte operand
      case 0xE0:
      case 0xE4:
        if(load_value != LOAD_VALUE_X)
          return;
        advance_pc(1);
        break;

      // cpx, 2 byte operand
      case 0xEC:
        if(load_value != LOAD_VALUE_X)
          return;
        advance_pc(2);
        break;

      // cpy, 1 byte operand
      case 0xC0:
      case 0xC4:
        if(load_value != LOAD_VALUE_Y)
          return;
        advance_pc(1);
        break;

      // cpy, 2 byte operand
      case 0xCC:
        if(load_value != LOAD_VALUE_Y)
          return;
        advance_pc(2);
        break;

      default:
        return;
    }

    retrieve_pc(pc);
    if(pc == base_pc)
      break;
  }

  if(idle_branch_pc != 0)
  {
    // Patch branch
    printf("patching idle loop (%04x, pcs %x %x %x %x):\n", idle_loop_base_pc,
     pc_scanned[0], pc_scanned[1], pc_scanned[2], pc_scanned[3]);
    //disasm_block(idle_loop_base_pc, idle_position_state + 1);

    store_mem_safe(idle_loop_patch, idle_branch_pc);
  }
}

#define cpu_savestate_builder(type, type_b, version_gate)                     \
void cpu_##type_b##_savestate(savestate_##type_b##_type savestate_file)       \
{                                                                             \
  file_##type##_variable(savestate_file, cpu.a);                              \
  file_##type##_variable(savestate_file, cpu.x);                              \
  file_##type##_variable(savestate_file, cpu.y);                              \
  file_##type##_variable(savestate_file, cpu.s);                              \
  file_##type##_variable(savestate_file, cpu.p);                              \
  file_##type##_variable(savestate_file, cpu.cpu_divider);                    \
  file_##type##_variable(savestate_file, cpu.pc);                             \
                                                                              \
  file_##type##_variable(savestate_file, cpu.irq_raised);                     \
                                                                              \
  file_##type##_variable(savestate_file, cpu.global_cycles);                  \
                                                                              \
  file_##type##_variable(savestate_file, cpu.extra_cycles);                   \
}                                                                             \

build_savestate_functions(cpu);

