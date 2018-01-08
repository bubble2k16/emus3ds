#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef uint8
typedef uint8_t uint8;
typedef int8_t int8;
#endif

#ifndef uint16
typedef uint16_t uint16;
typedef int16_t int16;
#endif

#ifndef uint32
typedef uint32_t uint32;
typedef int32_t int32;
#endif

#ifndef uint64
typedef uint64_t uint64;
typedef int64_t int64;
#endif

#define _MAX_PATH                   512
#define SLASH_CHAR                  '/'

#define TICKS_PER_SEC               (268123480)
#define TICKS_PER_FRAME_NTSC        (4468724)
#define TICKS_PER_FRAME_PAL         (5362469)
#define TICKS_PER_FRAME_FASTFORWARD (1489574)

