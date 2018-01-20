#ifndef _3DSHACK_H_
#define _3DSHACK_H_


//-----------------------------------------------------------------------------
// Allocates memory from the heap and marks it as executable
//-----------------------------------------------------------------------------
void *hack3dsAllocateMemory(int virtualAddr, int size);

//-----------------------------------------------------------------------------
// Reallocates memory from the heap and marks it as non-executable.
//-----------------------------------------------------------------------------
void *hack3dsReallocMemory(int virtualAddr, void *buffer, int oldSize, int newSize);

//-----------------------------------------------------------------------------
// Frees memory from the heap and marks it as non-executable.
//-----------------------------------------------------------------------------
void hack3dsFreeMemory(int virtualAddr, void *buffer, int size);

//-----------------------------------------------------------------------------
// Invalidates the instruction cache.
//-----------------------------------------------------------------------------
void hack3dsInvalidateInstructionCache(void);

//-----------------------------------------------------------------------------
// Flushes the data cache.
//-----------------------------------------------------------------------------
void hack3dsFlushDataCache(void);

//-----------------------------------------------------------------------------
// Invalidates the instruction cache and flushes the data cache.
//-----------------------------------------------------------------------------
void hack3dsInvalidateAndFlushCaches(void);

//-----------------------------------------------------------------------------
// Tests if the svc hacks have given us the necessary accesses to perform
// dynarec, by doing dynarec!
//-----------------------------------------------------------------------------
int hack3dsTestDynamicRecompilation(void);

//-----------------------------------------------------------------------------
// Initializes the hack to gain kernel access to all services.
// The one that we really are interested is actually the 
// svcControlProcessMemory, because once we have kernel access, we can
// grant read/write/execute access to memory blocks, and which means we
// can do dynamic recompilation.
//-----------------------------------------------------------------------------
int hack3dsInitializeSvcHack(void);

#endif // _3DSHACK_H_
