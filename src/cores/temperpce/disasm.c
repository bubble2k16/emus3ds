#include "common.h"

/** Code fetch operations ****************************************************/

#define update_code_segment()                                                 \
  code_segment = mpr_translate_offset(memory.mpr_translated[pc_high],         \
   pc_high << 13)                                                             \

#define retrieve_pc(var)                                                      \
  var = ((pc_high << 13) + pc_low) & 0xFFFF                                   \

#define update_pc()                                                           \
{                                                                             \
  pc_high = pc >> 13;                                                         \
  pc_low = pc & 0x1FFF;                                                       \
  update_code_segment();                                                      \
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
  dest = code_segment[pc_low];                                                \
  advance_pc(1)                                                               \

#define fetch_8bit_signed(dest)                                               \
  dest = ((s8 *)code_segment)[pc_low];                                        \
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
    dest = code_segment[pc_low] | (code_segment[pc_low + 1] << 8);            \
    pc_low += 2;                                                              \
  }                                                                           \


#define disasm_na_implicit(opcode)                                            \
  sprintf(output, #opcode)                                                    \

#define disasm_na_label(opcode)                                               \
  fetch_8bit_signed(operand);                                                 \
  retrieve_pc(pc);                                                            \
  operand = (operand + pc) & 0xFFFF;                                          \
  sprintf(output, #opcode " %04x", operand)                                   \

#define disasm_na_transfer(opcode)                                            \
{                                                                             \
  u32 source, dest, length;                                                   \
                                                                              \
  fetch_16bit(source);                                                        \
  fetch_16bit(dest);                                                          \
  fetch_16bit(length);                                                        \
                                                                              \
  sprintf(output, #opcode " $%04x, $%04x, #$%04x", source, dest, length);     \
}                                                                             \

#define disasm_imm_immediate(opcode)                                          \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " #$%02x", operand)                                 \

#define disasm_abs_direct(opcode)                                             \
  fetch_16bit(operand);                                                       \
  sprintf(output, #opcode " $%04x", operand)                                  \

#define disasm_abs_direct_x(opcode)                                           \
  fetch_16bit(operand);                                                       \
  sprintf(output, #opcode " $%04x, x", operand)                               \

#define disasm_abs_direct_y(opcode)                                           \
  fetch_16bit(operand);                                                       \
  sprintf(output, #opcode " $%04x, y", operand)                               \

#define disasm_abs_indirect(opcode)                                           \
  fetch_16bit(operand);                                                       \
  sprintf(output, #opcode " ($%04x)", operand)                                \

#define disasm_abs_x_indirect(opcode)                                         \
  fetch_16bit(operand);                                                       \
  sprintf(output, #opcode " ($%04x, x)", operand)                             \

#define disasm_zp_direct(opcode)                                              \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $%02x", operand)                                  \

#define disasm_zp_direct_x(opcode)                                            \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $%02x, x", operand)                               \

#define disasm_zp_direct_y(opcode)                                            \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $%02x, y", operand)                               \

#define disasm_zp16_indirect(opcode)                                          \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $(%02x)", operand)                                \

#define disasm_zp16_x_indirect(opcode)                                        \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $(%02x, x)", operand)                             \

#define disasm_zp16_indirect_y(opcode)                                        \
  fetch_8bit(operand);                                                        \
  sprintf(output, #opcode " $(%02x), y", operand)                             \


#define op(opcode, operation, address_mode, address_range, cycles,            \
 cycle_usage, d_flag, t_flag)                                                 \
  case opcode:                                                                \
  {                                                                           \
    disasm_##address_range##_##address_mode(operation);                       \
    break;                                                                    \
  }                                                                           \

#include "op_list.h"

u32 disasm_instruction(char *_output, u32 *_pc)
{
  u32 pc = *_pc;
  u32 pc_high, pc_low;
  u32 current_instruction, operand;
  char alt_output[32];
  u32 use_alt_output = 0;
  u8 *code_segment;
  char *output = _output;

  update_pc();

  fetch_8bit(current_instruction);

  // The test opcodes have two operands but don't follow this in the oplist
  // since that would add a lot of extra modes just for it. So for the disasm
  // to properly match up they should just be special cased.
  // This will print it out funny (ie, tst #$ab tst EA) but it'll at least
  // line up correctly.

  if((current_instruction == 0x83) || (current_instruction == 0x93) ||
   (current_instruction == 0xA3) || (current_instruction == 0xB3))
  {
    u32 operand_imm;
    fetch_8bit(operand_imm);
    sprintf(output, "tst #$%02x, ", operand_imm);
    output = alt_output;
    use_alt_output = 1;
  }

  switch(current_instruction)
  {
    op_list(clear, clear);
  }

  retrieve_pc(pc);
  *_pc = pc;

  if(use_alt_output)
    sprintf(_output + 10, output + 4);

  return current_instruction;
}

void disasm_function(u32 pc)
{
  u32 current_instruction;
  char disasm_buffer[128];
  u32 old_pc;

  do
  {
    old_pc = pc;
    current_instruction = disasm_instruction(disasm_buffer, &pc);
    printf("%04x: %s\n", old_pc, disasm_buffer);
  } while((current_instruction != 0x60) && (current_instruction != 0x40));
}

void disasm_block(u32 pc, u32 count)
{
  char disasm_buffer[128];
  u32 old_pc;

  while(count)
  {
    old_pc = pc;
    disasm_instruction(disasm_buffer, &pc);
    printf("%04x: %s\n", old_pc, disasm_buffer);
    count--;
  }
}

