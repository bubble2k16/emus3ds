#ifndef COMMON_H
#define COMMON_H

#define MAX_PATH 512
#define DIR_SEPARATOR_CHAR '/'
/*
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short int u16;
typedef signed short int s16;
*/
// For some reason, other libraries define these on PSP

/*
#ifndef PSP_BUILD
typedef unsigned int u32;
typedef signed int s32;
#endif
*/
/*
typedef unsigned long long int u64;
typedef signed long long int s64;
*/

#include "3ds.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <stdarg.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <ctype.h>

#include <dirent.h>

#include <math.h>

#include "platform_defines.h"

#ifdef CRC_CHECK
#include "crc32.h"
#endif

#ifdef SDL_INIT
  #include <SDL.h>
#endif

#ifdef BZ_SUPPORT
  #include <bzlib.h>
#endif

#ifdef GP2X_BUILD
  #include "gp2x/gp2x_common.h"
#endif

#define TEMPER_VERSION "1.24.1 GCW"
#define CD_SWAP_OPTION
#define FASTFORWARD_FRAMESKIP
#define FASTFORWARD_FRAMESKIP_RATE 4

#ifdef OGG_SUPPORT

#ifdef OGG_TREMOR
#include <tremor/ivorbisfile.h>
#else
#include <vorbis/vorbisfile.h>
#endif

#endif


#define file_read(file_handle, ptr, size)                                     \
  fread(ptr, size, 1, file_handle)                                            \

#define file_write(file_handle, ptr, size)                                    \
  fwrite(ptr, size, 1, file_handle)                                           \

#define file_read_mem(file_handle, ptr, size)                                 \
{                                                                             \
  memcpy(ptr, file_handle->buffer_ptr, size);                                 \
  file_handle->buffer_ptr += size;                                            \
}                                                                             \

#define file_write_mem(file_handle, ptr, size)                                \
{                                                                             \
  memcpy(file_handle->buffer_ptr, ptr, size);                                 \
  file_handle->buffer_ptr += size;                                            \
}                                                                             \


#define file_read_open(file_handle, file_name)                                \
  FILE *file_handle = fopen(file_name, "rb")                                  \

#define file_write_open(file_handle, file_name)                               \
  FILE *file_handle = fopen(file_name, "wb")                                  \

#define file_read_mem_open(file_handle, file_name, _buffer, _memory_file,     \
 _memory_file_size)                                                           \
  mem_file_struct _##file_handle;                                             \
  mem_file_struct *file_handle = &_##file_handle;                             \
                                                                              \
  if(_memory_file)                                                            \
  {                                                                           \
    _##file_handle.memory_file = _memory_file;                                \
    _##file_handle.memory_file_size = _memory_file_size;                      \
    _##file_handle._file = NULL;                                              \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    _##file_handle.memory_file = NULL;                                        \
    _##file_handle._file = fopen(file_name, "rb");                            \
  }                                                                           \
  _##file_handle.buffer = _buffer;                                            \
  _##file_handle.buffer_ptr = _buffer                                         \

#define file_write_mem_open(file_handle, file_name, _buffer)                  \
  mem_file_struct _##file_handle;                                             \
  mem_file_struct *file_handle = &_##file_handle;                             \
  _##file_handle._file = fopen(file_name, "wb");                              \
  _##file_handle.buffer = _buffer;                                            \
  _##file_handle.buffer_ptr = _buffer                                         \


#define file_read_invalid(file_handle)                                        \
  (file_handle == NULL)                                                       \

#define file_write_invalid(file_handle)                                       \
  (file_handle == NULL)                                                       \

#define file_read_mem_invalid(file_handle)                                    \
  ((file_handle->memory_file == NULL) && (file_handle->_file == NULL))        \

#define file_write_mem_invalid(file_handle)                                   \
  (file_handle->_file == NULL)                                                \


#define file_read_mem_load(file_handle)                                       \
  if(file_handle->memory_file)                                                \
  {                                                                           \
    memcpy(file_handle->buffer_ptr, file_handle->memory_file,                 \
     file_handle->memory_file_size);                                          \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    u32 _file_size;                                                           \
    u32 _current_position = ftell(file_handle->_file);                        \
    fseek(file_handle->_file, 0, SEEK_END);                                   \
    _file_size = ftell(file_handle->_file) - _current_position;               \
    fseek(file_handle->_file, _current_position, SEEK_SET);                   \
    fread(file_handle->buffer_ptr, _file_size, 1, file_handle->_file);        \
  }                                                                           \

#define file_read_mem_load_fixed(file_handle, _size)                          \
  if(file_handle->memory_file)                                                \
  {                                                                           \
    memcpy(file_handle->buffer_ptr, file_handle->memory_file, _size);         \
    file_handle->memory_file += _size;                                        \
    file_handle->memory_file_size -= _size;                                   \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    fread(file_handle->buffer_ptr, _size, 1, file_handle->_file);             \
  }                                                                           \

#define file_read_mem_load_null(file_handle, _size)                           \
  if(file_handle->memory_file)                                                \
  {                                                                           \
    file_handle->memory_file += _size;                                        \
    file_handle->memory_file_size -= _size;                                   \
  }                                                                           \
  else                                                                        \
  {                                                                           \
    fseek(file_handle->_file, _size, SEEK_CUR);                               \
  }                                                                           \

#define file_write_mem_save(file_handle)                                      \
  fwrite(file_handle->buffer, file_handle->buffer_ptr - file_handle->buffer,  \
   1, file_handle->_file)                                                     \

#define file_write_mem_save_fixed(file_handle, _size)                         \
  fwrite(file_handle->buffer, _size, 1, file_handle->_file)                   \


#define file_read_close(file_handle)                                          \
  fclose(file_handle)                                                         \

#define file_write_close(file_handle)                                         \
  fclose(file_handle)                                                         \

#define file_read_mem_close(file_handle)                                      \
  fclose(file_handle->_file)                                                  \

#define file_write_mem_close(file_handle)                                     \
  fclose(file_handle->_file)                                                  \



#define file_read_skip_forward(file_handle, amount)                           \
  fseek(file_handle, amount, SEEK_CUR)                                        \

#define file_write_skip_forward(file_handle, amount)                          \
  fseek(file_handle, amount, SEEK_CUR)                                        \

#define file_read_mem_skip_forward(file_handle, amount)                       \
  file_handle->buffer_ptr += amount                                           \

#define file_write_mem_skip_forward(file_handle, amount)                      \
  file_handle->buffer_ptr += amount                                           \


// Some helpers to make writing things with compile-time defined sizes a
// little easier. If you use this on something that doesn't have a
// compile-time known size (like, a pointer instead of a static array) then
// you will be totally screwed.

#define file_read_variable(file_handle, variable)                             \
  file_read(file_handle, &variable, sizeof(variable))                         \

#define file_write_variable(file_handle, variable)                            \
  file_write(file_handle, &variable, sizeof(variable))                        \

#define file_read_mem_variable(file_handle, variable)                         \
  file_read_mem(file_handle, &variable, sizeof(variable))                     \

#define file_write_mem_variable(file_handle, variable)                        \
  file_write_mem(file_handle, &variable, sizeof(variable))                    \


#define file_read_array(file_handle, array)                                   \
  file_read(file_handle, array, sizeof(array))                                \

#define file_write_array(file_handle, array)                                  \
  file_write(file_handle, array, sizeof(array))                               \

#define file_read_mem_array(file_handle, array)                               \
  file_read_mem(file_handle, array, sizeof(array))                            \

#define file_write_mem_array(file_handle, array)                              \
  file_write_mem(file_handle, array, sizeof(array))                           \


#define percent_of(numerator, denominator)                                    \
  (((numerator) * 100.0) / (double)(denominator))                             \

//#define savestate_load_method read
#define savestate_load_method read_mem
#define savestate_store_method write_mem

//#define savestate_load_type FILE *
#define savestate_load_type mem_file_struct *
#define savestate_store_type mem_file_struct *

// This isn't really a file, it's actually a struct casing an array. Good
// for when you want to buffer something before spitting it out to a file.
// Before that, here's the actual format of the struct:

typedef struct
{
  FILE *_file;
  u8 *memory_file;
  u32 memory_file_size;

  u8 *buffer;
  u8 *buffer_ptr;
} mem_file_struct;


#include "cpu.h"
#include "debug.h"
#include "memory.h"
#include "video.h"
#include "video_hw.h"
#include "irq.h"
#include "io.h"
#include "timer.h"
#include "psg.h"
#include "main.h"
#include "disasm.h"
#include "event.h"
#include "menu.h"
#include "cd.h"
#include "adpcm.h"
#include "arcade_card.h"
#include "netplay.h"
#include "screen.h"
#include "audio.h"

#include "bin_cue.h"


#ifdef LINUX_PLATFORM

#include <fcntl.h>
#include <strings.h>
#include <sched.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <errno.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#endif

#include <getopt.h>

#ifdef WIN32_BUILD

#include <winsock.h>

#endif

#endif

