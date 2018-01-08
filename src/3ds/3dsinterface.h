#include "3dsemu.h"
#include "3dsgpu.h"
#include "3dsmenu.h"
#include "3dstypes.h"

#ifndef _3DSINTERFACE_H
#define _3DSINTERFACE_H


//---------------------------------------------------------
// Global settings and common core settings.
//---------------------------------------------------------
typedef struct
{
    // Global Settings
    //---------------------
    int     ScreenStretch = 0;              // 0 - no stretch, 1 - stretch full, 2 - aspect fit

    int     StretchWidth, StretchHeight;

    int     CropPixels;

    int     Font = 0;                       // 0 - Tempesta, 1 - Ronda, 2 - Arial

    int     HideUnnecessaryBottomScrText = 0;
                                            // Feature: add new option to disable unnecessary bottom screen text.
                                            // 0 - Default show FPS and "Touch screen for menu" text, 1 - Hide those text.

    int     UseGlobalButtonMappings = 0;    // Use global button mappings for all games
                                            // 0 - no, 1 - yes

    int     UseGlobalTurbo = 0;             // Use global button mappings for all games
                                            // 0 - no, 1 - yes

    int     UseGlobalVolume = 0;            // Use global button mappings for all games
                                            // 0 - no, 1 - yes

    int     GlobalButtonMapping[6] = {0, 0, 0, 0, 0, 0};  
                                            // Button Mapping using 3DS buttons: 0 - A, 1 - B, 2 - X, 3 - Y, 4 - L, 5 - R, 6 - ZL, 7 - ZR
                                            // Values: Specific to each core.

    int     GlobalTurbo[6] = {0, 0, 0, 0, 0, 0};  
                                            // Turbo buttons: 0 - No turbo, 1 - Release/Press every alt frame.
                                            // Indexes for 3DS buttons: 0 - A, 1 - B, 2 - X, 3 - Y, 4 - L, 5 - R

    int     GlobalVolume = 4;               // 0: 100%, 4: 200%, 8: 400%


    // Game-Specific Settings
    //-----------------------------------------------

    int     MaxFrameSkips = 1;              // 0 - disable,
                                            // 1 - enable (max 1 consecutive skipped frame)
                                            // 2 - enable (max 2 consecutive skipped frames)
                                            // 3 - enable (max 3 consecutive skipped frames)
                                            // 4 - enable (max 4 consecutive skipped frames)

    int     ForceFrameRate = 0;             // 0 - Use ROM's Region, 1 - Force 50 fps, 2 - Force 60 fps

    int     Turbo[6] = {0, 0, 0, 0, 0, 0};  // Turbo buttons: 0 - No turbo, 1 - Release/Press every alt frame.
                                            // Indexes for 3DS buttons: 0 - A, 1 - B, 2 - X, 3 - Y, 4 - L, 5 - R

    int     ButtonMapping[6] = {0, 0, 0, 0, 0, 0};  
                                            // Button Mapping using 3DS buttons: 0 - A, 1 - B, 2 - X, 3 - Y, 4 - L, 5 - R, 6 - ZL, 7 - ZR
                                            // Values: Specific to each core.

    long    TicksPerFrame = TICKS_PER_FRAME_NTSC;     
                                            // Ticks per frame. Will change depending on PAL/NTSC

    // Other Game-Specific Settings
    // (each core may implement this differently)
    //-----------------------------------------------

    int     Volume = 4;                     // 0: 100%, 4: 200%, 8: 300%

    int     PaletteFix;                     // Palette Fixes Changes (if applicable)

    int     SRAMSaveInterval;               // SRAM Save Interval (if applicable)

    int     OtherOptions[50];               // Use this to store other options.

} SSettings3DS;


//---------------------------------------------------------
// Provide a comma-separated list of file extensions
//---------------------------------------------------------
extern char *impl3dsRomExtensions;


//---------------------------------------------------------
// The title image .PNG filename.
//---------------------------------------------------------
extern char *impl3dsTitleImage;


//---------------------------------------------------------
// The title that displays at the bottom right of the
// menu.
//---------------------------------------------------------
extern char *impl3dsTitleText;


//---------------------------------------------------------
// Initializes the emulator core.
//
// You must call snd3dsSetSampleRate here to set 
// the CSND's sampling rate.
//---------------------------------------------------------
bool impl3dsInitializeCore();


//---------------------------------------------------------
// Finalizes and frees up any resources.
//---------------------------------------------------------
void impl3dsFinalize();


//---------------------------------------------------------
// Mix sound samples into a temporary buffer.
//
// This gives time for the sound generation to execute
// from the 2nd core before copying it to the actual
// output buffer.
//---------------------------------------------------------
void impl3dsGenerateSoundSamples();


//---------------------------------------------------------
// Mix sound samples into a temporary buffer.
//
// This gives time for the sound generation to execute
// from the 2nd core before copying it to the actual
// output buffer.
//---------------------------------------------------------
void impl3dsOutputSoundSamples(short *leftSamples, short *rightSamples);



//---------------------------------------------------------
// This is called when a ROM needs to be loaded and the
// emulator engine initialized.
//---------------------------------------------------------
bool impl3dsLoadROM(char *romFilePath);


//---------------------------------------------------------
// This is called to determine what the frame rate of the
// game based on the ROM's region.
//---------------------------------------------------------
int impl3dsGetROMFrameRate();


//---------------------------------------------------------
// This is called when the user chooses to reset the
// console
//---------------------------------------------------------
void impl3dsResetConsole();


//---------------------------------------------------------
// This is called when preparing to start emulating
// a new frame. Use this to do any preparation of data
// and the hardware before the frame is emulated.
//---------------------------------------------------------
void impl3dsPrepareForNewFrame();


//---------------------------------------------------------
// Initialize any variables or state of the GPU
// before the emulation loop begins.
//---------------------------------------------------------
void impl3dsEmulationBegin();


//---------------------------------------------------------
// Executes one frame and draw to the screen.
//
// Note: TRUE will be passed in the firstFrame if this
// frame is to be run just after the emulator has booted
// up or returned from the menu.
//---------------------------------------------------------
void impl3dsEmulationRunOneFrame(bool firstFrame, bool skipDrawingFrame);


//---------------------------------------------------------
// This is called when the bottom screen is touched
// during emulation, and the emulation engine is ready
// to display the pause menu.
//---------------------------------------------------------
void impl3dsEmulationPaused();


//---------------------------------------------------------
// This is called when the user chooses to save the state.
// This function should save the state into a file whose
// name contains the slot number. This will return
// true if the state is saved successfully.
//---------------------------------------------------------
bool impl3dsSaveState(int slotNumber);


//---------------------------------------------------------
// This is called when the user chooses to load the state.
// This function should save the state into a file whose
// name contains the slot number. This will return
// true if the state is loaded successfully.
//---------------------------------------------------------
bool impl3dsLoadState(int slotNumber);


//---------------------------------------------------------
// This menu will be displayed when the emulator
// is paused.
//---------------------------------------------------------
extern SMenuItem optionMenu[];


//---------------------------------------------------------
// This menu will be displayed when the emulator
// is paused.
//---------------------------------------------------------
extern SMenuItem cheatMenu[];


//---------------------------------------------------------
// This function will be called everytime the user
// selects an action on the menu.
//
// Returns true if the menu should close and the game 
// should resume
//---------------------------------------------------------
bool impl3dsOnMenuSelected(int ID);


//---------------------------------------------------------
// This function will be called everytime the user 
// changes the value in the specified menu item.
//
// Returns true if the menu should close and the game 
// should resume
//---------------------------------------------------------
bool impl3dsOnMenuSelectedChanged(int ID, int value);


//---------------------------------------------------------
// Settings used for the 3DS.
//---------------------------------------------------------
extern SSettings3DS settings3DS;


//---------------------------------------------------------
// Initializes the default global and game-specifi
// settings. This method is called everytime a game is
// loaded, but the configuration file does not exist.
//---------------------------------------------------------
void impl3dsInitializeDefaultSettings();


//----------------------------------------------------------------------
// Read/write all possible game specific settings into a file 
// created in this method.
//
// This must return true if the settings file exist.
//----------------------------------------------------------------------
bool impl3dsReadWriteSettingsByGame(bool writeMode);


//----------------------------------------------------------------------
// Read/write all possible global specific settings into a file 
// created in this method.
//
// This must return true if the settings file exist.
//----------------------------------------------------------------------
bool impl3dsReadWriteSettingsGlobal(bool writeMode);


//----------------------------------------------------------------------
// Apply settings into the emulator.
//
// This method normally copies settings from the settings3DS struct
// and updates the emulator's core's configuration.
//
// This must return true if any settings were modified.
//----------------------------------------------------------------------
bool impl3dsApplyAllSettings(bool updateGameSettings = true);


//----------------------------------------------------------------------
// Copy values from menu to settings3DS structure,
// or from settings3DS structure to the menu, depending on the
// copyMenuToSettings parameter.
//
// This must return return if any of the settings were changed.
//----------------------------------------------------------------------
bool impl3dsCopyMenuToOrFromSettings(bool copyMenuToSettings);


//----------------------------------------------------------------------
// Clears all cheats from the core.
//
// This method is called only when cheats are loaded.
// This only happens after a new ROM is loaded.
//----------------------------------------------------------------------
void impl3dsClearAllCheats();


//----------------------------------------------------------------------
// Adds cheats into the emulator core after being loaded up from 
// the .CHX file.
//
// This method is called only when cheats are loaded.
// This only happens after a new ROM is loaded.
//
// This method must return true if the cheat code format is valid,
// and the cheat is added successfully into the core.
//----------------------------------------------------------------------
bool impl3dsAddCheat(bool cheatEnabled, char *name, char *code);


//----------------------------------------------------------------------
// Enable/disables a cheat in the emulator core.
// 
// This method will be triggered when the user enables/disables
// cheats in the cheat menu.
//----------------------------------------------------------------------
void impl3dsSetCheatEnabledFlag(int cheatIdx, bool enabled);




#endif