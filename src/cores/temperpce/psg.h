#ifndef PSG_H
#define PSG_H

#define MASTER_CLOCK_RATE 21477270ll

// 32B + 44B
// 76B

typedef struct
{
  s8 waveform_data[32];
  u32 read_index;
  u32 write_index;

  u32 control;
  u32 noise_control;
  u32 frequency;
  u32 frequency_step;
  u32 balance;

  u32 audio_data;
  s32 l_volume;
  s32 r_volume;

  u32 update_frequency_step;
} psg_channel_struct;

// 456B + 64B + 368B + 32KB
// 32KB + 888B

typedef struct
{
  psg_channel_struct psg_channels[6];

  u32 current_channel_value;
  u32 global_balance;

  u32 lfo_control;
  u32 lfo_frequency;

  u32 psg_updated;
  u64 last_cycles;

  s64 lfo_cycles;
  u32 lfo_period;

  // Don't save in savestate, but do reconstruct the current_channel
  // pointer upon loading.

  u32 clock_step;

  s8 noise_data[256 * 1024];
  s32 volume_data[92];

  psg_channel_struct *current_channel;

  u64 cpu_sync_cycles;
} psg_struct;

extern psg_struct psg;

extern const u32 step_fractional_bits_clock;
extern const u32 step_fractional_bits_frequency;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void psg_channel_select(u32 value);
void psg_global_balance(u32 value);
void psg_fine_frequency(u32 value);
void psg_rough_frequency(u32 value);
void psg_channel_control(u32 value);
void psg_channel_balance(u32 value);
void psg_sound_data(u32 value);
void psg_noise_control(u32 value);
void psg_lfo_frequency(u32 value);
void psg_lfo_control(u32 value);

void psg_reg_queue_00(u32 value);
void psg_reg_queue_01(u32 value);
void psg_reg_queue_02(u32 value);
void psg_reg_queue_03(u32 value);
void psg_reg_queue_04(u32 value);
void psg_reg_queue_05(u32 value);
void psg_reg_queue_06(u32 value);
void psg_reg_queue_07(u32 value);
void psg_reg_queue_08(u32 value);
void psg_reg_queue_09(u32 value);

void initialize_psg();
void reset_psg();
void update_psg();
void audio_exit();
void render_psg(int num_samples);

void psg_load_savestate(savestate_load_type savestate_file);
void psg_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
