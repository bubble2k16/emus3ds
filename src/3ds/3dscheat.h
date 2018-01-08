#ifndef _3DSCHEAT_H_
#define _3DSCHEAT_H_

#define MAX_CHEATS      100

//----------------------------------------------------------
// This implements a generic text file format (.CHX)
// instead of the emulator's default cheat file formats.
//----------------------------------------------------------
bool cheat3dsLoadCheatTextFile (const char *filename);


//----------------------------------------------------------
// This saves the cheats back into the file, retaining
// the enabled/disabled states of each cheat.
//----------------------------------------------------------
bool cheat3dsSaveCheatTextFile (const char *filename);


//----------------------------------------------------------
// Sets the enabled flag of the cheat.
//----------------------------------------------------------
void cheat3dsSetCheatEnabledFlag(int cheatIndex, bool enabled);

#endif