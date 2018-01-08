
#ifndef _3DSVIDEO_H_
#define _3DSVIDEO_H_


//-----------------------------------------------------------------------------
// Initialize textures for hardware rendering.
//-----------------------------------------------------------------------------
bool video3dsInitializeHardwareRendering();

//-----------------------------------------------------------------------------
// Initialize textures and buffers for software rendering.
//-----------------------------------------------------------------------------
bool video3dsInitializeSoftwareRendering(int width, int height, GX_TRANSFER_FORMAT bufferFormat);

//-----------------------------------------------------------------------------
// Get the previous screen texture.
//-----------------------------------------------------------------------------
SGPUTexture *video3dsGetPreviousScreenTexture();

//-----------------------------------------------------------------------------
// Get the current screen texture.
//-----------------------------------------------------------------------------
SGPUTexture *video3dsGetCurrentScreenTexture();

//-----------------------------------------------------------------------------
// Clear the software buffers.
//-----------------------------------------------------------------------------
void *video3dsClearAllSoftwareBuffers();

//-----------------------------------------------------------------------------
// Get the previous software screen buffers.
//-----------------------------------------------------------------------------
void *video3dsGetPreviousSoftwareBuffer();

//-----------------------------------------------------------------------------
// Get the current software screen buffers.
//-----------------------------------------------------------------------------
void *video3dsGetCurrentSoftwareBuffer();

//-----------------------------------------------------------------------------
// Swaps the software buffers and textures.
//-----------------------------------------------------------------------------
void video3dsStartNewSoftwareRenderedFrame();

//-----------------------------------------------------------------------------
// Swaps the software buffers and textures.
//-----------------------------------------------------------------------------
void video3dsStartNewHardwareRenderedFrame();

//-----------------------------------------------------------------------------
// Transfer the screen from the frame buffer and swap. 
// This should be called as soon as possible in impl3dsEmulationRunOneFrame.
//-----------------------------------------------------------------------------
void video3dsTransferFrameBufferToScreenAndSwap();

//-----------------------------------------------------------------------------
// Performs a transfer from the software buffer to the hardware texture,
// which will then be used to draw to the frame buffer later on.
//-----------------------------------------------------------------------------
void video3dsCopySoftwareBufferToTexture();

//-----------------------------------------------------------------------------
// Frees up all buffers and textures.
//-----------------------------------------------------------------------------
void video3dsFinalize();

#endif
