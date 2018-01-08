
#include <stdio.h>

#include "3dsgpu.h"
#include "3dsmain.h"
#include "3dssound.h"

#include "3dsinterface.h"

static u32 currKeysHeld = 0;
static u32 lastKeysHeld = 0;

//int adjustableValue = 0x70;

//---------------------------------------------------------
// Reads and processes Joy Pad buttons.
//
// This should be called only once every frame only in the
// emulator loop. For all other purposes, you should
// use the standard hidScanInput.
//---------------------------------------------------------
u32 input3dsScanInputForEmulation()
{
    hidScanInput();
    currKeysHeld = hidKeysHeld();

    u32 keysDown = (~lastKeysHeld) & currKeysHeld;

#ifndef EMU_RELEASE
    // -----------------------------------------------
    // For debug only
    // -----------------------------------------------
    if (emulator.enableDebug)
    {
        keysDown = keysDown & (~lastKeysHeld);
        if (keysDown || (currKeysHeld & KEY_L))
        {
            //printf ("  kd:%x lkh:%x nkh:%x\n", keysDown, lastKeysHeld, currKeysHeld);
            //Settings.Paused = false;
        }
        else
        {
            //printf ("  kd:%x lkh:%x nkh:%x\n", keysDown, lastKeysHeld, currKeysHeld);
            //Settings.Paused = true;
        }
    }

    if (keysDown & (KEY_R))
    {
        emulator.enableDebug = !emulator.enableDebug;
        printf ("Debug mode = %d\n", emulator.enableDebug);
    }

    /*if (keysDown & (KEY_L))
    {
        adjustableValue -= 1;
        printf ("Adjust: %d\n", adjustableValue);
    }
    if (keysDown & (KEY_R))
    {
        adjustableValue += 1;
        printf ("Adjust: %d\n", adjustableValue);
    }*/
    // -----------------------------------------------
#endif

    if (keysDown & KEY_TOUCH)
    {
        snd3dsStopPlaying();
        impl3dsEmulationPaused();

        if (emulator.emulatorState == EMUSTATE_EMULATE)
            emulator.emulatorState = EMUSTATE_PAUSEMENU;
    }
    lastKeysHeld = currKeysHeld;
    return keysDown;

}


//---------------------------------------------------------
// Get the bitmap of keys currently held on by the user
//---------------------------------------------------------
u32 input3dsGetCurrentKeysHeld()
{
    return currKeysHeld;
}