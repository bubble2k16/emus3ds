
#ifndef _PALETTE_H_
#define _PALETTE_H_

#include "3ds.h"

typedef struct
{
    u8      red;
    u8      green;
    u8      blue;
} SNesPalette;

extern SNesPalette     nesPalette[64];
extern u16             rgbaPalette[16][64];

void nespalInitialize();

#endif