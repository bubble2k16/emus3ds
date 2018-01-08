
#include "stdio.h"
#include "vsect.h"

//------------------------------------------------------------------
// Use this to manage vertical sections for brightness, and
// other stuff that change in-frame so as to minimize
// render_flush
//------------------------------------------------------------------


// Resets the counter of the vertical section to zero.
//
void vsectReset(VerticalSections *verticalSections)
{
    verticalSections->Count = 0;
}


// Commits the final value to the list.
// This should be called just at the start of v-blank
// and before the final parts of the screen is rendered.
// 
void vsectCommit(VerticalSections *verticalSections, int endY)
{
    verticalSections->Section[verticalSections->Count].StartY = verticalSections->StartY;
    verticalSections->Section[verticalSections->Count].EndY = endY;
    verticalSections->Section[verticalSections->Count].Value = verticalSections->CurrentValue;
    verticalSections->Section[verticalSections->Count].Value2 = verticalSections->CurrentValue2;
    verticalSections->Section[verticalSections->Count].Value3 = verticalSections->CurrentValue3;
    verticalSections->Section[verticalSections->Count].Value4 = verticalSections->CurrentValue4;
    verticalSections->Count++;
    //printf ("commit ey=%3d c=%d v=%d,%d,%d,%d\n", endY, verticalSections->Count, verticalSections->CurrentValue, verticalSections->CurrentValue2, verticalSections->CurrentValue3, verticalSections->CurrentValue4);
}


// Sets a new value to this section.
//
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue)
{
    vsectUpdateValue(verticalSections, startY, newValue, 0, 0, 0);
}


// Sets a new value to this section.
//
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2)
{
    vsectUpdateValue(verticalSections, startY, newValue, newValue2, 0, 0);
}


// Sets a new value to this section.
//
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2, s32 newValue3)
{
    vsectUpdateValue(verticalSections, startY, newValue, newValue2, newValue3, 0);
}


// Sets a new value to this section.
//
void vsectUpdateValue(VerticalSections *verticalSections, int startY, s32 newValue, s32 newValue2, s32 newValue3, s32 newValue4)
{
    verticalSections->StartY = startY;
    verticalSections->CurrentValue = newValue;
    verticalSections->CurrentValue2 = newValue2;
    verticalSections->CurrentValue3 = newValue3;
    verticalSections->CurrentValue4 = newValue4;
}