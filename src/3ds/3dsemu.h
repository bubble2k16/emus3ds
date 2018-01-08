
#ifndef _3DSEMU_H_
#define _3DSEMU_H_

#include "3dstypes.h"


// Uncomment this to convert before releasing this to remove
// all the debugging stuff.
//
#define EMU_RELEASE 

// Uncomment this to allow user to break into debug mode (for the 65816 CPU)
//
//#define DEBUG_CPU

// Uncomment this to allow user to break into debug mode (for the SPC700 APU)
//
//#define DEBUG_APU



#define DEBUG_WAIT_L_KEY 	                                                \
    {                                                                       \
        hidScanInput();                                                     \
        uint32 prevkey = hidKeysHeld();                                     \
        while (aptMainLoop())                                               \
        {                                                                   \
            hidScanInput();                                                 \
            uint32 key = hidKeysHeld();                                     \
            if (key == KEY_L) break;                                        \
            if (key == KEY_TOUCH) break;                                    \
            if (prevkey == 0 && key != 0)                                   \
                break;                                                      \
            if (key == KEY_SELECT)                                          \
                { emulator.enableDebug = !emulator.enableDebug; break; }    \
            prevkey = key;                                                  \
        }                                                                   \ 
    }


//---------------------------------------------------------
// Information about the emulator.
//---------------------------------------------------------
typedef struct
{
    bool                isReal3DS;
    bool                enableDebug;
    int                 emulatorState;
    int                 waitBehavior;
    bool                fastForwarding;
} SEmulator;

extern SEmulator emulator;
    

#endif