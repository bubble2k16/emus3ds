
#ifndef _PPUVSECT_H_
#define _PPUVSECT_H_

#include "3ds.h"

typedef struct 
{
    int                 StartY;
    int                 EndY;
    union
    {
        u32             Value;
        struct
        {
            u8          V1;
            u8          V2;
            u8          V3;
            u8          V4;
        };
    };
} VerticalSection;

typedef struct 
{
    int                 StartY;
    u32                 CurrentValue;
    int                 Count;
    VerticalSection     Section[256];
} VerticalSections;



// Methods related to managing vertical sections for any general SNES registers.
//
void vsectReset(VerticalSections *verticalSections, int currentScanline);
void vsectReset(VerticalSections *verticalSections, int currentScanline, u32 currentValue);
void vsectCommit(VerticalSections *verticalSections, int currentScanline);
void vsectUpdateValue(VerticalSections *verticalSections, int currentScanline, u32 newValue);


#endif