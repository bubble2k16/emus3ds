#ifndef TIMER_H
#define TIMER_H

// 20B

typedef struct
{
  u32 reload;
  u32 value;
  u32 enabled;

  u32 read_hack;
  s32 cycles_remaining;
} timer_struct;

extern timer_struct timer;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

u32 timer_counter_read();
u32 timer_enable_read();
u32 timer_value_read();

void timer_reload_write(u32 value);
void timer_enable_write(u32 value);

void update_timer();

void initialize_timer();
void reset_timer();

void timer_load_savestate(savestate_load_type savestate_file);
void timer_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif

#endif
