#include "common.h"

#ifdef GP2X_OR_WIZ_BUILD

cd_struct cd __attribute__ ((aligned(8192)));

#else

cd_struct cd;

#endif

scsi_command_struct scsi_commands[256];

#define u8_to_bcd(value)                                                      \
  ((value % 10) + ((value / 10) * 16))                                        \

#define bcd_to_u8(value)                                                      \
  (((value >> 4) * 10) + (value & 0xF))                                       \

#define sector_offset_to_msf(offset, minutes, seconds, frames)                \
  minutes = (offset / 75) / 60;                                               \
  seconds = (offset / 75) % 60;                                               \
  frames = offset % 75;                                                       \
                                                                              \
  minutes = u8_to_bcd(minutes);                                               \
  seconds = u8_to_bcd(seconds);                                               \
  frames = u8_to_bcd(frames)                                                  \

#define data_buffer_queue(source, size)                                       \
  if((cd.data_buffer_top + size) > DATA_BUFFER_SIZE)                          \
  {                                                                           \
    memcpy(cd.data_buffer + cd.data_buffer_top, source,                       \
     DATA_BUFFER_SIZE - cd.data_buffer_top);                                  \
    memcpy(cd.data_buffer, source + DATA_BUFFER_SIZE - cd.data_buffer_top,    \
     cd.data_buffer_top + size - DATA_BUFFER_SIZE);                           \
    cd.data_buffer_top = cd.data_buffer_top + size - DATA_BUFFER_SIZE;        \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    memcpy(cd.data_buffer + cd.data_buffer_top, source, size);                \
    cd.data_buffer_top += size;                                               \
  }                                                                           \
  cd.data_buffer_bytes_written += size                                        \

#define data_buffer_dequeue_one(dest)                                         \
  dest = cd.data_buffer[cd.data_buffer_position];                             \
  cd.data_buffer_position = (cd.data_buffer_position + 1) %                   \
   DATA_BUFFER_SIZE;                                                          \
  cd.data_buffer_bytes_written--                                              \

#define data_buffer_empty()                                                   \
  (cd.data_buffer_bytes_written == 0)                                         \

#define data_buffer_capacity()                                                \
  (DATA_BUFFER_SIZE - cd.data_buffer_bytes_written)                           \

#define data_buffer_flush()                                                   \
  cd.data_buffer_position = 0;                                                \
  cd.data_buffer_top = 0;                                                     \
  cd.data_buffer_bytes_written = 0                                            \


void cd_raise_event(u32 event)
{
  cd.irq_status |= event;

  if(cd.irq_status & cd.irq_enable)
    raise_interrupt(IRQ_CD);
}

void cd_lower_event(u32 event)
{
  cd.irq_status &= ~event;

  if((cd.irq_status & cd.irq_enable) == 0)
    irq.status &= ~IRQ_CD;
}

void cd_set_status(u32 status)
{
  cd.scsi_bus_state = SCSI_STATE_STATUS;
  cd.data_bus = status;
  cd.message = 0;
  cd.message_sent = 0;
  cd.status_sent = 0;
  cd.req_signal = 1;

  if(cd.ack_signal)
  {
    cd.status_sent = 1;
    cd.req_signal = 0;
  }
}

// This is very very very rough and really won't come even close to
// modeling the real CD-ROM access time. But I hope that it's better
// than simple using a flat average constantly.

u32 access_factors[256];

void initialize_access_factors()
{
  u32 i;

  for(i = 0; i < 256; i++)
  {
    access_factors[i] = 357.60892 +
     (665.16876043591105 * log(i + 1));
  }
}

u32 cd_access_time(u32 sector_a, u32 sector_b)
{
  // ~0.125ms per ring delta + ~100ms latency
  // ring is approximated as sector / 12
  u32 ring_a = sector_a / access_factors[sector_a >> 11];
  u32 ring_b = sector_b / access_factors[sector_b >> 11];

  s32 ring_delta = abs(ring_b - ring_a);

  if((sector_a - sector_b) == 1)
    return 1;

  return ((MASTER_CLOCK_RATE * 875 * ring_delta / (6991300 / 256))) +
   (MASTER_CLOCK_RATE * 100 / 1000);
}

void scsi_command_test_unit_ready()
{
  cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
}

void scsi_command_request_sense()
{
  u8 results[18];

  memset(results, 0, 18);

  results[0] = 0x70;
  results[2] = cd.sense_key;
  results[12] = cd.additional_sense_code;
  results[13] = cd.additional_sense_code_qualifier;
  results[14] = cd.field_replaceable_unit_code;

  cd.sense_key = SENSE_KEY_NO_SENSE;
  cd.additional_sense_code = 0;
  cd.additional_sense_code_qualifier = 0;
  cd.field_replaceable_unit_code = 0;

  data_buffer_queue(results, 18);
  cd.data_transfer_done = 1;
  cd.scsi_bus_state = SCSI_STATE_DATA_IN;

  data_buffer_dequeue_one(cd.data_bus);
  cd.req_signal = 1;
}

void scsi_command_read()
{
  cd.read_sector_count = cd.command_buffer[4];

  if(cd.read_sector_count)
  {
    cd.read_sector_address = ((cd.command_buffer[1] & 0x1F) << 16) |
     (cd.command_buffer[2] << 8) | (cd.command_buffer[3]);

    cd.read_cycles = cd.read_sector_cycle_interval +
     cd_access_time(cd.read_sector_address, cd.last_sector_address);
    cd.data_transfer_done = 0;
  }
  else
  {
    cd.read_cycles = 0;
    cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
  }

  cd.cdda_status = CDDA_STATUS_STOPPED;
}

s32 get_playback_offset(u32 *track_format)
{
  switch(cd.command_buffer[9] >> 6)
  {
    default:
    case 0x0:
      *track_format = 0;
      return (cd.command_buffer[3] << 16) | (cd.command_buffer[4] << 8) |
       cd.command_buffer[5];

    case 0x1:
      *track_format = 0;
      return (60 * 75 * (bcd_to_u8(cd.command_buffer[2]))) +
       (75 * (bcd_to_u8(cd.command_buffer[3]))) +
       bcd_to_u8(cd.command_buffer[4]) - 150;

    case 0x2:
    {
      u32 track_number = bcd_to_u8(cd.command_buffer[2]);
      *track_format = bin_cue_get_track_format(track_number);
      return bin_cue_get_track_offset(track_number);
    }
  }
}

void scsi_command_set_audio_start_pos()
{
  u32 track_format;
  s32 start_sector_address = get_playback_offset(&track_format);

  // This hack is for Steam Hearts. Need to find out how it's working
  // on the actual hardware.

  if(((u32)(cpu.global_cycles - cd.cdda_start_last_cycles) > 0x3d0000) ||
  (cd.cdda_status != CDDA_STATUS_PLAYING) ||
   (cd.cdda_last_start_pos != start_sector_address))
  {
    if((start_sector_address >= 0) && (track_format == 0))
    {
      cd.cdda_start_sector_address = start_sector_address;
      cd.cdda_sector_address = start_sector_address;

      cd.cdda_play_mode = cd.command_buffer[1];

      if(cd.cdda_play_mode)
        cd.cdda_status = CDDA_STATUS_PLAYING;
      else
        cd.cdda_status = CDDA_STATUS_PAUSED;

      cd.cdda_cycles = 0;
      cd.cdda_read_offset = 2352 / 2;

      cd.cdda_access_cycles =
       cd_access_time(cd.cdda_start_sector_address, cd.last_sector_address);

    }
    else
    {
      cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
    }
  }
  else
  {
    cd.cdda_access_cycles = 0;
  }

  cd.cdda_last_start_pos = start_sector_address;
  cd.cdda_start_last_cycles = cpu.global_cycles;

  // Bug fix: Why was this not here?
  // Without this, all games that play CD audio freezes entirely.
  //
  cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
}

void scsi_command_set_audio_end_pos()
{
  u32 track_format;
  s32 end_sector_address = get_playback_offset(&track_format);

  if(end_sector_address >= 0)
  {
    cd.cdda_end_sector_address = end_sector_address;

    cd.cdda_play_mode = cd.command_buffer[1];

    if(cd.cdda_play_mode)
      cd.cdda_status = CDDA_STATUS_PLAYING;
    else
      cd.cdda_status = CDDA_STATUS_PAUSED;
  }
  else
  {
    cd.cdda_status = CDDA_STATUS_STOPPED;
  }

  cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
}

void scsi_command_pause_audio()
{
  if(cd.cdda_status != CDDA_STATUS_STOPPED)
  {
    cd.cdda_status = CDDA_STATUS_PAUSED;
    cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
  }
  else
  {
    // TODO: Set error codes
    cd_set_status(SCSI_STATUS_MESSAGE_CHECK_CONDITION);
  }
}

void scsi_command_read_subchannel_q()
{
  u8 results[10];
  u32 track_number, index_number, relative_offset;
  u32 minutes, seconds, frames;
  u32 lba_offset;

  switch(cd.cdda_status)
  {
    case CDDA_STATUS_PAUSED:
      results[0] = 2;
      break;

    case CDDA_STATUS_PLAYING:
      results[0] = 0;
      break;

    default:
      results[0] = 3;
      break;
  }

  bin_cue_get_current_track(cd.last_sector_address,
   &track_number, &index_number, &relative_offset);

  track_number += 1;

  results[1] = 0;
  results[2] = u8_to_bcd(track_number);
  results[3] = u8_to_bcd(index_number);

  sector_offset_to_msf(relative_offset, minutes, seconds, frames);

  results[4] = minutes;
  results[5] = seconds;
  results[6] = frames;

  lba_offset = cd.cdda_sector_address + 150;
  sector_offset_to_msf(lba_offset, minutes, seconds, frames);

  results[7] = minutes;
  results[8] = seconds;
  results[9] = frames;

/*  printf("subq: %x %x, %x %x %x, %x %x %x\n", results[2], results[3],
   results[4], results[5], results[6], results[7], results[8], results[9]); */

  data_buffer_queue(results, 10);
  data_buffer_dequeue_one(cd.data_bus);

  cd.scsi_bus_state = SCSI_STATE_DATA_IN;
  cd.data_transfer_done = 1;
  cd.req_signal = 1;
}

void scsi_command_get_directory_info()
{
  switch(cd.command_buffer[1])
  {
    // Return first and last tracks.
    case 0x0:
    {
      u8 results[2];
      results[0] = u8_to_bcd(bin_cue_get_first_track());
      results[1] = u8_to_bcd(bin_cue_get_last_track());
      data_buffer_queue(results, 2);

      break;
    }

    // Return sector count
    case 0x1:
    {
      u8 results[3];
      u32 sector_count = bin_cue_get_sector_count();
      u32 minutes, seconds, frames;

      sector_offset_to_msf(sector_count, minutes, seconds, frames);

      results[0] = minutes;
      results[1] = seconds;
      results[2] = frames;
      data_buffer_queue(results, 3);

      break;
    }

    // Return track start position
    case 0x2:
    {
      u8 results[4];
      u32 track_number = bcd_to_u8(cd.command_buffer[2]);
      u32 track_offset;
      u32 track_format;
      u32 minutes, seconds, frames;

      if(track_number == 0)
        track_number = 1;

      if(track_number == 0xAA)
        track_number = bin_cue_get_last_track();

      track_offset = bin_cue_get_track_offset(track_number) + 150;
      track_format = bin_cue_get_track_format(track_number);

      sector_offset_to_msf(track_offset, minutes, seconds, frames);

      results[0] = minutes;
      results[1] = seconds;
      results[2] = frames;
      results[3] = track_format;
      data_buffer_queue(results, 4);

      break;
    }
  }

  cd.scsi_bus_state = SCSI_STATE_DATA_IN;
  data_buffer_dequeue_one(cd.data_bus);
  cd.data_transfer_done = 1;
  cd.req_signal = 1;
}


void cd_process_ack_high()
{
  if(cd.req_signal)
  {
    switch(cd.scsi_bus_state)
    {
      case SCSI_STATE_BUS_FREE:
        break;

      case SCSI_STATE_DATA_IN:
        break;

      case SCSI_STATE_COMMAND:
        // Place data bus byte on the command buffer.
        cd.command_buffer[cd.command_buffer_position] = cd.data_bus;
        cd.command_buffer_position++;
        break;

      case SCSI_STATE_STATUS:
        cd.status_sent = 1;
        break;

      case SCSI_STATE_MESSAGE_IN:
        cd.message_sent = 1;
        break;
    }

    cd.req_signal = 0;
  }
}

void cd_data_buffer_read()
{
  if(data_buffer_empty())
  {
    cd_lower_event(CD_IRQ_DATA_TRANSFER_READY);

    if(cd.data_transfer_done)
    {
      cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
      cd.data_transfer_done = 0;
      cd_raise_event(CD_IRQ_DATA_TRANSFER_DONE);
    }
  }
  else
  {
    data_buffer_dequeue_one(cd.data_bus);
    cd.req_signal = 1;
  }
}


void cd_process_ack_low()
{
  if(!cd.req_signal)
  {
    switch(cd.scsi_bus_state)
    {
      case SCSI_STATE_BUS_FREE:
        break;

      case SCSI_STATE_COMMAND:
      {
        if(cd.command_buffer_position)
        {
          // Process command
          scsi_command_struct *scsi_command = scsi_commands +
           cd.command_buffer[0];

          if(scsi_command->command_function)
          {
            if(scsi_command->argument_count == cd.command_buffer_position)
            {
              /*printf("command %x: %x %x %x %x %x %x %x %x %x\n",
               cd.command_buffer[0], cd.command_buffer[1],
               cd.command_buffer[2], cd.command_buffer[3],
               cd.command_buffer[4], cd.command_buffer[5],
               cd.command_buffer[6], cd.command_buffer[7],
               cd.command_buffer[8], cd.command_buffer[9]);*/
              scsi_command->command_function();
              cd.command_buffer_position = 0;
            }
            else
            {
              cd.req_signal = 1;
            }
          }
          else
          {
            cd.sense_key = SENSE_KEY_ILLEGAL_REQUEST;
            cd.additional_sense_code = 0x20;
            cd.additional_sense_code_qualifier = 0x00;
            cd.field_replaceable_unit_code = 0x00;
            cd_set_status(SCSI_STATUS_MESSAGE_CHECK_CONDITION);

            cd.command_buffer_position = 0;
          }
        }
        break;
      }

      case SCSI_STATE_DATA_IN:
        // Pull byte from the data FIFO to the data bus
        cd_data_buffer_read();
        break;

      case SCSI_STATE_STATUS:
        // Place the message on the data bus and enter MESSAGE IN
        if(cd.status_sent)
        {
          cd.data_bus = cd.message;
          cd.scsi_bus_state = SCSI_STATE_MESSAGE_IN;
          cd.req_signal = 1;
          cd.status_sent = 0;
        }
        break;

      case SCSI_STATE_MESSAGE_IN:
        // If the message has been sent, enter BUS FREE
        if(cd.message_sent)
        {
          cd.scsi_bus_state = SCSI_STATE_BUS_FREE;
          cd_lower_event(CD_IRQ_DATA_TRANSFER_DONE);
          cd.message_sent = 0;
        }
        break;
    }
  }
}


u32 prev_bus_state = 0xffff;

u32 cd_bus_status()
{
  // The state determines the status bits:
  // hurr GCC is too stupid to realize this won't be unused.
  u32 status = 0;

  // debugging only
  /*if (prev_bus_state != cd.scsi_bus_state)
  {
    printf ("cd state: %x\n", cd.scsi_bus_state);
    prev_bus_state = cd.scsi_bus_state;
  }*/

  switch(cd.scsi_bus_state)
  {
    // all bits low
    case SCSI_STATE_BUS_FREE:
      break;

    // BSY: high, MSG: low, C/D: high, I/O: low
    case SCSI_STATE_COMMAND:
      status = SCSI_STATUS_BIT_BSY | SCSI_STATUS_BIT_CD;
      break;

    // BSY: high, MSG: low, C/D: low, I/O: high
    case SCSI_STATE_DATA_IN:
      status = SCSI_STATUS_BIT_BSY | SCSI_STATUS_BIT_IO;
      break;

    // BSY: high, MSG: low, C/D: high, I/O: high
    case SCSI_STATE_STATUS:
      status = SCSI_STATUS_BIT_BSY | SCSI_STATUS_BIT_CD | SCSI_STATUS_BIT_IO;
      break;

    // BSY: high, MSG: high, C/D: high, I/O: high
    case SCSI_STATE_MESSAGE_IN:
      status = SCSI_STATUS_BIT_BSY | SCSI_STATUS_BIT_MSG |
       SCSI_STATUS_BIT_CD | SCSI_STATUS_BIT_IO;
      break;
  }

  if(cd.req_signal)
    status |= SCSI_STATUS_BIT_REQ;

  return status;
}

u32 cd_read_data()
{
  return cd.data_bus;
}

u32 cd_read_data_acknowledge()
{
  u32 read_value = cd.data_bus;

  if(cd.req_signal && !cd.ack_signal &&
   (cd.scsi_bus_state == SCSI_STATE_DATA_IN))
  {
    // There should be 15 cycles between these but I'll be damned if I'm
    // going to emulate them at this point.

    cd.req_signal = 0;
    cd_data_buffer_read();
  }

  return read_value;
}

u32 cd_read_irq_enable()
{
  return cd.irq_enable;
}

u32 cd_irq_status()
{
  u32 irq_status = cd.irq_status;
  cd.bram_enable = 0;
  memory_remap_read_write_null(0xF7, 1);
  cd.irq_status ^= 0x2;
  return irq_status;
}

u32 cd_read_reset()
{
  return 0;
}

u32 cd_read_cdda_low()
{
  if(cd.irq_enable & 0x2)
  {
    return (cd.cdda_read_right & 0xFF);
  }
  else
  {
    return (cd.cdda_read_left & 0xFF);
  }
}

u32 cd_read_cdda_high()
{
  if(cd.irq_enable & 0x2)
  {
    return (cd.cdda_read_right >> 8);
  }
  else
  {
    return (cd.cdda_read_left >> 8);
  }
}

u32 cd_read_sub_channel()
{
  return cd.sub_channel_data;
}


void cd_select(u32 value)
{
  if(cd.scsi_bus_state == SCSI_STATE_BUS_FREE)
  {
    cd.scsi_bus_state = SCSI_STATE_COMMAND;
    cd.req_signal = 1;
  }

  cd_lower_event(CD_IRQ_DATA_TRANSFER_READY |
   CD_IRQ_DATA_TRANSFER_DONE | CD_IRQ_SUBCHANNEL_READ);
}

void cd_write_data(u32 value)
{
  cd.data_bus = value;

  // Apparently if it writes out 0x81 during command phase,
  // even if req/ack are low and command_bufer_position is 0
  // it causes a reset.

  if((value == 0x81) && (cd.scsi_bus_state == SCSI_STATE_COMMAND) &&
   (cd.command_buffer_position == 0))
  {
    cd.scsi_bus_state = SCSI_STATE_BUS_FREE;
    cd.req_signal = 0;
    cd.ack_signal = 0;

    cd.command_buffer_position = 0;
    cd.data_buffer_position = 0;
    cd.data_buffer_top = 0;
    cd.data_buffer_bytes_written = 0;

    cd.data_bus = 0;
  }
}

void cd_acknowledge(u32 value)
{
  cd.ack_signal = value >> 7;

  // Process ACK
  if(cd.ack_signal)
    cd_process_ack_high();
  else
    cd_process_ack_low();

  cd.irq_enable = value & ~0x80;

  if((cd.irq_status & cd.irq_enable) == 0)
  {
    // Cancel anything raised
    irq.status &= ~IRQ_CD;
    cpu.irq_raised = 0;
  }
  else
  {
    raise_interrupt(IRQ_CD);
  }
}


// TODO: See if this needs to be more accurate for anything.

void cd_prepare_cdda_read(u32 value)
{
  cd.cdda_read_left =
   (cd.cdda_cache[cd.cdda_read_offset] * cd.cdda_volume) >> 5;
  cd.cdda_read_right =
   (cd.cdda_cache[cd.cdda_read_offset + 1] * cd.cdda_volume) >> 5;
}

void cd_enable_bram(u32 value)
{
  if(value & 0x80)
  {
    cd.bram_enable = 1;
    memory_remap_read_write(0xF7, 1, cd.bram, cd.bram);
  }
}

void cd_full_volume()
{
  cd.cdda_volume = (1 << 5);
  cd.cdda_fade_cycles = 0;
}

void cd_fade_out_rate(u32 ms)
{
  if(cd.cdda_volume)
  {
    cd.cdda_fade_cycle_interval = (MASTER_CLOCK_RATE * ms) / (1000 * 32);
    cd.cdda_fade_cycles = cd.cdda_fade_cycle_interval;
  }
}

void cd_fadeout(u32 value)
{
  switch(value & 0xF)
  {
    // Set ADPCM/CD to full volume
    case 0x0:
      cd_full_volume();
      adpcm_full_volume();
      break;

    // Fade CD volume out slowly (6s intervals)
    case 0x8:
    case 0x9:
      cd_fade_out_rate(6500);
      break;

    // Fade ADPCM out slowly (6s intervals)
    case 0xA:
      adpcm_fade_out(6000);
      break;

    // Fade CD out quickly (2.5s intervals), ADPCM full volume
    case 0xC:
    case 0xD:
      cd_fade_out_rate(2500);
      adpcm_full_volume();
      break;

    // Fade ADPCM out quickly (2.5s intervals)
    case 0xE:
      adpcm_fade_out(2500);
      break;
  }
}

void cd_write_reset(u32 value)
{
  if(value & 0x2)
  {
    cd.scsi_bus_state = SCSI_STATE_BUS_FREE;
    cd.req_signal = 0;
    cd.ack_signal = 0;

    cd.command_buffer_position = 0;
    cd.data_buffer_position = 0;
    cd.data_buffer_top = 0;
    cd.data_buffer_bytes_written = 0;

    cd.data_bus = 0;

    cd.read_sector_address = 0;
    cd.read_sector_count = 0;

    cd.read_cycles = 0;

    cd.irq_status = 0;
    cd.irq_enable = 0;

    cd.cdda_cycles = 0;
    cd.cdda_status = CDDA_STATUS_STOPPED;

    cd.cdda_sector_address = 0;
    cd.cdda_start_sector_address = 0;
    cd.cdda_end_sector_address = 0xFFFFFFFF;

    cd.message = 0;
    cd.status_sent = 0;
    cd.message_sent = 0;
    cd.data_transfer_done = 0;

    cd.sense_key = SENSE_KEY_NO_SENSE;
    cd.additional_sense_code = 0;
    cd.additional_sense_code_qualifier = 0;
    cd.field_replaceable_unit_code = 0;

    cd_full_volume();
  }
}

void initialize_cd()
{
  u32 i;

  for(i = 0; i < 256; i++)
  {
    scsi_commands[i].command_function = NULL;
  }

  scsi_commands[SCSI_COMMAND_TEST_UNIT_READY].argument_count = 1;
  scsi_commands[SCSI_COMMAND_TEST_UNIT_READY].command_function =
   scsi_command_test_unit_ready;

  scsi_commands[SCSI_COMMAND_REQUEST_SENSE].argument_count = 6;
  scsi_commands[SCSI_COMMAND_REQUEST_SENSE].command_function =
   scsi_command_request_sense;

  scsi_commands[SCSI_COMMAND_READ].argument_count = 6;
  scsi_commands[SCSI_COMMAND_READ].command_function =
   scsi_command_read;

  scsi_commands[SCSI_COMMAND_SET_AUDIO_START_POS].argument_count = 10;
  scsi_commands[SCSI_COMMAND_SET_AUDIO_START_POS].command_function =
   scsi_command_set_audio_start_pos;

  scsi_commands[SCSI_COMMAND_SET_AUDIO_END_POS].argument_count = 10;
  scsi_commands[SCSI_COMMAND_SET_AUDIO_END_POS].command_function =
   scsi_command_set_audio_end_pos;

  scsi_commands[SCSI_COMMAND_PAUSE_AUDIO].argument_count = 10;
  scsi_commands[SCSI_COMMAND_PAUSE_AUDIO].command_function =
   scsi_command_pause_audio;

  scsi_commands[SCSI_COMMAND_READ_SUBCHANNEL_Q].argument_count = 10;
  scsi_commands[SCSI_COMMAND_READ_SUBCHANNEL_Q].command_function =
   scsi_command_read_subchannel_q;

  scsi_commands[SCSI_COMMAND_GET_DIRECTORY_INFO].argument_count = 10;
  scsi_commands[SCSI_COMMAND_GET_DIRECTORY_INFO].command_function =
   scsi_command_get_directory_info;

  // Convert from bytes per second to cycles:
  // (cycles per second) * bytes / (bytes per second) = cycles
  cd.read_sector_cycle_interval = (MASTER_CLOCK_RATE * 2048) / 153600;
  cd.cdda_sector_cycle_interval =
   ((MASTER_CLOCK_RATE * (2352 / 4)) << step_fractional_bits_clock) / 44100;

  // For now, this is about 800ms
  cd.access_sector_cycle_interval = (u32)(MASTER_CLOCK_RATE * 0.8);

  initialize_access_factors();
}

void reset_cd()
{
  memset(cd.ext_ram, 0, 1024 * 192);
  memset(cd.work_ram, 0, 1024 * 64);

  memset(cd.data_buffer, 0, DATA_BUFFER_SIZE);
  memset(cd.cdda_cache, 0, 2352);

  memset(cd.command_buffer, 0, 16);

  cd.last_read_cycles = 0;
  cd.last_cdda_cycles = 0;

  cd.scsi_bus_state = SCSI_STATE_BUS_FREE;
  cd.req_signal = 0;
  cd.ack_signal = 0;

  cd.command_buffer_position = 0;
  cd.data_buffer_position = 0;
  cd.data_buffer_top = 0;
  cd.data_buffer_bytes_written = 0;

  cd.data_bus = 0;

  cd.read_sector_address = 0;
  cd.read_sector_count = 0;

  cd.read_cycles = 0;

  cd.irq_status = 0;
  cd.irq_enable = 0;

  cd.cdda_cycles = 0;
  cd.cdda_status = CDDA_STATUS_STOPPED;
  cd.bram_enable = 0;

  cd.cdda_sector_address = 0;
  cd.cdda_start_sector_address = 0;
  cd.cdda_end_sector_address = 0xFFFFFFFF;

  cd.cdda_start_last_cycles = 0xFFFFFFFF;
  cd.cdda_read_offset = 0;

  cd.message = 0;
  cd.status_sent = 0;
  cd.message_sent = 0;
  cd.data_transfer_done = 0;

  cd.sense_key = SENSE_KEY_NO_SENSE;
  cd.additional_sense_code = 0;
  cd.additional_sense_code_qualifier = 0;
  cd.field_replaceable_unit_code = 0;

  cd_full_volume();
}

void update_cd_read()
{
  u32 clock_delta = cpu.global_cycles - cd.last_read_cycles;

  if(cd.read_cycles)
  {
    cd.read_cycles -= clock_delta;

    if(cd.read_cycles <= 0)
    {
      cd.last_sector_type = CD_SECTOR_LOAD_DATA;
      cd.last_sector_address = cd.read_sector_address;

      if(data_buffer_capacity() < 2048)
      {
        cd.read_cycles += cd.read_sector_cycle_interval;
      }
      else
      {
        u8 read_buffer[2048];

        bin_cue_read_sector_data(read_buffer, cd.read_sector_address);
        data_buffer_queue(read_buffer, 2048);

        cd.read_sector_address++;
        cd.read_sector_count--;

        cd_raise_event(CD_IRQ_DATA_TRANSFER_READY);

        if(cd.read_sector_count)
        {
          cd.read_cycles += cd.read_sector_cycle_interval;
        }
        else
        {
          cd.read_cycles = 0;
          cd.data_transfer_done = 1;
        }

        cd.scsi_bus_state = SCSI_STATE_DATA_IN;

        if((cd.req_signal == 0) && (cd.ack_signal == 0))
        {
          data_buffer_dequeue_one(cd.data_bus);
          cd.req_signal = 1;
        }
        else

        if((cd.req_signal == 1) && (cd.ack_signal == 1))
        {
          cd.req_signal = 0;
        }
      }
    }
  }

  cd.last_read_cycles += clock_delta;
}

void update_cdda()
{
  s32 clock_delta =
   (cpu.global_cycles << step_fractional_bits_clock) - cd.last_cdda_cycles;
  cd.last_cdda_cycles += clock_delta;

  if(cd.cdda_fade_cycles)
  {
    cd.cdda_fade_cycles -= (clock_delta >> step_fractional_bits_clock);
    if(cd.cdda_fade_cycles <= 0)
    {
      cd.cdda_volume--;
      if(cd.cdda_volume == 0)
        cd.cdda_fade_cycles = 0;
      else
        cd.cdda_fade_cycles += cd.cdda_fade_cycle_interval;
    }
  }

  if(cd.cdda_access_cycles)
  {
    cd.cdda_access_cycles -= clock_delta >> step_fractional_bits_clock;

    if(cd.cdda_access_cycles <= 0)
    {
      cd.cdda_access_cycles = 0;
      cd_set_status(SCSI_STATUS_MESSAGE_GOOD);
      cd_raise_event(CD_IRQ_DATA_TRANSFER_DONE);
    }
  }

  if((cd.cdda_status == CDDA_STATUS_PLAYING) && (!cd.cdda_access_cycles))
  {
    s32 sample_l, sample_r;

    while(clock_delta >= 0)
    {
      cd.last_sector_type = CD_SECTOR_LOAD_AUDIO;
      cd.last_sector_address = cd.cdda_sector_address;

      // Check to see if the end of the current sector has been hit
      if(cd.cdda_read_offset == (2352 / 2))
      {
        if(cd.cdda_sector_address == cd.cdda_end_sector_address)
        {
          switch(cd.cdda_play_mode)
          {
            default:
            case CDDA_PLAY_MODE_STOP:
              cd.cdda_status = CDDA_STATUS_STOPPED;
              break;

            case CDDA_PLAY_MODE_STOP_INTERRUPT:
              cd.cdda_status = CDDA_STATUS_STOPPED;
              cd_raise_event(CD_IRQ_DATA_TRANSFER_DONE);
              break;

            case CDDA_PLAY_MODE_REPEAT:
            case CDDA_PLAY_MODE_REPEAT2:
              cd.cdda_sector_address = cd.cdda_start_sector_address;
              break;
          }
        }

        if(cd.cdda_status == CDDA_STATUS_STOPPED)
          break;

        cd.cdda_read_offset = 0;
        bin_cue_read_sector_audio(cd.cdda_cache, cd.cdda_sector_address);
        cd.last_sector_address = cd.cdda_sector_address;
        cd.cdda_sector_address++;

        cd.cdda_cycles += cd.cdda_sector_cycle_interval;
      }

      sample_l = cd.cdda_cache[cd.cdda_read_offset];
      sample_r = cd.cdda_cache[cd.cdda_read_offset + 1];

      audio.buffer[cd.cdda_audio_buffer_index] +=
       sample_l * cd.cdda_volume;
      audio.buffer[cd.cdda_audio_buffer_index + 1] +=
       sample_r * cd.cdda_volume;

      cd.cdda_read_offset += 2;
      cd.cdda_audio_buffer_index =
       (cd.cdda_audio_buffer_index + 2) % AUDIO_BUFFER_SIZE;

      cd.cdda_cycles -= psg.clock_step;
      clock_delta -= psg.clock_step;

      if(cd.cdda_cycles < 0)
        break;
    }
  }

  while(clock_delta >= 0)
  {
    cd.cdda_audio_buffer_index =
     (cd.cdda_audio_buffer_index + 2) % AUDIO_BUFFER_SIZE;

    clock_delta -= psg.clock_step;
  }

  cd.last_cdda_cycles -= clock_delta;
}

void save_bram(char *path)
{
  FILE *bram_file = fopen(path, "wb");
  printf("saving bram %s...\n", path);
  fwrite(cd.bram, 1024 * 8, 1, bram_file);
  fclose(bram_file);
}

void load_bram(char *path)
{
  FILE *bram_file = fopen(path, "rb");
  printf("loading bram %s\n", path);
  fread(cd.bram, 1024 * 8, 1, bram_file);
  fclose(bram_file);
}

void create_bram(char *path)
{
  memset(cd.bram, 0, sizeof(cd.bram));
  cd.bram[0] = 'H';
  cd.bram[1] = 'U';
  cd.bram[2] = 'B';
  cd.bram[3] = 'M';
  cd.bram[4] = 0x00;
  cd.bram[5] = 0xA0;
  cd.bram[6] = 0x10;
  cd.bram[7] = 0x80;

  save_bram(path);
}

void dump_cd_status()
{
  printf("cd.scsi_bus_state: %04x\n", cd.scsi_bus_state);
  printf("cd.req_signal: %04x\n", cd.req_signal);
  printf("cd.ack_signal: %04x\n", cd.ack_signal);
  printf("cd.data_bus: %04x\n", cd.data_bus);

  printf("cd.irq_enable: %04x\n", cd.irq_enable);
  printf("cd.irq_status: %04x\n", cd.irq_status);

  printf("cd.read_cycles: %04x\n", cd.read_cycles);
  printf("cd.cdda_cycles: %04x\n", cd.cdda_cycles);
  printf("cd.cdda_access_cycles: %0x4\n", cd.cdda_access_cycles);
}

#define cd_savestate_extra_load()                                             \
  if(cd.last_sector_type == CD_SECTOR_LOAD_AUDIO)                             \
    bin_cue_read_sector_audio(cd.cdda_cache, cd.last_sector_address)          \

#define cd_savestate_extra_store()                                            \

#define cd_savestate_builder(type, type_b, version_gate)                      \
void cd_##type_b##_savestate(savestate_##type_b##_type savestate_file)        \
{                                                                             \
  file_##type##_array(savestate_file, cd.work_ram);                           \
  file_##type##_array(savestate_file, cd.data_buffer);                        \
  file_##type##_array(savestate_file, cd.command_buffer);                     \
                                                                              \
  file_##type##_variable(savestate_file, cd.last_read_cycles);                \
  file_##type##_variable(savestate_file, cd.last_cdda_cycles);                \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_start_last_cycles);          \
                                                                              \
  file_##type##_variable(savestate_file, cd.data_buffer_top);                 \
  file_##type##_variable(savestate_file, cd.data_buffer_position);            \
  file_##type##_variable(savestate_file, cd.data_buffer_bytes_written);       \
                                                                              \
  file_##type##_variable(savestate_file, cd.command_buffer_position);         \
                                                                              \
  file_##type##_variable(savestate_file, cd.scsi_bus_state);                  \
  file_##type##_variable(savestate_file, cd.req_signal);                      \
  file_##type##_variable(savestate_file, cd.ack_signal);                      \
                                                                              \
  file_##type##_variable(savestate_file, cd.data_bus);                        \
                                                                              \
  file_##type##_variable(savestate_file, cd.read_sector_address);             \
  file_##type##_variable(savestate_file, cd.read_sector_count);               \
                                                                              \
  file_##type##_variable(savestate_file, cd.read_cycles);                     \
                                                                              \
  file_##type##_variable(savestate_file, cd.irq_enable);                      \
  file_##type##_variable(savestate_file, cd.irq_status);                      \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_status);                     \
  file_##type##_variable(savestate_file, cd.cdda_play_mode);                  \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_cycles);                     \
  file_##type##_variable(savestate_file, cd.cdda_sector_address);             \
  file_##type##_variable(savestate_file, cd.cdda_start_sector_address);       \
  file_##type##_variable(savestate_file, cd.cdda_end_sector_address);         \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_access_cycles);              \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_volume);                     \
  file_##type##_variable(savestate_file, cd.cdda_read_left);                  \
  file_##type##_variable(savestate_file, cd.cdda_read_right);                 \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_read_offset);                \
  file_##type##_variable(savestate_file, cd.cdda_audio_buffer_index);         \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_fade_cycles);                \
  file_##type##_variable(savestate_file, cd.cdda_fade_cycle_interval);        \
                                                                              \
  file_##type##_variable(savestate_file, cd.cdda_last_start_pos);             \
                                                                              \
  file_##type##_variable(savestate_file, cd.sub_channel_data);                \
                                                                              \
  file_##type##_variable(savestate_file, cd.last_sector_address);             \
  file_##type##_variable(savestate_file, cd.last_sector_type);                \
                                                                              \
  file_##type##_variable(savestate_file, cd.bram_enable);                     \
                                                                              \
  file_##type##_variable(savestate_file, cd.message);                         \
  file_##type##_variable(savestate_file, cd.status_sent);                     \
  file_##type##_variable(savestate_file, cd.message_sent);                    \
  file_##type##_variable(savestate_file, cd.data_transfer_done);              \
                                                                              \
  file_##type##_variable(savestate_file, cd.sense_key);                       \
  file_##type##_variable(savestate_file, cd.additional_sense_code);           \
  file_##type##_variable(savestate_file, cd.additional_sense_code_qualifier); \
  file_##type##_variable(savestate_file, cd.field_replaceable_unit_code);     \
                                                                              \
  cd_savestate_extra_##type_b();                                              \
}                                                                             \

build_savestate_functions(cd);
