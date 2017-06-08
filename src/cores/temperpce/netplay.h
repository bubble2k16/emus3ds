#ifndef NETPLAY_H
#define NETPLAY_H

#define CONTROL_BUFFER_SIZE          1024

#define STALL_PERIOD                 5
#define LATENCY_CALCULATION_PERIOD   60

#define NETPLAY_SAVESTATE_BLOCK_SIZE (8 * 1024)

typedef enum
{
  NETPLAY_TYPE_NONE,
  NETPLAY_TYPE_SERVER,
  NETPLAY_TYPE_CLIENT,
} netplay_type_enum;

typedef enum
{
  NETPLAY_PACKET_TYPE_CONTROLS                = 0,
  NETPLAY_PACKET_TYPE_MESSAGE                 = 1,
  NETPLAY_PACKET_TYPE_SET_LATENCY             = 2,
  NETPLAY_PACKET_TYPE_LATENCY_REPORT          = 3,
  NETPLAY_PACKET_TYPE_PAUSE                   = 4,
  NETPLAY_PACKET_TYPE_UNPAUSE                 = 5,
  NETPLAY_PACKET_TYPE_SAVESTATE_START         = 6,
  NETPLAY_PACKET_TYPE_SAVESTATE_BLOCK         = 7,
  NETPLAY_PACKET_TYPE_SAVESTATE_BLOCK_CONFIRM = 8,
} netplay_packet_type_enum;

typedef enum
{
  NETPLAY_PAUSE_ACCEPTING_CONNECTION,
  NETPLAY_PAUSE_MENU,
  NETPLAY_PAUSE_SENDING_SAVESTATE,
  NETPLAY_PAUSE_RECEIVING_SAVESTATE,
  NETPLAY_PAUSE_RECEIVE_STALL,
} netplay_pause_type_enum;

typedef struct
{
  u16 frame_number;
  u16 controls;
} control_buffer_entry_struct;

typedef struct
{
  control_buffer_entry_struct entries[CONTROL_BUFFER_SIZE];
  control_buffer_entry_struct *last_written;
  u32 write_position;
} control_buffer_struct;

#define RECEIVE_BUFFER_LENGTH (32 * 1024)
#define SEND_BUFFER_LENGTH    (32 * 1024)

typedef struct
{
  u8 receive_buffer[RECEIVE_BUFFER_LENGTH];
  u8 send_buffer[SEND_BUFFER_LENGTH];

  control_buffer_struct control_buffer_local;
  control_buffer_struct control_buffer_remote;

  u32 frame_latency;
  s32 socket_handle;
  u32 stalls;
  s32 total_latency;
  u32 latency_calculation_frames;

  u32 receive_bytes_remaining;
  u8 *receive_buffer_ptr;

  u32 send_control_buffer_limit;
  u32 send_control_packets_buffered;
  u32 send_buffer_bytes;
  u8 *send_buffer_ptr;

  u32 active;
  u32 can_send;
  u32 pause;
  netplay_pause_type_enum pause_type;

  s32 period_latency;
  u32 period_stalls;

  s32 remote_latency_report;
  u32 remote_stalls_report;

  u32 frame_latency_update_at_frame;
  u32 frame_latency_update_to;

  u32 frame_number;  

  u8 *savestate_send_buffer;
  u8 *savestate_send_ptr;
  u32 savestate_send_bytes_remaining;
  s32 savestate_send_block;
  s32 savestate_send_block_confirmed;

  u8 *savestate_receive_buffer;
  u8 *savestate_receive_ptr;
  u32 savestate_receive_size;
  u32 savestate_receive_bytes_remaining;
} netplay_struct;

extern netplay_struct netplay;

#ifdef EXTERN_C_START
EXTERN_C_START
#endif 


u32 netplay_ip_string_value(const char *ip_string);

void netplay_connect(void);
void netplay_frame_update(u32 new_local_controls,
 u32 *_use_local_controls, u32 *_use_remote_controls);
void send_talk_message(char *message);

void send_netplay_pause(void);
void send_netplay_unpause(void);

s32 netplay_send_savestate(char *file_name);

#ifdef EXTERN_C_END
EXTERN_C_END
#endif 


#endif

