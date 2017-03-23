
#ifndef _3DSMAIN_H_
#define _3DSMAIN_H_

//---------------------------------------------------------
// Information about the emulator.
//---------------------------------------------------------
typedef struct
{
    bool                isReal3DS = false;
    bool                enableDebug = false;
    int                 emulatorState = 0;
} SEmulator;

extern SEmulator emulator;

extern char romFileNameFullPath[];
extern char romFileNameLastSelected[];

//---------------------------------------------------------
// The different emulator states.
//---------------------------------------------------------
#define EMUSTATE_EMULATE        1
#define EMUSTATE_PAUSEMENU      2
#define EMUSTATE_END            3

#endif
