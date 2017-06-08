#ifndef ADPCM_H
#define ADPCM_H

typedef enum
{
  ADPCM_STATE_NORMAL,
  ADPCM_STATE_PLAYBACK_HALF_DONE,
  ADPCM_STATE_PLAYBACK_DONE,
  ADPCM_STATE_STOPPED
} adpcm_state_enum;

// 64K + 96B

typedef struct
{
  u8 sample_ram[1024 * 64];
  u64 last_cycles;
  u64 last_dma_cycles;

  // This addresses in nibbles
  u32 sample_read_address;
  // While this addresses in bytes
  u32 sample_write_address;
  // Count is in nibbles
  u32 sample_length;
  u32 sample_length_cached;
  u32 sample_half_length;

  s32 last_sample;
  s32 last_sample_adjusted;
  u32 step_index;
  u32 latch_value;
  u32 last_command;
  u32 read_latch;

  u32 sample_index_fractional;

  u32 playback_rate;
  u32 dma_enabled;
  u32 audio_buffer_index;

  s32 volume;
  u32 fade_cycle_interval;
  s32 fade_cycles;

  // Constructed values not necessary to be saved in savestate:
  u32 frequency_step;

  u32 shift_write;
} adpcm_struct;


#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


u32 adpcm_read();
u32 adpcm_read_command();
u32 adpcm_dma_status();
u32 adpcm_status();

void adpcm_write_latch_high(u32 value);
void adpcm_write_latch_low(u32 value);
void adpcm_playback_rate(u32 value);
void adpcm_write_command(u32 value);
void adpcm_write_data(u32 value);
void adpcm_enable_dma(u32 value);

void adpcm_fade_out(u32 ms);
void adpcm_full_volume();

void initialize_adpcm();
void reset_adpcm();

void update_adpcm();
void update_adpcm_dma();

void adpcm_load_savestate(savestate_load_type savestate_file);
void adpcm_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
