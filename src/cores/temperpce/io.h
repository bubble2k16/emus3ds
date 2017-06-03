#ifndef IO_H
#define IO_H

typedef enum
{
  IO_BUTTON_NONE         = 0x000000,
  IO_BUTTON_UP           = 0x000001,
  IO_BUTTON_RIGHT        = 0x000002,
  IO_BUTTON_DOWN         = 0x000004,
  IO_BUTTON_LEFT         = 0x000008,
  IO_BUTTON_I            = 0x000010,
  IO_BUTTON_II           = 0x000020,
  IO_BUTTON_SELECT       = 0x000040,
  IO_BUTTON_RUN          = 0x000080,
  IO_BUTTON_III          = 0x000100,
  IO_BUTTON_IV           = 0x000200,
  IO_BUTTON_V            = 0x000400,
  IO_BUTTON_VI           = 0x000800,
  IO_BUTTON_CLEAR        = 0x800000
} io_buttons_enum;

// 24B

typedef struct
{
  u32 select;
  u32 clr;
  u32 port;

  io_buttons_enum button_status[5];

  // Not sure if these are worth being saved in a state or not.
  u32 six_button_toggle;
  u32 read_index;
} io_struct;

extern io_struct io;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


u32 io_port_read();
void io_port_write(u32 value);

void initialize_io();
void reset_io();

void io_load_savestate(savestate_load_type savestate_file);
void io_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
