#ifndef CD_H
#define CD_H

#define DATA_BUFFER_SIZE 2048

typedef enum
{
  SCSI_STATE_BUS_FREE,
  SCSI_STATE_COMMAND,
  SCSI_STATE_DATA_IN,
  SCSI_STATE_STATUS,
  SCSI_STATE_MESSAGE_IN
} scsi_bus_state_enum;

typedef enum
{
  SCSI_STATUS_MESSAGE_GOOD            = 0,
  SCSI_STATUS_MESSAGE_CHECK_CONDITION = 1
} scsi_status_message_enum;

typedef enum
{
  SCSI_COMMAND_TEST_UNIT_READY     = 0x00,
  SCSI_COMMAND_REQUEST_SENSE       = 0x03,
  SCSI_COMMAND_READ                = 0x08,
  SCSI_COMMAND_SET_AUDIO_START_POS = 0xD8,
  SCSI_COMMAND_SET_AUDIO_END_POS   = 0xD9,
  SCSI_COMMAND_PAUSE_AUDIO         = 0xDA,
  SCSI_COMMAND_READ_SUBCHANNEL_Q   = 0xDD,
  SCSI_COMMAND_GET_DIRECTORY_INFO  = 0xDE
} scsi_command_enum;

// Some combinations:
// 0x00: Bus free mode
// 0x90: BSY | CD: Command phase
// 0x98: BSY | CD | IO: Status
// 0xB8: BSY | MSG | CD | IO: Message in phase
// 0xC8: BSY | REQ | IO: Data in phase
// 0xD0: BSY | REQ | CD : Command w/REQ
// 0xD8: BSY | REQ | CD | IO: Status w/REQ
// 0xBF: BSY | REQ | MSG | CD | IO: Message in w/REQ

typedef enum
{
  SCSI_STATUS_BIT_IO               = 0x08,
  SCSI_STATUS_BIT_CD               = 0x10,
  SCSI_STATUS_BIT_MSG              = 0x20,
  SCSI_STATUS_BIT_REQ              = 0x40,
  SCSI_STATUS_BIT_BSY              = 0x80
} scsi_status_bit_enum;


typedef enum
{
  CD_IRQ_ADPCM_PLAYBACK_HALF_DONE  = 0x04,
  CD_IRQ_ADPCM_PLAYBACK_DONE       = 0x08,
  CD_IRQ_SUBCHANNEL_READ           = 0x10,
  CD_IRQ_DATA_TRANSFER_DONE        = 0x20,
  CD_IRQ_DATA_TRANSFER_READY       = 0x40
} cd_irq_enum;

typedef enum
{
  CDDA_STATUS_PLAYING,
  CDDA_STATUS_PAUSED,
  CDDA_STATUS_STOPPED
} cdda_status_enum;

typedef enum
{
  CDDA_PLAY_MODE_REPEAT            = 0x1,
  CDDA_PLAY_MODE_STOP_INTERRUPT    = 0x2,
  CDDA_PLAY_MODE_STOP              = 0x3,
  CDDA_PLAY_MODE_REPEAT2           = 0x4
} cdda_play_mode_enum;

typedef enum
{
  SENSE_KEY_NO_SENSE        = 0,
  SENSE_KEY_ILLEGAL_REQUEST = 5
} sense_key_status_enum;

typedef enum
{
  CD_SECTOR_LOAD_AUDIO,
  CD_SECTOR_LOAD_DATA
} cd_sector_load_type_enum;

// 256KB + 2KB + 2352B + 8KB + 196B
// 266KB + 2548B

typedef struct
{
  // It's possibly important that these go in this order, so they're
  // contiguous. However, only save ext_ram if the proper CD-ROM system
  // type is selected (v3 or Arcade Card)
  u8 ext_ram[1024 * 192];
  u8 work_ram[1024 * 64];

  u8 data_buffer[DATA_BUFFER_SIZE];
  s16 cdda_cache[2352 / 2];

  u8 command_buffer[16];

  u64 last_read_cycles;
  u64 last_cdda_cycles;

  u64 cdda_start_last_cycles;

  u32 data_buffer_top;
  u32 data_buffer_position;
  u32 data_buffer_bytes_written;

  u32 command_buffer_position;

  scsi_bus_state_enum scsi_bus_state;
  u32 req_signal;
  u32 ack_signal;

  u32 data_bus;

  u32 read_sector_address;
  u32 read_sector_count;

  s32 read_cycles;

  u32 irq_enable;
  u32 irq_status;

  cdda_status_enum cdda_status;
  cdda_play_mode_enum cdda_play_mode;

  s32 cdda_cycles;
  u32 cdda_sector_address;
  u32 cdda_start_sector_address;
  u32 cdda_end_sector_address;
  s32 cdda_access_cycles;

  s32 cdda_volume;
  u32 cdda_read_left;
  u32 cdda_read_right;

  u32 cdda_read_offset;
  u32 cdda_audio_buffer_index;

  s32 cdda_fade_cycles;
  u32 cdda_fade_cycle_interval;

  u32 cdda_last_start_pos;

  u32 last_sector_address;
  cd_sector_load_type_enum last_sector_type;

  u32 sub_channel_data;

  u32 bram_enable;

  u32 message;
  u32 status_sent;
  u32 message_sent;
  u32 data_transfer_done;

  u32 sense_key;
  u32 additional_sense_code;
  u32 additional_sense_code_qualifier;
  u32 field_replaceable_unit_code;

  // Don't save these in a savestate, except possibly BRAM -
  // but only as an option. Forcing it in saves is very rude.
  u32 read_sector_cycle_interval;
  u32 access_sector_cycle_interval;
  u32 cdda_sector_cycle_interval;

  u8 bram[1024 * 8];
} cd_struct;

// 8B

typedef struct
{
  u32 argument_count;
  void (* command_function)();
} scsi_command_struct;

extern cd_struct cd;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 

void cd_raise_event(u32 event);
void cd_lower_event(u32 event);
void cd_data_buffer_read();

u32 cd_bus_status();
u32 cd_read_data();
u32 cd_read_data_acknowledge();
u32 cd_read_irq_enable();
u32 cd_irq_status();
u32 cd_read_reset();
u32 cd_read_cdda_low();
u32 cd_read_cdda_high();
u32 cd_read_sub_channel();

void cd_select(u32 value);
void cd_write_data(u32 value);
void cd_acknowledge(u32 value);
void cd_prepare_cdda_read(u32 value);
void cd_enable_bram(u32 value);
void cd_full_volume();
void cd_fade_out_rate(u32 ms);
void cd_fadeout(u32 value);
void cd_write_reset(u32 value);

void initialize_cd();
void reset_cd();

void update_cd_read();
void update_cdda();

void save_bram(char *path);
void load_bram(char *path);
void create_bram(char *path);

void dump_cd_status();

void cd_load_savestate(savestate_load_type savestate_file);
void cd_store_savestate(savestate_store_type savestate_file);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif

