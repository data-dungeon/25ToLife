//-----------------------------------------------------------------------------
//                           PS2 Shell System Core
//
// shellMemory header file
//
// $Workfile: shellMemory.h $
// $Author: Tyler Colbert $
// $Date: 8/15/03 11:17a $
// $Revision: 1 $
//
// Copyright (c) 2002-3 Sony Computer Entertainment Europe.
// All Rights Reserved.
//
//-----------------------------------------------------------------------------

#ifndef _SHELL_MEMORY_H
#define _SHELL_MEMORY_H

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

typedef enum {
	ESHELLMEM_NORMAL=0,
	ESHELLMEM_UNCACHED=0x20000000,
	ESHELLMEM_UNCACHED_ACCELERATED=0x30000000
} EShellMemoryType;

typedef int ShellMemoryHeap;
typedef int ShellMemoryPool;

static const ShellMemoryHeap SHELLMEMORY_HEAP0=(ShellMemoryHeap) 1;
static const ShellMemoryHeap SHELLMEMORY_HEAP1=(ShellMemoryHeap) 2;

//#define SHELLMEMORY_HEAP0 (1)
//#define SHELLMEMORY_HEAP1 (2)

void shellMemoryInit(unsigned int reserve);

ShellMemoryHeap shellMemoryBindHeap(ShellMemoryHeap heap);
ShellMemoryHeap shellMemoryCreateHeap(unsigned int size, EShellMemoryType type);
void shellMemoryResetHeap(void);

unsigned int shellMemoryAvail(void);
void *shellMemoryAllocate(unsigned int alignment,unsigned int size);

ShellMemoryPool shellMemoryCreatePool(unsigned int alignment, unsigned int size, unsigned int number);
int shellMemoryExpandPool(ShellMemoryPool pool, unsigned int number);
void *shellMemoryPoolAllocate(ShellMemoryPool pool);
void shellMemoryPoolFree(ShellMemoryPool pool,void *pointer);

#if defined(__LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
