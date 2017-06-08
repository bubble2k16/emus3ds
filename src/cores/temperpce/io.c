#include "common.h"

io_struct io;

u32 io_port_read()
{
  if(io.read_index > 4)
    return io.port;

  //if(io.read_index != 0)
  //  return io.port | 0xF; 

  u32 button_status = io.button_status[io.read_index];
  /*static u32 last_button_status = 0;

  if(button_status != last_button_status)
    printf("status %x at frame %x\n", button_status, vce.frames_rendered);

  last_button_status = button_status;*/

  //if(vce.frames_rendered == 0x10)
  //  button_status &= ~IO_BUTTON_RUN;

  if(io.six_button_toggle)
  {
    if(io.select)
      memory.io_buffer = io.port;
    else
      memory.io_buffer = io.port | ((button_status >> 8) & 0xF);
  }
  else
  {
    if(io.select)
      memory.io_buffer = io.port | (button_status & 0xF);
    else
      memory.io_buffer = io.port | ((button_status >> 4) & 0xF);
  }

  return memory.io_buffer;
}

void io_port_write(u32 value)
{
  memory.io_buffer = value;

  if((io.clr == 0) && ((value & 0x3) == 0x3))
  {
    io.read_index = 0;
  }
  else

  if((io.select == 0) && (value & 0x1) && (io.read_index < 255))
  {
    io.read_index++;
  }

  io.select = value & 0x1;
  io.clr = (value >> 1) & 0x1;

  if(io.clr && (config.six_button_pad))
    io.six_button_toggle ^= 1;
}

void initialize_io()
{
  initialize_event();
}

void reset_io()
{
  u32 i;

  io.select = 0;
  io.port = 0x3 << 4;
  io.six_button_toggle = 0;
  io.read_index = 0;

  if(config.cd_loaded == 0)
    io.port |= 0x8 << 4;

  for(i = 0; i < 5; i++)
  {
    io.button_status[i] = 0xFFF;
  }
}

#define io_savestate_extra_load()                                             \
  if(config.cd_loaded == 0)                                                   \
    io.port |= 0x80;                                                          \
  else                                                                        \
    io.port &= 0x7F;                                                          \

#define io_savestate_extra_store()                                            \

#define io_savestate_builder(type, type_b, version_gate)                      \
void io_##type_b##_savestate(savestate_##type_b##_type savestate_file)        \
{                                                                             \
  file_##type##_variable(savestate_file, io.select);                          \
  file_##type##_variable(savestate_file, io.clr);                             \
  file_##type##_variable(savestate_file, io.port);                            \
  if(version_gate >= 3)                                                       \
  {                                                                           \
    file_##type##_array(savestate_file, io.button_status);                    \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    file_##type##_variable(savestate_file, io.button_status[0]);              \
  }                                                                           \
                                                                              \
  io_savestate_extra_##type_b();                                              \
}                                                                             \

build_savestate_functions(io);

