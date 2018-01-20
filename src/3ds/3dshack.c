
// Taken from PicoDrive's libretro implementation for 3DS.

#include "3dshack.h"

typedef int (*ctr_callback_type)(void);

int srvGetServiceHandle(unsigned int* out, const char* name);
int svcCloseHandle(unsigned int handle);
int svcBackdoor(ctr_callback_type);

static int hack3dsSvcInitialized = 0;

static int hack3dsTestBuffer[5];

//-----------------------------------------------------------------------------
// Internal function that enables all services. This must be run via
// svcBackdoor.
//-----------------------------------------------------------------------------
static void ctrEnableAllServices(void)
{
   __asm__ volatile("cpsid aif");

   unsigned int*  svc_access_control = *(*(unsigned int***)0xFFFF9000 + 0x22) - 0x6;

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
//-----------------------------------------------------------------------------
void hack3dsMirrorMemory(int virtualAddr, void *buffer, int size, int permission)
{
    #define MEMOP_MAP     4 ///< Mirror mapping

    unsigned int bufferAligned;
    unsigned int currentHandle;
    
    size = (size + 0xFFF) & ~0xFFF;
    bufferAligned = (((unsigned int)buffer) + 0xFFF) & ~0xFFF;
    svcDuplicateHandle(&currentHandle, 0xFFFF8001);
    svcControlProcessMemory(currentHandle, virtualAddr, bufferAligned, size, MEMOP_MAP, permission);
    svcCloseHandle(currentHandle);
}


//-----------------------------------------------------------------------------
// Sets the permissions for memory.
//-----------------------------------------------------------------------------
void hack3dsSetMemoryPermission(void *buffer, int size, int permission)
{
    #define MEMOP_PROT    6

    unsigned int currentHandle;
    svcDuplicateHandle(&currentHandle, 0xFFFF8001);
    svcControlProcessMemory(currentHandle, buffer, 0, size, MEMOP_PROT, permission);
    svcCloseHandle(currentHandle);
}


//-----------------------------------------------------------------------------
// Allocates memory from the heap and marks it as executable
//-----------------------------------------------------------------------------
void *hack3dsAllocateMemory(int virtualAddr, int size)
{
    void *buffer = malloc(size + 0x1000);
    hack3dsMirrorMemory(virtualAddr, buffer, size, 0b111); // read/write/exec
    return buffer;   
}


//-----------------------------------------------------------------------------
// Reallocates memory from the heap and marks it as non-executable.
//-----------------------------------------------------------------------------
void *hack3dsReallocMemory(int virtualAddr, void *buffer, int oldSize, int newSize)
{
    hack3dsMapMemory(virtualAddr, buffer, oldSize, 0b011); // read/write
    void *result = realloc(buffer, newSize);
    hack3dsMirrorMemory(virtualAddr, buffer, newSize, 0b111); // read/write/exec
    return result;
}



//-----------------------------------------------------------------------------
// Frees memory from the heap and marks it as non-executable.
//-----------------------------------------------------------------------------
void hack3dsFreeMemory(int virtualAddr, void *buffer, int size)
{
    hack3dsMirrorMemory(virtualAddr, buffer, size, 0b011); // read/write
    free(buffer);
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
    int *test_out = hack3dsTestBuffer;
    int (*testfunc)(void) = &hack3dsTestBuffer[0];

    *test_out++ = 0xe3a000dd; // mov r0, 0xdd
    *test_out++ = 0xe12fff1e; // bx lr
    hack3dsInvalidateAndFlushCaches();

    // we'll usually crash on broken platforms or bad ports,
    // but do a value check too just in case
    int ret = testfunc();

    if (ret == 0xdd)
    {
#ifndef EMU_RELEASE
        printf ("Successful!\n");
#endif
        return 1;
    }
    return 0;
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
   extern unsigned int __service_ptr;
   if(__service_ptr)          
      return 0;         // Means this was launched from Homebrew Launcher

   svcBackdoor((ctr_callback_type)ctrEnableAllServices);
   hack3dsSvcInitialized = 1;
   return 1;
}

