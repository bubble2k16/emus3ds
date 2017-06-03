#ifndef AUDIO_H
#define AUDIO_H

#define _sound_copy(source_offset, length, render_type, shift)                \
{                                                                             \
  _length = (length) / 2;                                                     \
  source = audio.buffer + source_offset;                                      \
  for(i = 0; i < _length; i++)                                                \
  {                                                                           \
    current_sample = source[i] >> shift;                                      \
    if(current_sample > 32767)                                                \
      current_sample = 32767;                                                 \
    if(current_sample < -32768)                                               \
      current_sample = -32768;                                                \
    stream_base[i] = current_sample;                                          \
    source[i] = 0;                                                            \
  }                                                                           \
}                                                                             \

#define sound_copy(source_offset, length, render_type)                        \
  if(config.sound_double)                                                     \
  {                                                                           \
    _sound_copy(source_offset, length, render_type, 4);                       \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    _sound_copy(source_offset, length, render_type, 5);                       \
  }                                                                           \

#define AUDIO_BUFFER_SIZE (1024 * 64)

// This is how to use it: write to buffer using buffer_index with the primary
// audio source (the one you want the callback to wait on), then increment
// buffer index by the amount written. Do this all inside audio_lock and
// audio_unlock pairs.

typedef struct
{
  s32 buffer[AUDIO_BUFFER_SIZE];
  u32 buffer_base;
  u32 buffer_index;

  u32 output_frequency;
  u32 playback_buffer_size;

  u32 pause_state;
} audio_struct;

extern audio_struct audio;


#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void initialize_audio();
void audio_sync_start();
void audio_sync_end();
void audio_exit();

void audio_signal_callback();
void audio_unstall_callback();
void audio_wait_callback();
void audio_reset_buffer();

void audio_lock();
void audio_unlock();

u32 audio_pause();
void audio_unpause();
void audio_revert_pause_state();

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif

