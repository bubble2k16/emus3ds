#ifndef ARCADE_CARD_H
#define ARCADE_CARD_H

// 16B

typedef struct
{
  u32 base_address;
  u32 offset;
  u32 increment;
  u32 control;
} arcade_card_port_struct;

// 256KB + 80B

typedef struct
{
  arcade_card_port_struct ports[4];
  u32 shift_value;
  u32 shift_amount;
  u32 shift_latch;

  // Do not save the pointer itself in save states. Do save what it points
  // to, for 2MB.
  u8 *acd_ram;

  // No need to save these in a save state
  io_read_function_ptr acd_read_functions_0[8192];
  io_write_function_ptr acd_write_functions_0[8192];
  io_read_function_ptr acd_read_functions_1[8192];
  io_write_function_ptr acd_write_functions_1[8192];
  io_read_function_ptr acd_read_functions_2[8192];
  io_write_function_ptr acd_write_functions_2[8192];
  io_read_function_ptr acd_read_functions_3[8192];
  io_write_function_ptr acd_write_functions_3[8192];
} arcade_card_struct;

typedef enum
{
  ARCADE_CARD_CONTROL_AUTO_INCREMENT_ENABLE = 0x01,
  ARCADE_CARD_CONTROL_USE_OFFSET            = 0x02,
  ARCADE_CARD_CONTROL_ADD_OFFSET_EXT        = 0x08,
  ARCADE_CARD_CONTROL_AUTO_INCREMENT_TYPE   = 0x10,
  ARCADE_CARD_CONTROL_ADD_OFFSET_A          = 0x20,
  ARCADE_CARD_CONTROL_ADD_OFFSET_6          = 0x40
} arcade_card_control_enum;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


void map_arcade_card();
void unmap_arcade_card();

void initialize_arcade_card();
void reset_arcade_card();

void dump_acd_ram(u32 start, u32 size);

void arcade_card_load_savestate(savestate_load_type savestate_file);
void arcade_card_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif







