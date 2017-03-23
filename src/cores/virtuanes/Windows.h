
// Stub for Windows.h

#ifndef	__WINDOWS_INCLUDED__
#define	__WINDOWS_INCLUDED__

#include "Typedef.h"
// stubs
typedef char *LPSTR;

typedef char *LPCSTR;

typedef char *LPCTSTR;

typedef int *LPINT;

typedef int HWND;

typedef struct _RECT {
    int left;
    int top;
    int right;
    int bottom;
} RECT, *PRECT;

typedef struct tagRGBQUAD {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
    BYTE rgbReserved;
} RGBQUAD;


typedef int LCID;

#define MAX_PATH       512
#define _MAX_PATH       512
#define _MAX_FNAME      512
#define _MAX_EXT        512
#define _MAX_DRIVE      512
#define _MAX_DIR        512

#define SLASH_STR       "/"
#define SLASH_CHAR      '/'


#define DEBUGOUT(s)

#define FillMemory(ptr, len, byt)   memset(ptr, byt, len);
#define ZeroMemory(ptr, len)        memset(ptr, 0, len);

#define wsprintf sprintf

#define __cdecl 


#endif