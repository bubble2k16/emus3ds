#ifndef _3DSDBG_H_
#define _3DSDBG_H_

#include "3ds.h"
#include "3dsmain.h"
#include "3dsemu.h"
#include "stdio.h"


#ifndef EMU_RELEASE
/*
    static void dbgprintf (const char *format, int a, int b, int c, int d)
    {
        char buffer[2048];
        va_list args;
        va_start (args, format);
        vsprintf (buffer, format, args);       
        va_end(args);

        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, buffer);    
        fprintf(fp, format, a, b, c, d);    
        fclose(fp);
    }
*/
    static void dbgprintf0 (const char *format)
    {
        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, format);    
        fclose(fp);
    }
    static void dbgprintf1 (const char *format, int a)
    {
        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, format, a);    
        fclose(fp);
    }
    static void dbgprintf2 (const char *format, int a, int b)
    {
        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, format, a, b);    
        fclose(fp);
    }
    static void dbgprintf3 (const char *format, int a, int b, int c)
    {
        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, format, a, b, c);    
        fclose(fp);
    }
    static void dbgprintf4 (const char *format, int a, int b, int c, int d)
    {
        FILE *fp = fopen("dbg.txt", "a");
        fprintf(fp, format, a, b, c, d);    
        fclose(fp);
    }
#else
    static void dbgprintf0 (const char *format) {}
    static void dbgprintf1 (const char *format, int a) {}
    static void dbgprintf2 (const char *format, int a, int b) {}
    static void dbgprintf3 (const char *format, int a, int b, int c) {}
    static void dbgprintf4 (const char *format, int a, int b, int c, int d) {}
#endif


#define CLEAR_BOTTOM_SCREEN \
    gfxSetDoubleBuffering(GFX_BOTTOM,false); \
    gfxSwapBuffers(); \
    consoleInit(GFX_BOTTOM, NULL); \
    consoleClear(); \



#endif

