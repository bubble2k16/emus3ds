// This will load the related BIN file too (just the BIN, for now)

#include "common.h"

cd_bin_struct cd_bin;

static char *skip_whitespace(char *str)
{
  while(*str == ' ')
    str++;

  return str;
}

#define address8(base, offset)                                                \
  *((u8 *)((u8 *)base + (offset)))                                            \

#define address16(base, offset)                                               \
  *((u16 *)((u8 *)base + (offset)))                                           \

#define address32(base, offset)                                               \
  *((u32 *)((u8 *)base + (offset)))                                           \

wav_info_struct *wav_open(FILE *fp)
{
  u8 wav_header[36];
  u8 *riff_header = wav_header + 0;
  u8 *fmt_header = wav_header + 0x0C;
  wav_info_struct *wav_info;

  fread(wav_header, 36, 1, fp);
  // RIFF type chunk
  if(strncmp((char *)riff_header + 0x00, "RIFF", 4) ||
   strncmp((char *)riff_header + 0x08, "WAVE", 4) ||
   strncmp((char *)fmt_header + 0x00, "fmt ", 4) ||
   (address16(fmt_header, 0x08) != 1))
  {
    printf("rejected wave file: bad header\n");
    return NULL;
  }

  wav_info = malloc(sizeof(wav_info_struct));
  wav_info->channels = address16(fmt_header, 0x0a);
  wav_info->frequency = address32(fmt_header, 0x0c);
  wav_info->bytes_per_sample = address16(fmt_header, 0x16) / 8;

  if(((wav_info->channels * wav_info->bytes_per_sample) !=
   address16(fmt_header, 0x14)) ||
   ((wav_info->channels != 1) && (wav_info->channels != 2)) ||
   ((wav_info->frequency != 11025) && (wav_info->frequency != 22050) &&
    (wav_info->frequency != 44100)) ||
   ((wav_info->bytes_per_sample != 1) &&
    (wav_info->bytes_per_sample != 2)))
  {
    printf("rejected wave file: bad format\n");
    free(wav_info);
    return NULL;
  }

  wav_info->frequency_div = 44100 / wav_info->frequency;

  printf("loaded wav file, channels: %d, frequency: %d,"
   " bytes per sample: %d\n", wav_info->channels, wav_info->frequency,
   wav_info->bytes_per_sample);

  wav_info->bytes_per_sector =
   588 * wav_info->channels * wav_info->bytes_per_sample *
   wav_info->frequency_div / 4;

  return wav_info;
}

#define frequency_div_11025 1
#define frequency_div_22050 2
#define frequency_div_44100 4

#define decode_wav_setup_variables_mono(frequency)                            \
  const u32 sample_pairs_per_sector =                                         \
   (2352 / 4 / (frequency_div_##frequency));                                  \
  const u32 channel_count = 1                                                 \

#define decode_wav_setup_variables_stereo(frequency)                          \
  const u32 sample_pairs_per_sector =                                         \
   (2352 / 2 / (frequency_div_##frequency));                                  \
  const u32 channel_count = 2                                                 \


#define decode_wav_sample_pair_8bit_mono()                                    \
  l_sample = ((_sector_buffer[sample_input_index] - 128) << 8);               \
  r_sample = l_sample                                                         \

#define decode_wav_sample_pair_16bit_mono()                                   \
  l_sample = _sector_buffer16[sample_input_index];                            \
  r_sample = l_sample                                                         \

#define decode_wav_sample_pair_8bit_stereo()                                  \
  l_sample = ((_sector_buffer[sample_input_index] - 128) << 8);               \
  r_sample = ((_sector_buffer[sample_input_index + 1] - 128) << 8)            \

#define decode_wav_sample_pair_16bit_stereo()                                 \
  l_sample = _sector_buffer16[sample_input_index];                            \
  r_sample = _sector_buffer16[sample_input_index + 1]                         \


#define decode_wav_to_output_pair(offset)                                     \
  buffer[sample_output_index + offset] = l_sample;                            \
  buffer[sample_output_index + offset + 1] = l_sample                         \


#define decode_wav_to_output_11025()                                          \
  decode_wav_to_output_pair(0);                                               \
  decode_wav_to_output_pair(2);                                               \
  decode_wav_to_output_pair(4);                                               \
  decode_wav_to_output_pair(6);                                               \
  sample_output_index += 8                                                    \

#define decode_wav_to_output_22050()                                          \
  decode_wav_to_output_pair(0);                                               \
  decode_wav_to_output_pair(2);                                               \
  sample_output_index += 4                                                    \

#define decode_wav_to_output_44100()                                          \
  decode_wav_to_output_pair(0);                                               \
  sample_output_index += 2                                                    \

#define decode_wav(bit_rate, channels, frequency)                             \
{                                                                             \
  decode_wav_setup_variables_##channels(frequency);                           \
  for(sample_input_index = 0, sample_output_index = 0;                        \
   sample_input_index < sample_pairs_per_sector; sample_input_index +=        \
   channel_count)                                                             \
  {                                                                           \
    decode_wav_sample_pair_##bit_rate##_##channels();                         \
    decode_wav_to_output_##frequency();                                       \
  }                                                                           \
  break;                                                                      \
}                                                                             \

s32 wav_decode_sector(FILE *wav_file, wav_info_struct *wav_info,
 s16 *buffer)
{
  // This is a 4 bit switch factor.
  u32 switch_factor = (wav_info->channels - 1) |
   ((wav_info->bytes_per_sample - 1) << 1) |
   ((0x4 - wav_info->frequency_div) << 2);

  if(switch_factor == 0xF)
  {
    if(fread(buffer, 2352, 1, wav_file) == 1)
      return 0;
  }
  else
  {
    u8 _sector_buffer[wav_info->bytes_per_sector];
    s16 *_sector_buffer16 = (s16 *)_sector_buffer;
    s16 l_sample, r_sample;
    u32 sample_input_index;
    u32 sample_output_index;

    fread(_sector_buffer, wav_info->bytes_per_sector, 1, wav_file);

    switch(switch_factor)
    {
      case 0x0:
        decode_wav(8bit, mono, 11025);

      case 0x1:
        decode_wav(8bit, stereo, 11025);

      case 0x2:
        decode_wav(16bit, mono, 11025);

      case 0x3:
        decode_wav(16bit, stereo, 11025);

      case 0x4:
        decode_wav(8bit, mono, 22050);

      case 0x5:
        decode_wav(8bit, stereo, 22050);

      case 0x6:
        decode_wav(16bit, mono, 22050);

      case 0x7:
        decode_wav(16bit, stereo, 22050);

      case 0x8:
      case 0x9:
      case 0xA:
      case 0xB:
        // 33075KHz not supported.
        break;

      case 0xC:
        decode_wav(8bit, mono, 44100);

      case 0xD:
        decode_wav(8bit, stereo, 44100);

      case 0xE:
        decode_wav(16bit, mono, 44100);
    }
  }

  return -1;
}

s32 wav_seek(FILE *wav_file, wav_info_struct *wav_info, u32 offset)
{
  offset = offset * wav_info->bytes_per_sector / 2352;
  fseek(wav_file, 44 + offset, SEEK_SET);
  return 0;
}

// Returns what the size would be if it were 44.1KHz, stereo, 16bit
s32 wav_size(FILE *wav_file, wav_info_struct *wav_info)
{
  u32 raw_size = ftell(wav_file);
  fseek(wav_file, 0, SEEK_END);
  raw_size = ftell(wav_file);
  fseek(wav_file, 0, SEEK_SET);
  raw_size -= 44;

  if(wav_info->channels == 1)
    raw_size *= 2;

  if(wav_info->bytes_per_sample == 1)
    raw_size *= 2;

  switch(wav_info->frequency_div)
  {
    default:
    case 1:
      raw_size *= 4;
      break;

    case 2:
      raw_size *= 2;
      break;
  }

  return raw_size;
}

void str_to_lower(char *output_str, char *input_str)
{
  char current_char = input_str[0];
  u32 i = 0;

  while(current_char)
  {
    if(isalpha(current_char))
      output_str[i] = tolower(current_char);
    else
      output_str[i] = current_char;
    i++;
    current_char = input_str[i];
  }
  output_str[i] = 0;
}

FILE *fopen_scan_dir_case_insensitive(char *file_name, char *open_mode)
{
  char file_name_lower_case[MAX_PATH];
  char current_file_name[MAX_PATH];
  char path_buffer[MAX_PATH];
  DIR *current_dir;
  struct dirent *current_file;

  // Stupid CUEs with absolute paths for a Windows machine...
  char *separator_pos = strrchr(file_name, '\\');

  getcwd(path_buffer, MAX_PATH);
  current_dir = opendir(path_buffer);

  printf("performing directory scan to try to find match for %s (dir %s)\n",
   file_name, path_buffer);

  if(separator_pos)
  {
    printf("stripping Windows absolute path from filename\n");
    strcpy(path_buffer, separator_pos + 1);
    str_to_lower(file_name_lower_case, path_buffer);
    file_name = path_buffer;
  }
  else
  {
    str_to_lower(file_name_lower_case, file_name);
  }

  do
  {
    current_file = readdir(current_dir);
    if(current_file)
    {
      str_to_lower(current_file_name, current_file->d_name);
      printf("checking %s against %s\n", current_file_name, file_name_lower_case);

      if(!strcmp(file_name_lower_case, current_file_name))
      {
        FILE *matched_file;
        printf("found a match (%s)\n", current_file->d_name);
        matched_file = fopen(current_file->d_name, open_mode);
        closedir(current_dir);
        return matched_file;
      }
    }
  } while(current_file != NULL);

  closedir(current_dir);
  return NULL;
}

s32 track_file_open(cd_track_file_struct *track_file, char *file_name,
 track_file_type_enum type)
{
  if(type == TRACK_FILE_TYPE_NULL)
    return -1;

  track_file->type = type;
  track_file->file_handle = fopen(file_name, "rb");
  track_file->current_offset = 0;

  if(track_file->file_handle == NULL)
  {
    // The file wasn't opened, but maybe something with the same case
    // can be. Try that.
    track_file->file_handle =
     fopen_scan_dir_case_insensitive(file_name, "rb");
    if(track_file->file_handle == NULL)
      return -1;
  }

  switch(type)
  {
    case TRACK_FILE_TYPE_BINARY:
    case TRACK_FILE_TYPE_NULL:
      break;

    case TRACK_FILE_TYPE_WAVE:
      track_file->ext_info.wav_info = wav_open(track_file->file_handle);
      if(track_file->ext_info.wav_info == NULL)
      {
        fclose(track_file->file_handle);
        track_file->type = TRACK_FILE_TYPE_NULL;
      }
      break;

#ifdef OGG_SUPPORT
    case TRACK_FILE_TYPE_OGG:
      track_file->ext_info.ov_file = malloc(sizeof(OggVorbis_File));
      if(ov_open(track_file->file_handle, track_file->ext_info.ov_file,
       NULL, 0) != 0)
      {
        fclose(track_file->file_handle);
        return -1;
      }
      break;
#endif
  }
  return 0;
}

void track_file_close(cd_track_file_struct *track_file)
{
  switch(track_file->type)
  {
    case TRACK_FILE_TYPE_BINARY:
      fclose(track_file->file_handle);
      break;

    case TRACK_FILE_TYPE_WAVE:
      free(track_file->ext_info.wav_info);
      fclose(track_file->file_handle);
      break;

    case TRACK_FILE_TYPE_NULL:
      break;

#ifdef OGG_SUPPORT
    case TRACK_FILE_TYPE_OGG:
      ov_clear(track_file->ext_info.ov_file);
      free(track_file->ext_info.ov_file);
      break;
#endif
  }
}

void track_file_seek(cd_track_file_struct *track_file, u32 offset)
{
  if(track_file->current_offset != offset)
  {
    track_file->current_offset = offset;
    switch(track_file->type)
    {
      case TRACK_FILE_TYPE_BINARY:
        fseek(track_file->file_handle, offset, SEEK_SET);
        break;

      case TRACK_FILE_TYPE_WAVE:
        wav_seek(track_file->file_handle, track_file->ext_info.wav_info,
         offset);
        break;

      case TRACK_FILE_TYPE_NULL:
        break;

#ifdef OGG_SUPPORT
      case TRACK_FILE_TYPE_OGG:
        ov_pcm_seek(track_file->ext_info.ov_file, offset / 4);
        break;
#endif
    }
  }
}

void track_file_read(cd_track_file_struct *track_file, u8 *buffer,
 u32 length)
{
  switch(track_file->type)
  {
    case TRACK_FILE_TYPE_BINARY:
      fread(buffer, length, 1, track_file->file_handle);
      break;

    case TRACK_FILE_TYPE_WAVE:
      // This only works with length 2352 right now.
      wav_decode_sector(track_file->file_handle,
       track_file->ext_info.wav_info, (s16 *)buffer);
      break;

    case TRACK_FILE_TYPE_NULL:
      memset(buffer, 0, length);
      break;

#ifdef OGG_SUPPORT
    case TRACK_FILE_TYPE_OGG:
    {
      int read_section;
      u32 read_count = length;
      s32 bytes_read;
      u8 *buffer_pos = buffer;

      while(read_count)
      {
#ifdef OGG_TREMOR
        bytes_read = ov_read(track_file->ext_info.ov_file, (char *)buffer_pos,
         read_count, &read_section);
#else
        bytes_read = ov_read(track_file->ext_info.ov_file, (char *)buffer_pos,
         read_count, 0, 2, 1, &read_section);
#endif
        if(bytes_read <= 0)
        {
          memset(buffer, 0, read_count);
          break;
        }
        buffer_pos += bytes_read;
        read_count -= bytes_read;
      }
      break;
    }
#endif
  }
  track_file->current_offset += length;
}

u32 track_file_size(cd_track_file_struct *track_file)
{
  u32 track_size;

  switch(track_file->type)
  {
    case TRACK_FILE_TYPE_BINARY:
      fseek(track_file->file_handle, 0, SEEK_END);
      track_size = ftell(track_file->file_handle);
      fseek(track_file->file_handle, 0, SEEK_SET);
      return track_size;

    case TRACK_FILE_TYPE_WAVE:
      return wav_size(track_file->file_handle, track_file->ext_info.wav_info);

    case TRACK_FILE_TYPE_NULL:
      return 0;

#ifdef OGG_SUPPORT
    case TRACK_FILE_TYPE_OGG:
      return ov_pcm_total(track_file->ext_info.ov_file, -1) * 4;
#endif
  }

  return 0;
}

void close_bin_cue()
{
  u32 i;

  for(i = 0; i < cd_bin.num_files; i++)
  {
    track_file_close(cd_bin.track_files + i);
  }
}


s32 load_bin_cue(char *cue_file_name)
{
  FILE *cue_file = fopen(cue_file_name, "rb");

  close_bin_cue();

  printf("loading cue file %s\n", cue_file_name);

  if(cue_file)
  {
    char line_buffer[256];
    char *line_buffer_ptr;
    char current_dir[MAX_PATH];
    char cue_file_dir[MAX_PATH];

    s32 current_physical_track_number = -1;
    u32 current_physical_offset;
    u32 current_pregap = 0;
    u32 num_files = 0;
    u32 current_file;
    u32 track_size;
    u32 global_sector_offset;

    char *separator_pos;

    cd_track_struct *current_physical_track = NULL;
    cd_track_struct *next_physical_track = NULL;
    u32 i;

    getcwd(current_dir, MAX_PATH);
    strcpy(cue_file_dir, cue_file_name);

    // Might have to change directory first.
    separator_pos = strrchr(cue_file_dir, DIR_SEPARATOR_CHAR);

    if(separator_pos)
    {
      *separator_pos = 0;
      printf("changing directory to %s\n", cue_file_dir);
      chdir(cue_file_dir);
      *separator_pos = DIR_SEPARATOR_CHAR;
    }

    for(i = 0; i < 100; i++)
    {
      cd_bin.logical_tracks[i] = NULL;
    }

    cd_bin.first_track = -1;
    cd_bin.last_track = -1;
    cd_bin.num_physical_tracks = 0;
    cd_bin.num_sectors = 0;

    // Get line
    while(fgets(line_buffer, 256, cue_file))
    {
      // Skip trailing whitespace
      line_buffer_ptr = skip_whitespace(line_buffer);

      // Dirty, but should work - switch on first character.
      switch(line_buffer_ptr[0])
      {
        // New file
        case 'F':
        {
          char track_file_name[MAX_PATH];
          track_file_type_enum track_file_type = TRACK_FILE_TYPE_NULL;

          // There may or may not be quotations around the FILE name.
          // Find the first space instead.
          separator_pos = strchr(line_buffer, ' ');
          // Now find the first non-space.
          separator_pos = skip_whitespace(separator_pos);
          // Now see if what's there is a quote.
          if(*separator_pos == '"')
          {
            strcpy(track_file_name, separator_pos + 1);
            separator_pos = strrchr(track_file_name, '"');
            *(separator_pos) = 0;
          }
          else
          {
            // Otherwise go to the next space.
            strcpy(track_file_name, separator_pos);
            separator_pos = strrchr(track_file_name, ' ');
            *(separator_pos) = 0;
          }

          // Skip remaining whitespace.
          separator_pos = skip_whitespace(separator_pos + 1);

          if(!strncmp(separator_pos, "BINARY", 6))
            track_file_type = TRACK_FILE_TYPE_BINARY;
          else

          if(!strncmp(separator_pos, "WAVE", 4))
            track_file_type = TRACK_FILE_TYPE_WAVE;

#ifdef OGG_SUPPORT
          else

          if(!strncmp(separator_pos, "OGG", 3))
            track_file_type = TRACK_FILE_TYPE_OGG;
#endif

          if(track_file_open(cd_bin.track_files + num_files, track_file_name,
           track_file_type) == -1)
          {
            goto bin_cue_load_error;
          }

          current_physical_offset = 0;
          num_files++;
          break;
        }

        // New track number
        case 'T':
        {
          u32 new_track_number;
          char track_type[64];

          sscanf(line_buffer_ptr, "TRACK %d %s", &new_track_number,
           track_type);

          current_physical_track_number++;
          current_physical_track =
           cd_bin.physical_tracks + current_physical_track_number;

          current_physical_track->file_number = num_files - 1;
          current_physical_track->sector_size = 2352;

          if(!strcmp(track_type, "AUDIO"))
          {
            current_physical_track->format_type = 0;
            current_physical_track->sector_size = 2352;
          }

          if(!strcmp(track_type, "MODE1/2352"))
          {
            current_physical_track->format_type = 4;
            current_physical_track->sector_size = 2352;
          }

          if(!strcmp(track_type, "MODE1/2048"))
          {
            current_physical_track->format_type = 4;
            current_physical_track->sector_size = 2048;
          }

          cd_bin.logical_tracks[new_track_number] = current_physical_track;
          cd_bin.num_physical_tracks++;

          if((cd_bin.first_track == -1) ||
           (new_track_number < cd_bin.first_track))
          {
            cd_bin.first_track = new_track_number;
          }

          if((cd_bin.last_track == -1) ||
           (new_track_number > cd_bin.last_track))
          {
            cd_bin.last_track = new_track_number;
          }

          break;
        }

        // Pregap
        case 'P':
        {
          u32 minutes, seconds, frames;

          sscanf(line_buffer_ptr, "PREGAP %d:%d:%d", &minutes,
           &seconds, &frames);

          current_pregap += frames + (seconds * 75) + (minutes * 75 * 60);
          break;
        }

        // Index
        case 'I':
        {
          u32 index_number;
          u32 minutes, seconds, frames;
          u32 sector_offset;

          sscanf(line_buffer_ptr, "INDEX %d %d:%d:%d", &index_number,
           &minutes, &seconds, &frames);

          sector_offset = frames + (seconds * 75) + (minutes * 75 * 60);

          if(index_number == 1)
          {
            current_physical_track->pregap_offset = current_pregap;
            current_physical_track->sector_offset = sector_offset;
          }

          break;
        }
      }
    }

    cd_bin.physical_tracks[current_physical_track_number + 1].file_number =
     0xFFFFFFFF;

    // Set the physical offset and sizes for each file

    current_physical_track = cd_bin.physical_tracks;
    next_physical_track = cd_bin.physical_tracks + 1;
    global_sector_offset = 0;

    printf("Done parsing cue, calculating physical offsets and sizes:\n");

    for(current_file = 0; current_file < num_files; current_file++)
    {
      current_physical_offset = 0;
      while(next_physical_track->file_number == current_file)
      {
        current_physical_track->sector_count =
         next_physical_track->sector_offset -
         current_physical_track->sector_offset;

        current_physical_track->physical_offset = current_physical_offset;
        current_physical_offset += (current_physical_track->sector_count *
         current_physical_track->sector_size);

        current_physical_track->sector_offset +=
         current_physical_track->pregap_offset + global_sector_offset;

        cd_bin.num_sectors += current_physical_track->sector_count;

        current_physical_track++;
        next_physical_track++;
      }

      // Transition to the next file - get the file size
      track_size = track_file_size(cd_bin.track_files + current_file);

      // Set the last track data
      current_physical_track->physical_offset = current_physical_offset;
      current_physical_track->sector_offset +=
       current_physical_track->pregap_offset + global_sector_offset;
      current_physical_track->sector_count =
       (track_size - current_physical_offset) /
       current_physical_track->sector_size;

      global_sector_offset += current_physical_track->sector_count;

      cd_bin.num_sectors += current_physical_track->sector_count;

      current_physical_track++;
      next_physical_track++;
    }

    printf("finished loading cue file %s\n", cue_file_name);
    printf("first track: %d, last track: %d\n", cd_bin.first_track,
     cd_bin.last_track);

    for(i = cd_bin.first_track; i <= cd_bin.last_track; i++)
    {
      printf("track %d:\n", i);
      if(cd_bin.logical_tracks[i] == NULL)
      {
        printf("  (invalid)\n");
      }
      else
      {
        printf("  file %p (%d)\n",
         cd_bin.track_files + cd_bin.logical_tracks[i]->file_number,
         cd_bin.logical_tracks[i]->file_number);
        printf("  physical offset 0x%x\n",
         cd_bin.logical_tracks[i]->physical_offset);
        printf("  sector offset 0x%x\n",
         cd_bin.logical_tracks[i]->sector_offset);
        printf("  sector count %x\n",
         cd_bin.logical_tracks[i]->sector_count);
        printf("  sector size %d\n",
         cd_bin.logical_tracks[i]->sector_size);
      }
    }

    cd_bin.num_files = num_files;
    cd_bin.last_seek_track = 0;

    chdir(current_dir);

    fclose(cue_file);
    return 0;
  }

 bin_cue_load_error:
  printf("Could not load CD image.\n");
  return -1;
}

s32 bin_cue_seek_track(u32 sector_offset)
{
  u32 i = 0;

  // This assumes some locality of reference in seeking: it'll start
  // looking at this track first if it fits.

  if(cd_bin.physical_tracks[cd_bin.last_seek_track].sector_offset <=
   sector_offset)
  {
    i = cd_bin.last_seek_track;
  }

  for(; i < cd_bin.num_physical_tracks; i++)
  {
    if(cd_bin.physical_tracks[i].sector_offset > sector_offset)
      break;
  }

  i--;

  cd_bin.last_seek_track = i;
  return i;
}

s32 bin_cue_read_sector(u8 *sector_buffer, u32 sector_offset, u32 offset,
 u32 size)
{
  u32 track_sector_offset;
  cd_track_struct *target_track;

  target_track = cd_bin.physical_tracks + bin_cue_seek_track(sector_offset);
  track_sector_offset = sector_offset - target_track->sector_offset;

  if(track_sector_offset < target_track->sector_count)
  {
    if(target_track->sector_size == 2048)
    {
      offset = 0;
      size = 2048;
    }

    track_file_seek(cd_bin.track_files + target_track->file_number,
     target_track->physical_offset +
     (target_track->sector_size * track_sector_offset) + offset);
    track_file_read(cd_bin.track_files + target_track->file_number,
     sector_buffer, size);
  }
  else
  {
    //printf("out of bounds read %x\n", sector_offset);
    memset(sector_buffer, 0, size);
  }

  return 0;
}

s32 bin_cue_read_sector_audio(s16 *sector_buffer, u32 sector_offset)
{
  return bin_cue_read_sector((u8 *)sector_buffer, sector_offset, 0, 2352);
}

s32 bin_cue_read_sector_data(u8 *sector_buffer, u32 sector_offset)
{
  return bin_cue_read_sector(sector_buffer, sector_offset, 16, 2048);
}

u32 bin_cue_get_first_track()
{
  return cd_bin.first_track;
}

u32 bin_cue_get_last_track()
{
  return cd_bin.last_track;
}

u32 bin_cue_get_sector_count()
{
  return cd_bin.num_sectors;
}

s32 bin_cue_get_track_offset(u32 track_number)
{
  if(cd_bin.logical_tracks[track_number])
    return cd_bin.logical_tracks[track_number]->sector_offset;

  if(track_number == (cd_bin.last_track + 1))
  {
    track_number--;
    return cd_bin.logical_tracks[track_number]->sector_offset +
     cd_bin.logical_tracks[track_number]->sector_count;
  }

  return -1;
}

u32 bin_cue_get_track_format(u32 track_number)
{
  if(cd_bin.logical_tracks[track_number])
    return cd_bin.logical_tracks[track_number]->format_type;

  return 0;
}

s32 bin_cue_get_current_track(u32 sector_offset, u32 *_track_number,
 u32 *index_number, u32 *relative_offset)
{
  u32 track_number = bin_cue_seek_track(sector_offset);

  *_track_number = track_number;
  // Okay, this isn't necessarily accurate, but should fit all PCE-CD games.
  *index_number = 1;
  *relative_offset = sector_offset -
   cd_bin.physical_tracks[track_number].sector_offset;

  return 0;
}

