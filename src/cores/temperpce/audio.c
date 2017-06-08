#include "common.h"

audio_struct audio;

void audio_sync_start()
{
  audio_signal_callback();
  audio_lock();
  audio_wait_callback();
}

void audio_sync_end()
{
  audio_signal_callback();
  audio_unlock();
}

void audio_unstall_callback()
{
  audio.buffer_index = AUDIO_BUFFER_SIZE - 1;
  audio.buffer_base = 0;
  audio_signal_callback();
}

void audio_reset_buffer()
{
  audio.buffer_index = 0;
  audio.buffer_base = 0;
  audio_signal_callback();
}

void audio_revert_pause_state(u32 pause_state)
{
  if(pause_state == 0)
    audio_unpause();
}

