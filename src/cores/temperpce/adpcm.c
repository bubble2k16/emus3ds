#include "common.h"

adpcm_struct adpcm;

u16 adpcm_step_values[49] =
{
    16,   17,   19,   21,   23,   25,   28,   31,
    34,   37,   41,   45,   50,   55,   60,   66,
    73,   80,   88,   97,  107,  118,  130,  143,
   157,  173,  190,  209,  230,  253,  279,  307,
   337,  371,  408,  449,  494,  544,  598,  658,
   724,  796,  876,  963, 1060, 1166, 1282, 1408,
  1552
};

s32 adpcm_table_move[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

s32 adpcm_convert_sample(u32 adpcm_code)
{
  s32 step_index = adpcm.step_index;
  u32 step_value = adpcm_step_values[step_index];
  s32 sample_step = step_value / 8;
  s32 sample;

  if(adpcm_code & 0x1)
    sample_step += step_value / 4;

  if(adpcm_code & 0x2)
    sample_step += step_value / 2;

  if(adpcm_code & 0x4)
    sample_step += step_value;

  if(adpcm_code & 0x8)
    sample_step = -sample_step;

  step_index += adpcm_table_move[adpcm_code & 0x7];

  if((u32)step_index > 48)
  {
    if(step_index < 0)
      step_index = 0;
    else
      step_index = 48;
  }

  sample = (adpcm.last_sample + sample_step);

  adpcm.last_sample = sample;
  adpcm.step_index = step_index;

  return sample - 2048;
}

u32 adpcm_read()
{
  u32 read_value = adpcm.read_latch;
  u32 read_address = adpcm.sample_read_address >> 1;
  u32 read_latch;

  if(adpcm.sample_read_address & 0x1)
  {
    read_latch = adpcm.sample_ram[read_address] >> 4;
    read_address = (read_address + 1) & 0xFFFF;
    read_latch |= (adpcm.sample_ram[read_address] & 0xF) << 4;
    adpcm.sample_read_address = (read_address << 1) + 1;
  }
  else
  {
    read_latch = adpcm.sample_ram[read_address];
    adpcm.sample_read_address = ((read_address + 1) & 0xFFFF) << 1;
  }

  adpcm.read_latch = read_latch;

  return read_value;
}

u32 adpcm_read_command()
{
  return adpcm.last_command;
}

u32 adpcm_dma_status()
{
  return adpcm.dma_enabled;
}

u32 adpcm_status()
{
  u32 status = 0;

  // TODO: Somehow implement read/write pending...
  if(adpcm.last_command & 0x20)
    status |= 0x8;
  else
    status |= 0x1;

  return status;
}

void adpcm_write_latch_high(u32 value)
{
  adpcm.latch_value = (adpcm.latch_value & 0xFF) | (value << 8);
}

void adpcm_write_latch_low(u32 value)
{
  adpcm.latch_value = (adpcm.latch_value & 0xFF00) | value;
}

void adpcm_playback_rate(u32 value)
{
  value &= 0xF;

  // TODO: Use a table for this
  // This is supposed to be (32087.5Hz / (16 - value)) /
  //  audio.output_frequency.
  // This is a value that's incremented until it spills over 1.
  // This translates into:
  // 32087.5 / (audio.output_frequency * (16 - value))
  // 64175 / (audio.output_frequency * 2 * (16 - value))
  // Then adjusted for the fixed point shift, with the * 2 taken out of it by
  // subtracting by 1.

  adpcm.frequency_step =
   ((u64)64175 << (u64)(step_fractional_bits_frequency - 1)) /
   (audio.output_frequency * (16 - value));
  adpcm.playback_rate = value;
}

void adpcm_set_state(adpcm_state_enum adpcm_state)
{
  switch(adpcm_state)
  {
    case ADPCM_STATE_NORMAL:
      cd_lower_event(CD_IRQ_ADPCM_PLAYBACK_HALF_DONE |
       CD_IRQ_ADPCM_PLAYBACK_DONE);
      break;

    case ADPCM_STATE_PLAYBACK_HALF_DONE:
      cd_lower_event(CD_IRQ_ADPCM_PLAYBACK_DONE);
      cd_raise_event(CD_IRQ_ADPCM_PLAYBACK_HALF_DONE);
      break;

    case ADPCM_STATE_PLAYBACK_DONE:
    case ADPCM_STATE_STOPPED:
      cd_lower_event(CD_IRQ_ADPCM_PLAYBACK_HALF_DONE);
      cd_raise_event(CD_IRQ_ADPCM_PLAYBACK_DONE);
      break;
  }
}

void adpcm_write_command(u32 value)
{
  adpcm_set_state(ADPCM_STATE_NORMAL);

  if((adpcm.last_command & 0x80) && !(value & 0x80))
  {
    adpcm.last_sample = 2048;
    adpcm.step_index = 0;
    adpcm.last_command = 0;
    adpcm.sample_length = 0;
    adpcm.sample_read_address = 0;
    adpcm.sample_write_address = 0;
  }

  if((value & 0x40) && !(adpcm.last_command & 0x40))
  {
    adpcm.last_sample = 2048;
    adpcm.step_index = 0;
  }

  if(value & 0x20)
  {
    if(value & 0x40)
      adpcm.sample_half_length = adpcm.sample_length_cached / 2;
    else
      adpcm.sample_half_length = adpcm.sample_length_cached;
  }

  if(value & 0x10)
  {
    adpcm.sample_length = adpcm.latch_value * 2;
    adpcm.sample_length_cached = adpcm.sample_length;
  }

  if(value & 0x8)
  {
    if(((adpcm.last_command & 0x4) == 0) && ((value & 0x1) == 0))
      adpcm.sample_read_address = ((adpcm.latch_value - 1) * 2) & 0x1FFFE;
    else
      adpcm.sample_read_address = (adpcm.latch_value * 2) & 0x1FFFE;
  }

  if(value & 0x2)
  {
    adpcm.sample_write_address = adpcm.latch_value;

    adpcm.shift_write = 0;

    if(((adpcm.last_command & 0x1) == 0) && ((value & 0x1) == 0))
      adpcm.sample_write_address = (adpcm.sample_write_address - 1) & 0xFFFF;
  }

  adpcm.last_command = value;
}

void adpcm_write_data(u32 value)
{
  u32 next_address = (adpcm.sample_write_address + 1) & 0xFFFF;

  if(0 && (adpcm.shift_write))
  {
    adpcm.sample_ram[adpcm.sample_write_address] =
     (adpcm.sample_ram[adpcm.sample_write_address] & 0xF) | value << 4;
    adpcm.sample_ram[next_address] =
     (adpcm.sample_ram[next_address] & 0xF0) | (value >> 4);
  }
  else
  {
    adpcm.sample_ram[adpcm.sample_write_address] = value;
  }

  adpcm.sample_write_address = next_address;

  adpcm.sample_length += 2;
  if(adpcm.sample_length >= adpcm.sample_half_length)
    adpcm_set_state(ADPCM_STATE_NORMAL);
}

void adpcm_enable_dma(u32 value)
{
  adpcm.dma_enabled = value;
}

void adpcm_full_volume()
{
  adpcm.volume = 438;
  adpcm.fade_cycles = 0;
}

void adpcm_fade_out(u32 ms)
{
  if(adpcm.volume)
  {
    adpcm.fade_cycle_interval =
     ((MASTER_CLOCK_RATE * ms) / (1000 * 438)) << step_fractional_bits_clock;
    adpcm.fade_cycles = adpcm.fade_cycle_interval;
  }
}

void initialize_adpcm()
{

}

void reset_adpcm()
{
  memset(adpcm.sample_ram, 0, 64 * 1024);
  adpcm.last_cycles = 0;
  adpcm.last_dma_cycles = 0;

  adpcm.sample_read_address = 0;
  adpcm.sample_write_address = 0;
  adpcm.sample_length = 0;
  adpcm.sample_length_cached = 0;
  adpcm.sample_half_length = 0;

  adpcm.last_sample = 0;
  adpcm.last_sample_adjusted = 0;
  adpcm.step_index = 0;
  adpcm.latch_value = 0;
  adpcm.last_command = 0;
  adpcm.read_latch = 0;

  adpcm.sample_index_fractional = 0;

  adpcm.dma_enabled = 0;
  adpcm.audio_buffer_index = 0;
  adpcm.frequency_step = 0;

  adpcm.fade_cycle_interval = 0;
  adpcm.fade_cycles = 0;

  adpcm_playback_rate(0);
  adpcm_full_volume();
}

void update_adpcm_dma()
{
  s32 clock_delta = cpu.global_cycles - adpcm.last_dma_cycles;

  adpcm.last_dma_cycles += clock_delta;

  if((adpcm.dma_enabled & 0x3) &&
   cd.req_signal && !cd.ack_signal &&
   (cd.scsi_bus_state == SCSI_STATE_DATA_IN))
  {
    while(clock_delta >= 0)
    {
      clock_delta -= 30;

      adpcm.sample_ram[adpcm.sample_write_address] = cd.data_bus;
      adpcm.sample_write_address = (adpcm.sample_write_address + 1) &
       0xFFFF;

      cd.req_signal = 0;
      cd_data_buffer_read();

      if((cd.req_signal == 0) || (cd.scsi_bus_state == SCSI_STATE_STATUS))
      {
        adpcm.dma_enabled &= ~0x1;
        break;
      }

      adpcm.sample_length += 2;
      if(adpcm.sample_length >= adpcm.sample_half_length)
        adpcm_set_state(ADPCM_STATE_NORMAL);
    }

    adpcm.last_dma_cycles -= clock_delta;
  }
}

void update_adpcm()
{
  u32 clock_delta =
   (cpu.global_cycles << step_fractional_bits_clock) - adpcm.last_cycles;

  if(clock_delta < 0)
    return;

  s32 clocks_remaining = clock_delta;
  s32 samples_remaining = adpcm.sample_length;
  u32 frequency_step = adpcm.frequency_step;

  u32 sample_index = adpcm.sample_read_address;
  u32 sample_index_fractional = adpcm.sample_index_fractional;

  u32 audio_buffer_index = adpcm.audio_buffer_index;
  s32 *audio_buffer = audio.buffer;
  u8 *sample_ram = adpcm.sample_ram;
  u32 current_adpcm_code;
  s32 current_sample;
  s32 last_sample = adpcm.last_sample_adjusted;
  s32 dest_sample;
  s32 sample_delta;

  if(adpcm.last_command & 0x20)
  {
    while((samples_remaining > 0) && (clocks_remaining >= 0))
    {
      sample_index = (sample_index + 1) & 0x1FFFF;

      // Decode ADPCM sample:
      // First, get the 8bits where the sample is.

      current_adpcm_code =
       sample_ram[sample_index >> 1];

      if(sample_index & 1)
        current_adpcm_code &= 0xF;
      else
        current_adpcm_code >>= 4;

      current_sample =
       adpcm_convert_sample(current_adpcm_code) * adpcm.volume;

      sample_delta = current_sample - last_sample;

      // Resample + output ADPCM sample

      while((sample_index_fractional &
       (0x7F << step_fractional_bits_frequency)) == 0)
      {
        // Linear interpolation, should sound noticably better without
        // taking a big speed hit.
        dest_sample = last_sample +
         ((sample_delta * sample_index_fractional) >>
         step_fractional_bits_frequency);

        // Use this for nearest neighbor resampling
        //dest_sample = current_sample;

        audio_buffer[audio_buffer_index] += dest_sample;
        audio_buffer[audio_buffer_index + 1] += dest_sample;

        sample_index_fractional += frequency_step;
        audio_buffer_index =
         (audio_buffer_index + 2) % AUDIO_BUFFER_SIZE;

        clocks_remaining -= psg.clock_step;
      }

      last_sample = current_sample;

      samples_remaining--;
      sample_index_fractional &= ((1 << step_fractional_bits_frequency) - 1);
    }

    adpcm.sample_length = samples_remaining;
    adpcm.sample_read_address = sample_index;
    adpcm.sample_index_fractional = sample_index_fractional;
    adpcm.last_sample_adjusted = last_sample;

    if(samples_remaining == 0)
    {
      adpcm.last_command &= ~(0x20 | 0x40);
      adpcm_set_state(ADPCM_STATE_PLAYBACK_DONE);
    }
    else

    if(samples_remaining < adpcm.sample_half_length)
      adpcm_set_state(ADPCM_STATE_PLAYBACK_HALF_DONE);

    adpcm.sample_read_address = sample_index;
    adpcm.sample_index_fractional = sample_index_fractional;
  }

  while(clocks_remaining >= 0)
  {
    audio_buffer_index =
     (audio_buffer_index + 2) % AUDIO_BUFFER_SIZE;

    clocks_remaining -= psg.clock_step;
  }

  adpcm.sample_length = samples_remaining;
  adpcm.audio_buffer_index = audio_buffer_index;

  clock_delta -= clocks_remaining;
  adpcm.last_cycles += clock_delta;

  if(adpcm.fade_cycles)
  {
    adpcm.fade_cycles -= clock_delta;
    if(adpcm.fade_cycles <= 0)
    {
      adpcm.volume--;
      if(adpcm.volume == 0)
      {
        adpcm.volume = 0;
        adpcm.fade_cycles = 0;
      }
      else
      {
        adpcm.fade_cycles += adpcm.fade_cycle_interval;
      }
    }
  }
}

#define adpcm_savestate_extra_load()                                          \
  adpcm_playback_rate(adpcm.playback_rate)                                    \

#define adpcm_savestate_extra_store()                                         \

#define adpcm_savestate_builder(type, type_b, version_gate)                   \
void adpcm_##type_b##_savestate(savestate_##type_b##_type savestate_file)     \
{                                                                             \
  file_##type##_array(savestate_file, adpcm.sample_ram);                      \
  file_##type##_variable(savestate_file, adpcm.last_cycles);                  \
  file_##type##_variable(savestate_file, adpcm.last_dma_cycles);              \
  file_##type##_variable(savestate_file, adpcm.sample_read_address);          \
  file_##type##_variable(savestate_file, adpcm.sample_write_address);         \
  file_##type##_variable(savestate_file, adpcm.sample_length);                \
  file_##type##_variable(savestate_file, adpcm.sample_length_cached);         \
  file_##type##_variable(savestate_file, adpcm.sample_half_length);           \
                                                                              \
  file_##type##_variable(savestate_file, adpcm.last_sample);                  \
  file_##type##_variable(savestate_file, adpcm.last_sample_adjusted);         \
  file_##type##_variable(savestate_file, adpcm.step_index);                   \
  file_##type##_variable(savestate_file, adpcm.latch_value);                  \
  file_##type##_variable(savestate_file, adpcm.last_command);                 \
  file_##type##_variable(savestate_file, adpcm.read_latch);                   \
                                                                              \
  file_##type##_variable(savestate_file, adpcm.sample_index_fractional);      \
                                                                              \
  file_##type##_variable(savestate_file, adpcm.playback_rate);                \
  file_##type##_variable(savestate_file, adpcm.dma_enabled);                  \
  file_##type##_variable(savestate_file, adpcm.audio_buffer_index);           \
                                                                              \
  file_##type##_variable(savestate_file, adpcm.volume);                       \
  file_##type##_variable(savestate_file, adpcm.fade_cycle_interval);          \
  file_##type##_variable(savestate_file, adpcm.fade_cycles);                  \
                                                                              \
  adpcm_savestate_extra_##type_b();                                           \
}                                                                             \

build_savestate_functions(adpcm);

