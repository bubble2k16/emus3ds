
#ifndef _3DSOPT_H_
#define _3DSOPT_H_


#include <3ds.h>

#include "3dsemu.h"
 
extern char *t3dsClockName[100];
extern int t3dsTotalCount[100];
extern uint64 t3dsStartTicks[100];
extern uint64 t3dsTotalTicks[100];


//---------------------------------------------------------------
// Resets the timer.
//---------------------------------------------------------------
void t3dsResetTimings();


//---------------------------------------------------------------
// Increments the counter.
//---------------------------------------------------------------
void t3dsCount(int bucket, char *name);


//---------------------------------------------------------------
// Shows the total time accumulated for the given bucket.
//---------------------------------------------------------------
void t3dsShowTotalTiming(int bucket);


//---------------------------------------------------------------
// Starts the timer.
//---------------------------------------------------------------
inline void t3dsStartTiming(int bucket, char *clockName)
{
#ifndef EMU_RELEASE
    t3dsStartTicks[bucket] = svcGetSystemTick(); 
    t3dsClockName[bucket] = clockName;
#endif
}


//---------------------------------------------------------------
// Ends the timer.
//---------------------------------------------------------------
inline void t3dsEndTiming(int bucket)
{
#ifndef EMU_RELEASE
    u64 endTicks = svcGetSystemTick(); 
    t3dsTotalTicks[bucket] += (endTicks - (u64)t3dsStartTicks[bucket]);
    t3dsTotalCount[bucket]++;
#endif
}

#endif