#include "common.h"

timer_struct timer;

u32 timer_value_read()
{
  // This is for Battle Royale, here's hoping it doesn't break anything else.
  if(timer.read_hack)
  {
    timer.read_hack = 0;
    memory.io_buffer = (memory.io_buffer & 0x80) | 0x7F;
  }
  else
  {
    memory.io_buffer = (memory.io_buffer & 0x80) | timer.value;
  }
  return memory.io_buffer;
}

u32 timer_enable_read()
{
  memory.io_buffer = (memory.io_buffer & 0xFE) | timer.enabled;
  return memory.io_buffer;
}

void timer_reload_write(u32 value)
{
  memory.io_buffer = value;

  timer.reload = value & 0x7F;
}

void timer_enable_write(u32 value)
{
  memory.io_buffer = value;
  if(timer.enabled == 0)
    timer.value = timer.reload;

  timer.enabled = value & 0x1;
}

void update_timer()
{
  if(timer.enabled)
  {
    timer.value--;

    if(timer.value & 0x80)
    {
      timer.read_hack = 1;
      timer.value = timer.reload;

      raise_interrupt(IRQ_TIMER);
    }
  }
}

void initialize_timer()
{
}

void reset_timer()
{
  timer.enabled = 0;
  timer.reload = 0;
  timer.value = 0;
  timer.read_hack = 0;

  timer.cycles_remaining = 1024 * 3;
}

s32 add_cycles;

#ifdef ARM_ARCH

void execute_instructions_fast(u32 count);

#define execute_instructions_select(count)                                    \
    execute_instructions_fast(count)                                          \

/*
void execute_instructions_compatible(u32 count);
void execute_instructions_fast(u32 count);

#define execute_instructions_select(count)                                    \
  if(config.compatibility_mode)                                               \
    execute_instructions_compatible(count);                                   \
  else                                                                        \
    execute_instructions_fast(count)                                          \
*/

#else

#define execute_instructions_select(count)                                    \
    execute_instructions(count)                                          \

#endif

void execute_instructions_timer(s32 cpu_cycles_remaining)
{
  if(timer.cycles_remaining < cpu_cycles_remaining)
  {
    if(timer.cycles_remaining != 0)
    {
      cpu_cycles_remaining -= timer.cycles_remaining;
      execute_instructions_select(timer.cycles_remaining);
    }

    timer.cycles_remaining = 1024 * 3;

    update_timer();
  }

  timer.cycles_remaining -= cpu_cycles_remaining;

  if(cpu_cycles_remaining != 0)
  {
    execute_instructions_select(cpu_cycles_remaining);
  }

  update_cd_read();
  update_adpcm_dma();
  update_adpcm();
}


#define timer_savestate_builder(type, type_b, version_gate)                   \
void timer_##type_b##_savestate(savestate_##type_b##_type savestate_file)     \
{                                                                             \
  file_##type##_variable(savestate_file, timer.reload);                       \
  file_##type##_variable(savestate_file, timer.value);                        \
  file_##type##_variable(savestate_file, timer.enabled);                      \
  file_##type##_variable(savestate_file, timer.read_hack);                    \
  file_##type##_variable(savestate_file, timer.cycles_remaining);             \
}                                                                             \

build_savestate_functions(timer);

