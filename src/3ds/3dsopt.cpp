
#define _3DSOPT_CPP_

#include <stdio.h>

#include "3dsopt.h"
#include "3dsemu.h"

#define TICKS_PER_SEC (268123)

//--------------------------------------------------------------------------------
// Variables used for the profiling.
//--------------------------------------------------------------------------------
char *t3dsClockName[100];
int t3dsTotalCount[100];
uint64 t3dsStartTicks[100];
uint64 t3dsTotalTicks[100];


char *emptyString = "";


//---------------------------------------------------------------
// Resets the timer.
//---------------------------------------------------------------
void t3dsResetTimings()
{
#ifndef EMU_RELEASE
	for (int i = 0; i < 100; i++)
    {
        t3dsTotalTicks[i] = 0; 
        t3dsTotalCount[i] = 0;
        t3dsClockName[i] = emptyString;
    }
#endif
}


//---------------------------------------------------------------
// Increments the counter.
//---------------------------------------------------------------
void t3dsCount(int bucket, char *clockName)
{
#ifndef EMU_RELEASE
    t3dsStartTicks[bucket] = -1; 
    t3dsClockName[bucket] = clockName;
    t3dsTotalCount[bucket]++;
#endif
}


//---------------------------------------------------------------
// Shows the total time accumulated for the given bucket.
//---------------------------------------------------------------
void t3dsShowTotalTiming(int bucket)
{
#ifndef EMU_RELEASE
    if (t3dsTotalTicks[bucket] > 0)
        printf ("%-20s: %2d %4d ms %d\n", t3dsClockName[bucket], bucket,
        (int)(t3dsTotalTicks[bucket] / TICKS_PER_SEC), 
        t3dsTotalCount[bucket]);
    else if (t3dsStartTicks[bucket] == -1 && t3dsTotalCount[bucket] > 0)
        printf ("%-20s: %2d %d\n", t3dsClockName[bucket], bucket,
        t3dsTotalCount[bucket]);
#endif
}
