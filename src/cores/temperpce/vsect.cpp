
#include "stdio.h"
#include "vsect.h"

//------------------------------------------------------------------
// Use this to manage vertical sections for brightness, and
// other stuff that change in-frame so as to minimize
// render_flush
//------------------------------------------------------------------

// Reset vertical sections with a specific value.
//
void vsectReset(VerticalSections *verticalSections, int currentScanline, u32 currentValue)
{
    if (currentScanline > 240 || currentScanline < 0)
        currentScanline = 240;
    
    verticalSections->CurrentValue = currentValue;
    verticalSections->StartY = currentScanline;
    verticalSections->Count = 0;
    //printf ("Reset %d %d\n", currentScanline, currentValue);
}


// Reset all vertical sections with no change in the register value.
//
void vsectReset(VerticalSections *verticalSections, int currentScanline)
{
    if (currentScanline > 240 || currentScanline < 0)
        currentScanline = 240;
    
    verticalSections->StartY = currentScanline;
    verticalSections->Count = 0;
    //printf ("Reset %d\n", currentScanline);
}


// Commits the final value to the list.
// This should be called just at the start of v-blank
// and before the final parts of the screen is rendered.
// 
void vsectCommit(VerticalSections *verticalSections, int currentScanline)
{
    if (currentScanline > 240 || currentScanline < 0)
        currentScanline = 240;
	if (currentScanline != verticalSections->StartY)
	{
		verticalSections->Section[verticalSections->Count].StartY = verticalSections->StartY;
		verticalSections->Section[verticalSections->Count].EndY = currentScanline - 1;
		verticalSections->Section[verticalSections->Count].Value = verticalSections->CurrentValue;
		verticalSections->Count ++;

        if (currentScanline == 240)
            currentScanline = 0;
		verticalSections->StartY = currentScanline;
        //printf ("commit value: sy=%d cnt=%d val=%d\n", currentScanline, verticalSections->Count, verticalSections->CurrentValue);
	}
}

// Sets a new value to this section, and commits it
// if the current scanline is different from the last.
//
void vsectUpdateValue(VerticalSections *verticalSections, int currentScanline, u32 newValue)
{
    if (currentScanline > 240 || currentScanline < 0)
        currentScanline = 240;
	if (verticalSections->CurrentValue != newValue)
	{
		verticalSections->Section[verticalSections->Count].StartY = verticalSections->StartY;
		verticalSections->Section[verticalSections->Count].EndY = currentScanline - 1;
		verticalSections->Section[verticalSections->Count].Value = verticalSections->CurrentValue;
		verticalSections->Count ++;

        if (currentScanline == 240)
            currentScanline = 0;
		verticalSections->StartY = currentScanline;
        //printf ("update value: sy=%d cnt=%d val=%d (%d)\n", currentScanline, verticalSections->Count, verticalSections->CurrentValue, newValue);
        verticalSections->CurrentValue = newValue;

	}
}

