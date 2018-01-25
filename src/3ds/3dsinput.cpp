
#include <stdio.h>

#include "3dsemu.h"
#include "3dsgpu.h"
#include "3dsmain.h"
#include "3dssound.h"
#include "3dsinput.h"

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

    if ((keysDown & KEY_TOUCH) || 
        (settings3DS.UseGlobalEmuControlKeys && (keysDown & settings3DS.GlobalButtonHotkeyOpenMenu)) ||
        (!settings3DS.UseGlobalEmuControlKeys && (keysDown & settings3DS.ButtonHotkeyOpenMenu)) 
        )
    {
        snd3dsStopPlaying();
        impl3dsEmulationPaused();

        if (emulator.emulatorState == EMUSTATE_EMULATE)
            emulator.emulatorState = EMUSTATE_PAUSEMENU;
    }
    lastKeysHeld = currKeysHeld;
    return keysDown;

}


u32 prevConsoleJoyPad;
u32 prevConsoleButtonPressed[10];
u32 buttons3dsPressed[10];

//---------------------------------------------------------
// Processes inputs and maps to the console's
// joy pad keys.
//
// Returns a u32 bitmap containing the keys pressed.
//---------------------------------------------------------
u32 input3dsProcess3dsKeys()
{
	u32 keysHeld3ds = input3dsGetCurrentKeysHeld();
    u32 consoleJoyPad = 0;

    if (keysHeld3ds & KEY_UP) consoleJoyPad |= input3dsDKeys[0];
    if (keysHeld3ds & KEY_DOWN) consoleJoyPad |= input3dsDKeys[1];
    if (keysHeld3ds & KEY_LEFT) consoleJoyPad |= input3dsDKeys[2];
    if (keysHeld3ds & KEY_RIGHT) consoleJoyPad |= input3dsDKeys[3];

	#define SET_CONSOLE_JOYPAD(i, mask, buttonMapping) 				\
		buttons3dsPressed[i] = (keysHeld3ds & mask);				\
		if (keysHeld3ds & mask) 									\
			consoleJoyPad |= 										\
				buttonMapping[i][0] |								\
				buttonMapping[i][1] |								\
				buttonMapping[i][2] |								\
				buttonMapping[i][3];								\

	if (settings3DS.UseGlobalButtonMappings)
	{
		SET_CONSOLE_JOYPAD(BTN3DS_L, KEY_L, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_R, KEY_R, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_A, KEY_A, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_B, KEY_B, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_X, KEY_X, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_Y, KEY_Y, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_SELECT, KEY_SELECT, settings3DS.GlobalButtonMapping);
		SET_CONSOLE_JOYPAD(BTN3DS_START, KEY_START, settings3DS.GlobalButtonMapping);
		SET_CONSOLE_JOYPAD(BTN3DS_ZL, KEY_ZL, settings3DS.GlobalButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_ZR, KEY_ZR, settings3DS.GlobalButtonMapping)
	}
	else
	{
		SET_CONSOLE_JOYPAD(BTN3DS_L, KEY_L, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_R, KEY_R, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_A, KEY_A, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_B, KEY_B, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_X, KEY_X, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_Y, KEY_Y, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_SELECT, KEY_SELECT, settings3DS.ButtonMapping);
		SET_CONSOLE_JOYPAD(BTN3DS_START, KEY_START, settings3DS.ButtonMapping);
		SET_CONSOLE_JOYPAD(BTN3DS_ZL, KEY_ZL, settings3DS.ButtonMapping)
		SET_CONSOLE_JOYPAD(BTN3DS_ZR, KEY_ZR, settings3DS.ButtonMapping)
	}


    // Handle turbo / rapid fire buttons.
    //
    int *turbo = settings3DS.Turbo;
    if (settings3DS.UseGlobalTurbo)
        turbo = settings3DS.GlobalTurbo;
    
    #define HANDLE_TURBO(i, buttonMapping) 										\
		if (turbo[i] && buttons3dsPressed[i]) { 		\
			if (!prevConsoleButtonPressed[i]) 						\
			{ 														\
				prevConsoleButtonPressed[i] = 11 - turbo[i]; 		\
			} 														\
			else 													\
			{ 														\
				prevConsoleButtonPressed[i]--; 						\
				consoleJoyPad &= ~(									\
				buttonMapping[i][0] |								\
				buttonMapping[i][1] |								\
				buttonMapping[i][2] |								\
				buttonMapping[i][3]									\
				); \
			} \
		} \

	if (settings3DS.UseGlobalButtonMappings)
	{
		HANDLE_TURBO(BTN3DS_A, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_B, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_X, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_Y, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_L, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_R, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_ZL, settings3DS.GlobalButtonMapping);
		HANDLE_TURBO(BTN3DS_ZR, settings3DS.GlobalButtonMapping);
	}
	else
	{
		HANDLE_TURBO(BTN3DS_A, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_B, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_X, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_Y, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_L, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_R, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_ZL, settings3DS.ButtonMapping);
		HANDLE_TURBO(BTN3DS_ZR, settings3DS.ButtonMapping);
	}

    prevConsoleJoyPad = consoleJoyPad;
    return consoleJoyPad;
}


//---------------------------------------------------------
// Sets the default buttons / turbo values.
//
// Pass the settings3DS.GlobalButtonMapping or 
// settings3DS.ButtonMapping into the first parameter.
//
// Pass the settings3DS.GlobalTurbo or 
// settings3DS.Turbo into the second parameter.
//---------------------------------------------------------
void input3dsSetDefaultButtonMappings(int buttonMapping[10][4], int turbo[8], bool overwrite)
{
    for (int i = 0; i < 10; i++)
    {
        bool allZero = true;

        for (int j = 0; j < 4; j++)
        {
            // Validates all button mapping input,
            // assign to zero, if invalid.
            //
            bool isValid = false;
            for (int k = 0; k < 10; k++)
                if (buttonMapping[i][j] == input3dsValidButtonMappings[k])
                {
                    isValid = true;
                    break;
                }
            if (!isValid)
                buttonMapping[i][j] = 0;

            if (buttonMapping[i][j])
                allZero = false;
        }
        if (allZero || overwrite)
            buttonMapping[i][0] = input3dsDefaultButtonMappings[i];
    }

    if (overwrite)
    {
        for (int i = 0; i < 8; i++)
            turbo[i] = 0;
    }
}


//---------------------------------------------------------
// Get the bitmap of keys currently held on by the user
//---------------------------------------------------------
u32 input3dsGetCurrentKeysHeld()
{
    return currKeysHeld;
}