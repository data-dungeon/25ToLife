/* memory.cpp */

#include "EngineHelper/filestream.h"


/* -----------------4/27/98 5:30PM-------------------
 * Local Defines
 * --------------------------------------------------*/

//#define ASSERT_ON_PERM_FREE

// UnModifyable

#define MEM_SZE_FIX(sze)	(((sze) + (MEM_ADDR_ALIGN - 1)) & ~(MEM_ADDR_ALIGN - 1))
#define MEM_SZE_BND(sze)	(((sze)) & ~(MEM_ADDR_ALIGN - 1))
#define MEM_ADR_FIX(adr)	(char *) (( (u32) (adr) + (MEM_ADDR_ALIGN - 1) ) & ~(MEM_ADDR_ALIGN - 1))

/******************* macros ******************************/

#ifdef MALLOC_STATS
#define MALLOC_STATS_INC(nSize)									\
{																			\
	nTotalMemory += nSize;											\
	if (nSize & 15)													\
		nWastedMemory16 += 16 - (nSize & 15);					\
	if (nSize & 31)													\
		nWastedMemory32 += 32 - (nSize & 31);					\
	if (nSize & 63)													\
		nWastedMemory64 += 64 - (nSize & 63);					\
	if (nSize < MAX_MALLOC_STATS_SIZE)							\
		nMallocStats[nSize]++;										\
	else																	\
		nMallocStats[MAX_MALLOC_STATS_SIZE - 1]++;			\
}
#define MALLOC_STATS_DEC(nSize)									\
{																			\
	nTotalMemory -= nSize;											\
	if (nSize & 15)													\
		nWastedMemory16 -= 16 - (nSize & 15);					\
	if (nSize & 31)													\
		nWastedMemory32 -= 32 - (nSize & 31);					\
	if (nSize & 63)													\
		nWastedMemory64 -= 64 - (nSize & 63);					\
	if (nSize < MAX_MALLOC_STATS_SIZE)							\
		nMallocStats[nSize]--;										\
	else																	\
		nMallocStats[MAX_MALLOC_STATS_SIZE - 1]--;			\
}
#define NEW_STATS_INC(nSize)										\
{																			\
	nTotalMemory += nSize;											\
	if (nSize & 15)													\
		nWastedMemory16 += 16 - (nSize & 15);					\
	if (nSize & 31)													\
		nWastedMemory32 += 32 - (nSize & 31);					\
	if (nSize & 63)													\
		nWastedMemory64 += 64 - (nSize & 63);					\
	if (nSize < MAX_MALLOC_STATS_SIZE)							\
		nNewStats[nSize]++;											\
	else																	\
		nNewStats[MAX_MALLOC_STATS_SIZE - 1]++;				\
}
#endif //MALLOC_STATS

/******************* structures **************************/

/* important to keep this as small as possible, since the memory block records come out of the memory region's */
/* available memory */

struct ts_memBlock
{
	ts_memBlock			*pNext;
	unsigned char		*pAddr;			// this block's memory in region
	u16					u16Align;		// requested alignment of block at malloc time. we can combine this with pAddr to find the memory addr returned to the app
	u16					u16FlagsObsolete;
	u32					u32Length;
#ifdef MEM_DEBUG_BLOCKS
	char					AllocName[ALLOC_NAME_LENGTH];
	u16					u16UseBits;
#endif //MEM_DEBUG_BLOCKS
};

struct ts_memRegion
{
	ts_memRegion		*pNext;

	/* there are a total of u32Handles (parameter to memAddRegion) memory block records. they are in */
	/* four possible lists: handle, free, alloc, pending free. the free list contains all the available */
	/* memory blocks-- it starts off as one big block of free memory, the whole region. a request for a */
	/* new block typically uses part of an existing free block, so it grabs an unused block from the */
	/* handle list to set up and then adds it to the alloc list. each memory block can only be in one */
	/* list, of course */

	ts_memBlock			*pHandleList;	// handle blocks are taken from start of region's memory
	ts_memBlock			*pFreeList;
	ts_memBlock			*pAllocList;
	ts_memBlock			*pPendingFreeList;
	unsigned char		*pMemStart;		// start of region memory after handle blocks are taken out of passed memory
	u32					u32MemLength;	// size of region memory after handle blocks are taken out of passed memory
	u32					u32Flags;
	t_Handle				Handle;
	char					*pDescriptor;
	unsigned short		usFitMethod;
	int					nMinMemory;		// for tracking high water mark if REGION_TEST_HIGH_WATER_MARK is set
	int					u32MemHandles;	// initial handle count
};

struct ts_memFit
{
	ts_memBlock			*pBlock;
	union
	{
		u32				u32Differential;
		void				*pAddr;
	} Compare;
};

typedef ts_memBlock *(*t_FindFunc)( ts_memBlock *pList, u32 u32Size);
typedef ts_memBlock *(*t_AllocFunc)( ts_memRegion *pRegion, u32 u32Size, u32 u32Align);

/* -----------------4/27/98 5:30PM-------------------
 * Local Prototypes
 * --------------------------------------------------*/
void					_memAddBlock( ts_memBlock **ppList, ts_memBlock *pBlock);
void					_memAddFreeBlock( ts_memRegion *pRegion, ts_memBlock *pBlock);
ts_memBlock	*		_memAllocFromTop( ts_memRegion *pRegion, u32 u32Size, u32 u32Align);
ts_memBlock	*		_memAllocFromBottom( ts_memRegion *pRegion, u32 u32Size, u32 u32Align);
void *				_memAllocFromRegion( ts_memRegion *pRegion, u32 u32Size, u32 u32Align);

u32					_memAllocSize( u32 u32Size, u32 u32Align);
u32					_memBlockListAvailable( ts_memBlock *pList);
void					_memCheckBlock( ts_memBlock *pBlock);
ts_memBlock *		_memFindAddress( ts_memRegion *pList, void *pAddr);
ts_memBlock *		_memFindBlockRaw( ts_memBlock *pBlockList, void *pAddr);
ts_memBlock *		_memFindBlock( ts_memBlock *pBlockList, void *pAddr);
ts_memBlock *		_memFindAndRemoveMemBlock( ts_memBlock **ppBlockList, void *pAddr);
ts_memRegion *		_memFindMemRegion( ts_memRegion *pList, void *pAddr);
ts_memRegion *		_memFindRegion( t_Handle Handle);
ts_memBlock *		_memFitBest( ts_memBlock *pList, u32 u32Size);
ts_memBlock *		_memFitFirst( ts_memBlock *pList, u32 u32Size);
ts_memBlock *		_memFitHigh( ts_memBlock *pList, u32 u32Size);
ts_memBlock *		_memFitLast( ts_memBlock *pList, u32 u32Size);
ts_memBlock *		_memFitLow( ts_memBlock *pList, u32 u32Size);

ts_memBlock *		_memNewBlock(ts_memRegion *pRegion, void *pStart, u32 u32Length, u32 u32Align);
void					_memInitData( void);
void					_memInitHandleList( ts_memBlock *pHandleList, u32 u32Handles);
void *				_memPrepBlock( ts_memBlock *pBlock);
void *				_memReallocFromRegion( ts_memRegion *pRegion, ts_memBlock *pBlock, u32 u32NewLength, u32 u32Align);
void					_memRemoveBlock( ts_memBlock **ppList, ts_memBlock *pBlock);

static u32			_memListLen( void *pNext);
static u32			_memBlockListSize( ts_memBlock *pList);
#ifdef GCN
static void *		_memResizeLocal( void *pMem, u32 u32NewSize, u32 u32Align);
static bool			_memFreeLocal( void *pMem);
static bool			_memFreePendingLocal( void *pMem);
#endif

/* -----------------4/28/98 12:23PM------------------
 * Local Data
 * --------------------------------------------------*/

ts_memRegion 		*pActiveRegion;
ts_memRegion		*pFreeRegionList;
ts_memRegion		*pUsedRegionList;

ts_memRegion		StaticRegions[MEM_MAX_REGIONS];

t_FindFunc			pFitList[FIT_MAX] =
{
	_memFitFirst,
	_memFitBest,
	_memFitLast,
	_memFitLow,
	_memFitHigh
} ;
t_FindFunc			memFitFunc = NULL;

t_AllocFunc			pAllocList[ALC_MAX] =
{
	_memAllocFromTop,
	_memAllocFromBottom
} ;
t_AllocFunc			memAllocFunc = NULL;

s32					s32FitMatch[FIT_MAX] =
{
	ALC_TOP,
	ALC_TOP,
	ALC_TOP,
	ALC_TOP,
	ALC_BTM
} ;

#ifdef MEM_DEBUG_BLOCKS
static bool bReportAlloc = false;
static int nReportTotalHandles = 0;
static int nReportTotalBytes = 0;
#endif //MEM_DEBUG_BLOCKS

#ifdef MALLOC_STATS
static int nMallocStats[MAX_MALLOC_STATS_SIZE] = {0};
static int nNewStats[MAX_MALLOC_STATS_SIZE] = {0};
static int nTotalMemory = 0;
static int nWastedMemory16 = 0;
static int nWastedMemory32 = 0;
static int nWastedMemory64 = 0;
#endif //MALLOC_STATS

static bool sMemIgnoreFrees = FALSE;

#ifdef GCN
OSMutex	mutexMemAlloc;
#endif


/* -----------------4/28/98 11:36AM------------------
 * Exportable Functions
 * --------------------------------------------------*/

void memInit( void)
{

	_memInitData( );

	memSetFitMethod( FIT_BEST);
	memSetAllocMethod( ALC_TOP);

#ifdef GCN
	OSInitMutex( &mutexMemAlloc);
#endif
}

void MemSetFitScheme( s32 s32Method)
{
	ASSERT( s32Method < FIT_MAX);

	memSetFitMethod( s32Method);

	memSetAllocMethod( s32FitMatch[s32Method]);
}

void memSetFitMethod( s32 s32Method)
{

	ASSERT( s32Method < FIT_MAX);

	memFitFunc = pFitList[s32Method];
}

void memSetAllocMethod( s32 s32Method)
{
	ASSERT( s32Method < ALC_MAX);

	memAllocFunc = pAllocList[s32Method];
}

#ifdef MEM_DEBUG_BLOCKS
/* this is a one-shot name set, it gets recorded in the next memory block alloc'd, then gets cleared */

void memSetAllocName(char *pcName)
{
//djr: removed assert so that we can mark some new memory as permanent
//	ASSERT(pcName[0] != PERMANENT_MEMORY_MARK);	// we reserve a character for marking permanent allocations
	strncpy(acAllocName, pcName, ALLOC_NAME_LENGTH);
	acAllocName[ALLOC_NAME_LENGTH - 1] = 0;	//make sure it's null-terminated
}
void memSetAllocFileName(char *pcType, const char *pcFileName)
{
#if 1
	const char *fname;
	char name[ALLOC_NAME_LENGTH*2], *cptr;
	strncpy(name, pcType, ALLOC_NAME_LENGTH);
	fname = pcFileName;
	while((cptr=strchr(fname, '\\')) != NULL)
		fname = cptr + 1;
	while((cptr=strchr(fname, '/')) != NULL)
		fname = cptr + 1;
	strncat(name, fname, (ALLOC_NAME_LENGTH - strlen(pcType)));
	memSetAllocName(name);
#else
	memSetAllocName(pcType);
#endif
}
#endif //MEM_DEBUG_BLOCKS

void *memAlloc( u32 u32Size, u32 u32Align /*= MEM_ADDR_ALIGN*/, , bool canFreeRamToDoIt /*=true*/)
{
#ifdef GCN
	OSLockMutex( &mutexMemAlloc);
#endif
	
	/* default alignment is the minimum */

	if (u32Align < MEM_ADDR_ALIGN)
		u32Align = MEM_ADDR_ALIGN;

#ifdef MALLOC_STATS
	/* collect stats */
   MALLOC_STATS_INC(u32Size);
#endif //MALLOC_STATS

	void* block;

	do
	{

#ifdef MEM_USE_WIN32_MANAGER
	/* if using windows memory manager, just malloc */
#if MMX_OK						// alignment only necessary if using simd instructions
		block = _aligned_malloc(u32Size, u32Align);
#else
		block = malloc(u32Size);
#endif
#else //MEM_USE_WIN32_MANAGER

		ASSERT( pActiveRegion != NULL);
		block = _memAllocFromRegion( pActiveRegion, u32Size, u32Align);

#endif //MEM_USE_WIN32_MANAGER

	}
	while(block == NULL && canFreeRamToDoIt && memCallOutOfMemCallback(u32Size, u32Align));

#ifdef GCN
	OSUnlockMutex( &mutexMemAlloc);
#endif
	
	return( block);
}

void memFreeRegion( t_Handle Handle)
{
	ts_memRegion		*pRegion;
	ts_memRegion		**ppRegionPtr;

	if( pUsedRegionList == NULL)
		return;

	pRegion = _memFindRegion( Handle);

	if( pRegion == NULL)
		return;

//	ASSERT( pActiveRegion != pRegion);

	ppRegionPtr = &pUsedRegionList;

	while( *ppRegionPtr)
	{
		if( *ppRegionPtr == pRegion)
			break;

		ppRegionPtr = &( (*ppRegionPtr)->pNext);
	}

	if( *ppRegionPtr)
	{
		if (pActiveRegion == pRegion)
			pActiveRegion = NULL;

		*ppRegionPtr = pRegion->pNext;

//		memset( pRegion, 0, sizeof( ts_memRegion));

		pRegion->pNext = pFreeRegionList;

		pFreeRegionList = pRegion;

	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/// is the allocation possible?
bool memCouldAlloc(u32 u32Size, u32 u32Align, bool canFreeRamToDoIt)
{
	// if so, this function would find the first fit that would hold it.
	do
	{
		ts_memBlock* block = _memFitFirst(pActiveRegion,u32Size,u32Align);
	}
	while(block == NULL && canFreeRamToDoIt && memCallOutOfMemCallback(u32Size, u32Align));

	return block != NULL;
}

/* pPoolConfig is ignored in this memory manager-- pools are not supported */

t_Handle memAddRegion( unsigned char *pStart, u32 u32Length, u32 u32Flags, char *pdesc, u32 u32Handles, unsigned short usfitmethod, poolCfg *pPoolConfig)
{
	unsigned char		*pHandleEnd;

	ts_memRegion 		*pNewRegion;

	ASSERT( pStart != NULL);
	ASSERT( u32Handles > 0);
	ASSERT( usfitmethod < FIT_MAX);
	ASSERT( u32Length  >= (u32Handles * sizeof( ts_memBlock)));

	if( pFreeRegionList == NULL)
		CRT_ERROR( "Out of Memory Regions.\n");


	pNewRegion = pFreeRegionList;

	pFreeRegionList = pNewRegion->pNext;

	pNewRegion->pNext = pUsedRegionList;
	pUsedRegionList = pNewRegion;

	pNewRegion->u32Flags = u32Flags;
	pNewRegion->usFitMethod = usfitmethod;
	pNewRegion->pDescriptor = pdesc;
	pNewRegion->pAllocList = NULL;
	pNewRegion->pPendingFreeList = NULL;
	pNewRegion->u32MemHandles = u32Handles;

	// Allocate handles from base of regions memory pool.

	pNewRegion->pHandleList = (ts_memBlock *) pStart;

	_memInitHandleList( pNewRegion->pHandleList, u32Handles);

	pHandleEnd = pStart + (u32Handles * sizeof( ts_memBlock));
	pNewRegion->pMemStart = (unsigned char *)MEM_ADR_FIX( pHandleEnd);

	u32Length -= u32Handles * sizeof( ts_memBlock);		// remove size of memory block array
	u32Length -= pNewRegion->pMemStart - pHandleEnd;	// remove effects of aligning to default alignment
	u32Length = MEM_SZE_BND( u32Length);					// reduce so that length is multiple of default alignment
	pNewRegion->u32MemLength = u32Length;

	/* free list starts as one big block of memory, after handles list */

	MEM_PUSH_ALLOC_NAME("free");
	pNewRegion->pFreeList = _memNewBlock(pNewRegion, pNewRegion->pMemStart, u32Length, MEM_ADDR_ALIGN);
	MEM_POP_ALLOC_NAME();

#ifndef CONSUMER_BUILD
	/* high water tracking off by default */

	memTrackHighWater(pNewRegion->Handle, false);
#endif //CONSUMER_BUILD

	return pNewRegion->Handle;
}

/* i provide this as a way around a problem when using the windows memory manager. i am using the _aligned_ versions of
malloc, free, and realloc, and _msize(pMem) (which should return the size of the malloc'd block) doesn't seem to work
on the aligned buffers. our memory manager guarantees that shrinking buffers will leave them in the same place, which
is important if you've resolved pointers within the buffer being shrunk. but realloc does not guarantee this. so if using
the windows memory manager, just leave the buffer intact, at the cost of some extra memory being used (possibly substantial
memory, since, for example, the texture set chunk gets resized away) */

void *memShrink( void *pMem, u32 u32NewSize)
{
#ifdef MEM_USE_WIN32_MANAGER
	/* if using windows memory manager, just leave buffer alone to make sure realloc doesn't move it */

	return(pMem);
#endif //MEM_USE_WIN32_MANAGER

	/* call memResize to do the work */

	return(memResize(pMem, u32NewSize));
}

#ifdef GCN
void *memResize( void *pMem, u32 u32NewSize, u32 u32Align /*= MEM_ADDR_ALIGN*/)
{
	OSLockMutex( &mutexMemAlloc);

	void	*pReturn = _memResizeLocal( pMem, u32NewSize, u32Align);

	OSUnlockMutex( &mutexMemAlloc);

	return( pReturn);
}

static void *_memResizeLocal( void *pMem, u32 u32NewSize, u32 u32Align)
#else
void *memResize( void *pMem, u32 u32NewSize, u32 u32Align /*= MEM_ADDR_ALIGN*/)
#endif
{
	u32				u32NewAllocSize;

	ts_memRegion	*pRegion;

	ts_memBlock		*pBlock;
	ts_memBlock		*pTempBlock;

	ASSERT( pMem != NULL);
	ASSERT( u32NewSize != 0);

#ifdef MEM_USE_WIN32_MANAGER
	/* if using windows memory manager, just use realloc. realloc does not guarantee that buffers being */
	/* shrunk stay in the same place. if that's important to you, call memShrink instead of memResize */

#if MMX_OK						// alignment only necessary if using simd instructions
	return(_aligned_realloc(pMem, u32NewSize, u32Align));
#else
	return(realloc(pMem, u32NewSize));
#endif
#endif //MEM_USE_WIN32_MANAGER

	/* find region that contains passed buffer */

	pRegion = _memFindMemRegion( pUsedRegionList, pMem);
	if (!pRegion)
		return NULL;

	/* find memory block for passed buffer */

	pBlock = _memFindBlock( pRegion->pAllocList, pMem);
	if (!pBlock)
		return NULL;

	/* alignment must be at least original alignment. this is for correct _memAllocSize calculation. for example, */
	/* if a buffer was created with 64-byte alignment, and is asked to resize to the same number of bytes, */
	/* _memAllocSize will calculate a smaller number and shrink it. we can't really ask the calling program */
	/* to handle this-- memResize can be called by the game engine, out of the app's control */

	if (u32Align < pBlock->u16Align)
		u32Align = pBlock->u16Align;

	/* if buffer isn't aligned to passed alignment, HAVE to move it. this will ONLY happen if calling program */
	/* is INCREASING the alignment value */

	if (((u32) pMem & (u32Align - 1)) != 0)
	{
		ASSERT(0);	// temporary assert-- in general, i think things should remain at same alignment
		return _memReallocFromRegion( pRegion, pBlock, u32NewSize, u32Align);
	}

	/* calculate necessary new size */

	u32NewAllocSize = _memAllocSize( u32NewSize, u32Align);

	/* shrink or expand? do nothing if equal in size */

	if (u32NewAllocSize < pBlock->u32Length)
	{
		/* shrink. if no unused block available (to put leftover memory in), just leave existing block alone-- */
		/* it will have unneeded space at end, but at least we won't fail */

		if( !(pRegion->pHandleList))
			return(pMem);

		/* create a new block to track the newly-freed memory from the end of the passed buffer */

		pTempBlock = _memNewBlock(pRegion, pBlock->pAddr + u32NewAllocSize, pBlock->u32Length - u32NewAllocSize, MEM_ADDR_ALIGN);
		_memAddFreeBlock( pRegion, pTempBlock);

		/* adjust size of existing block */

		pBlock->u32Length = u32NewAllocSize;
	}
	else if( u32NewAllocSize > pBlock->u32Length)
	{
		/* expand. see if there's already free memory at the end of current block, so we can stretch without copying data */

		pTempBlock = _memFindBlockRaw( pRegion->pFreeList, pBlock->pAddr + pBlock->u32Length);
		if ((pTempBlock == NULL) || (pTempBlock->u32Length < (u32NewAllocSize - pBlock->u32Length)))
			return _memReallocFromRegion( pRegion, pBlock, u32NewSize, u32Align);	// nope, have to move it

		/* block that contains memory following passed buffer shrinks, and passed buffer expands */

		pTempBlock->pAddr += u32NewAllocSize - pBlock->u32Length;
		pTempBlock->u32Length -= u32NewAllocSize - pBlock->u32Length;
		pBlock->u32Length = u32NewAllocSize;
	}

	return _memPrepBlock( pBlock);
}


t_Handle memSetActiveHandle( t_Handle Handle)
{
	t_Handle	Prev = (t_Handle)-1;

	if( pActiveRegion != NULL)
	{
		Prev = pActiveRegion->Handle;
	}

	pActiveRegion = _memFindRegion( Handle);
	
	return( Prev);
}

void    memIgnoreFrees(bool ignoreThem)
{
	sMemIgnoreFrees = ignoreThem;
}

#ifdef GCN
bool memFree( void *pMem)
{
	if(sMemIgnoreFrees)
	{
		return TRUE;
	}

	OSLockMutex( &mutexMemAlloc);
	
	bool	bReturn = _memFreeLocal( pMem);
	
	OSUnlockMutex( &mutexMemAlloc);
	
	return( bReturn);
}

static bool _memFreeLocal( void *pMem)
#else
bool memFree( void *pMem)
#endif
{
	ts_memRegion		*pRegion;
	ts_memBlock			*pBlock;

#ifdef MEM_USE_WIN32_MANAGER
	/* if using windows memory manager, just use free */

#if MMX_OK						// alignment only necessary if using simd instructions
	_aligned_free(pMem);
#else
	free(pMem);
#endif
	return(TRUE);
#endif //MEM_USE_WIN32_MANAGER

	pRegion = _memFindMemRegion( pUsedRegionList, pMem);

	if( pRegion == NULL)
		CRT_ERROR( "Address OOR");

	pBlock = _memFindAndRemoveMemBlock( &pRegion->pAllocList, pMem);

	if( pBlock == NULL)
		return FALSE;

#ifdef ASSERT_ON_PERM_FREE
	ASSERTS(!(pBlock->u16UseBits & MEM_PERMANENT_DATA), "Looks like your freeing something that is considered permanent!");
#endif

#ifdef MEM_DBG_CAPS
	_memCheckBlock( pBlock);
#endif

#ifdef MEM_DBG_LIST_REQUESTS
	//list memory free's as they happen
	u32 u32Handles = _memListLen( pRegion->pHandleList);
	dbgPrint( "Free Name: %s, free'd size = %d, free handles %d, name = %s\n", pRegion->pDescriptor, pBlock->u32Length, u32Handles, pBlock->AllocName);
#endif

	_memAddFreeBlock( pRegion, pBlock);

	return TRUE;
}

#ifdef GCN
bool memFreePending( void *pMem)
{
	OSLockMutex( &mutexMemAlloc);
	
	bool	bReturn = _memFreePendingLocal( pMem);
	
	OSUnlockMutex( &mutexMemAlloc);
	
	return( bReturn);
}

static bool _memFreePendingLocal( void *pMem)
#else
bool memFreePending( void *pMem)
#endif
{
	ts_memRegion		*pRegion;
	ts_memBlock			*pBlock;

#ifdef MEM_USE_WIN32_MANAGER
	/* if using windows memory manager, just use free */

#if MMX_OK						// alignment only necessary if using simd instructions
	_aligned_free(pMem);
#else
	free(pMem);
#endif
	return(TRUE);
#endif //MEM_USE_WIN32_MANAGER

	pRegion = _memFindMemRegion( pUsedRegionList, pMem);

	if( pRegion == NULL)
	{
		CRT_ERROR( "Address OOR");
	}

	pBlock = _memFindAndRemoveMemBlock( &pRegion->pAllocList, pMem);

	if( pBlock == NULL)
	{
		return FALSE;
	}

#ifdef ASSERT_ON_PERM_FREE
	ASSERTS(!(pBlock->u16UseBits & MEM_PERMANENT_DATA), "Looks like your freeing something that is considered permanent!");
#endif

#ifdef MEM_DBG_CAPS
	_memCheckBlock( pBlock);
#endif

	_memAddBlock( &pRegion->pPendingFreeList, pBlock);

	return TRUE;
}

void memProcessFreePending( void)
{
	ts_memRegion		*pRegion;
	ts_memBlock			*pBlock;

#ifdef GCN
	OSLockMutex( &mutexMemAlloc);
#endif

	pRegion = pUsedRegionList;

	while( pRegion != NULL)
	{
		while( pRegion->pPendingFreeList != NULL)
		{
			pBlock = pRegion->pPendingFreeList;
			pRegion->pPendingFreeList = pBlock->pNext;
			_memAddFreeBlock( pRegion, pBlock);
		}

		pRegion = pRegion->pNext;
	}
#ifdef GCN
	OSUnlockMutex( &mutexMemAlloc);
#endif
}

u32 memHandleFree( t_Handle Handle)
{
	ts_memRegion		*pRegion;

	pRegion = _memFindRegion( Handle);

	if( pRegion == NULL)
		return 0;

	return _memBlockListAvailable( pRegion->pFreeList);
}

#ifdef MEM_DEBUG_BLOCKS

/* set memory block's name. normally this is done with macros (e.g. MEM_SET_ONE_SHOT_NEW_NAME) before the memory allocation,
but sometimes a "new" happens in a place that we don't have a chance to pre-set a name */

void memSetBlockName(void *pMem, char *pcName)
{
	ts_memRegion *pRegion = _memFindMemRegion(pUsedRegionList, pMem);
	if (pRegion)
	{
		ts_memBlock *pBlock = _memFindBlock(pRegion->pAllocList, pMem);
		if (pBlock)
			strncpy(pBlock->AllocName, pcName, ALLOC_NAME_LENGTH);
	}
}
#endif //MEM_DEBUG_BLOCKS

/* -----------------4/28/98 11:37AM------------------
 * Local Functions
 * --------------------------------------------------*/
void _memInitData( void)
{
int					i;

	for( i = 0; i < MEM_MAX_REGIONS; i++)
	{
		StaticRegions[i].Handle = i + 1;
		StaticRegions[i].u32Flags = 0;
		StaticRegions[i].usFitMethod = FIT_BEST;
		StaticRegions[i].pDescriptor = NULL;
		StaticRegions[i].pHandleList = NULL;
		StaticRegions[i].pFreeList = NULL;
		StaticRegions[i].pAllocList = NULL;
		StaticRegions[i].pPendingFreeList = NULL;
		if( i == MEM_MAX_REGIONS - 1)
			StaticRegions[i].pNext = NULL;
		else
			StaticRegions[i].pNext = &StaticRegions[i + 1];
	}

	pFreeRegionList = StaticRegions;
	pUsedRegionList = NULL;
	pActiveRegion = NULL;

}

ts_memRegion *_memFindRegion( t_Handle Handle)
{
	ts_memRegion		*pWalker;

	pWalker = pUsedRegionList;

	while( pWalker)
	{
		if( pWalker->Handle == Handle)
			return pWalker;

		pWalker = pWalker->pNext;
	}

return NULL;
}

void _memInitHandleList( ts_memBlock *pHandleList, u32 u32Handles)
{
	u32		i;

	for( i = 0; i < (u32Handles - 1); i++)
		pHandleList[i].pNext = &pHandleList[i + 1];

	pHandleList[u32Handles - 1].pNext = NULL;
}

/* grab and fill in a memory block from the region's unused block list */

ts_memBlock *_memNewBlock(ts_memRegion *pRegion, void *pStart, u32 u32Length, u32 u32Align)
{
	ts_memBlock			*pNewMem;

	/* check alignment */

	ASSERT(((u32) pStart & (MEM_ADDR_ALIGN - 1)) == 0);
	ASSERT((u32Length & (MEM_ADDR_ALIGN - 1)) == 0);
	ASSERT(u32Align >= MEM_ADDR_ALIGN);	// we are currently assuming a minimum alignment

	/* get an unused memory block */

	ASSERT(pRegion->pHandleList);	// must be an unused block available
	pNewMem = pRegion->pHandleList;
	pRegion->pHandleList = pRegion->pHandleList->pNext;

	/* fill it in */

	pNewMem->pNext = NULL;
	pNewMem->pAddr = (unsigned char *) pStart;
	pNewMem->u32Length = u32Length;

	ASSERTS(u32Align < 1<<16, "Alignments must be less than 1<<16 as they are now stored in a 16 bit var");
	pNewMem->u16Align = (u16)u32Align;

	return pNewMem;
}

void *_memAllocFromRegion( ts_memRegion *pRegion, u32 u32Size, u32 u32Align)
{
	ASSERT( pRegion != NULL);
	ASSERT( u32Size != 0);
	ASSERT( memAllocFunc != NULL);
	ASSERT( memFitFunc != NULL);

#ifdef MEM_DBG_LIST_REQUESTS
	//list memory free's as they happen
	u32 u32Handles = _memListLen( pRegion->pHandleList);
	dbgPrint( "Name: %s, req'd size = %d, free handles %d, name = %s\n", pRegion->pDescriptor, u32Size, u32Handles, acAllocName);
#endif

	if( pRegion->pHandleList == NULL)
	{
		//show memory state if we ran out
		dbgPrint( "%s out of Handles.\n", pRegion->pDescriptor);
//		Layers__ShowMemoryState();
		Layers__CheckMemoryState(CHECK_MEMORY_STATE_PRINT_ALWAYS);
		ASSERTS( false, "Out of Handles");
		LOCKUP( );
	}

	if( pRegion->pFreeList == NULL)
	{
		//show memory state if we ran out
		dbgPrint( "%s out of Memory.\n", pRegion->pDescriptor);
//		Layers__ShowMemoryState();
		Layers__CheckMemoryState(CHECK_MEMORY_STATE_PRINT_ALWAYS);
		ASSERTS( false, "Out of System Memory - Free list empty");
		return( NULL);
	}

	u32 u32AllocSize = _memAllocSize( u32Size, u32Align);
	ts_memBlock *pBlock = memAllocFunc( pRegion, u32AllocSize, u32Align);
	if(!pBlock)
	{
		//show memory state if we ran out
		dbgPrint( "%s out of Memory.\n", pRegion->pDescriptor);
//		Layers__ShowMemoryState();
		Layers__CheckMemoryState(CHECK_MEMORY_STATE_PRINT_ALWAYS);
		ASSERTF( 0 && "Out of System Memory - No block big enough", ("Request bytes = %d", u32Size) );
		return(NULL);
	}

	/* maybe watch memory high-water mark */

	if (pRegion->u32Flags & REGION_TEST_HIGH_WATER_MARK)
	{
		/* watching how much free memory remains is reasonably fast since there are typically few free blocks */

		int nMinMemory = _memBlockListSize(pRegion->pFreeList);
		if (nMinMemory < pRegion->nMinMemory)
			pRegion->nMinMemory = nMinMemory;
	}

	/* handy for tracking down memory leaks-- camp and look for a particular address, typically one reported */
	/* by Layers__ShowMemoryState */

#ifndef CDROM
  	ASSERT((u32) pBlock->pAddr !=  0xffffffff  );
  	ASSERT((u32) pBlock->pAddr !=  0xffffffff  );
  	ASSERT((u32) pBlock->pAddr !=  0xffffffff  );
  	ASSERT((u32) pBlock->pAddr !=  0xffffffff  );
#endif

	/* return block-- _memPrepBlock handles no-man's-land stuff enabled by MEM_DBG_CAPS */

	return(_memPrepBlock(pBlock));
}

void *_memReallocFromRegion( ts_memRegion *pRegion, ts_memBlock *pOldBlock, u32 u32Size, u32 u32Align)
{
	void			*pNew;
	u32			u32CopyLen;
	ts_memBlock	*pNewBlock;

#ifdef MEM_DBG_CAPS
	/* check for memory overwriting on old block */

	_memCheckBlock( pOldBlock);
#endif

	/* malloc new block */

	pNew = _memAllocFromRegion( pRegion, u32Size, u32Align);
	if( !(pNew))
		return NULL;

	/* find memory block */

	pNewBlock = _memFindBlock( pRegion->pAllocList, pNew);

	/* get pointers to actual data within block, taking alignment into account */

	void *pOldAddr = _memPrepBlock(pOldBlock);
	void *pNewAddr = _memPrepBlock(pNewBlock);

	/* how much to copy? */

	if (pNewBlock->u32Length < pOldBlock->u32Length)
	{
		/* new block smaller */

		u32CopyLen = u32Size;
	}
	else
	{
		/* old block smaller */

		u32CopyLen = pOldBlock->u32Length - ((u32) pOldAddr - (u32) pOldBlock->pAddr);	// take old alignment into account
		if (u32CopyLen > u32Size)
			u32CopyLen = u32Size;	// paranoia
	}
	memcpy( pNewAddr, pOldAddr, u32CopyLen);

	/* remove old block from alloc list */

	_memRemoveBlock( &pRegion->pAllocList, pOldBlock);
	_memAddFreeBlock( pRegion, pOldBlock);

	/* done */

	return _memPrepBlock( pNewBlock);
}


ts_memBlock *_memAllocFromBottom( ts_memRegion *pRegion, u32 u32Length, u32 u32Align)
{
	ts_memBlock			*pFitBlock;
	ts_memBlock			*pNewBlock;

	/* find a block big enough */

	pFitBlock = memFitFunc( pRegion->pFreeList, u32Length);
	if( pFitBlock == NULL)
		return NULL;	// no big enough block found

	/* grab an unused memory block from region, fill it in to point to free memory it will use */

	pNewBlock = _memNewBlock(pRegion, pFitBlock->pAddr, u32Length, u32Align);

	/* update free block */

	if( pFitBlock->u32Length == u32Length)
	{
		/* free block is exactly the size we need, so it is no longer a valid entry in the free list. */
		/* move the memory block to the unused list (the memory itself will go with pNewBlock into the */
		/* alloc list) */

		_memRemoveBlock( &pRegion->pFreeList, pFitBlock);
		MEM_PUSH_ALLOC_NAME("Handle");
		_memAddBlock( &pRegion->pHandleList, pFitBlock);
		MEM_POP_ALLOC_NAME();
	}
	else
	{
		/* still space left in free block, update it by amount we're taking. pFitBlock remains in the free list */

		pFitBlock->pAddr += u32Length;
		pFitBlock->u32Length -= u32Length;
	}

	/* add new block to malloc'd block list */

	_memAddBlock( &pRegion->pAllocList, pNewBlock);

	/* done */

	return pNewBlock;
}

ts_memBlock *_memAllocFromTop( ts_memRegion *pRegion, u32 u32Length, u32 u32Align)
{
	u32				u32Offset;
	ts_memBlock		*pFitBlock;
	ts_memBlock		*pNewBlock;

	/* find a block big enough */

	pFitBlock = memFitFunc( pRegion->pFreeList, u32Length);
	if( pFitBlock == NULL)
		return NULL;	// no big enough block found

	/* grab an unused memory block from region, fill it in to point to free memory it will use */

	u32Offset = pFitBlock->u32Length - u32Length;
	pNewBlock = _memNewBlock(pRegion, pFitBlock->pAddr + u32Offset, u32Length, u32Align);

	/* update free block */

	if( pFitBlock->u32Length == u32Length)
	{
		/* free block is exactly the size we need, so it is no longer a valid entry in the free list. */
		/* move the memory block to the unused list (the memory itself will go with pNewBlock into the */
		/* alloc list) */

		_memRemoveBlock( &pRegion->pFreeList, pFitBlock);
		MEM_PUSH_ALLOC_NAME("Handle");
		_memAddBlock( &pRegion->pHandleList, pFitBlock);
		MEM_POP_ALLOC_NAME();
	}
	else
	{
		/* still space left in free block, update it by amount we're taking. pFitBlock remains in the free list */

		pFitBlock->u32Length -= u32Length;
	}

	/* add new block to malloc'd block list */

	_memAddBlock( &pRegion->pAllocList, pNewBlock);

	/* done */

	return pNewBlock;
}

/* calculate actual needed size of requested block of u32Size, taking alignment and buffer overwrite debugging into account */

u32 _memAllocSize( u32 u32Size, u32 u32Align)
{
	u32		u32AllocSize;

	/* round up to default alignment */

	u32AllocSize = MEM_SZE_FIX( u32Size);

	/* account for buffer overwrite guard band */

	u32AllocSize += 2 * MEM_DBG_OFFSET;

	/* if alignment > default alignment, then we have to take into account the number of bytes that may be */
	/* needed to align the buffer. for example, if the default alignment is 16, and the requested alignment */
	/* is 32, then we may have to may have to adjust the returned buffer pointer by 16 bytes to align it. so */
	/* look for a larger buffer. the disadvantage here is that depending on the free block's start address, */
	/* we may or may not have to adjust the pointer. so by adding the extra bytes, we may not find a smaller */
	/* free block that would work out ok. but since the vast majority of malloc's will be at the default */
	/* alignment, this shouldn't be much of an issue */

	ASSERT(u32Align >= MEM_ADDR_ALIGN);	// we are assuming the default alignment is the minimum alignment
	u32AllocSize += (u32Align - MEM_ADDR_ALIGN);

	/* done */

	return(u32AllocSize);
}

ts_memBlock *_memFitFirst( ts_memBlock *pList, u32 u32Size)
{
	while( pList)
	{
		if( pList->u32Length >= u32Size)
			return pList;

		pList = pList->pNext;
	}

	return NULL;
}

ts_memBlock *_memFitBest( ts_memBlock *pList, u32 u32Size)
{
	u32			u32Diff;
	ts_memFit	BestFit = { NULL, {MAXUINT} };

	while( pList)
	{
		if( pList->u32Length >= u32Size)
		{
			u32Diff = pList->u32Length - u32Size;

			if( u32Diff == 0)
				return pList;
			else if( u32Diff < BestFit.Compare.u32Differential)
			{
				BestFit.pBlock = pList;
				BestFit.Compare.u32Differential = u32Diff;
			}
		}

		pList = pList->pNext;
	}

	return BestFit.pBlock;
}

ts_memBlock *_memFitLast( ts_memBlock *pList, u32 u32Size)
{
	ts_memBlock			*pLastFit = NULL;

	while( pList)
	{
		if( pList->u32Length >= u32Size)
			pLastFit = pList;

		pList = pList->pNext;
	}

	return pLastFit;
}

ts_memBlock *_memFitLow( ts_memBlock *pList, u32 u32Size)
{
	ts_memFit			LowFit = { NULL, {MAXUINT} };

	while( pList)
	{
		if( pList->u32Length >= u32Size)
		{
			if( pList->pAddr < (unsigned char *) LowFit.Compare.pAddr)
			{
				LowFit.pBlock = pList;
				LowFit.Compare.pAddr = pList->pAddr;
			}
		}

		pList = pList->pNext;
	}

	return LowFit.pBlock;
}

ts_memBlock *_memFitHigh( ts_memBlock *pList, u32 u32Size)
{
	ts_memFit			HighFit = { NULL, {(u32) NULL} };

	while( pList)
	{
		if( pList->u32Length >= u32Size)
		{
			if( pList->pAddr > (unsigned char *) HighFit.Compare.pAddr)
			{
				HighFit.pBlock = pList;
				HighFit.Compare.pAddr = pList->pAddr;
			}
		}

		pList = pList->pNext;
	}

	return HighFit.pBlock;
}

ts_memBlock *_memFindAddress( ts_memRegion *pList, void *pAddr)
{
	ts_memRegion		*pReg;

	pReg = _memFindMemRegion( pList, pAddr);

	if( !pReg)
		return NULL;

	return _memFindBlock( pReg->pAllocList, pAddr);
}

ts_memRegion *_memFindMemRegion( ts_memRegion *pList, void *pAddr)
{

	while( pList)
	{
		if( (unsigned char *) pAddr >= pList->pMemStart  &&
			 (unsigned char *) pAddr < pList->pMemStart + pList->u32MemLength)
			return pList;

		pList = pList->pNext;
	}

	return NULL;

}

/* look for the memory block that corresponds to the passed buffer address. for this, we have to re-create the
calculations of _memPrepBlock that consider alignment and MEM_DBG_OFFSET */

ts_memBlock *_memFindBlock( ts_memBlock *pBlockList, void *pAddr)
{
	while( pBlockList)
	{
		/* when user requests an alignment > default alignment, then the pointer returned by memAlloc may not */
		/* be the block's pAddr member, but may have been aligned. we can figure out the returned pointer by */
		/* looking at the pAddr and the u32Align fields */
	
		if (pBlockList->u16Align > MEM_ADDR_ALIGN)
		{
			void *pAlignedAddr = (void *) (((u32) pBlockList->pAddr + MEM_DBG_OFFSET + (pBlockList->u16Align - 1)) & ~(pBlockList->u16Align - 1));
			if (pAddr == pAlignedAddr)
				return pBlockList;
		}
		else if (pAddr == pBlockList->pAddr + MEM_DBG_OFFSET)
			return pBlockList;

		/* to next memory block in list */

		pBlockList = pBlockList->pNext;
	}

	/* not found */

	return(NULL);
}

/* look for a block at the passed address, without considering alignment or MEM_DBG_OFFSET */

ts_memBlock *_memFindBlockRaw( ts_memBlock *pBlockList, void *pAddr)
{
	while( pBlockList)
	{
		if (pAddr == pBlockList->pAddr)
			return pBlockList;

		/* to next memory block in list */

		pBlockList = pBlockList->pNext;
	}

	/* not found */

	return(NULL);
}

ts_memBlock *_memFindAndRemoveMemBlock( ts_memBlock **ppBlockList, void *pAddr)
{
	ts_memBlock			*pRetBlock;

	/* it is ok to pass NULL to this routine */

	if (pAddr == NULL)
		return(NULL);

	/* find the block that corresponds to the passed address in the passed block list. typically, this is used for finding */
	/* an allocated memory block for memFree purposes, so ppBlockList is typically pAllocList */

	while( *ppBlockList)
	{
		/* when user requests an alignment > default alignment, then the pointer returned by memAlloc may not */
		/* be the block's pAddr member, but may have been aligned. we can figure out the returned pointer by */
		/* looking at the pAddr and the u32Align fields */
	
		if ((*ppBlockList)->u16Align > MEM_ADDR_ALIGN)
		{
			void *pAlignedAddr = (void *) (((u32) (*ppBlockList)->pAddr + MEM_DBG_OFFSET + ((*ppBlockList)->u16Align - 1)) & ~((*ppBlockList)->u16Align - 1));
			if (pAddr == pAlignedAddr)
			{
				/* found it */

				pRetBlock = *ppBlockList;
				*ppBlockList = (*ppBlockList)->pNext;	// remove from list
				return(pRetBlock);
			}
		}
		else if (pAddr == (*ppBlockList)->pAddr + MEM_DBG_OFFSET)	// in standard alignment, address calculation is simpler
		{
			/* found it */

			pRetBlock = *ppBlockList;
			*ppBlockList = (*ppBlockList)->pNext;	// remove from list
			return(pRetBlock);
		}

		ppBlockList = &(*ppBlockList)->pNext;
	}

	/* block not found to match passed memory address. this is potentially a big problem. the app should not be trying to free */
	/* memory that it does not own. also, we have seen a problem under the codewarrior compiler when using delete []-- it */
	/* adjusted the memory address by 16 bytes (our guess is that the codewarrior default new [] reserves the first 16 bytes */
	/* of the memory address for some auxiliary info, like record size) before calling delete [], so the block was not found. */
	/* this leads to memory fragmentation, so it is a big problem */

	/*** NOTE: it is ok to comment out this assert if it is getting in the way, but PLEASE report the problem to ned or dave ***/

	ASSERT(FALSE);	// see comment immediately above
	return(NULL);
}

/* find passed block in passed list, and remove it from singly-linked list */

void _memRemoveBlock( ts_memBlock **ppList, ts_memBlock *pBlock)
{
	while( *ppList != NULL)
	{
		if( *ppList == pBlock)
		{
			/* found it, remove from linked list */

			*ppList = pBlock->pNext;
			return;
		}

		/* to next entry in linked list */

		ppList = &(*ppList)->pNext;
	}
	ASSERT(FALSE);	// should always find it
}

/* add passed block to head of passed list */

void _memAddBlock( ts_memBlock **ppList, ts_memBlock *pBlock)
{
	pBlock->pNext = *ppList;
	*ppList = pBlock;

#ifdef MEM_DEBUG_BLOCKS
//	ASSERT(strcmp(acAllocName, "No name") != 0);
	strcpy(pBlock->AllocName, acAllocName);
	strcpy(acAllocName, "No name");	// this is a one-shot name
	pBlock->u16UseBits = u16UseBits;

	/* may want to see mallocs-- our memory manager only-- mark this malloc by name */

	if (bReportAlloc)
	{
//		ASSERT(strcmp(pBlock->AllocName, "No name") != 0);	// uncomment to find non-named memory allocations
//		ASSERT(strcmp(pBlock->AllocName, "new") != 0);		// uncomment to find non-named memory allocations
		ASSERT(pBlock->AllocName[0] > 0);
		pBlock->AllocName[0] = -pBlock->AllocName[0];		// sure, it's cheesy, what's it to ya?
	}

#endif //MEM_DEBUG_BLOCKS
}

void _memAddFreeBlock( ts_memRegion *pRegion, ts_memBlock *pBlock)
{

	ts_memBlock			*pPrevBlock,
							*pNextBlock;

	if( pRegion->pFreeList == NULL)			//none on list, add to head
	{
		pBlock->pNext = pRegion->pFreeList;
		pRegion->pFreeList = pBlock;
		return;
	}

	pNextBlock = pRegion->pFreeList;

	if( pNextBlock->pAddr > pBlock->pAddr) //addr of first on list is greater, add to head
	{
		pBlock->pNext = pNextBlock;
		pRegion->pFreeList = pBlock;

		//see if we can consolidate
		if( ( pBlock->pAddr + pBlock->u32Length) == pNextBlock->pAddr)
		{
			pBlock->pNext = pNextBlock->pNext;

			pBlock->u32Length += pNextBlock->u32Length;
			
			_memAddBlock( &pRegion->pHandleList, pNextBlock);
		}
		return;
	}

	pPrevBlock = pNextBlock;

	while( pPrevBlock->pNext != NULL)
	{
		pNextBlock = pPrevBlock->pNext;

		if( pNextBlock->pAddr > pBlock->pAddr)
		{
			//we have found our insertion point, add block into list
			pBlock->pNext = pNextBlock;
			pPrevBlock->pNext = pBlock;
			
			//see if we can consolidate with next block
			if( ( pBlock->pAddr + pBlock->u32Length) == pNextBlock->pAddr)
			{
				pBlock->pNext = pNextBlock->pNext;
				
				pBlock->u32Length += pNextBlock->u32Length;

				_memAddBlock( &pRegion->pHandleList, pNextBlock);
			}

			//now see if we can consolidate with prev block
			if( ( pPrevBlock->pAddr + pPrevBlock->u32Length) == pBlock->pAddr)
			{
				pPrevBlock->pNext = pBlock->pNext;

		 		pPrevBlock->u32Length += pBlock->u32Length;

				_memAddBlock( &pRegion->pHandleList, pBlock);
			}
			return;
		}
		
		pPrevBlock = pNextBlock;
	}

	//pBlock is last on list, see if we can consolidate
	if( ( pPrevBlock->pAddr + pPrevBlock->u32Length) == pBlock->pAddr)
	{
 		pPrevBlock->u32Length += pBlock->u32Length;
		_memAddBlock( &pRegion->pHandleList, pBlock);
		return;
	}

	//add to tail of list
	pPrevBlock->pNext = pBlock;
	pBlock->pNext = NULL;
}


void *_memPrepBlock( ts_memBlock *pBlock)
{
	void *pStart;

#ifdef MEM_DBG_CAPS
	*((u32 *) pBlock->pAddr) = MEM_DBG_MARKER;
	*((u32 *) (pBlock->pAddr + pBlock->u32Length - 4)) = MEM_DBG_MARKER;
#endif

	ASSERT(((u32) pBlock->pAddr & (MEM_ADDR_ALIGN - 1)) == 0);
	pStart = (void *) ((u32) pBlock->pAddr + MEM_DBG_OFFSET);

	/* handle alignment > default alignment */

	if (pBlock->u16Align > MEM_ADDR_ALIGN)
		pStart = (void *) (((u32) pStart + (pBlock->u16Align - 1)) & ~(pBlock->u16Align - 1));

	return(pStart);
}

void _memCheckBlock( ts_memBlock *pBlock)
{

	if( *((u32 *) pBlock->pAddr) != MEM_DBG_MARKER ||
		 *((u32 *) (pBlock->pAddr + pBlock->u32Length - 4)) != MEM_DBG_MARKER)
	{
		CRT_ERROR( "MEMORY OVERRUN");
	}

}


u32 _memBlockListAvailable( ts_memBlock *pList)
{
	u32	u32Total = 0;

	while( pList)
	{
		u32Total += pList->u32Length - (2 * MEM_DBG_OFFSET);

		pList = pList->pNext;
	}

	return u32Total;
}

/*
 Debug Stuff
 */
#ifdef MEM_DEBUG_BLOCKS
void memShowBlockInfo( ts_memBlock *pBlock);
void memShowBlockList( ts_memBlock *pList);
u32 _memCHeapInUse( void);
u32 _memCHeapTotal( void);

void memShowRegionFreeSize( t_Handle Handle)
{
	static int nLastHandles = 0;
	static int nLastMem = 0;
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion == NULL)
	{
		dbgPrint("Unable to find region %lu.\n", Handle);
		return;
	}
	int nHandles = _memListLen(pRegion->pFreeList);
	int nMem = _memBlockListSize(pRegion->pFreeList);

	dbgPrint("\n=Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free=\n");
	dbgPrint("Region %s: %d handles %d bytes; %d new handles %d new bytes\n",
				pRegion->pDescriptor,
				nHandles,
				nMem,
				nHandles - nLastHandles,
				nMem - nLastMem);
	dbgPrint("=Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free==Free=\n\n");
	nLastHandles = nHandles;
	nLastMem = nMem;
}

void memShowRegionAllocSize( t_Handle Handle)
{
	static int nLastHandles = 0;
	static int nLastMem = 0;
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion == NULL)
	{
		dbgPrint("Unable to find region %lu.\n", Handle);
		return;
	}
	int nHandles = _memListLen(pRegion->pAllocList);
	int nMem = _memBlockListSize(pRegion->pAllocList);

	dbgPrint("\n=Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc=\n");
	dbgPrint("Region %s: %d handles %d bytes; %d new handles %d new bytes\n",
				pRegion->pDescriptor,
				nHandles,
				nMem,
				nHandles - nLastHandles,
				nMem - nLastMem);
	dbgPrint("=Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc==Alloc=\n\n");
	nLastHandles = nHandles;
	nLastMem = nMem;
}

void memShowRegionBriefInfo( t_Handle Handle)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion == NULL)
	{
		dbgPrint("Unable to find region %lu.\n", Handle);
		return;
	}
	int nAllocHandles = _memListLen(pRegion->pAllocList);
	int nAllocMem = _memBlockListSize(pRegion->pAllocList);
	int nFreeHandles = _memListLen(pRegion->pFreeList);
	int nFreeMem = _memBlockListSize(pRegion->pFreeList);

	dbgPrint("\n=========================================================================\n");
	dbgPrint(" System Memory:  Used   %d handles, %d bytes\n",
				nAllocHandles,
				nAllocMem
				);
	dbgPrint(" System Memory:  Free   %d handles, %d bytes\n",
				nFreeHandles,
				nFreeMem
				);
//	dbgPrint("=========================================================================\n\n");
}

void memShowRegionInfo( t_Handle Handle)
{
	u32		u32TotalHandles = 0;
	u32		u32Handles = 0;
	u32		u32TotalMem = 0;
	u32		u32Mem = 0;

	ts_memRegion		*pRegion;

	pRegion = _memFindRegion( Handle);

	if( pRegion == NULL)
	{
		dbgPrint( "Unable to Find Region %lu.\n", Handle);
		return;
	}

	dbgPrint( "Name: %s\n", pRegion->pDescriptor);

	u32Handles = _memListLen( pRegion->pHandleList);
	u32TotalHandles += u32Handles;
	dbgPrint( "Handle  List: %08x Free: %lu\n", pRegion->pHandleList, u32Handles);

	u32Handles = _memListLen( pRegion->pFreeList);
	u32Mem = _memBlockListSize( pRegion->pFreeList);
	u32TotalHandles += u32Handles;
	u32TotalMem += u32Mem;
	dbgPrint( "Free  Blocks: %08x Handles: %lu Size: %lu\n", pRegion->pFreeList, u32Handles, u32Mem);
	memShowBlockList( pRegion->pFreeList);

	u32Handles = _memListLen( pRegion->pAllocList);
	u32Mem = _memBlockListSize( pRegion->pAllocList);
	u32TotalHandles += u32Handles;
	u32TotalMem += u32Mem;
	dbgPrint( "Alloc Blocks: %08x Handles: %lu Size: %lu\n", pRegion->pAllocList, u32Handles, u32Mem);
	memShowBlockList( pRegion->pAllocList);

	dbgPrint( "Total Handles: %lu Useable Mem: %lu Mem: %lu\n\n",
			u32TotalHandles, u32TotalMem, u32TotalMem + (u32TotalHandles * sizeof( ts_memBlock)));
}

void memCheckHeapUsage( t_Handle Handle, u32 *LayersUsage, u32 *LayersTotal, u32 *CHeapUsage, u32 *CHeapTotal, u32 *u32TotalHandles)
{
	u32		u32Handles = 0;
	u32		u32TotalMem = 0;
	u32		u32Mem = 0;

	ts_memRegion		*pRegion;

	*u32TotalHandles = 0;

	pRegion = _memFindRegion( Handle);

	if(pRegion)
	{
		u32Handles = _memListLen( pRegion->pFreeList);
		u32Mem = _memBlockListSize( pRegion->pFreeList);
		*u32TotalHandles += u32Handles;
		u32TotalMem += u32Mem;

		u32Handles = _memListLen( pRegion->pAllocList);
		u32Mem = _memBlockListSize( pRegion->pAllocList);
		*u32TotalHandles += u32Handles;
		u32TotalMem += u32Mem;
	}

	*LayersUsage = u32Mem;
	*LayersTotal = u32TotalMem + (*u32TotalHandles * sizeof( ts_memBlock));

	*CHeapUsage = _memCHeapInUse();
	*CHeapTotal = _memCHeapTotal();
}

void memShowBlockList( ts_memBlock *pList)
{
	while( pList != NULL)
	{
		memShowBlockInfo( pList);
		pList = pList->pNext;
	}
}

/* set/clear bits in the usage bitfield that gets put in each memory block's u16UseBits field */

void memSetUseBit(const u16 u16Bit)
{
	u16UseBits |= u16Bit;
	ASSERT((u16UseBits & MEM_PERMANENT_DATA) == 0);	// can only mark things as permanent through memMarkPermanentMemory
}
void memClearUseBit(const u16 u16Bit)
{
	u16UseBits &= ~u16Bit;
}

/* return total size of all memory blocks marked with passed bit(s) */

int memQueryBlockBit(const u16 u16Bit, const char *pBlockName /*= NULL*/, int *pnHandleCount /*= NULL*/, const bool bMatchAllBits /*= false*/, const bool bDetail /*= false*/, FileStream *pLogFile /*= NULL*/)
{
	/* loop through all memory blocks */

	int nTotalSize = 0;
	int nHandles = 0;
	int nIndex = 0;
	int nBlockNameLength = 0;
	if (pBlockName)
		nBlockNameLength = strlen(pBlockName);
	char Message[200];
	ts_memBlock *pBlock = pActiveRegion->pAllocList;
	while (pBlock)
	{
		bool bFound;
		if (bMatchAllBits)
			bFound = (pBlock->u16UseBits & u16Bit) == u16Bit;
		else
			bFound = (pBlock->u16UseBits & u16Bit) != 0;
		if (bFound)
		{
			/* optional block name can total up a subset of blocks marked with current bit */

			if ((pBlockName == NULL) || (strnicmp(pBlockName, pBlock->AllocName, nBlockNameLength) == 0))
			{
				if (bDetail)
				{
					sprintf(Message, "  %3d: 0x%08x - 0x%08x (%8lu), %s\n", nIndex++, pBlock->pAddr, ((char *) pBlock->pAddr) + pBlock->u32Length - 1, pBlock->u32Length, pBlock->AllocName);
					if (pLogFile) pLogFile->Print(Message); else dbgPrint(Message);
				}
				nTotalSize += pBlock->u32Length;
				nHandles++;
			}
		}

		/* to next block */

		pBlock = pBlock->pNext;
	}

	/* return results */

	if (bDetail)
	{
		sprintf(Message, "  Done\n");
		if (pLogFile) pLogFile->Print(Message); else dbgPrint(Message);
	}
	if (pnHandleCount)
		*pnHandleCount = nHandles;
	return(nTotalSize);
}

/* return total size of all buffers marked with MEM_REUSABLE_DATA, clear bit for re-use. this is useful for measuring how
much a particular section malloc's without having to devote a #defined bit to it */

void memClearBlockBit(const u16 u16Bit, const char *pBlockName /*= NULL*/)
{
	/* loop through all memory blocks clearing the bit */

	u16 u16ClearMask = ~u16Bit;
	int nBlockNameLength = 0;
	if (pBlockName)
		nBlockNameLength = strlen(pBlockName);
	ts_memBlock *pBlock = pActiveRegion->pAllocList;
	while (pBlock)
	{
		if (pBlock->u16UseBits & u16Bit)
		{
			/* optional block name can total up a subset of blocks marked with current bit */

			if ((pBlockName == NULL) || (strnicmp(pBlockName, pBlock->AllocName, nBlockNameLength) == 0))
				pBlock->u16UseBits &= u16ClearMask;
		}

		/* to next block */

		pBlock = pBlock->pNext;
	}
}

void memShowBlockInfo( ts_memBlock *pBlock)
{
	dbgPrint( "  0x%08x - 0x%08x (%8lu), %s\n", 
		pBlock->pAddr, 
		((char *) pBlock->pAddr) + pBlock->u32Length - 1, 
		pBlock->u32Length, 
		pBlock->AllocName);
#ifdef WIN32
	Sleep(10);//ensure the memory listing has finished printing
#endif
}

#else
#define memShowBlockInfo(pBlock)
#endif //MEM_DEBUG_BLOCKS

#ifndef CONSUMER_BUILD

/* look at length of list, cumulative size of list */

static u32 _memBlockListSize( ts_memBlock *pList)
{
	u32	u32ListSize = 0;

	while( pList != NULL)
	{
		u32ListSize += pList->u32Length;
		pList = pList->pNext;
	}

	return u32ListSize;
}

u32 _memListLen( void *pNext)
{
	u32	u32Length = 0;

	while( pNext != NULL)
	{
		u32Length++;
		pNext = *((void **) pNext);
	}

	return u32Length;
}

/* fill in some interesting stats-- need this for cd rom build too because it supports the debug menu */

void memGetRegionStats(t_Handle Handle, int *pnFreeBytes, int *pnFreeHandles, int *pnLargestFree, int *pnAllocBytes, int *pnAllocHandles)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion)
	{
		if (pnFreeBytes)
			*pnFreeBytes = _memBlockListSize(pRegion->pFreeList);
		if (pnFreeHandles)
			*pnFreeHandles = _memListLen(pRegion->pFreeList);
		if (pnAllocBytes)
			*pnAllocBytes = _memBlockListSize(pRegion->pAllocList);
		if (pnAllocHandles)
			*pnAllocHandles = _memListLen(pRegion->pAllocList);
		if (pnLargestFree)
			*pnLargestFree = 0;
	}
	else
	{
		if (pnFreeBytes)
			*pnFreeBytes = 0;
		if (pnFreeHandles)
			*pnFreeHandles = 0;
		if (pnAllocBytes)
			*pnAllocBytes = 0;
		if (pnAllocHandles)
			*pnAllocHandles = 0;
		if (pnLargestFree)
			*pnLargestFree = 0;
	}
}

/* get region's u32Length field-- actual malloc'able memory in region after handle blocks are allocated */

int memGetRegionLength(t_Handle Handle)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion)
		return(pRegion->u32MemLength);
	else
		return(0);
}

/* turn high-water tracking on or off */

void memTrackHighWater(t_Handle Handle, bool bState)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion)
	{
		if (bState)
			pRegion->u32Flags |= REGION_TEST_HIGH_WATER_MARK;
		else
			pRegion->u32Flags &= ~REGION_TEST_HIGH_WATER_MARK;
		memResetHighWater(Handle);
	}
}

/* status of high water check */

bool memHighWaterOn(t_Handle Handle)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion)
		return((pRegion->u32Flags & REGION_TEST_HIGH_WATER_MARK) != 0);
	return(false);
}

/* reset high water before a world is loaded */

void memResetHighWater(t_Handle Handle)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if (pRegion)
		pRegion->nMinMemory = MAXINT;
}

/* report on high water mark */

void memGetHighWater(t_Handle Handle, int *pnMinBytes, int *pnMaxBytes)
{
	ts_memRegion *pRegion = _memFindRegion(Handle);
	if ((pRegion) && (pRegion->nMinMemory != MAXINT))
	{
		if (pnMinBytes)
			*pnMinBytes = pRegion->nMinMemory;
		if (pnMaxBytes)
			*pnMaxBytes = pRegion->u32MemLength - pRegion->nMinMemory;		// high water mark is (initial size - low water mark)
	}
	else
	{
		if (pnMinBytes)
			*pnMinBytes = 0;
		if (pnMaxBytes)
			*pnMaxBytes = 0;
	}
}
#endif //CONSUMER_BUILD

// !!! - N8 took these out for SNSYS. 
#if defined( PS2 ) && defined( CW )
//a routine to return a processor register (a3)
asm char *GetA3(void)
{
// return A3
move v0, a3
jr ra
}

/* The mallinfo strucutre is defined in malloc.h */
struct ts_Mallinfo
{
  int arena;    /* total space allocated from system */
  int ordblks;  /* number of non-inuse chunks */
  int smblks;   /* unused -- always zero */
  int hblks;    /* number of mmapped regions */
  int hblkhd;   /* total space in mmapped regions */
  int usmblks;  /* unused -- always zero */
  int fsmblks;  /* unused -- always zero */
  int uordblks; /* total allocated space */
  int fordblks; /* total non-inuse space */
  int keepcost; /* top-most, releasable (via malloc_trim) space */
};	

//stupid way of getting return ptr from mallinfo()
asm ts_Mallinfo *GetV0(void)
{
// return v0
jr ra
}

//get the current number of bytes of the C heap in use
u32 _memCHeapInUse( void)
{
	ts_Mallinfo *mi;

	mallinfo();
	mi = GetV0();
	return mi->uordblks;
}

//get the total space allocated from the system
u32 _memCHeapTotal( void)
{
	ts_Mallinfo *mi;

	mallinfo();
	mi = GetV0();
	return mi->arena;
}

#else
//not PS2...
u32 _memCHeapInUse( void){return 0;}
u32 _memCHeapTotal( void){return 0;}
#endif

#ifdef MALLOC_STATS
/* malloc/new stats colection routines */

void MallocStats__Clear(void)
{
	memset(nMallocStats, 0, MAX_MALLOC_STATS_SIZE * sizeof(int));
	memset(nNewStats, 0, MAX_MALLOC_STATS_SIZE * sizeof(int));
}

void MallocStats__Report(void)
{
#ifdef WIN32
	static int nFileCount = 0;
	char FileName[100];
	sprintf(FileName, "c:\\temp\\MallocStats%d.log", nFileCount++);	// make it easy to capture multiple snapshots
	FILE *pFile = fopen(FileName, "wt");
	if (pFile == NULL)
	{
		ASSERT(FALSE);	// file name is hard-coded-- you may have to change it
		return;
	}

	/* header */

	fprintf(pFile, "size\tmalloc\tnew\n");
	
	/* print out count for each size. collect some together (e.g. 1k to 2k) */

	int nMallocTotal = 0;
	int nNewTotal = 0;
	for (int i = 0; i < MAX_MALLOC_STATS_SIZE; i++)
	{
		/* time to start a new total? */

		if ((i < 256) || (i == 256) || (i == 1024) || (i == 2048) || (i == 4096))
		{
			/* print out previous total */

			if ((nMallocTotal != 0) || (nNewTotal != 0))
			{
				if (i <= 256)
					fprintf(pFile, "%08d\t%d\t%d\n", i - 1, nMallocTotal, nNewTotal);
				else if (i == 1024)
					fprintf(pFile, "1k-2k\t%d\t%d\n", nMallocTotal, nNewTotal);
				else if (i == 2048)
					fprintf(pFile, "2k-4k\t%d\t%d\n", nMallocTotal, nNewTotal);
				else
					fprintf(pFile, ">=4k\t%d\t%d\n", nMallocTotal, nNewTotal);
			}

			/* reset totals */

			nMallocTotal = 0;
			nNewTotal = 0;
		}

		/* add to total */

		nMallocTotal += nMallocStats[i];
		nNewTotal += nNewStats[i];
	}

	/* print out final total */

	if ((nMallocTotal != 0) || (nNewTotal != 0))
	{
		if (i <= 256)
			fprintf(pFile, "%08d\t%d\t%d\n", i - 1, nMallocTotal, nNewTotal);
		else if (i == 1024)
			fprintf(pFile, "1k-2k\t%d\t%d\n", nMallocTotal, nNewTotal);
		else if (i == 2048)
			fprintf(pFile, "2k-4k\t%d\t%d\n", nMallocTotal, nNewTotal);
		else
			fprintf(pFile, ">=4k\t%d\t%d\n", nMallocTotal, nNewTotal);
	}

	/* final stats */

	fprintf(pFile, "\nTotal memory = %d, wasted at 16-byte align = %d, 32-byte align = %d, 64-byte align = %d\n", nTotalMemory, nWastedMemory16, nWastedMemory32, nWastedMemory64);

	/* and done */

	fclose(pFile);
#endif //WIN32
}
#endif //MALLOC_STATS

/* mark currently allocate blocks as "permanent." this routine is called after game engine initialization is finished, but
before the first fluff or game loop has started. it marks the memory allocations that are in place for the entire game. on
game platforms, these blocks will never be free'd, since the game never "ends" (that is, GameEngine__Shutdown is never called).
the intent of this routine is to provide a way to look for memory leaks. in between switching game loops, the memory manager
state should make it back to this initial state (all blocks marked as permanent) or else we have a memory leak that will
eventually gum up the works (though, of course, it may take a long time) */
/* TJC - I also hijacked it for auto-freeing memory leaks in cdrom
builds which was done primarily for cleanup speed using
memIgnoreFrees() */
void memMarkPermanentMemory(t_Handle RegionHandle)
{
#ifdef MEM_DEBUG_BLOCKS

	/* find requested region */

	ts_memRegion *pRegion = _memFindRegion(RegionHandle);
	if (pRegion == NULL)
	{
		dbgPrint("memMarkPermanentMemory: unable to find requested region %lu.\n", RegionHandle);
		return;
	}

	/* report */

	dbgPrint("memMarkPermanentMemory: region %s, alloc bytes %d, handles %d, free bytes %d, handles %d\n",
				pRegion->pDescriptor,
				_memBlockListSize(pRegion->pAllocList),
				_memListLen(pRegion->pAllocList),
				_memBlockListSize(pRegion->pFreeList),
				_memListLen(pRegion->pFreeList));
	
	/* mark allocated blocks-- just edit name */

	ts_memBlock *pBlock = pRegion->pAllocList;
	while (pBlock)
	{
		// TJC - remember that its perm
		pBlock->u16UseBits |= MEM_PERMANENT_DATA;

		/* to next allocated block */

		pBlock = pBlock->pNext;
	}

#endif //MEM_DEBUG_BLOCKS
}

/* mark an individual buffer as permanent. only used on gcn as far as i can tell, and seems dangerous to me (ned) */

void memMarkBufferPermanent(void *pAddr)
{
#ifdef MEM_DEBUG_BLOCKS

#ifndef GCN
	ASSERT(false);	// gcn only?
#endif //GCN

	/* find the block */

	ts_memRegion *pRegion = _memFindMemRegion( pUsedRegionList, pAddr);
	if( pRegion == NULL)
		CRT_ERROR( "Address OOR");
	ts_memBlock *pBlock = _memFindBlock( pRegion->pAllocList, pAddr);
	if( pBlock == NULL)
	{
		ASSERT(false);
		return;
	}

	/* set bit */

	pBlock->u16UseBits |= MEM_PERMANENT_DATA;

#endif //MEM_DEBUG_BLOCKS
}

/* return TRUE if all current allocations are marked as permanent */

bool memCheckPermanentMemory(t_Handle RegionHandle)
{
	/* find requested region */
#ifdef GCN
	OSLockMutex( &mutexMemAlloc);
#endif

	ts_memRegion *pRegion = _memFindRegion(RegionHandle);
	if (pRegion == NULL)
	{
		dbgPrint("memCheckPermanentMemory: unable to find requested region %lu.\n", RegionHandle);
		return(FALSE);
	}

	/* check allocated blocks-- a character gets prepended to permanent blocks by memMarkPermanentMemory */

	ts_memBlock **pBlock = &pRegion->pAllocList;
	
	bool bRetVal = TRUE;

	ASSERT_PTR(pBlock);
	while (*pBlock)
	{
		ASSERT_PTR(*pBlock);

		if(!((*pBlock)->u16UseBits & MEM_PERMANENT_DATA))
		{
			// TJC - get it
			ts_memBlock *pLeakBlock = *pBlock;

			// TJC - unlink it
			(*pBlock) = pLeakBlock->pNext;

			// TJC - check it
#ifdef MEM_DBG_CAPS
			_memCheckBlock( pLeakBlock);
#endif

			// TJC - are we supposed to have leaks?
			if(!sMemIgnoreFrees)
			{
				// TJC - tell someone!!
				dbgPrint( "Leak Recovered!  ");
				memShowBlockInfo(pLeakBlock);

				/* we should return a false */
				bRetVal = FALSE;
			}

			// TJC - now add it back to free list..
			_memAddFreeBlock( pRegion, pLeakBlock);

			/* we shouldnt step to the next or we will miss some.  You
				see, we changed *pBlock to point to pNext, so now to get
				to pNext, we dont have to do anything */
			continue;
		}

		/* to next allocated block */
		pBlock = &(*pBlock)->pNext;
		ASSERT_PTR(pBlock);
	}

	/* always start paying attention to frees at this point */
	sMemIgnoreFrees = FALSE;

	/* all blocks marked as permanent */

#ifdef GCN
	OSUnlockMutex( &mutexMemAlloc);
#endif
	
	return bRetVal;
}

