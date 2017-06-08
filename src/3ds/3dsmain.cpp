#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>

#include <dirent.h>

#include "3dstypes.h"
#include "3dsexit.h"
#include "3dsgpu.h"
#include "3dsopt.h"
#include "3dssound.h"
#include "3dsmenu.h"
#include "3dsui.h"
#include "3dsfont.h"
#include "3dsconfig.h"
#include "3dsfiles.h"
#include "3dsinput.h"
#include "3dslodepng.h"
#include "3dsmenu.h"
#include "3dsmain.h"

#include "3dsinterface.h"
#include "3dscheat.h"


SEmulator emulator;

int frameCount60 = 60;
u64 frameCountTick = 0;
int framesSkippedCount = 0;
char *romFileName = 0;
char romFileNameFullPath[_MAX_PATH];
char romFileNameLastSelected[_MAX_PATH];



//-------------------------------------------------------
// Clear top screen with logo.
//-------------------------------------------------------
void clearTopScreenWithLogo()
{
	unsigned char* image;
	unsigned width, height;

    int error = lodepng_decode32_file(&image, &width, &height, impl3dsTitleImage);

    if (!error && width == 400 && height == 240)
    {
        // lodepng outputs big endian rgba so we need to convert
        for (int i = 0; i < 2; i++)
        {
            u8* src = image;
            uint32* fb = (uint32 *) gfxGetFramebuffer(GFX_TOP, GFX_LEFT, NULL, NULL);
            for (int y = 0; y < 240; y++)
                for (int x = 0; x < 400; x++)
                {
                    uint32 r = *src++;
                    uint32 g = *src++;
                    uint32 b = *src++;
                    uint32 a = *src++;

                    uint32 c = ((r << 24) | (g << 16) | (b << 8) | 0xff);
                    fb[x * 240 + (239 - y)] = c;
                }
            gfxSwapBuffers();
        }

        free(image);
    }
}


//----------------------------------------------------------------------
// Start up menu.
//----------------------------------------------------------------------
SMenuItem emulatorNewMenu[] = {
    MENU_MAKE_ACTION(6001, "  Exit"),
    MENU_MAKE_LASTITEM  ()
    };

extern SMenuItem emulatorMenu[];


//-------------------------------------------------------
// Load the ROM and reset the CPU.
//-------------------------------------------------------

bool emulatorSettingsLoad(bool, bool);
bool emulatorSettingsSave(bool, bool);

bool emulatorLoadRom()
{
    menu3dsShowDialog("Load ROM", "Loading... this may take a while.", DIALOGCOLOR_CYAN, NULL);

    emulatorSettingsSave(false, false);
    snprintf(romFileNameFullPath, _MAX_PATH, "%s%s", file3dsGetCurrentDir(), romFileName);

    char romFileNameFullPath2[_MAX_PATH];
    strncpy(romFileNameFullPath2, romFileNameFullPath, _MAX_PATH - 1);
    
    if (!impl3dsLoadROM(romFileNameFullPath2))
    {
        menu3dsHideDialog();
        return false;
    }

    emulator.emulatorState = EMUSTATE_EMULATE;

    emulatorSettingsLoad(true, false);
    impl3dsApplyAllSettings();
    cheat3dsLoadCheatTextFile(file3dsReplaceFilenameExtension(romFileNameFullPath, ".chx"));
    menu3dsHideDialog();

    // Fix: Game-specific settings that never get saved.
    impl3dsCopyMenuToOrFromSettings(false);

    return true;
}


//----------------------------------------------------------------------
// Menus
//----------------------------------------------------------------------
SMenuItem fileMenu[1000];
char romFileNames[1000][_MAX_PATH];

int totalRomFileCount = 0;

//----------------------------------------------------------------------
// Load all ROM file names (up to 1000 ROMs)
//----------------------------------------------------------------------
void fileGetAllFiles(void)
{
    std::vector<std::string> files = file3dsGetFiles(impl3dsRomExtensions, 1000);

    totalRomFileCount = 0;

    // Increase the total number of files we can display.
    for (int i = 0; i < files.size() && i < 1000; i++)
    {
        strncpy(romFileNames[i], files[i].c_str(), _MAX_PATH);
        totalRomFileCount++;
        fileMenu[i].Type = MENUITEM_ACTION;
        fileMenu[i].ID = i;
        fileMenu[i].Text = romFileNames[i];
    }
    fileMenu[files.size()].Type = MENUITEM_LASTITEM;
}


//----------------------------------------------------------------------
// Find the ID of the last selected file in the file list.
//----------------------------------------------------------------------
int fileFindLastSelectedFile()
{
    for (int i = 0; i < totalRomFileCount && i < 1000; i++)
    {
        if (strncmp(fileMenu[i].Text, romFileNameLastSelected, _MAX_PATH) == 0)
            return i;
    }
    return -1;
}



//----------------------------------------------------------------------
// Load global settings, and game-specific settings.
//----------------------------------------------------------------------
bool emulatorSettingsLoad(bool includeGameSettings, bool showMessage = true)
{
    bool success = impl3dsReadWriteSettingsGlobal(false);
    if (!success)
        return false;
    impl3dsApplyAllSettings(false);

    if (includeGameSettings)
    {
        success = impl3dsReadWriteSettingsByGame(false);
        if (success)
        {
            impl3dsApplyAllSettings();
            /*if (impl3dsApplyAllSettings())
                emulatorSettingsSave(true, showMessage);*/
            return true;
        }
        else
        {
            impl3dsInitializeDefaultSettings();

            impl3dsApplyAllSettings();

            //return emulatorSettingsSave(true, showMessage);
            return true;
        }
    }
    return true;
}


//----------------------------------------------------------------------
// Save global settings, and game-specific settings.
//----------------------------------------------------------------------
bool emulatorSettingsSave(bool includeGameSettings, bool showMessage)
{
    if (showMessage)
    {
        consoleClear();
        ui3dsDrawRect(50, 140, 270, 154, 0x000000);
        ui3dsDrawStringWithNoWrapping(50, 140, 270, 154, 0x3f7fff, HALIGN_CENTER, "Saving settings to SD card...");
    }

    if (includeGameSettings)
        impl3dsReadWriteSettingsByGame(true);

    impl3dsReadWriteSettingsGlobal(true);
    if (showMessage)
    {
        ui3dsDrawRect(50, 140, 270, 154, 0x000000);
    }

    return true;
}



//----------------------------------------------------------------------
// Start up menu.
//----------------------------------------------------------------------
void menuSelectFile(void)
{
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    
    fileGetAllFiles();
    int previousFileID = fileFindLastSelectedFile();
    menu3dsClearMenuTabs();
    menu3dsAddTab("Emulator", emulatorNewMenu);
    menu3dsAddTab("Select ROM", fileMenu);
    menu3dsSetTabSubTitle(0, NULL);
    menu3dsSetTabSubTitle(1, file3dsGetCurrentDir());
    menu3dsSetCurrentMenuTab(1);
    if (previousFileID >= 0)
        menu3dsSetSelectedItemIndexByID(1, previousFileID);
    menu3dsSetTransferGameScreen(false);

    bool animateMenu = true;
    int selection = 0;
    do
    {
        if (appExiting)
            return;

        selection = menu3dsShowMenu(NULL, animateMenu);
        animateMenu = false;

        if (selection >= 0 && selection < 1000)
        {
            // Load ROM
            //
            romFileName = romFileNames[selection];
            strncpy(romFileNameLastSelected, romFileName, _MAX_PATH);
            if (romFileName[0] == 1)
            {
                if (strcmp(romFileName, "\x01 ..") == 0)
                    file3dsGoToParentDirectory();
                else
                    file3dsGoToChildDirectory(&romFileName[2]);

                fileGetAllFiles();
                menu3dsClearMenuTabs();
                menu3dsAddTab("Emulator", emulatorNewMenu);
                menu3dsAddTab("Select ROM", fileMenu);
                menu3dsSetCurrentMenuTab(1);
                menu3dsSetTabSubTitle(1, file3dsGetCurrentDir());
                selection = -1;
            }
            else
            {
                if (!emulatorLoadRom())
                {
                    menu3dsShowDialog("Load ROM", "Hmm... unable to load ROM.", DIALOGCOLOR_RED, optionsForOk);
                    menu3dsHideDialog();
                }
                else
                {
                    menu3dsHideMenu();
                    consoleInit(GFX_BOTTOM, NULL);
                    consoleClear();
                    return;
                }
            }
        }
        else if (selection == 6001)
        {
            int result = menu3dsShowDialog("Exit",  "Leaving so soon?", DIALOGCOLOR_RED, optionsForNoYes);
            menu3dsHideDialog();

            if (result == 1)
            {
                emulator.emulatorState = EMUSTATE_END;
                return;
            }
        }

        selection = -1;     // Bug fix: Fixes crashing when setting options before any ROMs are loaded.
    }
    while (selection == -1);

    menu3dsHideMenu();

}


//----------------------------------------------------------------------
// Checks if file exists.
//----------------------------------------------------------------------
bool IsFileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}


//----------------------------------------------------------------------
// Menu when the emulator is paused in-game.
//----------------------------------------------------------------------
bool menuSelectedChanged(int ID, int value)
{
    if (ID >= 50000 && ID <= 51000)
    {
        // Handle cheats
        int enabled = menu3dsGetValueByID(2, ID);
        impl3dsSetCheatEnabledFlag(ID - 50000, enabled == 1);
        cheat3dsSetCheatEnabledFlag(ID - 50000, enabled == 1);
        return false;
    }

    return impl3dsOnMenuSelectedChanged(ID, value);
}


//----------------------------------------------------------------------
// Menu when the emulator is paused in-game.
//----------------------------------------------------------------------
void menuPause()
{
    gfxSetDoubleBuffering(GFX_BOTTOM, true);
    
    bool settingsUpdated = false;
    bool cheatsUpdated = false;
    bool loadRomBeforeExit = false;
    bool returnToEmulation = false;


    menu3dsClearMenuTabs();
    menu3dsAddTab("Emulator", emulatorMenu);
    menu3dsAddTab("Options", optionMenu);
    menu3dsAddTab("Cheats", cheatMenu);
    menu3dsAddTab("Select ROM", fileMenu);

    impl3dsCopyMenuToOrFromSettings(false);

    int previousFileID = fileFindLastSelectedFile();
    menu3dsSetTabSubTitle(0, NULL);
    menu3dsSetTabSubTitle(1, NULL);
    menu3dsSetTabSubTitle(2, NULL);
    menu3dsSetTabSubTitle(3, file3dsGetCurrentDir());
    if (previousFileID >= 0)
        menu3dsSetSelectedItemIndexByID(3, previousFileID);
    menu3dsSetCurrentMenuTab(0);
    menu3dsSetTransferGameScreen(true);

    bool animateMenu = true;

    while (true)
    {
        if (appExiting)
        {
            break;
        }

        int selection = menu3dsShowMenu(menuSelectedChanged, animateMenu);
        animateMenu = false;
        

        if (selection == -1 || selection == 1000)
        {
            // Cancels the menu and resumes game
            //
            returnToEmulation = true;

            break;
        }
        else if (selection < 1000)
        {
            // Load ROM
            //
            romFileName = romFileNames[selection];
            if (romFileName[0] == 1)
            {
                if (strcmp(romFileName, "\x01 ..") == 0)
                    file3dsGoToParentDirectory();
                else
                    file3dsGoToChildDirectory(&romFileName[2]);

                fileGetAllFiles();
                menu3dsClearMenuTabs();
                menu3dsAddTab("Emulator", emulatorMenu);
                menu3dsAddTab("Options", optionMenu);
                menu3dsAddTab("Cheats", cheatMenu);
                menu3dsAddTab("Select ROM", fileMenu);
                menu3dsSetCurrentMenuTab(3);
                menu3dsSetTabSubTitle(3, file3dsGetCurrentDir());
            }
            else
            {
                strncpy(romFileNameLastSelected, romFileName, _MAX_PATH);

                // Save settings and cheats, before loading
                // your new ROM.
                //
                if (impl3dsCopyMenuToOrFromSettings(true))
                    emulatorSettingsSave(true, true);
                
                if (!emulatorLoadRom())
                {
                    menu3dsShowDialog("Load ROM", "Hmm... unable to load ROM.", DIALOGCOLOR_RED, optionsForOk);
                    menu3dsHideDialog();
                }
                else
                    break;
            }
        }
        else if (selection >= 2001 && selection <= 2010)
        {
            int slot = selection - 2000;
            char text[200];
           
            sprintf(text, "Saving into slot %d...\nThis may take a while", slot);
            menu3dsShowDialog("Savestates", text, DIALOGCOLOR_CYAN, NULL);
            bool result = impl3dsSaveState(slot);
            menu3dsHideDialog();

            if (result)
            {
                sprintf(text, "Slot %d save completed.", slot);
                result = menu3dsShowDialog("Savestates", text, DIALOGCOLOR_GREEN, optionsForOk);
                menu3dsHideDialog();
            }
            else
            {
                sprintf(text, "Oops. Unable to save slot %d!", slot);
                result = menu3dsShowDialog("Savestates", text, DIALOGCOLOR_RED, optionsForOk);
                menu3dsHideDialog();
            }

            menu3dsSetSelectedItemIndexByID(0, 1000);
        }
        else if (selection >= 3001 && selection <= 3010)
        {
            int slot = selection - 3000;
            char text[200];

            bool result = impl3dsLoadState(slot);
            if (result)
            {
                emulator.emulatorState = EMUSTATE_EMULATE;
                consoleClear();
                break;
            }
            else
            {
                sprintf(text, "Oops. Unable to load slot %d!", slot);
                menu3dsShowDialog("Savestates", text, DIALOGCOLOR_RED, optionsForOk);
                menu3dsHideDialog();
            }
        }
        else if (selection == 4001)
        {
            menu3dsShowDialog("Screenshot", "Now taking a screenshot...\nThis may take a while.", DIALOGCOLOR_CYAN, NULL);

            char ext[256];
            const char *path = NULL;

            // Loop through and look for an non-existing
            // file name.
            //
            int i = 1;
            while (i <= 999)
            {
                snprintf(ext, 255, ".b%03d.bmp", i);
                path = file3dsReplaceFilenameExtension(romFileNameFullPath, ext);
                if (!IsFileExists(path))
                    break;
                path = NULL;
                i++;
            }

            bool success = false;
            if (path)
            {
                success = menu3dsTakeScreenshot(path);
            }
            menu3dsHideDialog();

            if (success)
            {
                char text[600];
                snprintf(text, 600, "Done! File saved to %s", path);
                menu3dsShowDialog("Screenshot", text, DIALOGCOLOR_GREEN, optionsForOk);
                menu3dsHideDialog();
            }
            else 
            {
                menu3dsShowDialog("Screenshot", "Oops. Unable to take screenshot!", DIALOGCOLOR_RED, optionsForOk);
                menu3dsHideDialog();
            }
        }
        else if (selection == 5001)
        {
            int result = menu3dsShowDialog("Reset Console", "Are you sure?", DIALOGCOLOR_RED, optionsForNoYes);
            menu3dsHideDialog();

            if (result == 1)
            {
                impl3dsResetConsole();
                emulator.emulatorState = EMUSTATE_EMULATE;
                consoleClear();

                break;
            }
            
        }
        else if (selection == 6001)
        {
            int result = menu3dsShowDialog("Exit",  "Leaving so soon?", DIALOGCOLOR_RED, optionsForNoYes);
            if (result == 1)
            {
                emulator.emulatorState = EMUSTATE_END;

                break;
            }
            else
                menu3dsHideDialog();
            
        }
        else
        {
            bool endMenu = impl3dsOnMenuSelected(selection);
            if (endMenu)
            {
                returnToEmulation = true;
                break;
            }
        }

    }

    menu3dsHideMenu();

    // Save settings and cheats
    //
    if (impl3dsCopyMenuToOrFromSettings(true))
        emulatorSettingsSave(true, true);
    impl3dsApplyAllSettings();

    cheat3dsSaveCheatTextFile (file3dsReplaceFilenameExtension(romFileNameFullPath, ".chx"));

    if (returnToEmulation)
    {
        emulator.emulatorState = EMUSTATE_EMULATE;
        consoleClear();
    }

    // Loads the new ROM if a ROM was selected.
    //
    //if (loadRomBeforeExit)
    //    emulatorLoadRom();

}

//-------------------------------------------------------
// Sets up all the cheats to be displayed in the menu.
//-------------------------------------------------------
SMenuItem cheatMenu[401] =
{
    MENU_MAKE_HEADER2   ("Cheats"),
    MENU_MAKE_LASTITEM  ()
};


char *noCheatsText[] {
    "",
    "    No cheats available for this game ",
    "",
    "    To enable cheats:  ",
    "      Copy your file into the same folder as  ",
    "      ROM file and make sure it has the same name. ",
    "",
    "      If your ROM filename is: ",
    "          MyGame.abc",
    "      Then your cheat filename must be: ",
    "          MyGame.CHX",
    "",
    "    Refer to readme.md for the .CHX file format. ",
    ""
     };


//--------------------------------------------------------
// Initialize the emulator engine and everything else.
// This calls the impl3dsInitializeCore, which executes
// initialization code specific to the emulation core.
//--------------------------------------------------------
void emulatorInitialize()
{
    file3dsInitialize();

    romFileNameLastSelected[0] = 0;

    if (!gpu3dsInitialize())
    {
        printf ("Unable to initialize GPU\n");
        exit(0);
    }

    printf ("Initializing...\n");

    if (!impl3dsInitializeCore())
    {
        printf ("Unable to initialize emulator core\n");
        exit(0);
    }

    if (!snd3dsInitialize())
    {
        printf ("Unable to initialize CSND\n");
        exit (0);
    }

    ui3dsInitialize();

    /*if (romfsInit()!=0)
    {
        printf ("Unable to initialize romfs\n");
        exit (0);
    }
    */
    printf ("Initialization complete\n");

    osSetSpeedupEnable(1);    // Performance: use the higher clock speed for new 3DS.

    enableExitHook();

    emulatorSettingsLoad(false, true);

    // Do this one more time.
    if (file3dsGetCurrentDir()[0] == 0)
        file3dsInitialize();
}


//--------------------------------------------------------
// Finalize the emulator.
//--------------------------------------------------------
void emulatorFinalize()
{
    consoleClear();

    impl3dsFinalize();

#ifndef EMU_RELEASE
    printf("gspWaitForP3D:\n");
#endif
    gspWaitForVBlank();
    gpu3dsWaitForPreviousFlush();
    gspWaitForVBlank();

#ifndef EMU_RELEASE
    printf("snd3dsFinalize:\n");
#endif
    snd3dsFinalize();

#ifndef EMU_RELEASE
    printf("gpu3dsFinalize:\n");
#endif
    gpu3dsFinalize();

#ifndef EMU_RELEASE
    printf("ptmSysmExit:\n");
#endif
    ptmSysmExit ();

    //printf("romfsExit:\n");
    //romfsExit();
    
#ifndef EMU_RELEASE
    printf("hidExit:\n");
#endif
	hidExit();
    
#ifndef EMU_RELEASE
    printf("aptExit:\n");
#endif
	aptExit();
    
#ifndef EMU_RELEASE
    printf("srvExit:\n");
#endif
	srvExit();
}



bool firstFrame = true;


//---------------------------------------------------------
// Counts the number of frames per second, and prints
// it to the bottom screen every 60 frames.
//---------------------------------------------------------
char frameCountBuffer[70];
void updateFrameCount()
{
    if (frameCountTick == 0)
        frameCountTick = svcGetSystemTick();

    if (frameCount60 == 0)
    {
        u64 newTick = svcGetSystemTick();
        float timeDelta = ((float)(newTick - frameCountTick))/TICKS_PER_SEC;
        int fpsmul10 = (int)((float)600 / timeDelta);

#if !defined(EMU_RELEASE) && !defined(DEBUG_CPU) && !defined(DEBUG_APU)
        consoleClear();
#endif

        if (settings3DS.HideUnnecessaryBottomScrText == 0)
        {
            if (framesSkippedCount)
                snprintf (frameCountBuffer, 69, "FPS: %2d.%1d (%d skipped)\n", fpsmul10 / 10, fpsmul10 % 10, framesSkippedCount);
            else
                snprintf (frameCountBuffer, 69, "FPS: %2d.%1d \n", fpsmul10 / 10, fpsmul10 % 10);

            ui3dsDrawRect(2, 2, 200, 16, 0x000000);
            ui3dsDrawStringWithNoWrapping(2, 2, 200, 16, 0x7f7f7f, HALIGN_LEFT, frameCountBuffer);
        }

        frameCount60 = 60;
        framesSkippedCount = 0;


#if !defined(EMU_RELEASE) && !defined(DEBUG_CPU) && !defined(DEBUG_APU)
        printf ("\n\n");
        for (int i=0; i<100; i++)
        {
            t3dsShowTotalTiming(i);
        }
        t3dsResetTimings();
#endif
        frameCountTick = newTick;

    }

    frameCount60--;
}





//----------------------------------------------------------
// This is the main emulation loop. It calls the 
//    impl3dsRunOneFrame
//   (which must be implemented for any new core)
// for the execution of the frame.
//----------------------------------------------------------
void emulatorLoop()
{
	// Main loop
    //emulator.enableDebug = true;

    int emuFramesSkipped = 0;
    long emuFrameTotalActualTicks = 0;
    long emuFrameTotalAccurateTicks = 0;

    bool firstFrame = true;

    gpu3dsResetState();

    frameCount60 = 60;
    frameCountTick = 0;
    framesSkippedCount = 0;

    long startFrameTick = svcGetSystemTick();

    bool skipDrawingFrame = false;

    // Reinitialize the console.
    consoleInit(GFX_BOTTOM, NULL);
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    menu3dsDrawBlackScreen();
    if (settings3DS.HideUnnecessaryBottomScrText == 0)
    {
        ui3dsDrawStringWithNoWrapping(0, 100, 320, 115, 0x7f7f7f, HALIGN_CENTER, "Touch screen for menu");
    }

    snd3dsStartPlaying();

    impl3dsEmulationBegin();

	while (true)
	{
        startFrameTick = svcGetSystemTick();
        aptMainLoop();

        if (appExiting)
            break;

        gpu3dsStartNewFrame();
        gpu3dsCheckSlider();
        updateFrameCount();

    	input3dsScanInputForEmulation();
        impl3dsEmulationRunOneFrame(firstFrame, skipDrawingFrame);

        if (emulator.emulatorState != EMUSTATE_EMULATE)
            break;

        firstFrame = false; 

        // This either waits for the next frame, or decides to skip
        // the rendering for the next frame if we are too slow.
        //
#ifndef EMU_RELEASE
        //if (emulator.isReal3DS)
#endif
        {

            long currentTick = svcGetSystemTick();
            long actualTicksThisFrame = currentTick - startFrameTick;

            emuFrameTotalActualTicks += actualTicksThisFrame;  // actual time spent rendering past x frames.
            emuFrameTotalAccurateTicks += settings3DS.TicksPerFrame;  // time supposed to be spent rendering past x frames.

            int isSlow = 0;


            long skew = emuFrameTotalAccurateTicks - emuFrameTotalActualTicks;

            if (skew < 0)
            {
                // We've skewed out of the actual frame rate.
                // Once we skew beyond 0.1 (10%) frames slower, skip the frame.
                //
                if (skew < -settings3DS.TicksPerFrame/10 && emuFramesSkipped < settings3DS.MaxFrameSkips)
                {
                    skipDrawingFrame = true;
                    emuFramesSkipped++;

                    framesSkippedCount++;   // this is used for the stats display every 60 frames.
                }
                else
                {
                    skipDrawingFrame = false;

                    if (emuFramesSkipped >= settings3DS.MaxFrameSkips)
                    {
                        emuFramesSkipped = 0;
                        emuFrameTotalActualTicks = actualTicksThisFrame;
                        emuFrameTotalAccurateTicks = settings3DS.TicksPerFrame;
                    }
                }
            }
            else
            {

                float timeDiffInMilliseconds = (float)skew * 1000000 / TICKS_PER_SEC;

                // Reset the counters.
                //
                emuFrameTotalActualTicks = 0;
                emuFrameTotalAccurateTicks = 0;
                emuFramesSkipped = 0;

                svcSleepThread ((long)(timeDiffInMilliseconds * 1000));

                skipDrawingFrame = false;
            }

        }

	}

    snd3dsStopPlaying();
}


//---------------------------------------------------------
// Main entrypoint.
//---------------------------------------------------------
int main()
{
    emulatorInitialize();
    clearTopScreenWithLogo();

    menuSelectFile();

    while (true)
    {
        if (appExiting)
            goto quit;

        switch (emulator.emulatorState)
        {
            case EMUSTATE_PAUSEMENU:
                menuPause();
                break;

            case EMUSTATE_EMULATE:
                emulatorLoop();
                break;

            case EMUSTATE_END:
                goto quit;

        }

    }

quit:
    printf("emulatorFinalize:\n");
    emulatorFinalize();
    printf ("Exiting...\n");
	exit(0);
}
