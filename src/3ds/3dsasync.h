#include <3ds.h>

#ifndef _3DSASYNC_H_
#define _3DSASYNC_H_
/*
typedef struct
{
    int     param1;
    int     param2;
    int     param3;
    int     param4;
    int     param5;
    int     param6;
} SAsyncParams;

typedef void (* FAsyncTask)(SAsyncParams *);

typedef struct
{
    FAsyncTask          task;
    SAsyncParams        params;
} SAsyncTask;

extern SAsyncTask asyncTasks[];


//---------------------------------------------------------
// Execute tasks in parallel with the main app core. 
//---------------------------------------------------------
void async3dsExecuteTasks();


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task);


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1);


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2);


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2, int param3);


//---------------------------------------------------------
// Add tasks to be executed by sys core.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsScheduleTask(FAsyncTask task, int param1, int param2, int param3, int param4);


//---------------------------------------------------------
// Wait for all tasks to be executed completely.
// This should be called only by the app core.
//---------------------------------------------------------
void async3dsWaitTasks();
*/
#endif