
#include "stdio.h"
#include "3dsmain.h"
#include "3dsasync.h"
#include "3dsopt.h"
#define MAX_TASKS 2048

/*
SAsyncTask asyncTasks[MAX_TASKS];
int asyncTaskAddPtr = 0;
int asyncTaskExecPtr = 0;


//---------------------------------------------------------
// Execute commands in parallel with the main app core. 
// This should only be called by the sys core.
//---------------------------------------------------------
void async3dsExecuteTasks()
{
    while (asyncTaskExecPtr != asyncTaskAddPtr)
    {
        SAsyncTask *task = &asyncTasks[asyncTaskExecPtr];

        t3dsStartTiming(45, "AsyncTasks");
        ((FAsyncTask)(task->task)) (&task->params);
        t3dsEndTiming(45);

        asyncTaskExecPtr = (asyncTaskExecPtr + 1) & (MAX_TASKS - 1);
    }
}


//---------------------------------------------------------
// Add commands to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task)
{
    asyncTasks[asyncTaskAddPtr].task = task;

    asyncTaskAddPtr = (asyncTaskAddPtr + 1) & (MAX_TASKS - 1);
    if (!emulator.isReal3DS)
        svcSleepThread(1);
}


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1)
{
    asyncTasks[asyncTaskAddPtr].task = task;
    asyncTasks[asyncTaskAddPtr].params.param1 = param1;

    asyncTaskAddPtr = (asyncTaskAddPtr + 1) & (MAX_TASKS - 1);
    if (!emulator.isReal3DS)
        svcSleepThread(1);

}



//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2)
{
    asyncTasks[asyncTaskAddPtr].task = task;
    asyncTasks[asyncTaskAddPtr].params.param1 = param1;
    asyncTasks[asyncTaskAddPtr].params.param2 = param2;

    asyncTaskAddPtr = (asyncTaskAddPtr + 1) & (MAX_TASKS - 1);
    if (!emulator.isReal3DS)
        svcSleepThread(1);

}


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2, int param3)
{
    asyncTasks[asyncTaskAddPtr].task = task;
    asyncTasks[asyncTaskAddPtr].params.param1 = param1;
    asyncTasks[asyncTaskAddPtr].params.param2 = param2;
    asyncTasks[asyncTaskAddPtr].params.param3 = param3;

    asyncTaskAddPtr = (asyncTaskAddPtr + 1) & (MAX_TASKS - 1);
    if (!emulator.isReal3DS)
        svcSleepThread(1);
}


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2, int param3, int param4)
{
    asyncTasks[asyncTaskAddPtr].task = task;
    asyncTasks[asyncTaskAddPtr].params.param1 = param1;
    asyncTasks[asyncTaskAddPtr].params.param2 = param2;
    asyncTasks[asyncTaskAddPtr].params.param3 = param3;
    asyncTasks[asyncTaskAddPtr].params.param4 = param4;

    asyncTaskAddPtr = (asyncTaskAddPtr + 1) & (MAX_TASKS - 1);
    if (!emulator.isReal3DS)
        svcSleepThread(1);

}


//---------------------------------------------------------
// Wait for all tasks to be executed completely.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsWaitTasks()
{
    while (asyncTaskExecPtr != asyncTaskAddPtr)
    {
        svcSleepThread(1);
    }
}
*/