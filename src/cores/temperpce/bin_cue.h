#ifndef BIN_CUE_H
#define BIN_CUE_H

typedef enum
{
  TRACK_FILE_TYPE_BINARY,
  TRACK_FILE_TYPE_WAVE,
  TRACK_FILE_TYPE_NULL,
#ifdef OGG_SUPPORT
  TRACK_FILE_TYPE_OGG
#endif
} track_file_type_enum;

// 28B

typedef struct
{
  u32 file_number;
  u32 physical_offset;

  u32 sector_offset;
  u32 sector_count;
  u32 pregap_offset;

  u32 sector_size;
  u32 format_type;
} cd_track_struct;

// 20B

typedef struct
{
  u32 channels;
  u32 frequency;
  u32 frequency_div;
  u32 bytes_per_sample;
  u32 bytes_per_sector;
} wav_info_struct;

// w/o OGG support: 23B

typedef struct
{
  track_file_type_enum type;
  FILE *file_handle;

  u32 current_offset;
  union
  {
    wav_info_struct *wav_info;
#ifdef OGG_SUPPORT
    OggVorbis_File *ov_file;
#endif
  } ext_info;
} cd_track_file_struct;

// 7B + 2300B + 2800B + 400B

typedef struct
{
  FILE *bin_file;
  cd_track_file_struct track_files[100];
  u32 num_files;

  s32 first_track;
  s32 last_track;
  u32 num_physical_tracks;
  u32 num_sectors;
  s32 last_seek_track;

  cd_track_struct physical_tracks[100];
  cd_track_struct *logical_tracks[100];
} cd_bin_struct;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

s32 load_bin_cue(char *cue_file_name);

u32 bin_cue_get_first_track();
u32 bin_cue_get_last_track();
u32 bin_cue_get_sector_count();
s32 bin_cue_get_track_offset(u32 track_number);
u32 bin_cue_get_track_format(u32 track_number);
s32 bin_cue_get_current_track(u32 sector_offset, u32 *_track_number,
 u32 *index_number, u32 *relative_offset);

s32 bin_cue_read_sector_audio(s16 *sector_buffer, u32 sector_offset);
s32 bin_cue_read_sector_data(u8 *sector_buffer, u32 sector_offset);

void bin_cue_preload_audio_sector(u32 sector_offset);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
