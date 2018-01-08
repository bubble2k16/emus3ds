
#ifndef _3DSINPUT_H
#define _3DSINPUT_H

#include <3ds.h>

#define BTN3DS_A        0
#define BTN3DS_B        1
#define BTN3DS_X        2
#define BTN3DS_Y        3
#define BTN3DS_L        4
#define BTN3DS_R        5
#define BTN3DS_ZL       6
#define BTN3DS_ZR       7
#define BTN3DS_SELECT   8
#define BTN3DS_START    9


//---------------------------------------------------------
// Reads and processes Joy Pad buttons.
//
// This should be called only once every frame only in the
// emulator loop. For all other purposes, you should
// use the standard hidScanInput.
//---------------------------------------------------------
u32 input3dsScanInputForEmulation();


//---------------------------------------------------------
// Processes inputs and maps to the console's
// joy pad keys.
//
// Returns a u32 bitmap containing the keys pressed.
//---------------------------------------------------------
u32 input3dsProcess3dsKeys();


//---------------------------------------------------------
// Sets the default buttons / turbo values.
//
// Pass the settings3DS.GlobalButtonMapping or 
// settings3DS.ButtonMapping into the first parameter.
//
// Pass the settings3DS.GlobalTurbo or 
// settings3DS.Turbo into the second parameter.
//---------------------------------------------------------
void input3dsSetDefaultButtonMappings(int buttonMapping[10][4], int turbo[8], bool overwrite);


//---------------------------------------------------------
// Get the bitmap of keys currently held on by the user
//---------------------------------------------------------
u32 input3dsGetCurrentKeysHeld();

#endif