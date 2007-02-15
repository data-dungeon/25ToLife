/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Debugging defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// This causes an assert when we run out of memory.
#define MEM_ASSERT_OUT_OF_MEM

// The per platform IO Buffer information
#if defined(WIN32) && !defined(_XBOX)
	#define IO_ALIGN	16
	#define IO_SLOP	0
#elif defined(_XBOX)
	#define IO_ALIGN	16
	#define IO_SLOP	XBOX_DVD_SECTOR_SIZE
#elif defined(PS2)
	#define IO_ALIGN	64
	#define IO_SLOP	64
#elif defined(GCN)
	#define IO_ALIGN	32
	#define IO_SLOP	0
#endif

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Configuration defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @{

/// @}

/// this is a wrapper for assert to make sure that we dont ever
/// get caught  in an assert loop. Seems like assert should do this for
/// us.  but it doesnt yet.
#define MEM_CORRUPT_HANDLER() memCorruptHandler
#define MEM_HEAP_ASSERT_BASE(assrt) do { if(!MEM_CORRUPT_HANDLER()) {memCorruptHandler = true; assrt; memCorruptHandler = false; }} while(0)

#if 0
/// @{
/// definitions for assert that dont require asserts to be
/// active.  this can be useful at times.
#define MEM_ASSERTS(cond,str)		do { if(!(cond)){         memCorruptHandler = false; LOCKUP(); } } while(0)
#define MEM_ASSERTS_PTR(ptr,str) do { if(!CHECK_PTR(ptr)){ memCorruptHandler = false; LOCKUP(); } } while(0)
/// @}
#else
/// @{
/// the normal definitions for assert.
#define MEM_ASSERTS(cond,str)		ASSERTS    (cond,str)
#define MEM_ASSERTS_PTR(ptr,str) ASSERTS_PTR(ptr,str)
/// @}
#endif

/// this macro is used to add an assert that can be used to debug
/// when certain addresses are referenced within the memory manager.
#define MEM_HEAP_DEBUG_STOP(ptr,val)  do { if(((u32)ptr) == (val)) LOCKUP(); } while(0)

/// a couple of addresses to debug so you don't have to search the code.
#define MEM_HEAP_DEBUG_NORMAL_ADDR 0x00000000

/// @{
/// here are the strings used in the asserts in different conditions.
#define MEM_INTERNAL_ERROR "Heap system Internal Error"
/// @}

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Nonconfigurable defines
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// macro to output a debug string.
#define DEBUGOUTPUT(x) dbgPrint x

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Local types
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// local functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// local variables
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// Debugging helpers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/// @}

/// @{
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// The meat and potatoes
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void memInit(void)
{
}

void memSetFitScheme(s32 s32Method)
{
}

void memSetFitMethod(s32 s32Method)
{
}

void memSetAllocMethod(s32 s32Method)
{
}

int memGetRegionLength(t_Handle Handle)
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
	return 0;
}

void memGetRegionStats(t_Handle Handle, int *pnFreeBytes, int *pnFreeHandles, int *pnLargestFree, int *pnAllocBytes, int *pnAllocHandles)
{
	// return that count
	*pnFreeHandles = 0;

	// return the total free space
	*pnFreeBytes = 0;

	// largest free
	*pnLargestFree = 0;

	// we're not tracking alloc'd memory
	*pnAllocHandles = 0;

	// return the difference between region size and free bytes.  that's
	// how much we have allocated.
	*pnAllocBytes = 0;
}

t_Handle	memAddRegion( unsigned char *pStart, u32 u32Length, u32 u32Flags, char *pdesc, u32 u32Handles, unsigned short usfitmethod, CMemPool* pools )
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
	return INVALID_HANDLE;
}

/* returns previously-active region, if any */

t_Handle	memSetActiveHandle(t_Handle Handle)
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
	// if we set it to something, return its handle, else -1.
	return INVALID_HANDLE;
}

/* move specified region to front or back of region list. when memFree is called, it looks through the regions to
find which one contains the memory block. normally, the main memory region is at the front of the list, since
almost all the the malloc'd buffers are in it. but it is possible to have a sub-region within the main region, and
when we free any buffer that is in that region is also in the main region. we want to find the sub-region, so we need
to be able to move the sub-region to the front of the region list, and then restore it to the back. this is to support
the different versions of tak (regular or costumed) in tak2, which have to share the same permanent memory block */

void memMoveRegionInList(t_Handle Handle, bool bFront)
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
}

/* check if region is empty */

bool memRegionIsEmpty(t_Handle Handle)
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
	return false;
}

u32 memHandleFree(t_Handle Handle)
{
	// not valid in malloc/free memory allocation scheme
	ASSERT(false);
	return 0;
}

void *memAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	/* if using windows memory manager, just malloc */

	void *pBlock;
	do
	{
#if MMX_OK						// alignment only necessary if using simd instructions
		pBlock = _aligned_malloc(u32Size, u32Align);
#else //MMX_OK
		pBlock = malloc(u32Size);
#endif //MMX_OK
	}
	while(pBlock == NULL && canFreeRamToDoIt && memCallOutOfMemCallback(u32Size, u32Align));


#ifdef MEM_ASSERT_OUT_OF_MEM
	ASSERT(pBlock);
#endif

	return(pBlock);
}

void *memAllocForIO(u32 u32Size)
{
	return memAlloc(u32Size + IO_SLOP, IO_ALIGN, false);
}

void *memAllocForIO(u32 u32Size, u32 u32Align)
{
	if (u32Align < IO_ALIGN)
		return memAllocForIO(u32Size, false);
	else
		return memAlloc(u32Size + IO_SLOP, u32Size, false);
}

void *memShrink(void *pAddr, u32 u32NewSize)
{
	/* ignore request-- we don't know buffer's requested alignment, so hard to use _aligned_realloc */

	return(pAddr);
}

void *memResize(void *pAddr, u32 u32NewSize, u32 u32Align)
{
	// simple implementation
	void *pNewMem = memAlloc(u32NewSize, u32Align);
	if (pNewMem == NULL)
	{
		/* well, THAT didn't go well */

		memFree(pAddr);
		return(NULL);
	}
	memcpy(pNewMem, pAddr, u32NewSize);
	return(pNewMem);
}

bool memFree(void *pAddr)
{
	/* if using windows memory manager, just free */

#if MMX_OK						// alignment only necessary if using simd instructions
	_aligned_free(pAddr);
#else //MMX_OK
	free(pAddr);
#endif //MMX_OK
	return(true);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// is the allocation possible?
bool memCouldAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	// if so, this function would find the first fit that would hold it.
	void* block = memAlloc(u32Size,u32Align, canFreeRamToDoIt);
	if(!block)
		return false;

	memFree(block);
	return true;
}

/* turn high-water tracking on or off */

void memTrackHighWater(t_Handle Handle, bool bState)
{
}

/* status of high water check */

bool memHighWaterOn(t_Handle Handle)
{
	return false;
}

void memResetHighWater(t_Handle Handle)
{
}

void memGetHighWater(t_Handle Handle, int *pnMinBytes, int *pnMaxBytes)
{
	// not valid in malloc/free memory allocation scheme
	*pnMinBytes = 0;
	*pnMinBytes = 0;
}

/* mark currently allocate blocks as "permanent." this routine is called after game engine initialization is finished,
but before the first fluff or game loop has started. it marks the memory allocations that are in place for the entire
game. on game platforms, these blocks will never be free'd, since the game never "ends" (that is, GameEngine__Shutdown
is never called). the intent of this routine is to provide a way to look for memory leaks. in between switching game
loops, the memory manager state should make it back to this initial state (all blocks marked as permanent) or else we
have a memory leak that will eventually gum up the works (though, of course, it may take a long time) */

void memMarkPermanentMemory(t_Handle RegionHandle)
{
}

/* mark an individual buffer as permanent. only used on gcn as far as i can tell, and seems dangerous to me (ned) */

void memMarkBufferPermanent(void *pAddr)
{
}

/* return true if all current allocations are marked as permanent-- helps looking for memory leaks-- after
exiting a level and resetting the game engine, all mallocs should be permanent */

bool memCheckPermanentMemory(t_Handle RegionHandle)
{
	return(true);
}


#ifdef MEM_DEBUG_BLOCKS
void memShowRegionFreeSize(t_Handle Handle)
{
}

void memShowRegionAllocSize(t_Handle Handle)
{
}

void memShowRegionBriefInfo(t_Handle Handle)
{
}

void memCheckHeapUsage( t_Handle Handle, u32 *LayersUsage, u32 *LayersTotal, u32 *CHeapUsage, u32 *CHeapTotal, u32 *u32TotalHandles)
{
}

void memSetUseBit(const u16 u16Bit)
{
}

void memClearUseBit(const u16 u16Bit)
{
}

int memDumpBlockInfo( FileStream* logFile )
{
	return 0;
}

int memQueryBlockBit(const u16 u16Bit, const char *pBlockName, int *pnHandleCount, const bool bMatchAllBits, const bool bDetail, FileStream *pLogFile)
{
	/* loop through all memory blocks */

	*pnHandleCount = 0;
	return(0);
}

void memClearBlockBit(const u16 u16Bit, const char *pBlockName)
{
}

void memSetAllocName(char *pcName)
{
}

void memSetAllocFileName(char *pcType, const char *pcFileName)
{
}

void memSetBlockName(void *pAddr, char *pcName)
{
}

#endif

void memShowAllocationInfo(void* alloc, char* msg)
{
}

#ifdef MEM_DEBUG_BLOCKS
void memShowRegionInfo( t_Handle Handle)
{
}
#endif

/// @}
