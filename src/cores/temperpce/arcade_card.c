#include "common.h"

arcade_card_struct arcade_card;

#define arcade_card_generate_address(port_number)                             \
  u32 address = arcade_card.ports[port_number].base_address;                  \
  if(control & ARCADE_CARD_CONTROL_USE_OFFSET)                                \
  {                                                                           \
    address += arcade_card.ports[port_number].offset;                         \
                                                                              \
    if(control & ARCADE_CARD_CONTROL_ADD_OFFSET_EXT)                          \
      address += 0xFF0000;                                                    \
  }                                                                           \
                                                                              \
  address &= 0x1FFFFF                                                         \


#define arcade_card_auto_increment(port_number)                               \
  if(control & ARCADE_CARD_CONTROL_AUTO_INCREMENT_ENABLE)                     \
  {                                                                           \
    if(control & ARCADE_CARD_CONTROL_AUTO_INCREMENT_TYPE)                     \
    {                                                                         \
      arcade_card.ports[port_number].base_address +=                          \
       arcade_card.ports[port_number].increment;                              \
      arcade_card.ports[port_number].base_address &= 0xFFFFFF;                \
    }                                                                         \
    else                                                                      \
    {                                                                         \
      arcade_card.ports[port_number].offset +=                                \
       arcade_card.ports[port_number].increment;                              \
      arcade_card.ports[port_number].offset &= 0xFFFF;                        \
    }                                                                         \
  }                                                                           \

#define arcade_card_functions_builder(port_number)                            \
                                                                              \
  u32 arcade_card_read_port_##port_number()                                   \
  {                                                                           \
    u32 control = arcade_card.ports[port_number].control;                     \
    arcade_card_generate_address(port_number);                                \
                                                                              \
    u32 read_value = arcade_card.acd_ram[address];                            \
    arcade_card_auto_increment(port_number);                                  \
    return read_value;                                                        \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_base0_port_##port_number()                             \
  {                                                                           \
    return arcade_card.ports[port_number].base_address & 0xFF;                \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_base1_port_##port_number()                             \
  {                                                                           \
    return (arcade_card.ports[port_number].base_address >> 8) & 0xFF;         \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_base2_port_##port_number()                             \
  {                                                                           \
    return arcade_card.ports[port_number].base_address >> 16;                 \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_offset0_port_##port_number()                           \
  {                                                                           \
    return arcade_card.ports[port_number].offset & 0xFF;                      \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_offset1_port_##port_number()                           \
  {                                                                           \
    return arcade_card.ports[port_number].offset >> 8;                        \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_increment0_port_##port_number()                        \
  {                                                                           \
    return arcade_card.ports[port_number].increment & 0xFF;                   \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_increment1_port_##port_number()                        \
  {                                                                           \
    return arcade_card.ports[port_number].increment >> 8;                     \
  }                                                                           \
                                                                              \
  u32 arcade_card_read_control_port_##port_number()                           \
  {                                                                           \
    return arcade_card.ports[port_number].control;                            \
  }                                                                           \
                                                                              \
  void arcade_card_write_port_##port_number(u32 value)                        \
  {                                                                           \
    u32 control = arcade_card.ports[port_number].control;                     \
    arcade_card_generate_address(port_number);                                \
                                                                              \
    arcade_card.acd_ram[address] = value;                                     \
    arcade_card_auto_increment(port_number);                                  \
  }                                                                           \
                                                                              \
  void arcade_card_write_base0_port_##port_number(u32 value)                  \
  {                                                                           \
    arcade_card.ports[port_number].base_address =                             \
     (arcade_card.ports[port_number].base_address & 0xFFFF00) | value;        \
  }                                                                           \
                                                                              \
  void arcade_card_write_base1_port_##port_number(u32 value)                  \
  {                                                                           \
    arcade_card.ports[port_number].base_address =                             \
     (arcade_card.ports[port_number].base_address & 0xFF00FF) | (value << 8); \
  }                                                                           \
                                                                              \
  void arcade_card_write_base2_port_##port_number(u32 value)                  \
  {                                                                           \
    arcade_card.ports[port_number].base_address =                             \
     (arcade_card.ports[port_number].base_address & 0x00FFFF) |               \
     (value << 16);                                                           \
  }                                                                           \
                                                                              \
  void arcade_card_write_offset0_port_##port_number(u32 value)                \
  {                                                                           \
    arcade_card.ports[port_number].offset =                                   \
     (arcade_card.ports[port_number].offset & 0xFFFF00) | value;              \
  }                                                                           \
                                                                              \
  void arcade_card_write_offset1_port_##port_number(u32 value)                \
  {                                                                           \
    u32 control = arcade_card.ports[port_number].control;                     \
                                                                              \
    arcade_card.ports[port_number].offset =                                   \
     (arcade_card.ports[port_number].offset & 0xFF00FF) | (value << 8);       \
                                                                              \
    if(((control & ARCADE_CARD_CONTROL_ADD_OFFSET_A) == 0) &&                 \
     (control & ARCADE_CARD_CONTROL_ADD_OFFSET_6))                            \
    {                                                                         \
      arcade_card.ports[port_number].base_address +=                          \
       arcade_card.ports[port_number].offset;                                 \
      arcade_card.ports[port_number].base_address &= 0xFFFFFF;                \
    }                                                                         \
  }                                                                           \
                                                                              \
  void arcade_card_write_increment0_port_##port_number(u32 value)             \
  {                                                                           \
    arcade_card.ports[port_number].increment =                                \
     (arcade_card.ports[port_number].increment & 0xFF00) | value;             \
  }                                                                           \
                                                                              \
  void arcade_card_write_increment1_port_##port_number(u32 value)             \
  {                                                                           \
    arcade_card.ports[port_number].increment =                                \
     (arcade_card.ports[port_number].increment & 0x00FF) | (value << 8);      \
  }                                                                           \
                                                                              \
  void arcade_card_write_control_port_##port_number(u32 value)                \
  {                                                                           \
    arcade_card.ports[port_number].control = value & 0x7F;                    \
  }                                                                           \
                                                                              \
  void arcade_card_write_adjust_base_port_##port_number(u32 value)            \
  {                                                                           \
    u32 control = arcade_card.ports[port_number].control;                     \
    if((control & (ARCADE_CARD_CONTROL_ADD_OFFSET_A |                         \
     ARCADE_CARD_CONTROL_ADD_OFFSET_6)) ==                                    \
     (ARCADE_CARD_CONTROL_ADD_OFFSET_A | ARCADE_CARD_CONTROL_ADD_OFFSET_6))   \
    {                                                                         \
      arcade_card.ports[port_number].base_address +=                          \
       arcade_card.ports[port_number].offset;                                 \
      if(control & ARCADE_CARD_CONTROL_ADD_OFFSET_EXT)                        \
       arcade_card.ports[port_number].base_address += 0xFF0000;               \
                                                                              \
      arcade_card.ports[port_number].base_address &= 0x1FFFFF;                \
    }                                                                         \
  }                                                                           \

arcade_card_functions_builder(0);
arcade_card_functions_builder(1);
arcade_card_functions_builder(2);
arcade_card_functions_builder(3);

u32 arcade_card_read_shift0()
{
  return arcade_card.shift_value & 0xFF;
}

u32 arcade_card_read_shift1()
{
  return (arcade_card.shift_value >> 8) & 0xFF;
}

u32 arcade_card_read_shift2()
{
  return (arcade_card.shift_value >> 16) & 0xFF;
}

u32 arcade_card_read_shift3()
{
  return arcade_card.shift_value >> 24;
}

u32 arcade_card_read_shift_amount()
{
  return arcade_card.shift_amount;
}

u32 arcade_card_read_shift_latch()
{
  return arcade_card.shift_latch;
}

u32 arcade_card_read_version_number()
{
  return 0x10;
}

u32 arcade_card_read_identification_number()
{
  return 0x51;
}

void arcade_card_write_shift0(u32 value)
{
  arcade_card.shift_value = (arcade_card.shift_value & 0xFFFFFF00) | value;
}

void arcade_card_write_shift1(u32 value)
{
  arcade_card.shift_value = (arcade_card.shift_value & 0xFFFF00FF) |
   (value << 8);
}

void arcade_card_write_shift2(u32 value)
{
  arcade_card.shift_value = (arcade_card.shift_value & 0xFF00FFFF) |
   (value << 16);
}

void arcade_card_write_shift3(u32 value)
{
  arcade_card.shift_value = (arcade_card.shift_value & 0x00FFFFFF) |
   (value << 24);
}

void arcade_card_write_shift_amount(u32 value)
{
  value &= 0xF;

  if(value > 0x7)
    arcade_card.shift_value >>= (0x10 - value);
  else
    arcade_card.shift_value <<= value;

  arcade_card.shift_amount = value;
}

void arcade_card_write_shift_latch(u32 value)
{
  arcade_card.shift_latch = value;
}

void initialize_arcade_card()
{
  u32 i;

  for(i = 0; i < 0x2000; i++)
  {
    arcade_card.acd_read_functions_0[i] = arcade_card_read_port_0;
    arcade_card.acd_write_functions_0[i] = arcade_card_write_port_0;

    arcade_card.acd_read_functions_1[i] = arcade_card_read_port_1;
    arcade_card.acd_write_functions_1[i] = arcade_card_write_port_1;

    arcade_card.acd_read_functions_2[i] = arcade_card_read_port_2;
    arcade_card.acd_write_functions_2[i] = arcade_card_write_port_2;

    arcade_card.acd_read_functions_3[i] = arcade_card_read_port_3;
    arcade_card.acd_write_functions_3[i] = arcade_card_write_port_3;
  }

  // This puts it beyond the 392KB needed for the syscard itself
  arcade_card.acd_ram = memory.hucard_rom + (512 * 1024);
}

#define map_arcade_card_io_read(port_number, reg_number, ptr)                 \
  memory.io_read_functions[0x1A00 + (port_number * 0x10) + reg_number] = ptr  \

#define map_arcade_card_io_write(port_number, reg_number, ptr)                \
  memory.io_write_functions[0x1A00 + (port_number * 0x10) + reg_number] = ptr \

#define map_arcade_card_io(pn)                                                \
  map_arcade_card_io_read(pn, 0x0, arcade_card_read_port_##pn);               \
  map_arcade_card_io_read(pn, 0x1, arcade_card_read_port_##pn);               \
  map_arcade_card_io_read(pn, 0x2, arcade_card_read_base0_port_##pn);         \
  map_arcade_card_io_read(pn, 0x3, arcade_card_read_base1_port_##pn);         \
  map_arcade_card_io_read(pn, 0x4, arcade_card_read_base2_port_##pn);         \
  map_arcade_card_io_read(pn, 0x5, arcade_card_read_offset0_port_##pn);       \
  map_arcade_card_io_read(pn, 0x6, arcade_card_read_offset1_port_##pn);       \
  map_arcade_card_io_read(pn, 0x7, arcade_card_read_increment0_port_##pn);    \
  map_arcade_card_io_read(pn, 0x8, arcade_card_read_increment1_port_##pn);    \
  map_arcade_card_io_read(pn, 0x9, arcade_card_read_control_port_##pn);       \
  map_arcade_card_io_read(pn, 0xA, zero_read);                                \
                                                                              \
  map_arcade_card_io_write(pn, 0x0, arcade_card_write_port_##pn);             \
  map_arcade_card_io_write(pn, 0x1, arcade_card_write_port_##pn);             \
  map_arcade_card_io_write(pn, 0x2, arcade_card_write_base0_port_##pn);       \
  map_arcade_card_io_write(pn, 0x3, arcade_card_write_base1_port_##pn);       \
  map_arcade_card_io_write(pn, 0x4, arcade_card_write_base2_port_##pn);       \
  map_arcade_card_io_write(pn, 0x5, arcade_card_write_offset0_port_##pn);     \
  map_arcade_card_io_write(pn, 0x6, arcade_card_write_offset1_port_##pn);     \
  map_arcade_card_io_write(pn, 0x7, arcade_card_write_increment0_port_##pn);  \
  map_arcade_card_io_write(pn, 0x8, arcade_card_write_increment1_port_##pn);  \
  map_arcade_card_io_write(pn, 0x9, arcade_card_write_control_port_##pn);     \
  map_arcade_card_io_write(pn, 0xA, arcade_card_write_adjust_base_port_##pn)  \

void map_arcade_card()
{
  // Map the segments 0x40 to 0x43
  memory_remap_read_write_io(0x40, 1, arcade_card.acd_read_functions_0,
   arcade_card.acd_write_functions_0);
  memory_remap_read_write_io(0x41, 1, arcade_card.acd_read_functions_1,
   arcade_card.acd_write_functions_1);
  memory_remap_read_write_io(0x42, 1, arcade_card.acd_read_functions_2,
   arcade_card.acd_write_functions_2);
  memory_remap_read_write_io(0x43, 1, arcade_card.acd_read_functions_3,
   arcade_card.acd_write_functions_3);

  // Map the I/O slots
  map_arcade_card_io(0);
  map_arcade_card_io(1);
  map_arcade_card_io(2);
  map_arcade_card_io(3);

  memory.io_read_functions[0x1AE0] = arcade_card_read_shift0;
  memory.io_read_functions[0x1AE1] = arcade_card_read_shift1;
  memory.io_read_functions[0x1AE2] = arcade_card_read_shift2;
  memory.io_read_functions[0x1AE3] = arcade_card_read_shift3;
  memory.io_read_functions[0x1AE4] = arcade_card_read_shift_amount;
  memory.io_read_functions[0x1AE5] = arcade_card_read_shift_latch;
  memory.io_read_functions[0x1AED] = zero_read;
  memory.io_read_functions[0x1AEE] = arcade_card_read_version_number;
  memory.io_read_functions[0x1AEF] = arcade_card_read_identification_number;

  memory.io_write_functions[0x1AE0] = arcade_card_write_shift0;
  memory.io_write_functions[0x1AE1] = arcade_card_write_shift1;
  memory.io_write_functions[0x1AE2] = arcade_card_write_shift2;
  memory.io_write_functions[0x1AE3] = arcade_card_write_shift3;
  memory.io_write_functions[0x1AE4] = arcade_card_write_shift_amount;
  memory.io_write_functions[0x1AE5] = arcade_card_write_shift_latch;

  memcpy(memory.io_read_functions + 0x1A40, memory.io_read_functions + 0x1A00,
   sizeof(io_read_function_ptr) * 0x40);
  memcpy(memory.io_write_functions + 0x1A40,
   memory.io_write_functions + 0x1A00, sizeof(io_write_function_ptr) * 0x40);

  memcpy(memory.io_read_functions + 0x1A80, memory.io_read_functions + 0x1A00,
   sizeof(io_read_function_ptr) * 0x40);
  memcpy(memory.io_write_functions + 0x1A80,
   memory.io_write_functions + 0x1A00, sizeof(io_write_function_ptr) * 0x40);

  memcpy(memory.io_read_functions + 0x1AC0, memory.io_read_functions + 0x1A00,
   sizeof(io_read_function_ptr) * 0x20);
  memcpy(memory.io_write_functions + 0x1AC0,
   memory.io_write_functions + 0x1A00, sizeof(io_write_function_ptr) * 0x20);

  memcpy(memory.io_read_functions + 0x1AF0, memory.io_read_functions + 0x1AE0,
   sizeof(io_read_function_ptr) * 0x10);
  memcpy(memory.io_write_functions + 0x1AF0,
   memory.io_write_functions + 0x1AE0, sizeof(io_write_function_ptr) * 0x10);

  memcpy(memory.io_read_functions + 0x1B00, memory.io_read_functions + 0x1A00,
   sizeof(io_read_function_ptr) * 0x100);
  memcpy(memory.io_write_functions + 0x1B00,
   memory.io_write_functions + 0x1A00, sizeof(io_write_function_ptr) * 0x100);
}

void unmap_arcade_card()
{
  u32 i;

  // Segments 0x40 to 0x43 should already be unmapped, leave them alone

  // Unmap the I/O slots
  for(i = 0; i < 0x40; i++)
  {
    memory.io_read_functions[0x1A00 + i] = ff_read;
    memory.io_write_functions[0x1A00 + i] = null_write;
  }
}

void reset_arcade_card()
{
  u32 i;

  for(i = 0; i < 4; i++)
  {
    arcade_card.ports[i].base_address = 0;
    arcade_card.ports[i].offset = 0;
    arcade_card.ports[i].increment = 0;
    arcade_card.ports[i].control = 0;
  }

  arcade_card.shift_value = 0;
  arcade_card.shift_amount = 0;
  arcade_card.shift_latch = 0;
}

void dump_acd_ram(u32 start, u32 size)
{
  u32 i, i2, offset;

  for(i = 0, offset = start; i < size / 8; i++)
  {
    for(i2 = 0; i2 < 8; i2++, offset++)
    {
      printf("%04x ", arcade_card.acd_ram[offset]);
    }
    printf("\n");
  }
}


#define arcade_card_savestate_builder(type, type_b, version_gate)             \
void arcade_card_##type_b##_savestate(savestate_##type_b##_type               \
 savestate_file)                                                              \
{                                                                             \
  u32 i;                                                                      \
                                                                              \
  for(i = 0; i < 4; i++)                                                      \
  {                                                                           \
    file_##type##_variable(savestate_file,                                    \
     arcade_card.ports[i].base_address);                                      \
    file_##type##_variable(savestate_file, arcade_card.ports[i].offset);      \
    file_##type##_variable(savestate_file, arcade_card.ports[i].increment);   \
    file_##type##_variable(savestate_file, arcade_card.ports[i].control);     \
  }                                                                           \
                                                                              \
  file_##type##_variable(savestate_file, arcade_card.shift_value);            \
  file_##type##_variable(savestate_file, arcade_card.shift_amount);           \
  file_##type##_variable(savestate_file, arcade_card.shift_latch);            \
                                                                              \
  file_##type(savestate_file, arcade_card.acd_ram, (1024 * 1024 * 2));        \
}                                                                             \

build_savestate_functions(arcade_card);

