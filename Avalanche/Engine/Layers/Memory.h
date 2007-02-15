#ifndef __L_MEMORY_H
#define __L_MEMORY_H

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"
#include "platform/CompilerControl.h"

/// This define controls which of two memory managers are used.  If
/// its defined, the old style, handle based, memory manager is used.
/// If its undefined, the more recent memory manager which puts its
/// headers with the managed memory is used.
#undef MEM_SEPARATE_HEADERS

/******************* forward declarations ****************/

class FileStream;					// in EngineHelper/FileStream.h
struct CMemPool;

/* -----------------7/1/98 12:42PM-------------------
 * Fit Methods
 * --------------------------------------------------*/
enum
{
	FIT_FIRST = 0,								// Find first fit
	FIT_BEST,									// Find best fit
	FIT_LAST,									// Find last fit
	FIT_LOW,
	FIT_HIGH,

	FIT_MAX							          // Must be last
} ;

enum
{
	ALC_TOP = 0,								// Alloc from top of block
	ALC_BTM,										// Alloc from bottom of block

	ALC_MAX										// Must be last
} ;

/* default alignment of returned blocks. math routines may require alignment (16-byte for vu0 processing on ps2, 16-byte
for mmx simd processing on pc), so the default alignment is set to reflect that. this is important, since some variables
are created via the new operator, which doesn't currently have a way to request alignment. since the variable's class may
contain math-aligned classes (like Graphics4x4), the variable itself needs to be 16-byte aligned. the memAlloc routine
takes an optional alignment. this is useful for things like buffers for ps2 iop dma, which benefit from 64-byte alignment.
we talked about having the default alignment be the size of a cache line to promote cache coherency, but collecting stats
showed that there was a lot of wasted memory at a default alignment of 64 (cache line size) on the ps2 (like 1%), so let's
stick with 16

we currently assume that all blocks of memory tracked by the memory manager will be default aligned, both in starting address
and length. alignment requests of less than the default will be bumped up to the default. since the vast majority of
malloc requests will be at the default alignment, this saves us some work */

#if defined(WIN32) && !defined(_XBOX)
#define MEM_ADDR_ALIGN			16
#elif defined(_XBOX)
#define MEM_ADDR_ALIGN			16
#elif defined(PS2)
#define MEM_ADDR_ALIGN			16
#elif defined(GCN)
#define MEM_ADDR_ALIGN			8
#endif //WIN32/PS2

/* a define to enable stats collection abous size of buffers requested through malloc and new */

#ifdef MEM_SEPARATE_HEADERS

//#define MALLOC_STATS
#ifdef CDROM	// paranoia
#undef MALLOC_STATS
#endif //CDROM
#ifdef MALLOC_STATS
#define MAX_MALLOC_STATS_SIZE	4096 // size of stats tables-- we increment the table based on the size in bytes of the malloc request
#endif //MALLOC_STATS

#endif // MEM_SEPARATE_HEADERS


/* there are times when we want to enable the named memory blocks on a cdrom build for nasty debugging, so make it
separate from CDROM */

#ifdef CDROM
//#define MEM_DEBUG_BLOCKS	// can define if needed for CDROM build
#else //CDROM
#define MEM_DEBUG_BLOCKS	// always define for non-CDROM build
#endif //CDROM

/* on the pc, we use windows' memory manager instead of ours. this is so we can get clean shut-downs. the issue is
that since we are overriding new and delete, things get malloc'd (through new) before the layers heap gets created.
then other things get malloc'd after the layers heap is ready, so we have a mixture of C runtime heap and layers heap
memory buffers. at shutdown (which we only care about on the pc, currently), we get into problem with delete'ing order.
buffers that get malloc'd from the layers heap may not get free'd until after the layers heap has been shut down, which
is a problem. i don't think that we have to worry about the converse case, where a buffer malloc'd from the c runtime
heap gets free'd before the layers heap is shut down, but that would be a problem too. i tried other ways of fixing
this, but there's no simple solution, so we'll ignore our memory manager

the con's to this are

1. our memory manager doesn't get exercised on the pc (but it's pretty stable at this point)
2. we get no information about # of handles and heap size needed for the game on the pc (but data size differs platform to platform anyway)

you can still comment out MEM_USE_WIN32_MANAGER to use our memory manager, but you may not get clean shutdowns

using the windows memory manager also enables us to use BoundsChecker */

#if defined( WIN32_TOOLS_DEBUG ) || defined( WIN32_TOOLS_RELEASE ) || defined( WIN32_TOOLS_HYBRID )
#define MEM_USE_WIN32_MANAGER
#undef MEM_DEBUG_BLOCKS
#endif //WIN32

// Define this for poor-man's bounds checker
//#define PARANOIDHEAP
#if defined(WIN32) && defined(_DEBUG) && defined(PARANOIDHEAP)

#define MEM_USE_WIN32_MANAGER
#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>
#else
#undef PARANOIDHEAP
#endif

//#define BOUNDSCHECKER
#if defined(WIN32) && defined(BOUNDSCHECKER) && !defined(_XBOX)
#define MEM_USE_WIN32_MANAGER
#undef MEM_DEBUG_BLOCKS
#endif

/* in an attempt to be able to more closely track where memory is used, we use the last <n> characters in
ts_memBlock.AllocName as a bitfield. each malloc can belong to more than one category, so we can analyze in
different ways. these categories are carefully ordered... */

#define MEM_PERMANENT_DATA				0x0001	// only set by memMarkPermanentMemory, not via memSetUseBit
#define MEM_PRELOAD_DATA				0x0002

/* catch-all for SimLoop::Initialize-- everything except permanent memory buffers */

#define MEM_GAMELOOP_INIT_DATA		0x0004	// simloop is a gameloop, contains all subsequent categories

/* independent sub-sections of SimLoop::Initialize. adding these up should roughly equal SimLoop memory */

#define MEM_WORLD_DATA					0x0008	// independent sub-category of simloop-- LoadWorld
#define MEM_LEVEL_DATA					0x0010	// independent sub-category of simloop-- LoadLevelData
#define MEM_AVATAR_DATA					0x0020	// independent sub-category of simloop-- CreatePlayer
#define MEM_ENV_DATA						0x0040	// independent sub-category of simloop-- g_EnvActorMgr.Load
#define MEM_WATER_DATA					0x0080	// independent sub-category of simloop-- g_aniHFMgr.CreateAll and g_waterMgr.CreateAll
#define MEM_EFFECTS_DATA				0x0100	// independent sub-category of simloop-- SetupEffects
//available									0x0200	//
#define MEM_HUD_DATA						0x0400	// independent sub-category of simloop-- m_Hud.Initialize
#define MEM_PARTICLE_DATA				0x0800	// particle data
#define MEM_AUDIO_DATA					0x1000	// audio data
//available									0x2000	//

/* ps2 only, dma display lists (wrap calls to PS2Renderer::createDDMAObject) */

#define MEM_DMA_DATA						0x4000

/* reusable-- use it, query and clear with memClearReusableBit, re-use it. used during environmental actor memory analysis */

#define MEM_REUSABLE_DATA				0x8000

#define MEM_ALL_USE_BITS				0xffff

/* flags for ts_memRegion.u32Flags */

#define REGION_TEST_HIGH_WATER_MARK	0x00000001	// keep track of lowest available memory for high water mark test


/******************* global prototypes *******************/

void		memInit( void);

void		memSetFitScheme(s32 s32Method);
void		memSetFitMethod(s32 s32Method);
void		memSetAllocMethod(s32 s32Method);

void		memSetOneShotNewAlign(u32 u32Align);
void		memSetReturnAddress(u32 ra);

// callbacks should return true if the alloc should be retried,
// otherwise, the alloc will fail and assert.
typedef bool OutOfMemCallback(u32 size, u32 align);
void		memAddOutOfMemCallback(OutOfMemCallback* callback);
void		memRemoveOutOfMemCallback(OutOfMemCallback* callback);
bool		memCallOutOfMemCallback(u32 size, u32 align);

#ifdef MEM_DEBUG_BLOCKS
void		memSetAllocName(char *pcName);
void		memSetAllocFileName(char *pcType, const char *pcFileName);
void		memSetNewName(char *pcName);
void		memSetOneShotNewName(char *pcName);
void		memSetBlockName(void *pMem, char *pcName);
#define	MEM_SET_ALLOC_NAME(pcName)					memSetAllocName(pcName)
#define	MEM_SET_ALLOC_FILENAME(pcType, pcName)	memSetAllocFileName(pcType, pcName)
#define	MEM_SET_ONE_SHOT_NEW_NAME(pcName)		memSetOneShotNewName(pcName)
#define	MEM_SET_BLOCK_NAME(pMem, pcName)			memSetBlockName(pMem, pcName)
#define  MEM_SET_RETURN_ADDRESS()               do { u32 ra; getra(ra); memSetReturnAddress(ra); } while(0)
#else //MEM_DEBUG_BLOCKS
#define	MEM_SET_ALLOC_NAME(pcName)					((void)0)
#define	MEM_SET_ALLOC_FILENAME(pcType, pcName)	((void)0)
#define	MEM_SET_ONE_SHOT_NEW_NAME(pcName)		((void)0)
#define	MEM_SET_BLOCK_NAME(pMem, pcName)			((void)0)
#define  MEM_SET_RETURN_ADDRESS()               ((void)0)
#endif //MEM_DEBUG_BLOCKS

// memory regions
t_Handle	memAddRegion( unsigned char *pStart, u32 u32Length, u32 u32Flags, char *pdesc, u32 u32Handles, unsigned short usfitmethod, CMemPool* pools );
t_Handle	memSetActiveHandle( t_Handle Handle );
void		memMoveRegionInList( t_Handle Handle, bool bFront );
bool		memRegionIsEmpty( t_Handle Handle );
u32		memHandleFree( t_Handle Handle );

void		*memAlloc(u32 u32Size, u32 u32Align = MEM_ADDR_ALIGN, bool canFreeRamToDoIt = true);
void		*memAllocForIO(u32 u32Size);
void		*memAllocForIO(u32 u32Size, u32 u32Align);
void		*memShrink(void *pAddr, u32 u32NewSize);
void		*memResize(void *pAddr, u32 u32NewSize, u32 u32Align = MEM_ADDR_ALIGN);
bool		memFree(void *pAddr);
bool		memCouldAlloc(u32 u32Size, u32 u32Align = MEM_ADDR_ALIGN, bool canFreeRamToDoIt = true);

// Debug Stuff
void     memShowAllocationInfo(void* alloc, char* msg);
#ifdef MEM_DEBUG_BLOCKS
void		memShowRegionFreeSize(t_Handle Handle);
void		memShowRegionAllocSize(t_Handle Handle);
void		memShowRegionInfo( t_Handle Handle);
void		memShowRegionBriefInfo( t_Handle Handle);
void		memCheckHeapUsage( t_Handle Handle, u32 *LayersUsage, u32 *LayersTotal, u32 *CHeapUsage, u32 *CHeapTotal, u32 *u32TotalHandles);
void		memSetUseBit(const u16 u16Bit);
void		memClearUseBit(const u16 u16Bit);
int		memQueryBlockBit(const u16 u16Bit, const char *pBlockName = NULL, int *pnHandleCount = NULL, const bool bMatchAllBits = false, const bool bDetail = false, FileStream *pLogFile = NULL);
void		memClearBlockBit(const u16 u16Bit, const char *pBlockName = NULL);
#else
#define memShowRegionInfo(Handle)				 ((void)0)
#define memCheckHeapUsage(handle,layersusage,layerstotal,cheapusage,cheaptotal,handles)		 ((void)0)
#define memSetUseBit(u16Bit)				 ((void)0)
#define memClearUseBit(u16Bit)			 ((void)0)
#define memQueryBlockBit(u16Bit)	 ((int)0)
#define memClearBlockBit(u16Bit)	 ((void)0)
#endif //MEM_DEBUG_BLOCKS

void		memGetRegionStats(t_Handle Handle, int *pnFreeBytes, int *pnFreeHandles, int *pnLargestFree, int *pnAllocBytes, int *pnAllocHandles);
int		memGetRegionLength(t_Handle Handle);

#ifndef CONSUMER_BUILD
/* simple access, needed for CDROM build, because it supports the debug menu */
void		memTrackHighWater(t_Handle Handle, bool bState);
bool		memHighWaterOn(t_Handle Handle);
void		memResetHighWater(t_Handle Handle);
void		memGetHighWater(t_Handle Handle, int *pnMinBytes, int *pnMaxBytes);
#endif //CONSUMER_BUILD

#ifdef MALLOC_STATS
#ifndef MEM_SEPARATE_HEADERS
#error MALLOC_STATS not supported by !MEM_SEPARATE_HEADERS
#endif
void		MallocStats__Clear(void);
void		MallocStats__Report(void);
#endif //MALLOC_STATS

void		memMarkPermanentMemory(t_Handle RegionHandle);
void		memMarkBufferPermanent(void *pAddr);

// TJC - check for memory leaks (report and free them).
// TJC - if you call memIgnoreFrees(true), then this will set it back
// to false, as well as free your stuff, but it wont report leaks.
bool		memCheckPermanentMemory(t_Handle RegionHandle);

#endif //__L_MEMORY_H
