
#include "stdio.h"
#include "3ds.h"
#include "3dsgpu.h"
#include "3dsopt.h"
#include "3dsvideo.h"

typedef struct
{
    float   tx1, ty1, tx2, ty2;
    int     sx1, sy1, sx2, sy2;
} SDrawToScreenVertex;


static int                      videoBufferIndex = 0;
static int                      videoBufferWidth = 0;
static int                      videoBufferHeight = 0;

static int                      videoSoftwareBufferBitDepth = 16;
static GX_TRANSFER_FORMAT       videoSoftwareBufferFormat = GX_TRANSFER_FMT_RGBA8;

static int                      videoVertexCount[2] = { 0, 0 };
static SDrawToScreenVertex      videoVertices0[512];
static SDrawToScreenVertex      videoVertices1[512];
static SDrawToScreenVertex      *videoVertices[2] = { videoVertices0, videoVertices1 };

static SGPUTexture              *videoScreenTexture[2] = { NULL, NULL };
SGPUTexture                     *videoHardwareScreenTexture = NULL;
SGPUTexture                     *videoHardwareScreenDepth = NULL;
 void                     *videoSoftwareBuffer[2] = { NULL, NULL };

static bool                     videoSkipPreviousFrame = false;


bool video3dsInitializeHardwareRendering(int width, int height)
{
    video3dsFinalize();
    videoBufferWidth = width;
    videoBufferHeight = height;

    // Allocate texture for main screen & depth in VRAM
    //
    videoScreenTexture[0] = gpu3dsCreateTextureInVRAM(videoBufferWidth, videoBufferHeight, GPU_RGBA8);       // 0.250 MB
    videoScreenTexture[1] = gpu3dsCreateTextureInVRAM(videoBufferWidth, videoBufferHeight, GPU_RGBA8);         // 0.250 MB

    if (videoScreenTexture[0] == NULL || videoScreenTexture[1] == NULL)
    {
        printf("Unable to initialize videoScreenTexture\n");
        return false;
    }
    return true;
}

bool video3dsInitializeSoftwareRendering(int width, int height, GX_TRANSFER_FORMAT bufferFormat)
{
    video3dsFinalize();
    int textureFormat = GPU_RGBA8;

    if (bufferFormat == GX_TRANSFER_FMT_RGBA8)
    {
        videoSoftwareBufferBitDepth = 32;
        textureFormat = GPU_RGBA8;
    }
    else if (bufferFormat == GX_TRANSFER_FMT_RGB565)
    {
        videoSoftwareBufferBitDepth = 16;
        textureFormat = GPU_RGB565;
    }
    else if (bufferFormat == GX_TRANSFER_FMT_RGB5A1)
    {
        videoSoftwareBufferBitDepth = 16;
        textureFormat = GPU_RGBA5551;
    }
    else
    {
        printf("Software buffer format not supported. Only GX_TRANSFER_FMT_RGBA8, GX_TRANSFER_FMT_RGB565, GX_TRANSFER_FMT_RGB5A1 supported.\n");
        return false;
    }
    videoSoftwareBufferFormat = bufferFormat;
    videoBufferWidth = width;
    videoBufferHeight = height;

    // Allocate texture for main screen in VRAM.
    //
    videoScreenTexture[0] = gpu3dsCreateTextureInVRAM(videoBufferWidth, videoBufferHeight, textureFormat);  
    videoScreenTexture[1] = gpu3dsCreateTextureInVRAM(videoBufferWidth, videoBufferHeight, textureFormat);  

    // Allocate buffer in linear memory to be used by the emulator to draw into
    //
	videoSoftwareBuffer[0] = linearMemAlign(videoBufferWidth * videoBufferHeight * videoSoftwareBufferBitDepth / 8, 0x80);
	videoSoftwareBuffer[1] = linearMemAlign(videoBufferWidth * videoBufferHeight * videoSoftwareBufferBitDepth / 8, 0x80);

    if (videoScreenTexture[0] == NULL || videoScreenTexture[1] == NULL)
    {
        printf("Unable to initialize videoScreenTexture\n");
        return false;
    }
    if (videoSoftwareBuffer[0] == NULL || videoSoftwareBuffer[1] == NULL)
    {
        printf("Unable to initialize videoSoftwareBuffer\n");
        return false;
    }
    return true;
}

SGPUTexture *video3dsGetPreviousScreenTexture()
{
    return videoScreenTexture[videoBufferIndex ^ 1];
}

SGPUTexture *video3dsGetCurrentScreenTexture()
{
    return videoScreenTexture[videoBufferIndex];
}

void *video3dsGetPreviousSoftwareBuffer()
{
    return videoSoftwareBuffer[videoBufferIndex ^ 1];
}

void *video3dsGetCurrentSoftwareBuffer()
{
    return videoSoftwareBuffer[videoBufferIndex];
}

void video3dsStartNewSoftwareRenderedFrame()
{
    videoBufferIndex = videoBufferIndex ^ 1;
}

void video3dsStartNewHardwareRenderedFrame()
{
    videoBufferIndex = videoBufferIndex ^ 1;

    gpu3dsSetRenderTargetToTexture(videoScreenTexture[0], videoScreenTexture[1]);
}


void video3dsTransferFrameBufferToScreenAndSwap()
{
    t3dsStartTiming(16, "Transfer");
    gpu3dsTransferToScreenBuffer();	
    t3dsEndTiming(16);

    t3dsStartTiming(19, "SwapBuffers");
    gpu3dsSwapScreenBuffers();
    t3dsEndTiming(19);
}

void video3dsCopySoftwareBufferToTexture()
{
    t3dsStartTiming(11, "FlushDataCache");
    GSPGPU_FlushDataCache(videoSoftwareBuffer[videoBufferIndex], videoBufferWidth * videoBufferHeight * videoSoftwareBufferBitDepth / 8);
    t3dsEndTiming(11);

    t3dsStartTiming(12, "DisplayTransfer");
    GX_DisplayTransfer(
        (uint32 *)(videoSoftwareBuffer[videoBufferIndex]), GX_BUFFER_DIM(videoBufferWidth, videoBufferHeight),
        (uint32 *)(videoScreenTexture[videoBufferIndex]->PixelData), GX_BUFFER_DIM(videoBufferWidth, videoBufferHeight),
        GX_TRANSFER_OUT_TILED(1) |
        GX_TRANSFER_FLIP_VERT(1) |
        GX_TRANSFER_IN_FORMAT(videoSoftwareBufferFormat) |
        GX_TRANSFER_OUT_FORMAT(videoSoftwareBufferFormat) |
        0
    );
    t3dsEndTiming(12);
}

void video3dsFinalize()
{
	if (videoSoftwareBuffer[0]) linearFree(videoSoftwareBuffer[0]);
	if (videoSoftwareBuffer[1]) linearFree(videoSoftwareBuffer[1]);
    
	if (videoScreenTexture[0]) gpu3dsDestroyTextureFromVRAM(videoScreenTexture[0]);
	if (videoScreenTexture[1]) gpu3dsDestroyTextureFromVRAM(videoScreenTexture[1]);

    videoSoftwareBuffer[0] = NULL;
    videoSoftwareBuffer[1] = NULL;
    videoScreenTexture[0] = NULL;
    videoScreenTexture[1] = NULL;
}
