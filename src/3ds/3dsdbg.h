#ifndef _3DSDBG_H_
#define _3DSDBG_H_

#include "3ds.h"
#include "3dsmain.h"
#include "3dsemu.h"
#include "stdio.h"
#include "cstdarg"


#ifndef EMU_RELEASE
    static void dbgprintf (const char *format, ...)
    {
        char buffer[2048];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);       
        va_end(args);

        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, buffer);    
        fclose(fp);
    }
#else
    static void dbgprintf (const char *format, ...) {}
#endif


#define CLEAR_BOTTOM_SCREEN \
    gfxSetDoubleBuffering(GFX_BOTTOM,false); \
    gfxSwapBuffers(); \
    consoleInit(GFX_BOTTOM, NULL); \
    consoleClear(); \



#endif

