#include "common.h"

/*
void setup_main_dirs()
{
  struct stat sb;

#ifdef LETSGO_HOME
	char save_path[MAX_PATH], bram_path[MAX_PATH], config_path[MAX_PATH], images_path[MAX_PATH], sys_path[MAX_PATH];
	snprintf(config.main_path, MAX_PATH, "%s/.temper", getenv("HOME")); 
	
	snprintf(save_path, MAX_PATH, "%s/%s", config.main_path, "save_states"); 
	snprintf(bram_path, MAX_PATH, "%s/%s", config.main_path, "bram"); 
	snprintf(config_path, MAX_PATH, "%s/%s", config.main_path, "config"); 
	snprintf(images_path, MAX_PATH, "%s/%s", config.main_path, "images"); 
	snprintf(sys_path, MAX_PATH, "%s/%s", config.main_path, "syscards"); 
	
	if(stat(config.main_path, &sb))
	{
		printf("save_states directory doesn't exist, creating\n");
		make_directory(config.main_path);
	}
	
	if(stat(save_path, &sb))
	{
		printf("save_states directory doesn't exist, creating\n");
		make_directory(save_path);
	}

	if(stat(config_path, &sb))
	{
		printf("config directory doesn't exist, creating\n");
		make_directory(config_path);
	}

	if(stat(bram_path, &sb))
	{
		printf("bram directory doesn't exist, creating\n");
		make_directory(bram_path);
	}

	if(stat(images_path, &sb))
	{
		printf("images directory doesn't exist, creating\n");
		make_directory(images_path);
	}
	
	if(stat(sys_path, &sb))
	{
		printf("syscard directory doesn't exist, creating\n");
		make_directory(sys_path);
	}
#else
	getcwd(config.main_path, MAX_PATH);
	
	if(stat("save_states", &sb))
	{
    printf("save_states directory doesn't exist, creating\n");
    make_directory("save_states");
	}

	if(stat("config", &sb))
	{
    printf("config directory doesn't exist, creating\n");
    make_directory("config");
	}

	if(stat("bram", &sb))
	{
    printf("bram directory doesn't exist, creating\n");
    make_directory("bram");
	}

	if(stat("images", &sb))
	{
    printf("images directory doesn't exist, creating\n");
    make_directory("images");
	}
#endif

}

const u32 benchmark_frame_interval = 1000;

void benchmark_step()
{
  static u64 benchmark_ms;
  static u32 benchmark_number = 0;

#ifdef PSP_BUILD
  static PspDebugProfilerRegs *profile_struct;
#endif

  switch(benchmark_number)
  {
    case 0:
#ifdef PSP_BUILD
      profile_struct = sceKernelReferGlobalProfiler();

      memset(profile_struct, 0, sizeof(PspDebugProfilerRegs));
      profile_struct->enable = 1;
#endif
      delay_us(1000 * 2500);

      get_ticks_us(&benchmark_ms);
      break;

    case 1:
    {
      u64 new_ms;
      u32 ms_delta;

      get_ticks_us(&new_ms);
      ms_delta = (new_ms - benchmark_ms) / 1000;

      printf("benchmark took %d ms (%lf ms per frame)\n",
       ms_delta, (double)ms_delta / benchmark_frame_interval);

#ifdef PSP_BUILD
      {
        u32 total_stalled_cycles;

        profile_struct->enable = 0;

        total_stalled_cycles = profile_struct->internal +
         profile_struct->memory + profile_struct->copz +
         profile_struct->vfpu;

        printf("PSP profile results:\n"
         "system cycles:    %d\n"
         "cpu cycles:       %d\n"
         "stalled cycles:   %d (%d) (%lf%%)\n"
         "  internal:       %d (%lf%%)\n"
         "  memory:         %d (%lf%%)\n"
         "  copz:           %d (%lf%%)\n"
         "  vfpu:           %d (%lf%%)\n"
         "bus accesses:     %d\n"
         "uncached load:    %d\n"
         "uncached store:   %d\n"
         "cached loads:     %d\n"
         "cached stores:    %d\n"
         "icache misses:    %d\n"
         "dcache misses:    %d\n"
         "dcache writeback: %d\n",

         profile_struct->systemck,
         profile_struct->cpuck,
         total_stalled_cycles,
         profile_struct->sleep,
         percent_of(total_stalled_cycles, profile_struct->cpuck),
         profile_struct->internal,
         percent_of(profile_struct->internal, total_stalled_cycles),
         profile_struct->memory,
         percent_of(profile_struct->memory, total_stalled_cycles),
         profile_struct->copz,
         percent_of(profile_struct->copz, total_stalled_cycles),
         profile_struct->vfpu,
         percent_of(profile_struct->vfpu, total_stalled_cycles),
         profile_struct->bus_access,
         profile_struct->uncached_load,
         profile_struct->uncached_store,
         profile_struct->cached_load,
         profile_struct->cached_store,
         profile_struct->i_miss,
         profile_struct->d_miss,
         profile_struct->d_writeback);
      }
#endif
      quit();
      break;
    }
  }

  benchmark_number++;
}

// TODO: Count argcs
u32 process_arguments(int argc, char *argv[])
{
  u32 options_parsed = 0;
  u32 current_option;
  s32 option_index;

  static struct option long_options[] =
  {
    { "netplay-server", no_argument, NULL, 0 },
    { "netplay-connect", required_argument, NULL, 0 },
    { "netplay-port", required_argument, NULL, 0 },
    { "netplay-latency", required_argument, NULL, 0 },
    { "netplay-username", required_argument, NULL, 0 },
    { "countdown-breakpoint", required_argument, NULL, 0 },
    { "pc-breakpoint", required_argument, NULL, 0 },
  };

  config.benchmark_mode = 0;
  config.fast_forward = 0;
  config.load_state_0 = 0;
  config.relaunch_shell_on_quit = 1;

  while(1)
  {
    current_option = getopt_long(argc, argv, "dbfsx",
     long_options, &option_index);

    if(current_option == -1)
      break;

    options_parsed++;

    switch(current_option)
    {
      case 0:
        switch(option_index)
        {
          case 0:
            printf("Selected netplay server.\n");
            config.netplay_type = NETPLAY_TYPE_SERVER;
            break;

          case 1:
            config.netplay_type = NETPLAY_TYPE_CLIENT;
            config.netplay_ip = netplay_ip_string_value(optarg);
            printf("Selected netplay client, connecting to IP %x.\n",
             config.netplay_ip);
            break;

          case 2:
            config.netplay_port = atoi(optarg);
            printf("Selected netplay port %d.\n", config.netplay_port);
            break;

          case 3:
            config.netplay_server_frame_latency = atoi(optarg);
            printf("Selected netplay latency %d.\n",
             config.netplay_server_frame_latency);
            break;

          case 4:
            strncpy(config.netplay_username, optarg, 128);
            printf("Setting netplay username to %s\n", config.netplay_username);
            break;

          case 5:
            debug.breakpoint = strtol(optarg, NULL, 16) + 1;
            debug.breakpoint_original = debug.breakpoint;
            set_debug_mode(DEBUG_COUNTDOWN_BREAKPOINT);
            break;

          case 6:
            debug.breakpoint = strtol(optarg, NULL, 16);
            debug.breakpoint_original = debug.breakpoint;
            set_debug_mode(DEBUG_PC_BREAKPOINT);
            printf("Breakpoint set to PC %04x", debug.breakpoint);
            break;

          default:
            break;
        }
        break;

      case 'd':
        set_debug_mode(DEBUG_STEP);
        break;

      case 'b':
        config.benchmark_mode = 1;
        break;

      case 'f':
        config.fast_forward = 1;
        break;

      case 's':
        config.load_state_0 = 1;
        break;

      case 'x':
        printf("Not relaunching shell on quit.\n");
        config.relaunch_shell_on_quit = 0;
        break;
    }
  }

  return options_parsed;
}

int main(int argc, char *argv[])
{
  u32 command_line_options;
  u32 benchmark_frames = benchmark_frame_interval;


#ifdef PSP_BUILD
  delay_us(2000000);

  scePowerSetClockFrequency(config.clock_speed, config.clock_speed,
   config.clock_speed / 2);
#endif

  //config.use_opengl = 1;

  printf("platform initialize now\n");
  platform_initialize();

  printf("sizeof(cpu_struct): %d\n", sizeof(cpu_struct));

  printf("Setting up main directories.\n");
  setup_main_dirs();

  printf("Initialize PCE\n");
  initialize_pce();

  printf("Loading secondary configuration file\n");
  load_directory_config_file("temper.cf2");

  printf("Loading primary configuration file\n");
  load_config_file("temper.cfg");

  printf("Processing arguments\n");
  command_line_options = process_arguments(argc, argv);

  if((argc > 1) && (argv[argc - 1][0] != '-'))
  {
    printf("Loading ROM %s\n", argv[argc - 1]);
  
	if (strstr(argv[1], ".bin") || strstr(argv[1], ".iso") )
	{
		return;
	}

    if(load_rom(argv[argc - 1]) == -1)
    {
      printf("Error: Could not load %s\n", argv[1]);
      return -1;
    }
    reset_pce();
  }
  else
  {
    printf("Starting menu\n");
    menu(1);
  }

  if(config.load_state_0)
  {
    char state_name[MAX_PATH];
    sprintf(state_name, "%s_0.svs", config.rom_filename);

    load_state(state_name, NULL, 0);
  }

  if(config.benchmark_mode)
  {
    char state_name[MAX_PATH];
    sprintf(state_name, "%s_0.svs", config.rom_filename);

    load_state(state_name, NULL, 0);

    benchmark_step();

    config.fast_forward = 1;
  }

  printf("Initializing netplay.\n");
  fflush(stdout);

  printf("Running game.\n");

  if(netplay.pause == 0)
    audio_unpause();

  while(1)
  {
    if(netplay.active && netplay.pause)
    {
      delay_us(10000);
      update_events();
      update_screen();
      continue;
    }

    //synchronize();

#ifdef FASTFORWARD_FRAMESKIP
    if(config.fast_forward && !config.benchmark_mode)
    {
      static u32 frameskip_counter = 0;

      update_frame(frameskip_counter == 0);
      frameskip_counter = (frameskip_counter + 1) %
       FASTFORWARD_FRAMESKIP_RATE;
    }
    else
    {
      update_frame(0);
    }

#else
    update_frame(0);
#endif

    audio_sync_start();
    update_psg();
    update_cdda();
    audio_sync_end();

    update_events();

    if(config.benchmark_mode)
    {
      benchmark_frames--;
      if(benchmark_frames == 0)
      {
        benchmark_step();
        benchmark_frames = benchmark_frame_interval;
      }
    }
  }

  return 0;
}

void initialize_pce()
{
  initialize_video();
  initialize_memory();
  initialize_io();
  initialize_irq();
  initialize_timer();
  initialize_psg();
  initialize_cpu();
  initialize_cd();
  initialize_adpcm();
  initialize_arcade_card();
  initialize_debug();
}

void reset_pce()
{
  reset_video();
  reset_memory();
  reset_io();
  reset_irq();
  reset_timer();
  reset_psg();
  reset_cpu();
  reset_cd();
  reset_adpcm();
  reset_arcade_card();

  reset_debug();

  netplay_connect();
}

// For now this will save a full sized snapshot.

#define SAVESTATE_MAX_SIZE ((1024 * 256 * 3) + (1024 * 1024 * 2))

void save_state(char *file_name, u16 *snapshot)
{
  char path[MAX_PATH];
  sprintf(path, "%s%csave_states%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
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

  status_message("Saved state %d", config.savestate_number);
  free(savestate_buffer);
}

void load_state_date(char *path, char *state_date)
{
  struct tm *current_tm;
  struct stat state_stat;
  char *current_time_str;

  stat(path, &state_stat);
  current_tm = gmtime(&(state_stat.st_mtime));

  current_time_str = asctime(current_tm);
  strcpy(state_date, current_time_str);
}

savestate_extension_enum load_state(char *file_name, u8 *in_memory_state,
 u32 in_memory_state_size)
{
  char path[MAX_PATH];
  sprintf(path, "%s%csave_states%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
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
    netplay_send_savestate(file_name);
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

  status_message("Loaded state %d", config.savestate_number);

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

u8 *preload_state(char *file_name, u32 *_file_length, u32 trim_snapshot)
{
  char path[MAX_PATH];
  sprintf(path, "%s%csave_states%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
  FILE *savestate_file = fopen(path, "rb");
  u8 *savestate_buffer;
  u32 savestate_length;
  savestate_header_struct savestate_header;
  u32 savestate_header_raw[SAVESTATE_HEADER_LENGTH / 4];

  if(savestate_file == NULL)
    return NULL;

  fseek(savestate_file, 0, SEEK_END);
  savestate_length = ftell(savestate_file);
  fseek(savestate_file, 0, SEEK_SET);

  fread(savestate_header_raw, SAVESTATE_HEADER_LENGTH, 1, savestate_file);
  fseek(savestate_file, 0, SEEK_SET);

  file_read_array(savestate_file, savestate_header.magic_string);
  file_read_variable(savestate_file, savestate_header.version);
  file_read_variable(savestate_file, savestate_header.extensions);
  file_read_variable(savestate_file, savestate_header.snapshot_format);
  file_read_array(savestate_file, savestate_header.reserved);

  if(trim_snapshot && (savestate_header.extensions & SS_EXT_SNAPSHOT))
  {
    // Clear snapshot flag from extensions field
    savestate_header_raw[2] &= ~SS_EXT_SNAPSHOT;

    savestate_length -= 320 * 240 * 2;
    savestate_buffer = malloc(savestate_length);

    memcpy(savestate_buffer, savestate_header_raw, SAVESTATE_HEADER_LENGTH);
    fseek(savestate_file, 320 * 240 * 2, SEEK_CUR);
    fread(savestate_buffer + SAVESTATE_HEADER_LENGTH, savestate_length, 1,
     savestate_file);
  }
  else
  {
    savestate_buffer = malloc(savestate_length);

    memcpy(savestate_buffer, savestate_header_raw, SAVESTATE_HEADER_LENGTH);
    fread(savestate_buffer + SAVESTATE_HEADER_LENGTH, savestate_length, 1,
     savestate_file);
  }

  *_file_length = savestate_length;
  fclose(savestate_file);

  return savestate_buffer;
}


savestate_extension_enum load_state_snapshot(char *file_name, u16 *buffer,
 char *state_date)
{
  char path[MAX_PATH];
  sprintf(path, "%s%csave_states%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
  savestate_header_struct savestate_header;
  u8 *savestate_buffer = malloc(SAVESTATE_HEADER_LENGTH + (320 * 240 * 2));

  file_read_mem_open(savestate_file, path, savestate_buffer, NULL, 0);

  if(file_read_mem_invalid(savestate_file))
  {
    free(savestate_buffer);
    return SS_EXT_INVALID;
  }

  if(state_date)
    load_state_date(path, state_date);

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

  if(savestate_header.version > TEMPER_SAVESTATE_VERSION)
    goto invalid;

  // Does it have a snapshot? Load it.
  if(savestate_header.extensions & SS_EXT_SNAPSHOT)
  {
    // Okay, must support these as they're added.
    switch(savestate_header.snapshot_format)
    {
      default:
      case SS_SNAPSHOT_FULL:
        file_read_mem_load_fixed(savestate_file, 320 * 240 * 2);
        file_read_mem(savestate_file, buffer, 320 * 240 * 2);
        break;
    }
  }

  free(savestate_buffer);
  return savestate_header.extensions;

 invalid:
  file_read_mem_close(savestate_file);
  free(savestate_buffer);
  return SS_EXT_INVALID;
}

#define config_file_action(type, version_gate)                                \
  file_##type##_array(config_file, config.pad);                               \
  file_##type##_variable(config_file, config.show_fps);                       \
  file_##type##_variable(config_file, config.enable_sound);                   \
  file_##type##_variable(config_file, config.fast_forward);                   \
  file_##type##_variable(config_file, config.audio_output_frequency);         \
  file_##type##_variable(config_file, config.patch_idle_loops);               \
  file_##type##_variable(config_file, config.snapshot_format);                \
  file_##type##_variable(config_file, config.force_usa);                      \
  file_##type##_variable(config_file, config.clock_speed);                    \
  file_##type##_variable(config_file, config.ram_timings);                    \
  file_##type##_variable(config_file, config.gamma_percent);                  \
  if(version_gate >= 2)                                                       \
  {                                                                           \
    file_##type##_variable(config_file, config.six_button_pad);               \
    file_##type##_variable(config_file, config.cd_system_type);               \
    file_##type##_variable(config_file, config.bz2_savestates);               \
    file_##type##_variable(config_file, config.per_game_bram);                \
  }                                                                           \
                                                                              \
  if(version_gate >= 3)                                                       \
  {                                                                           \
    file_##type##_variable(config_file, config.sound_double);                 \
    file_##type##_variable(config_file, config.scale_factor);                 \
    file_##type##_variable(config_file, config.scale_width);                  \
    file_##type##_variable(config_file, config.unlimit_sprites);              \
    file_##type##_variable(config_file, config.compatibility_mode);           \
  }                                                                           \
                                                                              \
  if(version_gate >= 4)                                                       \
  {                                                                           \
    file_##type##_array(config_file, config.netplay_username);                \
    file_##type##_variable(config_file, config.netplay_type);                 \
    file_##type##_variable(config_file, config.netplay_port);                 \
    file_##type##_variable(config_file, config.netplay_ip);                   \
    file_##type##_variable(config_file, config.netplay_server_frame_latency); \
  }                                                                           \

void save_config_file(char *file_name)
{
  char path[MAX_PATH];
  sprintf(path, "%s%cconfig%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
  u8 *config_file_buffer = malloc(16384);

  printf("saving config to file named %s\n", path);

  file_write_mem_open(config_file, path, config_file_buffer);
  config_header_struct config_header;

  // The header is TMCF for "Temper Config"
  config_header.magic_string[0] = 'T';
  config_header.magic_string[1] = 'M';
  config_header.magic_string[2] = 'C';
  config_header.magic_string[3] = 'F';

  config_header.version = TEMPER_CONFIG_VERSION;

  file_write_mem_array(config_file, config_header.magic_string);
  file_write_mem_variable(config_file, config_header.version);
  file_write_mem_array(config_file, config_header.reserved);

  config_file_action(write_mem, TEMPER_CONFIG_VERSION);

  file_write_mem_save(config_file);
  file_write_mem_close(config_file);
  free(config_file_buffer);
}

void save_directory_config_file(char *file_name)
{
  char path[MAX_PATH];
  sprintf(path, "%s%cconfig%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);
  u8 *config_file_buffer = malloc(16384);

  printf("saving directory config to file named %s\n", path);

  file_write_mem_open(config_file, path, config_file_buffer);
  config_header_struct config_header;

  // The header is TMC2 for "Temper Config type 2"
  config_header.magic_string[0] = 'T';
  config_header.magic_string[1] = 'M';
  config_header.magic_string[2] = 'C';
  config_header.magic_string[3] = '2';

  config_header.version = TEMPER_CONFIG2_VERSION;

  file_write_mem_array(config_file, config_header.magic_string);
  file_write_mem_variable(config_file, config_header.version);
  file_write_mem_array(config_file, config_header.reserved);

  file_write_mem_array(config_file, config.rom_directory);

  file_write_mem_save(config_file);
  file_write_mem_close(config_file);
  free(config_file_buffer);
}

s32 load_config_file(char *file_name)
{
  char path[MAX_PATH];
  sprintf(path, "%s%cconfig%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);

  u32 old_netplay_type = config.netplay_type;

#ifdef CONFIG_OPTIONS_RAM_TIMINGS
  u32 old_ram_timings = config.ram_timings;
#endif

#ifdef CONFIG_OPTIONS_CLOCK_SPEED
  u32 old_clock_speed = config.clock_speed;
#endif

#ifdef CONFIG_OPTIONS_GAMMA
  u32 old_gamma_percent = config.gamma_percent;
#endif

  u32 i;

  config_buttons_enum v1_to_v2_button_config[] =
  {
    CONFIG_BUTTON_UP, CONFIG_BUTTON_DOWN, CONFIG_BUTTON_LEFT,
    CONFIG_BUTTON_RIGHT, CONFIG_BUTTON_I, CONFIG_BUTTON_II,
    CONFIG_BUTTON_RUN, CONFIG_BUTTON_SELECT, CONFIG_BUTTON_RAPID_I,
    CONFIG_BUTTON_RAPID_II, CONFIG_BUTTON_MENU, CONFIG_BUTTON_SAVE_STATE,
    CONFIG_BUTTON_LOAD_STATE, CONFIG_BUTTON_VOLUME_DOWN,
    CONFIG_BUTTON_VOLUME_UP, CONFIG_BUTTON_FAST_FORWARD, CONFIG_BUTTON_NONE
  };

  printf("loading config file %s\n", path);

  file_read_open(config_file, path);
  config_header_struct config_header;

  if(file_read_invalid(config_file))
  {
    printf("config file %s does not exist\n", path);
    return -1;
  }

  file_read_array(config_file, config_header.magic_string);
  file_read_variable(config_file, config_header.version);
  file_read_array(config_file, config_header.reserved);

  // First, the magic string must match.
  if(strncmp(config_header.magic_string, "TMCF", 4))
    goto invalid;

  if(config_header.version > TEMPER_CONFIG_VERSION)
    goto invalid;

  config_file_action(read, config_header.version);

  // The key map was changed in v2.
  if(config_header.version == 1)
  {
    for(i = 0; i < 16; i++)
    {
      config.pad[i] = v1_to_v2_button_config[config.pad[i]];
    }
  }

  // Others not allowed on the menu for now.
  config.audio_output_frequency = 44100;
  file_read_close(config_file);

#ifdef CONFIG_OPTIONS_RAM_TIMINGS
  if(config.ram_timings != old_ram_timings)
  {
    if(config.ram_timings)
      set_fast_ram_timings();
    else
      set_default_ram_timings();
  }
#endif

#ifdef CONFIG_OPTIONS_CLOCK_SPEED
  if(config.clock_speed != old_clock_speed)
    set_clock_speed(config.clock_speed);
#endif

#ifdef CONFIG_OPTIONS_GAMMA
  if(config.gamma_percent != old_gamma_percent)
    set_gamma(config.gamma_percent);
#endif

  config.netplay_type = old_netplay_type;

  return 0;

 invalid:
  printf("an error occured while trying to load the config file\n");
  file_read_close(config_file);
  return -1;
}

s32 load_directory_config_file(char *file_name)
{
  char path[MAX_PATH];
  sprintf(path, "%s%cconfig%c%s", config.main_path, DIR_SEPARATOR_CHAR,
   DIR_SEPARATOR_CHAR, file_name);

  printf("loading directory config file %s\n", path);

  file_read_open(config_file, path);
  config_header_struct config_header;

  if(file_read_invalid(config_file))
  {
    printf("directory config file %s does not exist\n", path);
    return -1;
  }

  file_read_array(config_file, config_header.magic_string);
  file_read_variable(config_file, config_header.version);
  file_read_array(config_file, config_header.reserved);

  // First, the magic string must match.
  if(strncmp(config_header.magic_string, "TMC2", 4))
    goto invalid;

  // Current version is 1
  if(config_header.version > TEMPER_CONFIG2_VERSION)
    goto invalid;

  file_read_array(config_file, config.rom_directory);
  file_read_close(config_file);

  chdir(config.rom_directory);

  return 0;

 invalid:
  printf("an error occured while trying to load the directory config file\n");
  file_read_close(config_file);
  return -1;
}

s32 save_screenshot(u16 *snapshot)
{
  char images_path[MAX_PATH];
  char image_filename[MAX_PATH];
  char current_dir[MAX_PATH];
  sprintf(images_path, "%s%cimages", config.main_path, DIR_SEPARATOR_CHAR);
  u32 shot_number = 0;
  u16 bmp_header[54 / 2];
  u32 x, y;
  u16 current_pixel;
  u16 *bmp_offset;
  u8 *screenshot_buffer = malloc((320 * 240 * 2) + (26 * 2));

  struct stat sb;

  getcwd(current_dir, MAX_PATH);
  chdir(images_path);

  do
  {
    sprintf(image_filename, "%s_%d.bmp", config.rom_filename, shot_number);
    shot_number++;
  } while(!stat(image_filename, &sb));

  printf("saving image file %s\n", image_filename);

  file_write_mem_open(bmp_file, image_filename, screenshot_buffer);

  if(file_write_mem_invalid(bmp_file))
  {
    printf("could not create image file %s\n", image_filename);
    chdir(current_dir);
    return -1;
  }

  // 0 2  signature, must be 4D42 hex
  bmp_header[0] = 0x4D42;
  // 2 4 size of BMP file in bytes (unreliable)
  bmp_header[1] = ((320 * 240 * 2) + 54) & 0xFFFF;
  bmp_header[2] = ((320 * 240 * 2) + 54) >> 16;
  // 6 2 reserved, must be zero
  // 8 2 reserved, must be zero
  bmp_header[3] = 0;
  bmp_header[4] = 0;
  // 10 4 offset to start of image data in bytes
  bmp_header[5] = 54;
  bmp_header[6] = 0;
  // 14 4 size of BITMAPINFOHEADER structure, must be 40
  bmp_header[7] = 40;
  bmp_header[8] = 0;
  // 18 4 image width in pixels
  bmp_header[9] = 320;
  bmp_header[10] = 0;
  // 22 4 image height in pixels
  bmp_header[11] = 240;
  bmp_header[12] = 0;
  // 26 2 number of planes in the image, must be 1
  bmp_header[13] = 1;
  // 28 2 number of bits per pixel (1, 4, 8, 16, or 24)
  bmp_header[14] = 16;
  // 30 4 compression type (0=none, 1=RLE-8, 2=RLE-4)
  bmp_header[15] = 0;
  bmp_header[16] = 0;
  // 34 4 size of image data in bytes (including padding)
  bmp_header[17] = (320 * 240 * 2) & 0xFFFF;
  bmp_header[18] = (320 * 240 * 2) >> 16;
  // 38 4 horizontal resolution in pixels per meter (unreliable)
  bmp_header[19] = 0;
  bmp_header[20] = 0;
  // 42 4 vertical resolution in pixels per meter (unreliable)
  bmp_header[21] = 0;
  bmp_header[22] = 0;
  // 46 4 number of colors in image, or zero
  bmp_header[23] = 512;
  bmp_header[24] = 0;
  // 50 4 number of important colors, or zero
  bmp_header[25] = 512;
  bmp_header[26] = 0;

  file_write_mem_array(bmp_file, bmp_header);

  for(y = 0; y < 240; y++)
  {
    bmp_offset = snapshot + ((239 - y) * 320);

    for(x = 0; x < 320; x++)
    {
      current_pixel = bmp_offset[x];
      current_pixel = (current_pixel & 0x1F) |
       ((current_pixel >> 1) & ((0x1F << 5) | (0x1F << 10)));
      file_write_mem_variable(bmp_file, current_pixel);
    }
  }

  file_write_mem_save(bmp_file);
  file_write_mem_close(bmp_file);
  chdir(current_dir);
  free(screenshot_buffer);
  return 0;
}

void quit()
{
  char bram_path[MAX_PATH];
  get_bram_path(bram_path);
  save_bram(bram_path);

  chdir(config.main_path);

  save_directory_config_file("temper.cf2");

  audio_exit();
  platform_quit();

  exit(0);
}

void status_message_raw(char *message)
{
  if(config.status_message_lines == STATUS_MESSAGE_LINES)
  {
    u32 i;
    for(i = 0; i < STATUS_MESSAGE_LINES - 1; i++)
    {
      strncpy(config.status_message[i], config.status_message[i + 1], 128);
    }
  }
  else
  {
    (config.status_message_lines)++;
  }
  strncpy(config.status_message[config.status_message_lines - 1], message, 128);

  config.status_message_counter += 
   (STATUS_MESSAGE_DURATION * strlen(message)) / 5;
}

*/