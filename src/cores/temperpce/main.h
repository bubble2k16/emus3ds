#ifndef MAIN_H
#define MAIN_H

#define TEMPER_SAVESTATE_VERSION 3
#define TEMPER_CONFIG_VERSION    4
#define TEMPER_CONFIG2_VERSION   1

typedef enum
{
  CD_SYSTEM_TYPE_V1,
  CD_SYSTEM_TYPE_V2,
  CD_SYSTEM_TYPE_V3,
  CD_SYSTEM_TYPE_ACD,
  CD_SYSTEM_TYPE_GECD
} cd_system_type_enum;

#define STATUS_MESSAGE_LINES    4
#define STATUS_MESSAGE_DURATION 30

// ~3K + 172B

typedef struct
{
  // This should be stored in the config file.
  u32 pad[16];

  u32 show_fps;
  u32 enable_sound;
  u32 fast_forward;
  u32 audio_output_frequency;
  u32 patch_idle_loops;
  u32 snapshot_format;
  u32 force_usa;

  u32 clock_speed;
  u32 ram_timings;
  u32 gamma_percent;
  u32 six_button_pad;

  u32 cd_system_type;
  u32 bz2_savestates;
  u32 per_game_bram;

  u32 sound_double;
  u32 scale_factor;

  u32 fast_cd_access;
  u32 fast_cd_load;
  u32 scale_width;
  u32 unlimit_sprites;
  u32 compatibility_mode;

  u32 software_rendering;

  char netplay_username[16];
  u32 netplay_type;
  u32 netplay_port;
  u32 netplay_ip;
  u32 netplay_server_frame_latency;

  // This will save the last directory you loaded a ROM from,
  // in a separate config file.
  char rom_directory[MAX_PATH];

  // This reflects runtime configuration/status and shouldn't be.
  u32 savestate_number;

  u32 savestate_version;

  char main_path[MAX_PATH];
  char rom_filename[MAX_PATH];
  u32 cd_loaded;
  u32 populous_loaded;
  u32 sgx_mode;

  u32 bg_off;
  u32 spr_off;

  u32 relaunch_shell_on_quit;
  u32 load_state_0;
  u32 benchmark_mode;

  u32 use_opengl;

  u32 status_message_counter;
  u32 status_message_lines;
  char status_message[STATUS_MESSAGE_LINES][128];
  char *input_message;

} config_struct;

typedef struct
{
  char magic_string[4];                     // 4 bytes
  u32 version;                              // 4 bytes
  u8 reserved[8];                           // 8 bytes
} config_header_struct;                     // total: 16 bytes

// This is just debugging/setting stuff, doesn't have to go into savestates
// or anything like that.

extern u32 synchronize_flag;
extern config_struct config;

// Just some ideas for the future. These would be optional elements in a
// savestate. The extensions field in the header says which of these are
// present; it's a bitmap, so you can only have up to 32 things here
// (if that's not enough more can be expanded into the reserved space...)

// This could really specify not just optional things that appear in the
// savestate but how it's saved in general, such as compression.

// One is reserved for "invalid" to show that something is messed up when
// it's returned.

typedef enum
{
  SS_EXT_SNAPSHOT         = 0x00000001,
  SS_EXT_CDROM_DATA       = 0x00000002,
  SS_EXT_SUPER_CD_DATA    = 0x00000004,
  SS_EXT_SUPERGRAFX_DATA  = 0x00000008,
  SS_EXT_SF2_DATA         = 0x00000010,
  SS_EXT_6BUTTON_DATA     = 0x00000020,
  SS_EXT_ARCADE_CARD_DATA = 0x00000040,
  SS_EXT_POPULOUS_DATA    = 0x00000080,
  SS_EXT_BZ2_COMPRESSED   = 0x00000100,
  SS_EXT_INVALID          = 0x80000000
} savestate_extension_enum;

// Different ways to save snapshot, I guess. Space saving technique...

typedef enum
{
  SS_SNAPSHOT_OFF,
  SS_SNAPSHOT_FULL,
  SS_SNAPSHOT_QUARTER,
  SS_SNAPSHOT_EIGHTH
} snapshot_format_enum;

// This is what a load state can return

// The bytes that come at the beginning of a savestate. Keep this padded
// out to a nice power of 2, with a lot of reserved space.

// For now, it's 32 bytes. That should leave a comfortable amount of extra
// space. If more is needed an extension can provide it.

typedef struct
{
  char magic_string[4];                     // 4 bytes
  u32 version;                              // 4 bytes
  savestate_extension_enum extensions;      // 4 bytes
  snapshot_format_enum snapshot_format;     // 4 bytes
  u8 reserved[16];                          // 16 bytes
} savestate_header_struct;                  // total: 32 bytes

#define SAVESTATE_HEADER_LENGTH                                               \
  (sizeof(savestate_header.magic_string) +                                    \
   sizeof(savestate_header.version) +                                         \
   sizeof(savestate_header.extensions) +                                      \
   sizeof(savestate_header.snapshot_format) +                                 \
   sizeof(savestate_header.reserved))                                         \

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void synchronize();
void initialize_pce();
void reset_pce();
void quit();
void platform_initialize();
void platform_quit();

void get_ticks_us(u64 *ticks_return);

void save_state(char *file_name, u16 *snapshot);
savestate_extension_enum load_state(char *file_name, u8 *in_memory_state,
 u32 in_memory_state_size);
savestate_extension_enum load_state_snapshot(char *file_name, u16 *buffer,
 char *state_date);
u8 *preload_state(char *file_name, u32 *_file_length, u32 trim_snapshot);

void save_config_file(char *file_name);
void save_directory_config_file(char *file_name);
s32 load_config_file(char *file_name);
s32 load_directory_config_file(char *file_name);

s32 save_screenshot(u16 *snapshot);

void status_message_raw(char *message);

void set_fast_ram_timings();
void set_default_ram_timings();

void delay_us(u32 us_count);

#ifdef CONFIG_OPTIONS_GAMMA
  void set_gamma(u32 gamma);
#endif

#ifdef CONFIG_OPTIONS_RAM_TIMINGS
  void set_ram_timings_fast();
  void set_ram_timings_default();
#endif

#ifdef CONFIG_OPTIONS_CLOCK_SPEED
  void set_clock_speed();
#endif

#define build_savestate_functions(name)                                       \
  name##_savestate_builder(read_mem, load, config.savestate_version);         \
  name##_savestate_builder(write_mem, store, TEMPER_SAVESTATE_VERSION)        \


#define status_message(format, ...)                                           \
{                                                                             \
  char message[128];                                                          \
  sprintf(message, format, __VA_ARGS__);                                      \
  status_message_raw(message);                                                \
}                                                                             \

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif
