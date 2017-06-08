#ifndef PLATFORM_DEFINES_H
#define PLATFORM_DEFINES_H

// Everything here must be C preprocessor material only. That way even
// assembler files can load this.

#define DEBUGGER_ON

#ifdef GP2X_BUILD

#define GP2X_OR_WIZ_BUILD

#define ARM_ARCH

#define COLOR_RGB_565

#define OGG_SUPPORT
#define OGG_TREMOR
#define BZ_SUPPORT

#define CONFIG_OPTIONS_CLOCK_SPEED
#define CONFIG_OPTIONS_RAM_TIMINGS
#define CONFIG_OPTIONS_GAMMA

#define CLOCK_SPEED_MIN 25
#define CLOCK_SPEED_MAX 300

#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define MAX_PATH 512
#define make_directory(name) mkdir(name)

#define printf(format, ...)                                                 \
  fprintf(stderr, format, ##__VA_ARGS__)                                    \


#endif

#ifdef IPHONE_BUILD

#define GP2X_OR_WIZ_BUILD

#define ARM_ARCH
#define ARM_V5

#define COLOR_RGB_565

#define OGG_SUPPORT
#define OGG_TREMOR
#define BZ_SUPPORT

#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define MAX_PATH 512
#define make_directory(name) mkdir(name)

#define printf(format, ...)                                                 \
  fprintf(stderr, format, ##__VA_ARGS__)                                    \

#endif


#ifdef WIZ_BUILD

#define GP2X_OR_WIZ_BUILD

#define ARM_ARCH
#define ARM_V5
#define OGG_SUPPORT
#define OGG_TREMOR
#define BZ_SUPPORT

#define COLOR_RGB_565
#define FRAMEBUFFER_PORTRAIT_ORIENTATION

#define CONFIG_OPTIONS_CLOCK_SPEED

#define CLOCK_SPEED_MIN 25
#define CLOCK_SPEED_MAX 900

#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define MAX_PATH 512
#define make_directory(name) mkdir(name, 0755)

#define printf(format, ...)                                                 \
  fprintf(stderr, format, ##__VA_ARGS__)                                    \


#endif


#ifdef WIN32_BUILD

#define SDL_INIT        (SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK)

#define COLOR_RGB_565

#define X86_ARCH
#define OGG_SUPPORT
#define BZ_SUPPORT
#define DIR_SEPARATOR_CHAR '\\'
#define DIR_SEPARATOR_CHAR_STR "\\"
#define make_directory(name) mkdir(name)

#endif


#ifdef LINUX_X86_BUILD

#define SDL_INIT        (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)

#define COLOR_RGB_565

#define LINUX_PLATFORM
#define LETSGO_HOME
#define CRC_CHECK
/*#define OGG_SUPPORT
#define BZ_SUPPORT*/
#define KEYBOARD_SUPPORT
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define make_directory(name) mkdir(name, 0755)
#define MAX_PATH 1024

#endif


#ifdef PANDORA_BUILD

#define SDL_INIT                                                               \
 (SDL_INIT_AUDIO | SDL_INIT_EVENTTHREAD | SDL_INIT_JOYSTICK)

#define COLOR_RGB_565

#define MAX_PATH 512

#define LINUX_PLATFORM
#define ARM_ARCH
#define ARM_V5
#define OGG_SUPPORT
#define OGG_TREMOR
#define KEYBOARD_SUPPORT
#define BZ_SUPPORT
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"

#define make_directory(name) mkdir(name, 0777)

#endif


// TODO: OGG support

#ifdef PSP_BUILD

#define COLOR_BGR_565

#define CONFIG_OPTIONS_CLOCK_SPEED

#define MIPS_ARCH
#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define MAX_PATH 512
#define make_directory(name) mkdir(name, 0755)

#endif

#ifdef GCW_BUILD

#define SDL_INIT        (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK)

#define COLOR_RGB_565
#define CRC_CHECK

#define LINUX_PLATFORM
#define LETSGO_HOME
#define KEYBOARD_SUPPORT
#define CONFIG_OPTIONS_GAMMA

#define OGG_SUPPORT
#define OGG_TREMOR
#define BZ_SUPPORT

#define DIR_SEPARATOR_CHAR '/'
#define DIR_SEPARATOR_CHAR_STR "/"
#define MAX_PATH 512
#define make_directory(name) mkdir(name, 0755)

#endif

#endif

