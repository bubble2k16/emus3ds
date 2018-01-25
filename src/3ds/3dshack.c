
// Taken from PicoDrive's libretro implementation for 3DS.

#include "3ds.h"
#include "3dsemu.h"
#include "3dsdbg.h"
#include "3dshack.h"

typedef int (*ctr_callback_type)(void);


static int hack3dsSvcInitialized = 0;
static int __attribute__ ((aligned (4096))) hack3dsTestBuffer[4096];

static unsigned int s1, s2, s3, s0;

//-----------------------------------------------------------------------------
// Internal function that enables all services. This must be run via
// svcBackdoor.
//-----------------------------------------------------------------------------
static void ctrEnableAllServices(void)
{
   __asm__ volatile("cpsid aif");

   unsigned int *svc_access_control = *(*(unsigned int***)0xFFFF9000 + 0x22) - 0x6;

   s0 = svc_access_control[0];
   s1 = svc_access_control[1];
   s2 = svc_access_control[2];
   s3 = svc_access_control[3];
   
   svc_access_control[0]=0xFFFFFFFE;
   svc_access_control[1]=0xFFFFFFFF;
   svc_access_control[2]=0xFFFFFFFF;
   svc_access_control[3]=0x3FFFFFFF;

}


//-----------------------------------------------------------------------------
// Internal function that invalidates the instruction cache. This must be run 
// via svcBackdoor.
//-----------------------------------------------------------------------------
static void ctrInvalidateInstructionCache(void)
{
   __asm__ volatile(
      "cpsid aif\n\t"
      "mov r0, #0\n\t"
      "mcr p15, 0, r0, c7, c5, 0\n\t");
}


//-----------------------------------------------------------------------------
// Internal function that flushes the data cache. This must be run 
// via svcBackdoor.
//-----------------------------------------------------------------------------
static void ctrFlushDataCache(void)
{
   __asm__ volatile(
      "cpsid aif\n\t"
      "mov r0, #0\n\t"
      "mcr p15, 0, r0, c7, c10, 0\n\t");

}


//-----------------------------------------------------------------------------
// Mirrors memory and marks its access.
// You must ensutre that buffer and size are 0x1000-aligned.
//-----------------------------------------------------------------------------
int hack3dsMapMemory(int virtualAddr, void *buffer, int size, int permission)
{
    #define MEMOP_MAP     4 ///< Mirror mapping

    unsigned int currentHandle;
    svcDuplicateHandle(&currentHandle, 0xFFFF8001);
    int res = svcControlProcessMemory(currentHandle, virtualAddr, buffer, size, MEMOP_MAP, permission);
    svcCloseHandle(currentHandle);

    return res;
}


//-----------------------------------------------------------------------------
// Mirrors memory and marks its access.
// You must ensutre that buffer and size are 0x1000-aligned.
//-----------------------------------------------------------------------------
int hack3dsUnmapMemory(int virtualAddr, void *buffer, int size)
{
    #define MEMOP_MAP     4 ///< Mirror mapping

    unsigned int currentHandle;
    svcDuplicateHandle(&currentHandle, 0xFFFF8001);
    int res = svcControlProcessMemory(currentHandle, virtualAddr, buffer, size, MEMOP_UNMAP, 0x3);
    svcCloseHandle(currentHandle);

    return res;
}


//-----------------------------------------------------------------------------
// Sets the permissions for memory.
// You must ensutre that buffer and size are 0x1000-aligned.
//-----------------------------------------------------------------------------
int hack3dsSetMemoryPermission(void *buffer, int size, int permission)
{
    unsigned int currentHandle;
    svcDuplicateHandle(&currentHandle, 0xFFFF8001);
    int res = svcControlProcessMemory(currentHandle, buffer, 0, size, MEMOP_PROT, permission);
    svcCloseHandle(currentHandle);

    return res;
}


//-----------------------------------------------------------------------------
// Invalidates the instruction cache.
//-----------------------------------------------------------------------------
void hack3dsInvalidateInstructionCache(void)
{
//   __asm__ volatile("svc 0x2E\n\t");
   svcBackdoor((ctr_callback_type)ctrInvalidateInstructionCache);

}

//-----------------------------------------------------------------------------
// Flushes the data cache.
//-----------------------------------------------------------------------------
void hack3dsFlushDataCache(void)
{
//   __asm__ volatile("svc 0x4B\n\t");
   svcBackdoor((ctr_callback_type)ctrFlushDataCache);
}


//-----------------------------------------------------------------------------
// Invalidates the instruction cache and flushes the data cache.
//-----------------------------------------------------------------------------
void hack3dsInvalidateAndFlushCaches(void)
{
   hack3dsFlushDataCache();
   hack3dsInvalidateInstructionCache();
}


//-----------------------------------------------------------------------------
// Tests if the svc hacks have given us the necessary accesses to perform
// dynarec, by doing dynarec!
//-----------------------------------------------------------------------------
int hack3dsTestDynamicRecompilation(void)
{
    if (hack3dsSetMemoryPermission(&hack3dsTestBuffer[0], 4096, 0x7))
    {
        // If svcControlProcessMemory didn't succeed, likely dynarec
        // will just crash. 
#ifndef EMU_RELEASE
        printf ("Dynarec test: svcControlProcessMemory failed!\n");
#endif

        return 0;
    }

    int *test_out = hack3dsTestBuffer;
    int (*testfunc)(void) = hack3dsTestBuffer;

    *test_out++ = 0xe3a000dd; // mov r0, 0xdd
    *test_out++ = 0xe12fff1e; // bx lr
    hack3dsInvalidateAndFlushCaches();

    // we'll usually crash on broken platforms or bad ports,
    // but do a value check too just in case
    int ret = testfunc();
    int result = 0;

    if (ret == 0xdd)
    {
#ifndef EMU_RELEASE
        printf ("Dynarec test: SUCCESSFUL!!\n");
#endif
        result = 1;
    }

    return result;
}


//-----------------------------------------------------------------------------
// Initializes the hack to gain kernel access to all services.
// The one that we really are interested is actually the 
// svcControlProcessMemory, because once we have kernel access, we can
// grant read/write/execute access to memory blocks, and which means we
// can do dynamic recompilation.
//-----------------------------------------------------------------------------
int hack3dsInitializeSvcHack(void)
{
    if(envIsHomebrew())
    {
#ifndef EMU_RELEASE
        printf("svcHack failed: Inside homebrew\n");
#endif
        return 0;         // Means this was launched from Homebrew Launcher
    }

    svcBackdoor((ctr_callback_type)ctrEnableAllServices);
    svcBackdoor((ctr_callback_type)ctrEnableAllServices);
#ifndef EMU_RELEASE
    printf("svc_access_control: %x %x %x %x\n", s0, s1, s2, s3);
#endif
    if (s0 != 0xFFFFFFFE || s1 != 0xFFFFFFFF || s2 != 0xFFFFFFFF || s3 != 0x3FFFFFFF)
    {
#ifndef EMU_RELEASE
        printf("svcHack failed: svcBackdoor unsuccessful.\n");
#endif
        return 0;
    }

    hack3dsSvcInitialized = 1;
    return 1;
}

