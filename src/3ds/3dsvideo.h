
#ifndef _3DSVIDEO_H_
#define _3DSVIDEO_H_


bool video3dsInitializeHardwareRendering();

bool video3dsInitializeSoftwareRendering(int width, int height, GX_TRANSFER_FORMAT bufferFormat);

SGPUTexture *video3dsGetPreviousScreenTexture();

SGPUTexture *video3dsGetCurrentScreenTexture();

void *video3dsGetPreviousSoftwareBuffer();

void *video3dsGetCurrentSoftwareBuffer();

void video3dsStartNewSoftwareRenderedFrame();

void video3dsTransferFrameBufferToScreenAndSwap();

void video3dsCopySoftwareBufferToTexture();

void video3dsFinalize();

#endif
