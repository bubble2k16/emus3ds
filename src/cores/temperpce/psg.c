#include "common.h"

// TODO: Use a table for the frequency steps.

// clock_delta is a 20.12 fixed point number. Thus it shouldn't
// be allowed to get too high. So long as an update happens at
// least once per frame 20 bits should be fine (technically only
// 19 should be needed)

const u32 step_fractional_bits_clock = 12;
const u32 step_fractional_bits_frequency = 27;
const u32 step_fractional_bits_noise = 14;

psg_struct psg;


#define update_volume(channel_ptr)                                            \
{                                                                             \
  u32 channel_volume = channel_ptr->control & 0x1F;                           \
  channel_ptr->l_volume = psg.volume_data[channel_volume +                    \
   (((psg.global_balance >> 4) + (channel_ptr->balance >> 4)) * 2)];          \
  channel_ptr->r_volume = psg.volume_data[channel_volume +                    \
   (((psg.global_balance & 0xF) + (channel_ptr->balance & 0xF)) * 2)];        \
}                                                                             \

void psg_channel_select(u32 value)
{
  value &= 0x7;
  if(value < 6)
  {
    psg.current_channel_value = value;
    psg.current_channel = psg.psg_channels + value;
  }
}

void psg_global_balance(u32 value)
{
  //if(psg.global_balance != value)
  {
    u32 i;
    psg_channel_struct *current_channel;

    update_psg();

    psg.global_balance = value;

    for(i = 0; i < 6; i++)
    {
      current_channel = psg.psg_channels + i;
      if(current_channel->control & 0x80)
      {
        update_volume(current_channel);
      }
    }
  }
}

void psg_fine_frequency(u32 value)
{
  //if((psg.current_channel->frequency & 0xFF) != value)
  {
    update_psg();
    psg.current_channel->update_frequency_step = 1;

    psg.current_channel->frequency =
     (psg.current_channel->frequency & 0xFF00) | value;
  }
}

void psg_rough_frequency(u32 value)
{
  value &= 0xF;

  //if((psg.current_channel->frequency >> 8) != value)
  {
    update_psg();
    psg.current_channel->update_frequency_step = 1;

    psg.current_channel->frequency =
     (psg.current_channel->frequency & 0xFF) | (value << 8);
  }
}

void psg_channel_control(u32 value)
{
  update_psg();
  psg.current_channel->control = value;

  update_volume(psg.current_channel);
}

void psg_channel_balance(u32 value)
{
  //if(psg.current_channel->balance != value)
  {
    update_psg();
    psg.current_channel->balance = value;

    update_volume(psg.current_channel);
  }
}

void psg_sound_data(u32 value)
{
  update_psg();

  switch(psg.current_channel->control >> 6)
  {
    case 0:
      psg.current_channel->waveform_data[psg.current_channel->write_index] =
       (value & 0x1F) - 16;

      psg.current_channel->write_index =
       (psg.current_channel->write_index + 1) % 32;

      break;

    case 1:
      psg.current_channel->read_index = 0;
      psg.current_channel->write_index = 0;
      break;

    case 2:
      break;

    case 3:
      value = (value & 0x1F) - 16;
      psg.current_channel->audio_data = value;
      break;
  }
}

void psg_noise_control(u32 value)
{
//  if((psg.current_channel_value >= 4) &&
//   (psg.current_channel->noise_control != value))
  {
    update_psg();

    if((value & 0x1F) != (psg.current_channel->noise_control & 0x1F))
      psg.current_channel->update_frequency_step = 1;

    psg.current_channel->noise_control = value;
  }
}

void psg_lfo_frequency(u32 value)
{
  //if(psg.lfo_frequency != value)
  {
    update_psg();
    psg.psg_channels[1].update_frequency_step = 1;
    psg.lfo_frequency = value;
  }
}

void psg_lfo_control(u32 value)
{
  update_psg();
  psg.lfo_control = value;

  if(((value & 0x80) == 0x80) && ((value & 0x3) != 0))
  {
    psg.psg_channels[1].read_index = 0;
  }
}


void psg_reg_queue_00(u32 value)
{
  psgqueue_add(0x0, cpu.global_cycles, value);
}

void psg_reg_queue_01(u32 value)
{
  psgqueue_add(0x1, cpu.global_cycles, value);
}

void psg_reg_queue_02(u32 value)
{
  psgqueue_add(0x2, cpu.global_cycles, value);
}

void psg_reg_queue_03(u32 value)
{
  psgqueue_add(0x3, cpu.global_cycles, value);
}

void psg_reg_queue_04(u32 value)
{
  psgqueue_add(0x4, cpu.global_cycles, value);
}

void psg_reg_queue_05(u32 value)
{
  psgqueue_add(0x5, cpu.global_cycles, value);
}

void psg_reg_queue_06(u32 value)
{
  psgqueue_add(0x6, cpu.global_cycles, value);
}

void psg_reg_queue_07(u32 value)
{
  psgqueue_add(0x7, cpu.global_cycles, value);
}

void psg_reg_queue_08(u32 value)
{
  psgqueue_add(0x8, cpu.global_cycles, value);
}

void psg_reg_queue_09(u32 value)
{
  psgqueue_add(0x9, cpu.global_cycles, value);
}




void initialize_noise_data()
{
  u32 i;
  u32 lfsr_reg = 0x1;
  u32 lfsr_bit;

  for(i = 0; i < (1 << 18); i++)
  {
    lfsr_bit = ((lfsr_reg >> 0) ^ (lfsr_reg >> 1) ^
     (lfsr_reg >> 11) ^ (lfsr_reg >> 12)) & 0x1;

    if(lfsr_bit)
      psg.noise_data[i] = -16;
    else
      psg.noise_data[i] = 15;

    lfsr_reg = (lfsr_reg >> 1) | (lfsr_bit << 17);
  }
}

s32 volume_data[] =
{
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  3,
  4,
  4,
  5,
  6,
  8,
  9,
  11,
  13,
  16,
  19,
  23,
  27,
  32,
  38,
  46,
  55,
  65,
  77,
  92,
  109,
  130,
  155,
  184,
  219,
  260,
  309,
  367,
  436,
  519,
  617,
  733,
  871,
  1036,
  1231,
  1463,
  1739,
  2067,
  2457,
  2920,
  3470,
  4125,
  4902,
  5827,
  6925,
  8230,
  9782,
  11626,
  13818,
  16422,
  19518,
  23197,
  27570,
  32768
};


io_write_function_ptr psg_io_write_function[] = {
  psg_channel_select,
  psg_global_balance,
  psg_fine_frequency,
  psg_rough_frequency,
  psg_channel_control,
  psg_channel_balance,
  psg_sound_data,
  psg_noise_control,
  psg_lfo_frequency,
  psg_lfo_control
};


void initialize_volume_data()
{
  u32 i;

  for(i = 0; i < 92; i++)
  {
    psg.volume_data[i] = volume_data[i] / 2;
  }
}

void initialize_psg()
{
  initialize_audio();

  psg.clock_step = (MASTER_CLOCK_RATE << step_fractional_bits_clock) /
   audio.output_frequency;

  psg.cpu_sync_cycles = 0;

  initialize_noise_data();
  initialize_volume_data();
}

void reset_psg()
{
  u32 i;

  psg.current_channel = psg.psg_channels;

  audio_lock();
  audio_reset_buffer();

  for(i = 0; i < 6; i++)
  {
    psg.psg_channels[i].control = 0;
    psg.psg_channels[i].noise_control = 0;
    psg.psg_channels[i].write_index = 0;
    psg.psg_channels[i].read_index = 0;
    psg.psg_channels[i].update_frequency_step = 0;
  }

  psg.last_cycles = 0;
  psg.cpu_sync_cycles = 0;
  reset_psgqueue();

  audio_unlock();
}


#define update_frequency_step(_psg_channel, var)                              \
{                                                                             \
  u32 frequency = var;                                                        \
                                                                              \
  if(frequency == 0)                                                          \
    frequency = 4096;                                                         \
                                                                              \
  _psg_channel->frequency_step =                                              \
   (u64)((u64)3579545ll << (u64)step_fractional_bits_frequency) /             \
   (audio.output_frequency * frequency);                                      \
}                                                                             \


#define update_frequency_step_noise(_psg_channel)                             \
{                                                                             \
  u32 frequency = (_psg_channel->noise_control & 0x1F) ^ 0x1F;                \
                                                                              \
  if(frequency == 0)                                                          \
  {                                                                           \
    _psg_channel->frequency_step =                                            \
     (u64)((3579545ll << (u64)step_fractional_bits_noise) / 32ll) /           \
     audio.output_frequency;                                                  \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    _psg_channel->frequency_step =                                            \
     (u64)((3579545ll << (u64)step_fractional_bits_noise) / 64ll) /           \
     (audio.output_frequency * frequency);                                    \
  }                                                                           \
}                                                                             \


#define render_psg_sample_wave()                                              \
{                                                                             \
  s32 next_sample;                                                            \
  s32 index_weight;                                                           \
  current_sample =                                                            \
   waveform_data[read_index >> step_fractional_bits_frequency];               \
  next_sample =                                                               \
   waveform_data[(read_index + (1 << step_fractional_bits_frequency)) >>      \
   step_fractional_bits_frequency];                                           \
  index_weight = read_index & ((1 << step_fractional_bits_frequency) - 1);    \
  current_sample += ((s64)(next_sample - current_sample) * index_weight) >>   \
   step_fractional_bits_frequency;                                            \
                                                                              \
  audio_buffer[audio_buffer_index] += current_sample * l_volume;              \
  audio_buffer[audio_buffer_index + 1] += current_sample * r_volume;          \
}                                                                             \
                                                                              \
  read_index += frequency_step                                                \


#define render_psg_sample_noise()                                             \
  current_sample =                                                            \
   psg.noise_data[read_index >> step_fractional_bits_noise];                  \
  audio_buffer[audio_buffer_index] += current_sample * l_volume;              \
  audio_buffer[audio_buffer_index + 1] += current_sample * r_volume;          \
                                                                              \
  read_index += frequency_step                                                \


#define render_psg_sample_lfo()                                               \
  render_psg_sample_wave();                                                   \
  lfo_cycles -= clock_step;                                                   \
                                                                              \
  if(lfo_cycles < 0)                                                          \
  {                                                                           \
    s32 new_frequency = psg_channel->frequency;                               \
    lfo_read_index = (lfo_read_index + 1) % 32;                               \
    lfo_delta = lfo_fm_data[lfo_read_index];                                  \
    new_frequency += (s32)(lfo_delta << lfo_shift);                           \
                                                                              \
    update_frequency_step(psg_channel, new_frequency);                        \
    frequency_step = psg_channel->frequency_step;                             \
    lfo_cycles += (u64)psg.lfo_period << step_fractional_bits_clock;          \
  }                                                                           \

#define render_psg_sample_dda()                                               \
  audio_buffer[audio_buffer_index] += l_current_sample;                       \
  audio_buffer[audio_buffer_index + 1] += r_current_sample                    \


#define render_psg_sample_null()                                              \



#define render_psg_loop(type)                                                 \
  clocks_remaining = clock_delta;                                             \
  audio_buffer_index = audio_buffer_base_index;                               \
  while(clocks_remaining >= 0)                                                \
  {                                                                           \
    render_psg_sample_##type();                                               \
    clocks_remaining  -= clock_step;                                          \
                                                                              \
    audio_buffer_index = (audio_buffer_index + 2) % AUDIO_BUFFER_SIZE;        \
  }                                                                           \


#define render_psg_channel(type)                                              \
  if(psg_channel->control & 0x80)                                             \
  {                                                                           \
    psg_rendered = 1;                                                         \
    read_index = psg_channel->read_index;                                     \
    frequency_step = psg_channel->frequency_step;                             \
    l_volume = psg_channel->l_volume;                                         \
    r_volume = psg_channel->r_volume;                                         \
    waveform_data = psg_channel->waveform_data;                               \
                                                                              \
    render_psg_loop(type);                                                    \
                                                                              \
    psg_channel->read_index = read_index;                                     \
  }                                                                           \

#define render_psg_channel_wave(channel_number)                               \
  if((psg_channel->frequency - 1) > 5)                                        \
  {                                                                           \
    if(psg_channel->update_frequency_step)                                    \
    {                                                                         \
      update_frequency_step(psg_channel, psg_channel->frequency);             \
      psg_channel->update_frequency_step = 0;                                 \
    }                                                                         \
                                                                              \
    render_psg_channel(wave);                                                 \
  }                                                                           \

#define render_psg_channel_lfo(channel_number)                                \
{                                                                             \
  psg_channel_struct *lfo_channel = psg.psg_channels + 1;                     \
  s64 lfo_cycles;                                                             \
  u32 lfo_shift;                                                              \
  u32 lfo_read_index;                                                         \
  s8 *lfo_fm_data;                                                            \
  s32 lfo_delta;                                                              \
                                                                              \
  if(lfo_channel->control & 0x80)                                             \
  {                                                                           \
    lfo_shift = ((psg.lfo_control & 0x3) - 1) * 2;                            \
    lfo_read_index = lfo_channel->read_index;                                 \
    if(lfo_channel->update_frequency_step)                                    \
    {                                                                         \
      psg.lfo_period = lfo_channel->frequency * psg.lfo_frequency * 6;        \
      psg.lfo_cycles = (u64)psg.lfo_period << step_fractional_bits_clock;     \
      lfo_channel->update_frequency_step = 0;                                 \
    }                                                                         \
    lfo_cycles = psg.lfo_cycles;                                              \
    lfo_fm_data = lfo_channel->waveform_data;                                 \
                                                                              \
    render_psg_channel(lfo);                                                  \
                                                                              \
    lfo_channel->read_index = lfo_read_index;                                 \
    psg.lfo_cycles = lfo_cycles;                                              \
  }                                                                           \
}                                                                             \

#define render_psg_channel_noise(channel_number)                              \
  if(psg_channel->noise_control & 0x80)                                       \
  {                                                                           \
    if(psg_channel->update_frequency_step)                                    \
    {                                                                         \
      update_frequency_step_noise(psg_channel);                               \
      frequency_step = psg_channel->frequency_step;                           \
      psg_channel->update_frequency_step = 0;                                 \
    }                                                                         \
                                                                              \
    render_psg_channel(noise)                                                 \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    render_psg_channel_wave(channel_number);                                  \
  }                                                                           \


#define render_psg_channel_switch(type, channel_number)                       \
  psg_channel = psg.psg_channels + channel_number;                            \
                                                                              \
  if(psg_channel->control & 0x40)                                             \
  {                                                                           \
    s32 l_current_sample, r_current_sample;                                   \
    l_current_sample = psg_channel->audio_data * psg_channel->l_volume;       \
    r_current_sample = psg_channel->audio_data * psg_channel->r_volume;       \
    psg_rendered = 1;                                                         \
    render_psg_loop(dda);                                                     \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    render_psg_channel_##type(channel_number);                                \
  }                                                                           \


/*
  Original code
void update_psg()
{
  return;

  // PSG clocks remaining
  s32 clock_delta =
   (cpu.global_cycles << step_fractional_bits_clock) - psg.last_cycles;

  if(clock_delta <= 0)
    return;

  u32 clock_step = psg.clock_step;
  s32 clocks_remaining = clock_delta;

  // PSG channel variables
  psg_channel_struct *psg_channel;
  u32 read_index;
  u32 frequency_step;
  u32 l_volume, r_volume;
  s8 *waveform_data;
  s32 current_sample;

  // Audio output variables
  s32 *audio_buffer = audio.buffer;
  u32 audio_buffer_base_index;
  u32 audio_buffer_index = audio.buffer_index;

  u32 psg_rendered = 0;

  audio_buffer_base_index = audio.buffer_index;

  if(((psg.lfo_control & 0x80) == 0x80) || ((psg.lfo_control & 0x3) == 0))
  {
    render_psg_channel_switch(wave, 0);
    render_psg_channel_switch(wave, 1);
  }
  else
  {
    render_psg_channel_switch(lfo, 0);
  }

  render_psg_channel_switch(wave, 2);
  render_psg_channel_switch(wave, 3);

  render_psg_channel_switch(noise, 4);
  render_psg_channel_switch(noise, 5);

  if(psg_rendered == 0)
  {
    render_psg_loop(null);
  }

  psg.last_cycles += (u64)(clock_delta - clocks_remaining);
  audio.buffer_index = audio_buffer_index;
}
*/

bool ignore_update_psg = false;
void update_psg()
{
  if (ignore_update_psg)
    return;

  // PSG clocks remaining
  //s32 clock_delta =
  // (cpu.global_cycles << step_fractional_bits_clock) - psg.last_cycles;
  s32 clock_delta = psg.cpu_sync_cycles - psg.last_cycles;

  if(clock_delta <= 0)
    return;

  u32 clock_step = psg.clock_step;
  s32 clocks_remaining = clock_delta;

  // PSG channel variables
  psg_channel_struct *psg_channel;
  u32 read_index;
  u32 frequency_step;
  u32 l_volume, r_volume;
  s8 *waveform_data;
  s32 current_sample;

  // Audio output variables
  s32 *audio_buffer = audio.buffer;
  u32 audio_buffer_base_index;
  u32 audio_buffer_index = audio.buffer_index;

  u32 psg_rendered = 0;

  audio_buffer_base_index = audio.buffer_index;

  if(((psg.lfo_control & 0x80) == 0x80) || ((psg.lfo_control & 0x3) == 0))
  {
    render_psg_channel_switch(wave, 0);
    render_psg_channel_switch(wave, 1);
  }
  else
  {
    render_psg_channel_switch(lfo, 0);
  }

  render_psg_channel_switch(wave, 2);
  render_psg_channel_switch(wave, 3);

  render_psg_channel_switch(noise, 4);
  render_psg_channel_switch(noise, 5);

  if(psg_rendered == 0)
  {
    render_psg_loop(null);
  }

  psg.last_cycles += (u64)(clock_delta - clocks_remaining);
  audio.buffer_index = audio_buffer_index;
}


void render_psg(int num_samples)
{
  audio.buffer_index = 0;
  for (int i = 0; i < num_samples * 2; i++)
  {
    audio.buffer[i] = 0;
  }

  u64 final_cpu_sync_cycles = psg.cpu_sync_cycles + num_samples * psg.clock_step;

  // dequeue all writes to the registers and play them back.
  //
  ignore_update_psg = false;
  u64 cpu_clock;
  int psg_reg;
  u32 psg_value;
  bool first_sample = true;
  while (true)
  {
    bool has_data = psgqueue_peek_next(&cpu_clock, &psg_reg, &psg_value);
    if (has_data)
    {
      if (first_sample)
      {
        psg.cpu_sync_cycles = cpu_clock << step_fractional_bits_clock;
        psg.last_cycles = cpu_clock << step_fractional_bits_clock;
        final_cpu_sync_cycles = psg.cpu_sync_cycles + num_samples * psg.clock_step;
        
        first_sample = false;
      }

      if ((cpu_clock << step_fractional_bits_clock) > final_cpu_sync_cycles)
        break;

      psgqueue_read_next(&cpu_clock, &psg_reg, &psg_value);

      psg.cpu_sync_cycles = cpu_clock << step_fractional_bits_clock;
      //printf ("dequeue: %d %2x @ %llx %x %d\n", psg_reg, psg_value, cpu_clock, psg.clock_step, audio.buffer_index);
      //printf ("         %10llx %10llx\n", psg.cpu_sync_cycles, psg.last_cycles);
      psg_io_write_function[psg_reg](psg_value);
    }
    else
      break;
  }


  ignore_update_psg = false;
  psg.cpu_sync_cycles = final_cpu_sync_cycles;
  update_psg();

/*
  s64 current_global_cycles = cpu.global_cycles << step_fractional_bits_clock;
  s64 total_cycles_per_frame = psg.clock_step * 600;
  if (psg.cpu_sync_cycles - current_global_cycles > total_cycles_per_frame ||
    psg.cpu_sync_cycles - current_global_cycles < -total_cycles_per_frame)
  {
    psg.cpu_sync_cycles = current_global_cycles;
    psg.last_cycles = current_global_cycles;
  }
  */
  //printf ("end: %llx %llx\n", old_cpu_sync_cycles >> 12, psg.cpu_sync_cycles >> 12);
  //printf ("%d\n", audio.buffer_index);
}


#define psg_savestate_extra_load()                                            \
  memset(audio.buffer, 0, sizeof(audio.buffer));                              \
  psg.current_channel = psg.psg_channels + psg.current_channel_value          \

#define psg_savestate_extra_store()                                           \

#define psg_savestate_builder(type, type_b, version_gate)                     \
void psg_##type_b##_savestate(savestate_##type_b##_type savestate_file)       \
{                                                                             \
  u32 i;                                                                      \
  psg_channel_struct *current_psg_channel = psg.psg_channels;                 \
                                                                              \
  for(i = 0; i < 6; i++, current_psg_channel++)                               \
  {                                                                           \
    file_##type##_array(savestate_file, current_psg_channel->waveform_data);  \
    file_##type##_variable(savestate_file, current_psg_channel->read_index);  \
    file_##type##_variable(savestate_file, current_psg_channel->write_index); \
    file_##type##_variable(savestate_file, current_psg_channel->control);     \
    file_##type##_variable(savestate_file,                                    \
     current_psg_channel->noise_control);                                     \
    file_##type##_variable(savestate_file, current_psg_channel->frequency);   \
    file_##type##_variable(savestate_file,                                    \
     current_psg_channel->frequency_step);                                    \
    file_##type##_variable(savestate_file, current_psg_channel->balance);     \
    file_##type##_variable(savestate_file, current_psg_channel->audio_data);  \
    file_##type##_variable(savestate_file, current_psg_channel->l_volume);    \
    file_##type##_variable(savestate_file, current_psg_channel->r_volume);    \
    file_##type##_variable(savestate_file,                                    \
     current_psg_channel->update_frequency_step);                             \
  }                                                                           \
  file_##type##_variable(savestate_file, psg.current_channel_value);          \
  file_##type##_variable(savestate_file, psg.global_balance);                 \
  file_##type##_variable(savestate_file, psg.lfo_control);                    \
  file_##type##_variable(savestate_file, psg.lfo_frequency);                  \
  file_##type##_variable(savestate_file, psg.psg_updated);                    \
  file_##type##_variable(savestate_file, psg.last_cycles);                    \
                                                                              \
  if(version_gate >= 2)                                                       \
  {                                                                           \
    audio_lock();                                                             \
                                                                              \
    file_##type##_variable(savestate_file, audio.buffer_base);                \
    file_##type##_variable(savestate_file, audio.buffer_index);               \
                                                                              \
    audio_unlock();                                                           \
  }                                                                           \
                                                                              \
  psg_savestate_extra_##type_b();                                             \
}                                                                             \

build_savestate_functions(psg);

