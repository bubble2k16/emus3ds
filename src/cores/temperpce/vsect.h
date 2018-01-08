
#ifndef _PPUVSECT_H_
#define _PPUVSECT_H_

#include "3ds.h"

typedef struct 
{
    int                 StartY;
    int                 EndY;
    s32                 Value;
    s32                 Value2;
    s32                 Value3;
    s32                 Value4;

} VerticalSection;

typedef struct 
{
    int                 StartY;
    s32                 CurrentValue;
    s32                 CurrentValue2;
    s32                 CurrentValue3;
    s32                 CurrentValue4;
    int                 Count;
    VerticalSection     Section[256];
} VerticalSections;



// Methods related to managing vertical sections for any general registers.
//
void vsectReset(VerticalSections *verticalSections);
void vsectCommit(VerticalSections *verticalSections, int endY);

void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue);
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2);
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2, s32 newValue3);
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2, s32 newValue3, s32 newValue4);


#endif