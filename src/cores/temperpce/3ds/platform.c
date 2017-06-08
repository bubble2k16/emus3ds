
#include "3ds.h"

#include "common.h"

config_struct config =
{
  // u32 pad[16];
  {
    CONFIG_BUTTON_UP, CONFIG_BUTTON_DOWN, CONFIG_BUTTON_LEFT,
    CONFIG_BUTTON_RIGHT, CONFIG_BUTTON_NONE, CONFIG_BUTTON_I,
    CONFIG_BUTTON_II, CONFIG_BUTTON_MENU, CONFIG_BUTTON_LOAD_STATE,
    CONFIG_BUTTON_SAVE_STATE, CONFIG_BUTTON_RUN, CONFIG_BUTTON_SELECT,
    CONFIG_BUTTON_VOLUME_DOWN, CONFIG_BUTTON_VOLUME_UP, CONFIG_BUTTON_MENU,
    CONFIG_BUTTON_NONE
  },
  0,                 // u32 show_fps;
  1,                 // u32 enable_sound;
  0,                 // u32 fast_forward;
  32000,             // u32 audio_output_frequency;
  1,                 // u32 patch_idle_loops;
  SS_SNAPSHOT_OFF,   // u32 snapshot_format;
  0,                 // u32 force_usa;

  200,               // u32 clock_speed;
  0,                 // u32 gp2x_ram_timings;
  100,               // u32 gp2x_gamma_percent;
  0,                 // u32 six_button_pad;
  CD_SYSTEM_TYPE_V3, // cd_system_type_enum cd_system_type;
  1,                 // u32 bz2_savestates;
  0,                 // u32 per_game_bram;
  0,                 // u32 sound_double;
  0,                 // u32 scale_factor;
  0,                 // u32 fast_cd_access;
  0,                 // u32 fast_cd_load;
  0,                 // u32 scale_width;
  0,                 // u32 unlimit_sprites
  0,                 // u32 compatibility_mode
  1                  // u32 software_rendering;
};

void set_screen_resolution(u32 width, u32 height)
{

}

void clear_screen()
{

}

void initialize_event()
{
}

void initialize_audio()
{

}

void audio_signal_callback()
{
}

u32 audio_pause()
{
}

void audio_unpause()
{
}

void audio_lock()
{
}

void audio_unlock()
{
}


s32 load_config_file(char *file_name)
{
}


#define SAVESTATE_MAX_SIZE ((1024 * 256 * 3) + (1024 * 1024 * 2))

void save_state(char *file_name, u16 *snapshot)
{
  char path[MAX_PATH];
  sprintf(path, "%s", file_name);
  u8 *savestate_buffer = malloc(SAVESTATE_MAX_SIZE);
  u32 snapshot_length = 0;

  printf("saving state to file named %s\n", path);

  file_write_mem_open(savestate_file, path, savestate_buffer);
  savestate_header_struct savestate_header;

  // The header is TMSS for "Temper Savestate"
  savestate_header.magic_string[0] = 'T';
  savestate_header.magic_string[1] = 'M';
  savestate_header.magic_string[2] = 'S';
  savestate_header.magic_string[3] = 'S';

  // Version 1 = first version. 0 is reserved as invalid.
  savestate_header.version = TEMPER_SAVESTATE_VERSION;

  savestate_header.extensions = 0;

  // Only save a snapshot if it's given a valid one.

  if(snapshot != NULL)
  {
    savestate_header.extensions |= SS_EXT_SNAPSHOT;
    savestate_header.snapshot_format = SS_SNAPSHOT_FULL;
  }

  if(config.cd_loaded)
  {
    savestate_header.extensions |= SS_EXT_CDROM_DATA;

    if((config.cd_system_type == CD_SYSTEM_TYPE_V3) ||
     (config.cd_system_type == CD_SYSTEM_TYPE_ACD))
    {
      savestate_header.extensions |= SS_EXT_SUPER_CD_DATA;
    }

    if(config.cd_system_type == CD_SYSTEM_TYPE_ACD)
      savestate_header.extensions |= SS_EXT_ARCADE_CARD_DATA;
  }

  if(memory.sf2_region != -1)
    savestate_header.extensions |= SS_EXT_SF2_DATA;

  if(config.six_button_pad)
    savestate_header.extensions |= SS_EXT_6BUTTON_DATA;

  if(config.populous_loaded)
    savestate_header.extensions |= SS_EXT_POPULOUS_DATA;

  if(config.bz2_savestates)
    savestate_header.extensions |= SS_EXT_BZ2_COMPRESSED;

  file_write_mem_array(savestate_file, savestate_header.magic_string);
  file_write_mem_variable(savestate_file, savestate_header.version);
  file_write_mem_variable(savestate_file, savestate_header.extensions);
  file_write_mem_variable(savestate_file, savestate_header.snapshot_format);
  file_write_mem_array(savestate_file, savestate_header.reserved);

  if(savestate_header.extensions & SS_EXT_SNAPSHOT)
  {
    snapshot_length = 320 * 240 * 2;
    file_write_mem(savestate_file, snapshot, snapshot_length);
  }

  video_store_savestate(savestate_file);
  memory_store_savestate(savestate_file);
  io_store_savestate(savestate_file);
  irq_store_savestate(savestate_file);
  timer_store_savestate(savestate_file);
  psg_store_savestate(savestate_file);
  cpu_store_savestate(savestate_file);

  if(config.cd_loaded)
  {
    cd_store_savestate(savestate_file);
    adpcm_store_savestate(savestate_file);

    if(savestate_header.extensions & SS_EXT_SUPER_CD_DATA)
      file_write_mem_array(savestate_file, cd.ext_ram);

    if(savestate_header.extensions & SS_EXT_ARCADE_CARD_DATA)
      arcade_card_store_savestate(savestate_file);
  }

  if((savestate_header.extensions & SS_EXT_SF2_DATA))
    file_write_mem_variable(savestate_file, memory.sf2_region);

  if((savestate_header.extensions & SS_EXT_6BUTTON_DATA))
    file_write_mem_variable(savestate_file, io.six_button_toggle);

  if((savestate_header.extensions & SS_EXT_POPULOUS_DATA))
    file_write_mem_array(savestate_file, memory.hucard_rom + (0x40 * 0x2000));

#ifdef BZ_SUPPORT
  if(savestate_header.extensions & SS_EXT_BZ2_COMPRESSED)
  {
    int error_code;
    BZFILE *bz_savestate_file;
    u32 extra_length = SAVESTATE_HEADER_LENGTH + snapshot_length;

    file_write_mem_save_fixed(savestate_file, extra_length);
    bz_savestate_file = BZ2_bzWriteOpen(&error_code, savestate_file->_file,
     4, 0, 30);

    printf("saving bz2 compressed portion of state at offset %x\n",
     extra_length);
    BZ2_bzWrite(&error_code, bz_savestate_file, savestate_file->buffer +
     extra_length, (savestate_file->buffer_ptr - savestate_file->buffer) -
     extra_length);
    BZ2_bzWriteClose(&error_code, bz_savestate_file, 0, NULL, NULL);
  }
  else
#endif
  {
    file_write_mem_save(savestate_file);
  }
  file_write_mem_close(savestate_file);

  //status_message("Saved state %d", config.savestate_number);
  free(savestate_buffer);
}



savestate_extension_enum load_state(char *file_name, u8 *in_memory_state,
 u32 in_memory_state_size)
{
  char path[MAX_PATH];
  sprintf(path, "%s", file_name);
  savestate_header_struct savestate_header;
  u8 *savestate_buffer = malloc(SAVESTATE_MAX_SIZE);

  u32 audio_pause_state = audio_pause();

  file_read_mem_open(savestate_file, path, savestate_buffer, in_memory_state,
   in_memory_state_size);

  if(in_memory_state)
  {
    printf("loading state from memory.\n");
  }
  else
  {
    printf("loading state %s\n", path);
    //netplay_send_savestate(file_name);
  }

  if(file_read_mem_invalid(savestate_file))
  {
    printf("savestate %s does not exist\n", path);
    free(savestate_buffer);
    return SS_EXT_INVALID;
  }

  // Don't want things to hang as soon as the state is loaded because
  // of a deadlock between the audio buffer and the audio generation,
  // so make it wait now.

  file_read_mem_load_fixed(savestate_file, SAVESTATE_HEADER_LENGTH);

  file_read_mem_array(savestate_file, savestate_header.magic_string);
  file_read_mem_variable(savestate_file, savestate_header.version);
  file_read_mem_variable(savestate_file, savestate_header.extensions);
  file_read_mem_variable(savestate_file, savestate_header.snapshot_format);
  file_read_mem_array(savestate_file, savestate_header.reserved);

  // First, the savestate string must match.
  if(strncmp(savestate_header.magic_string, "TMSS", 4))
    goto invalid;

  // Right now this only supports savestate version 1. Later if more
  // are done it'll probably support those too! Version 0 is right out
  // though.

  // If savestates change too much there can be a lower limit here too.
  if(savestate_header.version > TEMPER_SAVESTATE_VERSION)
  {
    printf("Error: Savestate version not supported by Temper.\n");
    goto invalid;
  }

  config.savestate_version = savestate_header.version;

  // Does it have a snapshot? What do we care here? Skip it.
  if(savestate_header.extensions & SS_EXT_SNAPSHOT)
  {
    // Okay, must support these as they're added.
    switch(savestate_header.snapshot_format)
    {
      default:
      case SS_SNAPSHOT_FULL:
        file_read_mem_load_null(savestate_file, 320 * 240 * 2);
        file_read_mem_skip_forward(savestate_file, 320 * 240 * 2);
        break;
    }
  }

  // Now we can load all the actual stuff. In the future the extension
  // fields will dictate loading more, but this is the basic set.

#ifdef BZ_SUPPORT
  if(savestate_header.extensions & SS_EXT_BZ2_COMPRESSED)
  {
    int error_code;

    if(savestate_file->memory_file)
    {
      u32 read_length = SAVESTATE_MAX_SIZE;
      BZ2_bzBuffToBuffDecompress((char *)savestate_file->buffer, &read_length,
       (char *)savestate_file->memory_file, savestate_file->memory_file_size, 
       0, 0);
      savestate_file->buffer_ptr = savestate_file->buffer;
    }
    else
    {
      BZFILE *bz_savestate_file = BZ2_bzReadOpen(&error_code,
       savestate_file->_file, 0, 1, NULL, 0);
      BZ2_bzRead(&error_code, bz_savestate_file, savestate_file->buffer,
       SAVESTATE_MAX_SIZE);
      savestate_file->buffer_ptr = savestate_file->buffer;
      BZ2_bzReadClose(&error_code, bz_savestate_file);
    }
  }
  else
#endif
  {
    file_read_mem_load(savestate_file);
  }

  video_load_savestate(savestate_file);
  memory_load_savestate(savestate_file);
  io_load_savestate(savestate_file);
  irq_load_savestate(savestate_file);
  timer_load_savestate(savestate_file);
  psg_load_savestate(savestate_file);
  cpu_load_savestate(savestate_file);

  config.cd_loaded = 0;
  config.populous_loaded = 0;
  memory.sf2_region = -1;

  if(savestate_header.extensions & SS_EXT_CDROM_DATA)
  {
    config.cd_loaded = 1;
    cd_load_savestate(savestate_file);
    adpcm_load_savestate(savestate_file);

    if(savestate_header.extensions & SS_EXT_SUPER_CD_DATA)
      file_read_mem_array(savestate_file, cd.ext_ram);

    if(savestate_header.extensions & SS_EXT_ARCADE_CARD_DATA)
      arcade_card_load_savestate(savestate_file);
  }

  if(savestate_header.extensions & SS_EXT_SF2_DATA)
  {
    u32 current_sf2_region;
    file_read_mem_variable(savestate_file, current_sf2_region);
    map_sf2(current_sf2_region);
  }

  if(savestate_header.extensions & SS_EXT_6BUTTON_DATA)
    file_read_mem_variable(savestate_file, io.six_button_toggle);

  if(savestate_header.extensions & SS_EXT_POPULOUS_DATA)
  {
    config.populous_loaded = 1;
    map_populous_ram();
    file_read_mem_array(savestate_file, memory.hucard_rom + (0x40 * 0x2000));
  }

  //status_message("Loaded state %d", config.savestate_number);

  // And return the extensions we loaded.
  free(savestate_buffer);
  audio_revert_pause_state(audio_pause_state);
  return savestate_header.extensions;

 invalid:
  file_read_mem_close(savestate_file);

  printf("An error occured while trying to load the savestate file.\n");
  free(savestate_buffer);
  audio_revert_pause_state(audio_pause_state);
  return SS_EXT_INVALID;
}


void clear_line_edges(u32 line_number, u32 color, u32 edge, u32 middle)
{
}

u32 get_screen_pitch()
{
  return 512;
}

void update_screen()
{
}


void get_ticks_us(u64 *ticks_return)
{
  *ticks_return = svcGetSystemTick();
}


void quit()
{

  exit(0);
}

netplay_struct netplay;