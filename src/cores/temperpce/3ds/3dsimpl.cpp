//=============================================================================
// Contains all the hooks and interfaces between the emulator interface
// and the main emulator core.
//=============================================================================

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <3ds.h>

#include <dirent.h>

#include "3dstypes.h"
#include "3dsemu.h"
#include "3dsexit.h"
#include "3dsgpu.h"
#include "3dssound.h"
#include "3dsui.h"
#include "3dsinput.h"
#include "3dsfiles.h"
#include "3dsinterface.h"
#include "3dsmain.h"
#include "3dsasync.h"
#include "3dsimpl.h"
#include "3dsopt.h"
#include "3dsconfig.h"

//---------------------------------------------------------
// All other codes that you need here.
//---------------------------------------------------------
#include "3dsdbg.h"
#include "3dsimpl.h"
#include "3dsimpl_gpu.h"
#include "3dsimpl_tilecache.h"
#include "shaderfast2_shbin.h"
#include "shaderslow_shbin.h"
#include "shaderslow2_shbin.h"

#include "vsect.h"

#include "extern.h"
#include "common.h"

//----------------------------------------------------------------------
// Settings
//----------------------------------------------------------------------
SSettings3DS settings3DS;

#define SETTINGS_SOFTWARERENDERING      0
#define SETTINGS_IDLELOOPPATCH          1

//----------------------------------------------------------------------
// Menu options
//----------------------------------------------------------------------

SMenuItem optionsForFont[] = {
    MENU_MAKE_DIALOG_ACTION (0, "Tempesta",               ""),
    MENU_MAKE_DIALOG_ACTION (1, "Ronda",                  ""),
    MENU_MAKE_DIALOG_ACTION (2, "Arial",                  ""),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForStretch[] = {
    MENU_MAKE_DIALOG_ACTION (0, "No Stretch",               "'Pixel Perfect'"),
    MENU_MAKE_DIALOG_ACTION (1, "4:3 Fit",                  "Stretch to 320x240"),
    MENU_MAKE_DIALOG_ACTION (2, "Fullscreen",               "Stretch to 400x240"),
    MENU_MAKE_DIALOG_ACTION (3, "Cropped 4:3 Fit",          "Crop & Stretch to 320x240"),
    MENU_MAKE_DIALOG_ACTION (4, "Cropped Fullscreen",       "Crop & Stretch to 400x240"),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForFrameskip[] = {
    MENU_MAKE_DIALOG_ACTION (0, "Disabled",                 ""),
    MENU_MAKE_DIALOG_ACTION (1, "Enabled (max 1 frame)",    ""),
    MENU_MAKE_DIALOG_ACTION (2, "Enabled (max 2 frames)",    ""),
    MENU_MAKE_DIALOG_ACTION (3, "Enabled (max 3 frames)",    ""),
    MENU_MAKE_DIALOG_ACTION (4, "Enabled (max 4 frames)",    ""),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForFrameRate[] = {
    MENU_MAKE_DIALOG_ACTION (0, "Default based on ROM",     ""),
    MENU_MAKE_DIALOG_ACTION (1, "50 FPS",                   ""),
    MENU_MAKE_DIALOG_ACTION (2, "60 FPS",                   ""),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForAutoSaveSRAMDelay[] = {
    MENU_MAKE_DIALOG_ACTION (1, "1 second",     ""),
    MENU_MAKE_DIALOG_ACTION (2, "10 seconds",   ""),
    MENU_MAKE_DIALOG_ACTION (3, "60 seconds",   ""),
    MENU_MAKE_DIALOG_ACTION (4, "Disabled",     "Touch bottom screen to save"),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForTurboFire[] = {
    MENU_MAKE_DIALOG_ACTION (0, "None",         ""),
    MENU_MAKE_DIALOG_ACTION (10, "Slowest",      ""),
    MENU_MAKE_DIALOG_ACTION (8, "Slower",       ""),
    MENU_MAKE_DIALOG_ACTION (6, "Slow",         ""),
    MENU_MAKE_DIALOG_ACTION (4, "Fast",         ""),
    MENU_MAKE_DIALOG_ACTION (2, "Faster",         ""),
    MENU_MAKE_DIALOG_ACTION (1, "Very Fast",    ""),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForButtons[] = {
    MENU_MAKE_DIALOG_ACTION (0,                 "None",             ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_I,       "Button I",         ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_II,      "Button II",        ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_III,     "Button III",       ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_IV,      "Button IV",        ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_V,       "Button V",         ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_VI,      "Button VI",        ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_SELECT,  "PCE's SELECT",     ""),
    MENU_MAKE_DIALOG_ACTION (IO_BUTTON_RUN,     "PCE's RUN",        ""),
    MENU_MAKE_LASTITEM  ()
};

SMenuItem optionsForSpriteFlicker[] =
{
    MENU_MAKE_DIALOG_ACTION (0, "Hardware Accurate",   "Flickers like real hardware"),
    MENU_MAKE_DIALOG_ACTION (1, "Better Visuals",      "Looks better, less accurate"),
    MENU_MAKE_LASTITEM  ()  
};

SMenuItem optionsForIdleLoopPatch[] =
{
    MENU_MAKE_DIALOG_ACTION (1, "Enabled",              "Faster but some games may freeze"),
    MENU_MAKE_DIALOG_ACTION (0, "Disabled",             "Slower but better compatibility"),
    MENU_MAKE_LASTITEM  ()  
};

SMenuItem optionsForRendering[] =
{
    MENU_MAKE_DIALOG_ACTION (0, "Hardware",             "Faster"),
    MENU_MAKE_DIALOG_ACTION (1, "Software",             "More accurate"),
    MENU_MAKE_LASTITEM  ()  
};

SMenuItem optionMenu[] = {
    MENU_MAKE_HEADER1   ("GLOBAL SETTINGS"),
    MENU_MAKE_PICKER    (11000, "  Screen Stretch", "How would you like the final screen to appear?", optionsForStretch, DIALOGCOLOR_CYAN),
    MENU_MAKE_PICKER    (18000, "  Font", "The font used for the user interface.", optionsForFont, DIALOGCOLOR_CYAN),
    MENU_MAKE_CHECKBOX  (15001, "  Hide text in bottom screen", 0),
    MENU_MAKE_DISABLED  (""),
    MENU_MAKE_HEADER1   ("GAME-SPECIFIC SETTINGS"),
    MENU_MAKE_HEADER2   ("Speed Hack"),
    MENU_MAKE_PICKER    (20000, "  Idle Loop Patching", "You must reload the ROM after changing this.", optionsForIdleLoopPatch, DIALOGCOLOR_CYAN),
    MENU_MAKE_DISABLED  (""),
    MENU_MAKE_HEADER2   ("Graphics"),
    MENU_MAKE_PICKER    (10000, "  Frameskip", "Try changing this if the game runs slow. Skipping frames help it run faster but less smooth.", optionsForFrameskip, DIALOGCOLOR_CYAN),
    MENU_MAKE_PICKER    (12000, "  Framerate", "Some games run at 50 or 60 FPS by default. Override if required.", optionsForFrameRate, DIALOGCOLOR_CYAN),
    MENU_MAKE_PICKER    (19000, "  Rendering", "Hardware rendering improves speed.", optionsForRendering, DIALOGCOLOR_CYAN),
    MENU_MAKE_DISABLED  (""),
    MENU_MAKE_HEADER2   ("Audio"),
    MENU_MAKE_GAUGE     (14000, "  Volume Amplification", 0, 8, 4),
    MENU_MAKE_DISABLED  (""),
    MENU_MAKE_HEADER2   ("3DS Button Config"),
    MENU_MAKE_PICKER    (13010, "  3DS 'A'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13000, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_PICKER    (13011, "  3DS 'B'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13001, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_PICKER    (13012, "  3DS 'X'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13002, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_PICKER    (13013, "  3DS 'Y'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13003, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_PICKER    (13014, "  3DS 'L'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13004, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_PICKER    (13015, "  3DS 'R'", "", optionsForButtons, DIALOGCOLOR_CYAN),
    MENU_MAKE_GAUGE     (13005, "    Rapid-Fire Speed", 0, 10, 0),
    MENU_MAKE_LASTITEM  ()
};


//-------------------------------------------------------
SMenuItem optionsForDisk[] =
{
    MENU_MAKE_DIALOG_ACTION (0, "Eject Disk",               ""),
    MENU_MAKE_DIALOG_ACTION (1, "Change to Disk 1 Side A",  ""),
    MENU_MAKE_DIALOG_ACTION (2, "Change to Disk 1 Side B",  ""),
    MENU_MAKE_DIALOG_ACTION (3, "Change to Disk 2 Side A",  ""),
    MENU_MAKE_DIALOG_ACTION (4, "Change to Disk 2 Side B",  ""),
    MENU_MAKE_DIALOG_ACTION (5, "Change to Disk 3 Side A",  ""),
    MENU_MAKE_DIALOG_ACTION (6, "Change to Disk 3 Side B",  ""),
    MENU_MAKE_DIALOG_ACTION (7, "Change to Disk 4 Side A",  ""),
    MENU_MAKE_DIALOG_ACTION (8, "Change to Disk 4 Side B",  ""),
    MENU_MAKE_LASTITEM  ()  
};


//-------------------------------------------------------
// Standard in-game emulator menu.
// You should not modify those menu items that are
// marked 'do not modify'.
//-------------------------------------------------------
SMenuItem emulatorMenu[] = {
    MENU_MAKE_HEADER2   ("Emulator"),               // Do not modify
    MENU_MAKE_ACTION    (1000, "  Resume Game"),    // Do not modify
    MENU_MAKE_PICKER2   (30000,"  Choose Disk", "", optionsForDisk, DIALOGCOLOR_CYAN),
    MENU_MAKE_HEADER2   (""),

    MENU_MAKE_HEADER2   ("Savestates"),
    MENU_MAKE_ACTION    (2001, "  Save Slot #1"),   // Do not modify
    MENU_MAKE_ACTION    (2002, "  Save Slot #2"),   // Do not modify
    MENU_MAKE_ACTION    (2003, "  Save Slot #3"),   // Do not modify
    MENU_MAKE_ACTION    (2004, "  Save Slot #4"),   // Do not modify
    MENU_MAKE_HEADER2   (""),   
    
    MENU_MAKE_ACTION    (3001, "  Load Slot #1"),   // Do not modify
    MENU_MAKE_ACTION    (3002, "  Load Slot #2"),   // Do not modify
    MENU_MAKE_ACTION    (3003, "  Load Slot #3"),   // Do not modify
    MENU_MAKE_ACTION    (3004, "  Load Slot #4"),   // Do not modify
    MENU_MAKE_HEADER2   (""),

    MENU_MAKE_HEADER2   ("Others"),                 // Do not modify
    MENU_MAKE_ACTION    (4001, "  Take Screenshot"),// Do not modify
    MENU_MAKE_ACTION    (5001, "  Reset Console"),  // Do not modify
    MENU_MAKE_ACTION    (6001, "  Exit"),           // Do not modify
    MENU_MAKE_LASTITEM  ()
    };





//------------------------------------------------------------------------
// Memory Usage = 0.003 MB   for 4-point rectangle (triangle strip) vertex buffer
#define RECTANGLE_BUFFER_SIZE           0x20000

//------------------------------------------------------------------------
// Memory Usage = 0.003 MB   for 6-point quad vertex buffer (Citra only)
#define CITRA_VERTEX_BUFFER_SIZE        0x200000

// Memory Usage = Not used (Real 3DS only)
#define CITRA_TILE_BUFFER_SIZE          0x200000


//------------------------------------------------------------------------
// Memory Usage = 0.003 MB   for 6-point quad vertex buffer (Real 3DS only)
#define REAL3DS_VERTEX_BUFFER_SIZE      0x10000

// Memory Usage = 0.003 MB   for 2-point rectangle vertex buffer (Real 3DS only)
#define REAL3DS_TILE_BUFFER_SIZE        0x200000


//---------------------------------------------------------
// Our textures
//---------------------------------------------------------
SGPUTexture *emuMainScreenHWTarget;
SGPUTexture *emuMainScreenTarget[2];
SGPUTexture *emuTileCacheTexture;
SGPUTexture *emuDepthForScreens;
//SGPUTexture *nesDepthForOtherTextures;


uint32 *bufferRGBA[2];
unsigned char linecolor[256];



//---------------------------------------------------------
// Settings related to the emulator.
//---------------------------------------------------------
extern SSettings3DS settings3DS;


//---------------------------------------------------------
// Provide a comma-separated list of file extensions
//---------------------------------------------------------
char *impl3dsRomExtensions = "pce,cue";


//---------------------------------------------------------
// The title image .PNG filename.
//---------------------------------------------------------
char *impl3dsTitleImage = "./temperpce_3ds_top.png";


//---------------------------------------------------------
// The title that displays at the bottom right of the
// menu.
//---------------------------------------------------------
char *impl3dsTitleText = "TemperPCE for 3DS v0.9";


int soundSamplesPerGeneration = 0;
int soundSamplesPerSecond = 0;
short soundSamples[1000];


//---------------------------------------------------------
// Initializes the emulator core.
//---------------------------------------------------------
bool impl3dsInitializeCore()
{
	// compute a sample rate closes to 32000 kHz.
	//
    int numberOfGenerationsPerSecond = 60 * 2;
    soundSamplesPerGeneration = 32000 / numberOfGenerationsPerSecond;
	soundSamplesPerSecond = soundSamplesPerGeneration * numberOfGenerationsPerSecond;
    audio.output_frequency = soundSamplesPerSecond;

    config.per_game_bram = true;
    snprintf(config.main_path, MAX_PATH, ".");
    
    initialize_video();
    initialize_memory();
    initialize_io();
    initialize_irq();
    initialize_timer();
    initialize_psg();
    initialize_cpu();
    initialize_cd();
    initialize_adpcm();
    initialize_arcade_card();
    initialize_debug();
    
	// Initialize our GPU.
	// Load up and initialize any shaders
	//
    if (emulator.isReal3DS)
    {
    	gpu3dsLoadShader(0, (u32 *)shaderslow_shbin, shaderslow_shbin_size, 0);     // copy to screen
    	gpu3dsLoadShader(1, (u32 *)shaderfast2_shbin, shaderfast2_shbin_size, 6);   // draw tiles
    }
    else
    {
    	gpu3dsLoadShader(0, (u32 *)shaderslow_shbin, shaderslow_shbin_size, 0);     // copy to screen
        gpu3dsLoadShader(1, (u32 *)shaderslow2_shbin, shaderslow2_shbin_size, 0);   // draw tiles
    }

	gpu3dsInitializeShaderRegistersForRenderTarget(0, 10);
	gpu3dsInitializeShaderRegistersForTexture(4, 14);
	gpu3dsInitializeShaderRegistersForTextureOffset(6);
	
	
    // Create all the necessary textures
    //
    emuTileCacheTexture = gpu3dsCreateTextureInLinearMemory(1024, 1024, GPU_RGBA5551);

    // Main screen 
    emuMainScreenHWTarget = gpu3dsCreateTextureInVRAM(512, 256, GPU_RGBA8);               // 0.250 MB
    emuMainScreenTarget[0] = gpu3dsCreateTextureInLinearMemory(512, 256, GPU_RGBA8);      // 0.250 MB
    emuMainScreenTarget[1] = gpu3dsCreateTextureInLinearMemory(512, 256, GPU_RGBA8);      // 0.250 MB

	// Depth textures, if required
    //
    emuDepthForScreens = gpu3dsCreateTextureInVRAM(512, 256, GPU_RGBA8);       // 0.250 MB
    //nesDepthForOtherTextures = gpu3dsCreateTextureInVRAM(256, 256, GPU_RGBA8); // 0.250 MB

	bufferRGBA[0] = linearMemAlign(512*256*4, 0x80);
	bufferRGBA[1] = linearMemAlign(512*256*4, 0x80);

    if (emuTileCacheTexture == NULL || 
        emuMainScreenHWTarget == NULL ||
        emuMainScreenTarget[0] == NULL || 
        emuMainScreenTarget[1] == NULL || 
        emuDepthForScreens == NULL  /*|| 
		nesDepthForOtherTextures == NULL*/)
    {
        printf ("Unable to allocate textures\n");
        return false;
    }

	// allocate all necessary vertex lists
	//
    if (emulator.isReal3DS)
    {
        gpu3dsAllocVertexList(&GPU3DSExt.rectangleVertexes, RECTANGLE_BUFFER_SIZE, sizeof(SVertexColor), 2, SVERTEXCOLOR_ATTRIBFORMAT);
        gpu3dsAllocVertexList(&GPU3DSExt.quadVertexes, REAL3DS_VERTEX_BUFFER_SIZE, sizeof(SVertexTexCoord), 2, SVERTEXTEXCOORD_ATTRIBFORMAT);
        gpu3dsAllocVertexList(&GPU3DSExt.tileVertexes, REAL3DS_TILE_BUFFER_SIZE, sizeof(STileVertex), 2, STILETEXCOORD_ATTRIBFORMAT);
    }
    else
    {
        gpu3dsAllocVertexList(&GPU3DSExt.rectangleVertexes, RECTANGLE_BUFFER_SIZE, sizeof(SVertexColor), 2, SVERTEXCOLOR_ATTRIBFORMAT);
        gpu3dsAllocVertexList(&GPU3DSExt.quadVertexes, CITRA_VERTEX_BUFFER_SIZE, sizeof(SVertexTexCoord), 2, SVERTEXTEXCOORD_ATTRIBFORMAT);
        gpu3dsAllocVertexList(&GPU3DSExt.tileVertexes, CITRA_TILE_BUFFER_SIZE, sizeof(STileVertex), 2, STILETEXCOORD_ATTRIBFORMAT);
    }

    if (GPU3DSExt.quadVertexes.ListBase == NULL ||
        GPU3DSExt.tileVertexes.ListBase == NULL ||
        GPU3DSExt.rectangleVertexes.ListBase == NULL)
    {
        printf ("Unable to allocate vertex list buffers \n");
        return false;
    }

	gpu3dsUseShader(0);
    return true;
}


//---------------------------------------------------------
// Finalizes and frees up any resources.
//---------------------------------------------------------
void impl3dsFinalize()
{
    if (emuMainScreenHWTarget) gpu3dsDestroyTextureFromVRAM(emuMainScreenHWTarget);
	if (emuTileCacheTexture) gpu3dsDestroyTextureFromLinearMemory(emuTileCacheTexture);
	if (emuMainScreenTarget[0]) gpu3dsDestroyTextureFromLinearMemory(emuMainScreenTarget[0]);
	if (emuMainScreenTarget[1]) gpu3dsDestroyTextureFromLinearMemory(emuMainScreenTarget[1]);
	if (emuDepthForScreens) gpu3dsDestroyTextureFromVRAM(emuDepthForScreens);
	//if (nesDepthForOtherTextures) gpu3dsDestroyTextureFromVRAM(nesDepthForOtherTextures);

	if (bufferRGBA[0]) linearFree(bufferRGBA[0]);
	if (bufferRGBA[1]) linearFree(bufferRGBA[1]);

	
}


//---------------------------------------------------------
// Mix sound samples into a temporary buffer.
//
// This gives time for the sound generation to execute
// from the 2nd core before copying it to the actual
// output buffer.
//---------------------------------------------------------
void impl3dsGenerateSoundSamples()
{
    render_psg(soundSamplesPerGeneration);
}


//---------------------------------------------------------
// Mix sound samples into a temporary buffer.
//
// This gives time for the sound generation to execute
// from the 2nd core before copying it to the actual
// output buffer.
// 
// For a console with only MONO output, simply copy
// the samples into the leftSamples buffer.
//---------------------------------------------------------
void impl3dsOutputSoundSamples(short *leftSamples, short *rightSamples)
{
    int volume = 16;
    if (settings3DS.Volume <= 4)
        volume = 16 + settings3DS.Volume * 4;
    else if (settings3DS.Volume <= 8)
        volume = 16 + (settings3DS.Volume - 4) * 8;
    
    for (int i = 0; i < soundSamplesPerGeneration; i++)
    {
        int sample = 0;

        sample = (audio.buffer[i * 2] >> 5) * volume / 16;
        if(sample > 32767)
            sample = 32767;
        if(sample < -32768)
            sample = -32768;
        leftSamples[i] = sample;

        sample = (audio.buffer[i * 2 + 1] >> 5) * volume / 16;
        if(sample > 32767)
            sample = 32767;
        if(sample < -32768)
            sample = -32768;
        rightSamples[i] = sample;
    }
}


//---------------------------------------------------------
// This is called when a ROM needs to be loaded and the
// emulator engine initialized.
//---------------------------------------------------------
bool impl3dsLoadROM(char *romFilePath)
{
    //config.patch_idle_loops = 0;

    if (load_rom(romFilePath) == -1)
        return false;
    
    impl3dsResetConsole();

	snd3dsSetSampleRate(
		true,
		soundSamplesPerSecond, 
		soundSamplesPerGeneration, 
		true);

	return true;
}


//---------------------------------------------------------
// This is called to determine what the frame rate of the
// game based on the ROM's region.
//---------------------------------------------------------
int impl3dsGetROMFrameRate()
{
	return 60;
}



//---------------------------------------------------------
// This is called when the user chooses to reset the
// console
//---------------------------------------------------------
void impl3dsResetConsole()
{	
    cache3dsInit();
        
    reset_video();
    reset_video_hw();
    reset_memory();
    reset_io();
    reset_irq();
    reset_timer();
    reset_psg();
    reset_cpu();
    reset_cd();
    reset_adpcm();
    reset_arcade_card();

    reset_debug();

}


//---------------------------------------------------------
// This is called when preparing to start emulating
// a new frame. Use this to do any preparation of data,
// the hardware, swap any vertex list buffers, etc, 
// before the frame is emulated
//---------------------------------------------------------
void impl3dsPrepareForNewFrame()
{
	gpu3dsSwapVertexListForNextFrame(&GPU3DSExt.quadVertexes);
    gpu3dsSwapVertexListForNextFrame(&GPU3DSExt.tileVertexes);
    gpu3dsSwapVertexListForNextFrame(&GPU3DSExt.rectangleVertexes);
}




bool isOddFrame = false;
bool skipDrawingPreviousFrame = true;

uint32 			*bufferToTransfer = 0;
SGPUTexture 	*screenTexture = 0;


//---------------------------------------------------------
// Initialize any variables or state of the GPU
// before the emulation loop begins.
//---------------------------------------------------------
void impl3dsEmulationBegin()
{
	bufferToTransfer = 0;
	screenTexture = 0;
	skipDrawingPreviousFrame = true;

	gpu3dsUseShader(0);
	gpu3dsDisableAlphaBlending();
	gpu3dsDisableDepthTest();
	gpu3dsDisableAlphaTest();
	gpu3dsDisableStencilTest();
	gpu3dsSetTextureEnvironmentReplaceTexture0();
	gpu3dsSetRenderTargetToTopFrameBuffer();
	gpu3dsFlush();	
	//if (emulator.isReal3DS)
	//	gpu3dsWaitForPreviousFlush();
}


u32 prevConsoleJoyPad;
u32 prevConsoleButtonPressed[6];
u32 buttons3dsPressed[6];
void impl3dsEmulationPollInput()
{
	u32 keysHeld3ds = input3dsGetCurrentKeysHeld();
    u32 consoleJoyPad = 0;

    if (keysHeld3ds & KEY_UP) consoleJoyPad |= IO_BUTTON_UP;
    if (keysHeld3ds & KEY_DOWN) consoleJoyPad |= IO_BUTTON_DOWN;
    if (keysHeld3ds & KEY_LEFT) consoleJoyPad |= IO_BUTTON_LEFT;
    if (keysHeld3ds & KEY_RIGHT) consoleJoyPad |= IO_BUTTON_RIGHT;
    if (keysHeld3ds & KEY_SELECT) consoleJoyPad |= IO_BUTTON_SELECT;
    if (keysHeld3ds & KEY_START) consoleJoyPad |= IO_BUTTON_RUN;

	buttons3dsPressed[BTN3DS_L] = (keysHeld3ds & KEY_L);
	if (keysHeld3ds & KEY_L) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_L];
    
	buttons3dsPressed[BTN3DS_R] = (keysHeld3ds & KEY_R);
	if (keysHeld3ds & KEY_R) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_R];
    
	buttons3dsPressed[BTN3DS_A] = (keysHeld3ds & KEY_A);
    if (keysHeld3ds & KEY_A) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_A];
    
	buttons3dsPressed[BTN3DS_B] = (keysHeld3ds & KEY_B);
    if (keysHeld3ds & KEY_B) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_B];
    
	buttons3dsPressed[BTN3DS_X] = (keysHeld3ds & KEY_X);
    if (keysHeld3ds & KEY_X) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_X];
    
	buttons3dsPressed[BTN3DS_Y] = (keysHeld3ds & KEY_Y);
    if (keysHeld3ds & KEY_Y) consoleJoyPad |= settings3DS.ButtonMapping[BTN3DS_Y];

    // Handle turbo / rapid fire buttons.
    //
    #define HANDLE_TURBO(i, mask) \
		if (settings3DS.Turbo[i] && buttons3dsPressed[i]) { \
			if (!prevConsoleButtonPressed[i]) \
			{ \
				prevConsoleButtonPressed[i] = 11 - settings3DS.Turbo[i]; \
			} \
			else \
			{ \
				prevConsoleButtonPressed[i]--; \
				consoleJoyPad &= ~mask; \
			} \
		} \

    HANDLE_TURBO(BTN3DS_L, settings3DS.ButtonMapping[BTN3DS_L]);
    HANDLE_TURBO(BTN3DS_R, settings3DS.ButtonMapping[BTN3DS_R]);
    HANDLE_TURBO(BTN3DS_A, settings3DS.ButtonMapping[BTN3DS_A]);
    HANDLE_TURBO(BTN3DS_B, settings3DS.ButtonMapping[BTN3DS_B]);
    HANDLE_TURBO(BTN3DS_X, settings3DS.ButtonMapping[BTN3DS_X]);
    HANDLE_TURBO(BTN3DS_Y, settings3DS.ButtonMapping[BTN3DS_Y]);

    prevConsoleJoyPad = consoleJoyPad;

    io.button_status[0] = consoleJoyPad ^ 0xFFF;
}


//---------------------------------------------------------
// The following pipeline is used if the 
// emulation engine does software rendering.
//
// You can potentially 'hide' the wait latencies by
// waiting only after some work on the main thread
// is complete.
//---------------------------------------------------------

int lastWait = 0;
#define WAIT_PPF		1
#define WAIT_P3D		2

int currentFrameIndex = 0;


void impl3dsRenderTransferSoftRenderedScreenToTexture(u32 *buffer, int textureIndex)
{
	t3dsStartTiming(11, "FlushDataCache");
	GSPGPU_FlushDataCache(buffer, 512*240*4);
	t3dsEndTiming(11);

	t3dsStartTiming(12, "DisplayTransfer");
	GX_DisplayTransfer(
		(uint32 *)(buffer), GX_BUFFER_DIM(512, 240),
		(uint32 *)(emuMainScreenTarget[textureIndex]->PixelData), GX_BUFFER_DIM(512, 240),
		GX_TRANSFER_OUT_TILED(1) |
		GX_TRANSFER_FLIP_VERT(1) |
		0
	);
	t3dsEndTiming(12);
}

extern VerticalSections screenWidthVerticalSection;

void impl3dsRenderDrawTextureToTopFrameBuffer(SGPUTexture *texture, int tx_offset, int ty_offset)
{
	t3dsStartTiming(14, "Draw Texture");

    int scrWidth;
    bool cropped = false;

/*
    {
        gpu3dsUseShader(1);
        gpu3dsSetTextureEnvironmentReplaceColor();
        gpu3dsDrawRectangle(0, 0, 400, 240, 0, 0xff00ffff);
        gpu3dsSetRenderTargetToTexture(emuMainScreenHWTarget, emuDepthForScreens);
        gpu3dsSetTextureEnvironmentReplaceTexture0();
        gpu3dsBindTexture(emuTileCacheTexture, GPU_TEXUNIT0);

        gpu3dsDisableAlphaTest();
        gpu3dsDisableDepthTest();

        static int counter = 0;
        int texturePosition = 0;
        int tx = texturePosition % 128;
        int ty = (texturePosition / 128) & 0x7f;
        texturePosition = (127 - ty) * 128 + tx;    // flip vertically.
        uint32 base = texturePosition * 64;

        uint16 *tileTexture = (uint16 *)emuTileCacheTexture->PixelData;
        u16 *dest = emuTileCacheTexture->PixelData;
        for (int i = 0; i < 64; i++)
        {
            dest[base + i] = ((i + counter) & 0xffff) * 4;
        }
        counter++;

        {
        int x = 40;
        int y = 50;
                gpu3dsAddTileVertexes(
                    x, y, x + 8, y + 8,
                    0, 0,
                    8, 8, 0);
        }
        gpu3dsDrawVertexes();
    }*/

    // Draw a black colored rectangle covering the entire screen.
    //
    gpu3dsUseShader(1);
    gpu3dsSetRenderTargetToTopFrameBuffer();
	switch (settings3DS.ScreenStretch)
	{
		case 0:
            scrWidth = 256;
			break;
		case 1:
            scrWidth = 320;
			break;
		case 2:
            scrWidth = 400;
			break;
		case 3:
            scrWidth = 320;
            cropped = true;
			break;
		case 4:
            scrWidth = 400;
            cropped = true;
			break;
	}

    int sideBorderWidth = (400 - scrWidth) / 2;
    gpu3dsSetTextureEnvironmentReplaceColor();
    gpu3dsDrawRectangle(0, 0, sideBorderWidth, 240, 0, 0x0000ffff);
    gpu3dsDrawRectangle(400 - sideBorderWidth, 0, 400, 240, 0, 0x0000ffff);

    gpu3dsUseShader(0);
    gpu3dsSetTextureEnvironmentReplaceTexture0();
    gpu3dsBindTextureMainScreen(texture, GPU_TEXUNIT0);

/*    // Software rendering:
    gpu3dsAddQuadVertexes(
        sideBorderWidth, 0, 400 - sideBorderWidth, 240, 
        0, 0, 
        256, 240, 0);
    gpu3dsDrawVertexes();
*/

    // Hardware rendering:
    vsectCommit(&screenWidthVerticalSection, vce.frame_counter - 14);

    for (int i = 0; i < screenWidthVerticalSection.Count; i++)
    {
        int startY = screenWidthVerticalSection.Section[i].StartY;
        int endY = screenWidthVerticalSection.Section[i].EndY;
        int pceWidth = screenWidthVerticalSection.Section[i].Value;

        //printf ("width sect: %d (w:%3d) %3d to %3d\n", i, pceWidth, startY, endY);

        if (pceWidth == 0)
            continue;
        if (startY > endY)
            continue;
        if (pceWidth == 320)
            pceWidth = 344;

        float tx0 = 0, ty0 = startY;
        float tx1 = pceWidth, ty1 = endY + 1;
        if (cropped)
        {
            tx0 += 8; ty0 += 8;
            tx1 -= 8; ty1 -= 8;            
        }
        if (scrWidth == 320)
        {
            tx0 += 0.2;
            tx1 -= 0.2;
        }

        gpu3dsAddQuadVertexes(
            sideBorderWidth, startY, 400 - sideBorderWidth, endY + 1, 
            tx0 + tx_offset, ty0 + ty_offset, 
            tx1 + tx_offset, ty1 + ty_offset, 0);
    }

    gpu3dsDrawVertexes();

    vsectReset(&screenWidthVerticalSection, 0, 0);

    //gpu3dsSetTextureEnvironmentReplaceTexture0();
    //gpu3dsBindTextureMainScreen(emuTileCacheTexture, GPU_TEXUNIT0);
    //gpu3dsAddQuadVertexes(0, 0, 200, 200, 0, 0, 256, 256, 0);
    //gpu3dsDrawVertexes();

	t3dsEndTiming(14);

}


//---------------------------------------------------------
// Executes one frame and draw to the screen.
//
// Note: TRUE will be passed in the firstFrame if this
// frame is to be run just after the emulator has booted
// up or returned from the menu.
//---------------------------------------------------------

void impl3dsEmulationRunOneFrame(bool firstFrame, bool skipDrawingFrame)
{
    // Hardware rendering:
    //
    skipDrawingFrame = false;
	t3dsStartTiming(1, "RunOneFrame");

	t3dsStartTiming(10, "EmulateFrame");
	{
		impl3dsEmulationPollInput();

        if (!skipDrawingFrame)
			currentFrameIndex ^= 1;

        gpu3dsUseShader(1);
        gpu3dsSetRenderTargetToTexture(emuMainScreenHWTarget, emuDepthForScreens);
		update_frame(skipDrawingFrame);
        gpu3dsDrawVertexes();


        // debugging only
        /*render_psg(soundSamplesPerGeneration * 2);

        for (int i = 0; i < soundSamplesPerGeneration * 2; i += 2)
        {
            printf ("%4x", (u16)audio.buffer[i*2]);
        }
        printf ("---------------------------\n\n");
*/
        /*printf ("%10llx - %10llx = %10lld\n", 
            cpu.global_cycles, psg.cpu_sync_cycles >> step_fractional_bits_clock, 
            cpu.global_cycles - (psg.cpu_sync_cycles >> step_fractional_bits_clock));
        */
	}
	t3dsEndTiming(10);

	if (!skipDrawingFrame)
	{
		impl3dsRenderDrawTextureToTopFrameBuffer(emuMainScreenHWTarget, 32, 0);	
	}

	if (!skipDrawingPreviousFrame)
	{
		t3dsStartTiming(16, "Transfer");
		gpu3dsTransferToScreenBuffer();	
		t3dsEndTiming(16);

		t3dsStartTiming(19, "SwapBuffers");
		gpu3dsSwapScreenBuffers();
		t3dsEndTiming(19);
	}

	t3dsStartTiming(15, "Flush");
	gpu3dsFlush();
	t3dsEndTiming(15);

	skipDrawingPreviousFrame = skipDrawingFrame;
	t3dsEndTiming(1);
    

    /*
    // Software rendering:
    //
    skipDrawingFrame = false;
	t3dsStartTiming(1, "RunOneFrame");

	t3dsStartTiming(10, "EmulateFrame");
	{
		impl3dsEmulationPollInput();

		update_frame(skipDrawingFrame);
        if (!skipDrawingFrame)
			currentFrameIndex ^= 1;
	}
	t3dsEndTiming(10);

	if (!skipDrawingPreviousFrame)
	{
		t3dsStartTiming(16, "Transfer");
		gpu3dsTransferToScreenBuffer();	
		t3dsEndTiming(16);

		t3dsStartTiming(19, "SwapBuffers");
		gpu3dsSwapScreenBuffers();
		t3dsEndTiming(19);
	}

	if (!skipDrawingFrame)
	{
		// Transfer current screen to the texture
		impl3dsRenderTransferSoftRenderedScreenToTexture(
			bufferRGBA[currentFrameIndex], currentFrameIndex);
	}

	if (!skipDrawingPreviousFrame)
	{
		// emuMainScreenTarget[prev] -> GPU3DS.framebuffer (not flushed)
		impl3dsRenderDrawTextureToTopFrameBuffer(emuMainScreenTarget[currentFrameIndex ^ 1], 32, 16);	
        gpu3dsFlush();
	}

	skipDrawingPreviousFrame = skipDrawingFrame;
	t3dsEndTiming(1);
    */
}


//---------------------------------------------------------
// Finalize any variables or state of the GPU
// before the emulation loop ends and control 
// goes into the menu.
//---------------------------------------------------------
void impl3dsEmulationEnd()
{
	// We have to do this to clear the wait event
	//
	/*if (lastWait != 0 && emulator.isReal3DS)
	{
		if (lastWait == WAIT_PPF)
			gspWaitForPPF();
		else 
		if (lastWait == WAIT_P3D)
			gpu3dsWaitForPreviousFlush();
	}*/
}



//---------------------------------------------------------
// This is called when the bottom screen is touched
// during emulation, and the emulation engine is ready
// to display the pause menu.
//
// Use this to save the SRAM to SD card, if applicable.
//---------------------------------------------------------
void impl3dsEmulationPaused()
{
    {
        ui3dsDrawRect(50, 140, 270, 154, 0x000000);
        ui3dsDrawStringWithNoWrapping(50, 140, 270, 154, 0x3f7fff, HALIGN_CENTER, "Saving SRAM to SD card...");
    }
}


//---------------------------------------------------------
// This is called when the user chooses to save the state.
// This function should save the state into a file whose
// name contains the slot number. This will return
// true if the state is saved successfully.
//
// The slotNumbers passed in start from 1.
//---------------------------------------------------------
bool impl3dsSaveState(int slotNumber)
{
	char ext[_MAX_PATH];
	sprintf(ext, ".st%d", slotNumber - 1);

    save_state(file3dsReplaceFilenameExtension(romFileNameFullPath, ext), NULL);

    return true;
}


//---------------------------------------------------------
// This is called when the user chooses to load the state.
// This function should save the state into a file whose
// name contains the slot number. This will return
// true if the state is loaded successfully.
//
// The slotNumbers passed in start from 1.
//---------------------------------------------------------
bool impl3dsLoadState(int slotNumber)
{
	char ext[_MAX_PATH];
	sprintf(ext, ".st%d", slotNumber - 1);

    impl3dsResetConsole();
    load_state(file3dsReplaceFilenameExtension(romFileNameFullPath, ext), NULL, 0);

    return true;
}


//---------------------------------------------------------
// This function will be called everytime the user
// selects an action on the menu.
//
// Returns true if the menu should close and the game 
// should resume
//---------------------------------------------------------
bool impl3dsOnMenuSelected(int ID)
{
    return false;
}



//---------------------------------------------------------
// This function will be called everytime the user 
// changes the value in the specified menu item.
//
// Returns true if the menu should close and the game 
// should resume
//---------------------------------------------------------
bool impl3dsOnMenuSelectedChanged(int ID, int value)
{
    if (ID == 18000)
    {
        ui3dsSetFont(value);
        return false;
    }
    
    return false;
}


//---------------------------------------------------------
// Initializes the default global and game-specifi
// settings. This method is called everytime a game is
// loaded, but the configuration file does not exist.
//---------------------------------------------------------
void impl3dsInitializeDefaultSettings()
{
	settings3DS.MaxFrameSkips = 1;
	settings3DS.ForceFrameRate = 0;
	settings3DS.Volume = 4;

	for (int i = 0; i < 6; i++)     // and clear all turbo buttons.
		settings3DS.Turbo[i] = 0;
	settings3DS.ButtonMapping[0] = IO_BUTTON_I;
	settings3DS.ButtonMapping[1] = IO_BUTTON_II;
	settings3DS.ButtonMapping[2] = IO_BUTTON_III;
	settings3DS.ButtonMapping[3] = IO_BUTTON_IV;
	settings3DS.ButtonMapping[4] = IO_BUTTON_V;
	settings3DS.ButtonMapping[5] = IO_BUTTON_VI;

    settings3DS.OtherOptions[SETTINGS_IDLELOOPPATCH] = 1;	
    settings3DS.OtherOptions[SETTINGS_SOFTWARERENDERING] = 0;	
}



//----------------------------------------------------------------------
// Read/write all possible game specific settings into a file 
// created in this method.
//
// This must return true if the settings file exist.
//----------------------------------------------------------------------
bool impl3dsReadWriteSettingsByGame(bool writeMode)
{
    bool success = config3dsOpenFile(file3dsReplaceFilenameExtension(romFileNameFullPath, ".cfg"), writeMode);
    if (!success)
        return false;

    config3dsReadWriteInt32("#v1\n", NULL, 0, 0);
    config3dsReadWriteInt32("# Do not modify this file or risk losing your settings.\n", NULL, 0, 0);

    // set default values first.
    if (!writeMode)
    {
        settings3DS.PaletteFix = 0;
        settings3DS.SRAMSaveInterval = 0;
    }

    config3dsReadWriteInt32("Frameskips=%d\n", &settings3DS.MaxFrameSkips, 0, 4);
    config3dsReadWriteInt32("Framerate=%d\n", &settings3DS.ForceFrameRate, 0, 2);
    config3dsReadWriteInt32("TurboA=%d\n", &settings3DS.Turbo[0], 0, 10);
    config3dsReadWriteInt32("TurboB=%d\n", &settings3DS.Turbo[1], 0, 10);
    config3dsReadWriteInt32("TurboX=%d\n", &settings3DS.Turbo[2], 0, 10);
    config3dsReadWriteInt32("TurboY=%d\n", &settings3DS.Turbo[3], 0, 10);
    config3dsReadWriteInt32("TurboL=%d\n", &settings3DS.Turbo[4], 0, 10);
    config3dsReadWriteInt32("TurboR=%d\n", &settings3DS.Turbo[5], 0, 10);
    config3dsReadWriteInt32("Vol=%d\n", &settings3DS.Volume, 0, 8);
    config3dsReadWriteInt32("SRAMInterval=%d\n", &settings3DS.SRAMSaveInterval, 0, 4);
    config3dsReadWriteInt32("ButtonMapA=%d\n", &settings3DS.ButtonMapping[0], 0, 0xffff);
    config3dsReadWriteInt32("ButtonMapB=%d\n", &settings3DS.ButtonMapping[1], 0, 0xffff);
    config3dsReadWriteInt32("ButtonMapX=%d\n", &settings3DS.ButtonMapping[2], 0, 0xffff);
    config3dsReadWriteInt32("ButtonMapY=%d\n", &settings3DS.ButtonMapping[3], 0, 0xffff);
    config3dsReadWriteInt32("ButtonMapL=%d\n", &settings3DS.ButtonMapping[4], 0, 0xffff);
    config3dsReadWriteInt32("ButtonMapR=%d\n", &settings3DS.ButtonMapping[5], 0, 0xffff);

    // All new options should come here!

    config3dsCloseFile();
    return true;
}


//----------------------------------------------------------------------
// Read/write all possible global specific settings into a file 
// created in this method.
//
// This must return true if the settings file exist.
//----------------------------------------------------------------------
bool impl3dsReadWriteSettingsGlobal(bool writeMode)
{
    bool success = config3dsOpenFile("./temperpce_3ds.cfg", writeMode);
    if (!success)
        return false;
    
    config3dsReadWriteInt32("#v1\n", NULL, 0, 0);
    config3dsReadWriteInt32("# Do not modify this file or risk losing your settings.\n", NULL, 0, 0);

    config3dsReadWriteInt32("ScreenStretch=%d\n", &settings3DS.ScreenStretch, 0, 7);
    config3dsReadWriteInt32("HideUnnecessaryBottomScrText=%d\n", &settings3DS.HideUnnecessaryBottomScrText, 0, 1);
    config3dsReadWriteInt32("Font=%d\n", &settings3DS.Font, 0, 2);

    // Fixes the bug where we have spaces in the directory name
    config3dsReadWriteString("Dir=%s\n", "Dir=%1000[^\n]s\n", file3dsGetCurrentDir());
    config3dsReadWriteString("ROM=%s\n", "ROM=%1000[^\n]s\n", romFileNameLastSelected);

    // All new options should come here!

    config3dsCloseFile();
    return true;
}



//----------------------------------------------------------------------
// Apply settings into the emulator.
//
// This method normally copies settings from the settings3DS struct
// and updates the emulator's core's configuration.
//
// This must return true if any settings were modified.
//----------------------------------------------------------------------
bool impl3dsApplyAllSettings(bool updateGameSettings)
{
    bool settingsChanged = true;

    // update screen stretch
    //
    if (settings3DS.ScreenStretch == 0)
    {
        settings3DS.StretchWidth = 256;
        settings3DS.StretchHeight = 240;    // Actual height
        settings3DS.CropPixels = 0;
    }
    else if (settings3DS.ScreenStretch == 1)
    {
        // Added support for 320x240 (4:3) screen ratio
        settings3DS.StretchWidth = 320;
        settings3DS.StretchHeight = 240;
        settings3DS.CropPixels = 0;
    }
    else if (settings3DS.ScreenStretch == 2)
    {
        settings3DS.StretchWidth = 400;
        settings3DS.StretchHeight = 240;
        settings3DS.CropPixels = 0;
    }

    // Update the screen font
    //
    ui3dsSetFont(settings3DS.Font);

    if (updateGameSettings)
    {
        if (settings3DS.ForceFrameRate == 0)
            settings3DS.TicksPerFrame = TICKS_PER_SEC / impl3dsGetROMFrameRate();

        if (settings3DS.ForceFrameRate == 1)
            settings3DS.TicksPerFrame = TICKS_PER_FRAME_PAL;

        else if (settings3DS.ForceFrameRate == 2)
            settings3DS.TicksPerFrame = TICKS_PER_FRAME_NTSC;

        // update global volume
        //
        if (settings3DS.Volume < 0)
            settings3DS.Volume = 0;
        if (settings3DS.Volume > 8)
            settings3DS.Volume = 8;

        if (settings3DS.OtherOptions[SETTINGS_SOFTWARERENDERING])
            config.software_rendering = 1;
        else
            config.software_rendering = 0;

        if (settings3DS.OtherOptions[SETTINGS_IDLELOOPPATCH])
            config.patch_idle_loops = 1;
        else
            config.patch_idle_loops = 0;
        
    }

    return settingsChanged;
}


//----------------------------------------------------------------------
// Copy values from menu to settings3DS structure,
// or from settings3DS structure to the menu, depending on the
// copyMenuToSettings parameter.
//
// This must return return if any of the settings were changed.
//----------------------------------------------------------------------
bool impl3dsCopyMenuToOrFromSettings(bool copyMenuToSettings)
{
#define UPDATE_SETTINGS(var, tabIndex, ID)  \
    if (copyMenuToSettings && (var) != menu3dsGetValueByID(tabIndex, ID)) \
    { \
        var = menu3dsGetValueByID(tabIndex, ID); \
        settingsUpdated = true; \
    } \
    if (!copyMenuToSettings) \
    { \
        menu3dsSetValueByID(tabIndex, ID, (var)); \
    }

    bool settingsUpdated = false;
    UPDATE_SETTINGS(settings3DS.Font, 1, 18000);
    UPDATE_SETTINGS(settings3DS.ScreenStretch, 1, 11000);
    UPDATE_SETTINGS(settings3DS.HideUnnecessaryBottomScrText, 1, 15001);
    UPDATE_SETTINGS(settings3DS.MaxFrameSkips, 1, 10000);
    UPDATE_SETTINGS(settings3DS.ForceFrameRate, 1, 12000);
    UPDATE_SETTINGS(settings3DS.Turbo[0], 1, 13000);
    UPDATE_SETTINGS(settings3DS.Turbo[1], 1, 13001);
    UPDATE_SETTINGS(settings3DS.Turbo[2], 1, 13002);
    UPDATE_SETTINGS(settings3DS.Turbo[3], 1, 13003);
    UPDATE_SETTINGS(settings3DS.Turbo[4], 1, 13004);
    UPDATE_SETTINGS(settings3DS.Turbo[5], 1, 13005);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[0], 1, 13010);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[1], 1, 13011);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[2], 1, 13012);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[3], 1, 13013);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[4], 1, 13014);
    UPDATE_SETTINGS(settings3DS.ButtonMapping[5], 1, 13015);
    UPDATE_SETTINGS(settings3DS.Volume, 1, 14000);
    UPDATE_SETTINGS(settings3DS.PaletteFix, 1, 16000);
    UPDATE_SETTINGS(settings3DS.SRAMSaveInterval, 1, 17000);
    UPDATE_SETTINGS(settings3DS.OtherOptions[SETTINGS_SOFTWARERENDERING], 1, 19000);
    UPDATE_SETTINGS(settings3DS.OtherOptions[SETTINGS_IDLELOOPPATCH], 1, 20000);

    return settingsUpdated;
	
}



//----------------------------------------------------------------------
// Clears all cheats from the core.
//
// This method is called only when cheats are loaded.
// This only happens after a new ROM is loaded.
//----------------------------------------------------------------------
void impl3dsClearAllCheats()
{
}


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
bool impl3dsAddCheat(bool cheatEnabled, char *name, char *code)
{
}


//----------------------------------------------------------------------
// Enable/disables a cheat in the emulator core.
// 
// This method will be triggered when the user enables/disables
// cheats in the cheat menu.
//----------------------------------------------------------------------
void impl3dsSetCheatEnabledFlag(int cheatIdx, bool enabled)
{
}


void *get_screen_ptr()
{
    return bufferRGBA[currentFrameIndex];
}
