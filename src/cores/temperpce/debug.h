#ifndef DEBUG_H
#define DEBUG_H

typedef enum
{
  DEBUG_STEP,
  DEBUG_RUN,
  DEBUG_STEP_RUN,
  DEBUG_PC_BREAKPOINT,
  DEBUG_COUNTDOWN_BREAKPOINT,
  DEBUG_WRITE_BREAKPOINT,
  DEBUG_NEXT_FRAME_A,
  DEBUG_NEXT_FRAME_B
} debug_mode_enum;

typedef struct
{
  u32 instruction_count;
  u32 breakpoint;
  u32 breakpoint_original;
  u32 current_pc;
  u32 last_pc;

  u64 sprite_mask;

  debug_mode_enum mode;
  debug_mode_enum previous_mode;
} debug_struct;

extern debug_struct debug;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void step_debug(int a, int x, int y, int p, int s, int pc, int remaining);
void initialize_debug();
void reset_debug();

void set_debug_mode(debug_mode_enum debug_mode);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
