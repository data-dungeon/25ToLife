//
// MasterList.cpp
// Copyright (C) 2001 by Avalanche Software, All Rights Reserved
//
// Author: Tyler Colbert
// Creation Date: Nov 7, 2001
// 
// quite a bit of code taken from old version of MasterList by Ned Martin
//

// The usage of this module has changed extensively since the old version.
// Now you call MasterList__Start() to begin a frame, then, at the end
// of a frame, call MasterList__End().  Start of the display lists
// is handled automatically.  MasterList expects an externally defined
// function called void gfxSwapBuffers(void) to be defined to accomplish
// the frame buffer swap.


/******************* defines *****************************/

//#define SHOW_STATS
//#define SHOW_MSG
#define RENDER_WHILE_GENERATING
//#define IMMEDIATE_DMA_STALL
//#define DETECT_MASTERLIST_ERRORS
//#define ADDITIONAL_ASSERTS

// size in qw of packet at end of display list...
#define MASTERLIST_PATH3FLUSHPACKET_SIZE	25

#define MASTERLIST_CHUNK_FREE_RATE 0.01f
#define MASTERLIST_MINFREECHUNK_TARGET 2.0f
#define MASTERLIST_MAX_FREE_CHUNKS_BEFORE_FASTFREE 100

// GIF_MODE register values
#define GIF_PATH3_MASK		(1 << 0)
#define GIF_IMT_MODE			(1 << 2)

// alignment requirement of masterlist chunks
#define MASTERLIST_CHUNK_ALIGNMENT 128

// hardcoded PA grab on DMAC report...
//#define PAGRAB_ON_DMAC_REPORT
//#define PAGRAB_ON_VU1_DBIT

// hardcoded breakpoint on dma report (comment out to disable)
#define BREAK_ON_DMAC_REPORT

#ifdef SHOW_MSG
#define SHOW_START_END
#define SHOW_INIT_CLEANUP_RESET
#define SHOW_DMASTART
#define SHOW_DMAEND
#define SHOW_SWAP
#define SHOW_NOSTART
//#define SHOW_FLUSHCONDITIONAL
//#define SHOW_STALLCONDITIONAL
//#define SHOW_FREECONDITIONAL
#define SHOW_COMPLETECHUNKS
#define SHOW_WARNINGS
#endif

#if defined(PAGRAB_ON_DMAC_REPORT) || defined(PAGRAB_ON_VU1_DBIT)
#define PAGRAB_ENABLED
#endif



/******************** headers ***************************/
#define IN_MASTERLIST_CPP
#include "CompilerControl.h"
#include "MasterList.h"
#include "PS2Renderer.h"
#include "VUDataClass.h"
#include "../Common/GIFTag.h"
#include "../Common/VIFInline.h"
#include "../Common/GIFInline.h"
#include "../../../../Layers/exception/exception.h"

/* ick ick ick this is for a hack.. ick */
#include "DDMAObject.h"


using namespace VIFInline;
using namespace GIFInline;

/* system includes */

#include <eekernel.h>
#include <eeregs.h>
#include <stdio.h>
#include <string.h>
#include <libgraph.h>

//#ifndef SHIP_IT
#include <libdma.h>
#include <devvif0.h>
#include <devvif1.h>
#include <devvu0.h>
#include <devvu1.h>
#include <devgif.h>
//#endif

#if defined(SHOW_STATS) || defined(SHOW_MSG)
#include "ttyColors.h"
#endif

#ifdef HASH_MASTERLIST_CHUNKS
// Always, these kinds of include paths eventually come back to haunt one...
#include "../../../../../GameLib/EngineHelper/Hash.h"
#endif

// This code should NEVER have any engine includes in it...
// That way, the renderer & masterlist can be ripped out
// and used in other games like ports and stuff...

// however, the following external function must be defined.
extern "C" {
extern void gfxSwapBuffers(int bufferidx);
}

namespace MasterList {


/******************* macros ******************************/

#define UNCACHED_CHUNK(ptr) ((MasterListChunk*)(((u_int)ptr) | 0x20000000))
#define CACHED_CHUNK(ptr) ((MasterListChunk*)(((u_int)ptr) & 0x0FFFFFFF))

#ifdef SHOW_STATS
#define STATS() \
	scePrintf(TTY_BLUE "Cycles: %d  Times:%d" TTY_RESET "\n", gMostRecentCycleCount, gMostRecentDMAsThisFrame)
#else
#define STATS() do{}while(0)
#endif

#define DisableInterrupts() if(!gInterruptsAreDisabledExternally) { DBGASSERT(!gInterruptsAreDisabled); DI(); gInterruptsAreDisabled = true; }
#define EnableInterrupts() if(!gInterruptsAreDisabledExternally) { DBGASSERT(gInterruptsAreDisabled); gInterruptsAreDisabled = false; EI(); }


#ifdef SHOW_MSG
#define CMSG(color, msg) \
	scePrintf(color "%s (G%d R%d D%d : F%d R%d N%d R%d A%d)" TTY_RESET "\n", msg,\
			gFrameBeingGenerated, gFrameBeingRendered, gFrameBeingDisplayed, \
			gCurrFreeChunks, gCurrReadyChunks, gCurrNextFrameChunks, gCurrRenderingChunks, gCurrAllocedChunks)
#define MSG(msg) \
	scePrintf(TTY_RED "%s (G%d R%d D%d)" TTY_RESET "\n", msg, gFrameBeingGenerated, gFrameBeingRendered, gFrameBeingDisplayed)
#define DUMPLISTHEADS(color, msg) \
	scePrintf(color "%s (Rendering:%x Ready:%x Next:%x)" TTY_RESET "\n", msg, gRenderingChunks, gReadyChunks, gNextFrameChunks);
#else
#define CMSG(color, msg) do{}while(0)
#define MSG(msg) do{}while(0)
#endif


/******************* structures/types ********************/

typedef bool ConditionalFunction(void);

/******************* global variables ********************/

// let the cpu make use of idle time
void (*gWaitFunc)(void) = NULL;

// used by flushed tracking functions
bool gCurrentlyFlushed;
u_int gBase;
u_int gOffset;

// current and minimum # of vblanks.
u_int gNumVBlanks = 0;
u_int gMinVBlanks = 0;
u_int gVBlanksLastFrame = 0;

// a flag to let us make sure that Start & Render are called alternatingly
// and to make sure that things arent added when its not in one.
int gMasterListActiveThread = -1;
bool bSuspended = false;

// flag to disable allocs & frees
bool gChunkListLocked = false;

// a flag to help detect misuse of the api
#ifdef _DEBUG
bool bInMiscBlock = false;
#endif

u_char* pPath3DisplayList = NULL;

// cycle counters for performance measuring
u_int gCurrentCycleCount;
u_int gMostRecentCycleCount;
u_int gMostRecentDMAWaitCycleCount;
u_int gMostRecentBlockWaitCycleCount;
u_int gCurrentDMAWaitCycleCount;
u_int gCurrentBlockWaitCycleCount;
u_int gCurrentDMAsThisFrame;
u_int gMostRecentDMAsThisFrame;
u_int gDMAStartTime;

// Swap callback
SwapBufferCallback gSwapBufferCallback = ::gfxSwapBuffers;

/******************* scratchpad vars *********************/
#ifdef _DEBUG
#define STATIC
#else
#define STATIC
//#define STATIC static
#endif

#ifdef RENDER_WHILE_GENERATING
SCRATCHPAD(u_int, gRenderWhileGenerating);
#endif

// frame number
volatile SCRATCHPAD(u_int, gFrameBeingGenerated);
volatile SCRATCHPAD(u_int, gFrameBeingRendered);
volatile SCRATCHPAD(u_int, gFrameBeingDisplayed);

SCRATCHPAD(u_long128, u128Path3FlushPacket[MASTERLIST_PATH3FLUSHPACKET_SIZE]) __attribute__ ((aligned(16))); // spot to build open/close gate for maskpath3 stuff

// two Chunks in SPR that are MASTERLIST_CHUNK_SIZE in size.
// yes, thats 128 byte alignment.
#ifdef USE_SPR_BUFFERS
STATIC SCRATCHPAD(MasterListChunk, gMasterListSPRChunks[2]) __attribute__((aligned(128)));
#else
SCRATCHPAD(MasterListChunk, gMasterListMiscData) __attribute__((aligned(128)));
#endif

// pointers to the two SPR chunks.
#ifdef USE_SPR_BUFFERS
SCRATCHPAD(u_int, gCurrentSPRChunk);
#endif
SCRATCHPAD(MasterListChunkPtr, gCurrentSPRStart);
SCRATCHPAD(u_char, *gCurrentSPRPos);
SCRATCHPAD(u_char, *gCurrentSPREnd);


// is a managed dma currently in progress?
STATIC bool gManagedDMA;

// here are all the mainram Chunks for displaylists
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gFreeChunks);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gRenderingChunks);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gRenderingChunksEnd);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gReadyChunks);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gReadyChunksEnd);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gNextFrameChunks);
volatile STATIC SCRATCHPAD(MasterListChunkPtr, gNextFrameChunksEnd); // the last of the nextFrameChunks

/******************* local variables *********************/

// callbacks
// this callback gets called (if its !NULL) when the GS finishes
STATIC MasterListCallback* gCallbackEndOfDMA = NULL;
// this callback gets called (if its !NULL) when a dmatag with the
// interrupt bit set COMPLETES executing.
STATIC MasterListCallback* gCallbackDmaInterrupt = NULL;
// callback for whenever a vblank happens
STATIC MasterListCallback* gCallbackVBlankInterrupt = NULL;

// interrupt handler ID
STATIC int gDmacHandlerID = -1;
STATIC int gVBlankHandlerID;

// have we uploaded the first image of a set?
STATIC bool gPath3Started;
// last gif tag with a false eop
STATIC u_char *pLastFalseEOPTextureUpload = NULL;

// accumulated chunks this frame
STATIC u_int gChunksThisFrame;
// current chunks allocated
STATIC u_int gCurrAllocedChunks;
// most recent chunks per frame
STATIC u_int gMostRecentChunksPerFrame;
// number of chunks in the free list
STATIC u_int gCurrFreeChunks;
// lowest value so far this frame for gCurrFreeChunks
STATIC u_int gMinFreeChunks;
// lowest value last frame for gCurrFreeChunks
STATIC u_int gMostRecentMinFreeChunks;
// number of chunks in the ready list
STATIC u_int gCurrReadyChunks;
// number of chunks in the rendering list
STATIC u_int gCurrRenderingChunks;
// number of chunks in the next frame list
STATIC u_int gCurrNextFrameChunks;
// momentum value used for freeing chunks
STATIC float gCurrChunkCountMomentum;

// have interrupts been disabled externally already
STATIC bool gInterruptsAreDisabled = false;
STATIC bool gInterruptsAreDisabledExternally = false;

// disable interrupts and disable flush requests...
STATIC bool gDMABlocked = false;

STATIC bool gDisplayVSynced = true;

/******************* local prototypes ********************/

static u_int BuildPath3GatePacket(void);

// OS Callback for channel 1 dma interrupt
static int DMAInterruptHandler(int channel);

static u_int WaitForDMACompletion(ConditionalFunction* conditionalFunction);
static int VSyncInterruptHandler( int n );
static void AppendTextureUpload(u_char *&list, void* pData, u_int address, u_int width, u_int height, u_int format, bool bInterrupt, bool eop);
static bool FlushConditional(void);
static bool StallConditional(void);
static bool DMACompleteOrInProgressConditional(void);
static bool FreeConditional(void);
static bool FreeChunk(void);
static void FreeChunkList();
static MasterListChunk* AllocateChunk(void);
static void CompleteChunk(u_int vif0);
static void StartRender(MasterListChunk* head, bool inInterrupt );
static void RemoveRenderingChunks(void);
static bool RenderNextFrameChunks( bool inInterrupt );
static bool RenderReadyChunks( bool inInterrupt );
static void HandleDMAEnd(bool inInterrupt);
static void HandleSwap();
static void AddChunkToFreeList( MasterListChunk* singleChunk );
static MasterListChunk* ReserveChunk( void );

#ifdef PAGRAB_ENABLED
extern void HWBreak(void);
void SetHWBreakpointTrigger (void);
#endif

// TJC - icky icky icky...
void DeleteAfter2Frames(DDMAObject* obj);
static void DeleteAfter2Frames(void);

#ifdef DETECT_MASTERLIST_ERRORS
static void ScanMasterListForErrors(MasterListChunk* head);
#endif

static void DelayThread(u_int time);

/******************* functions ***************************/

//////////////////////////////////////////////////////////////////////////////

/* initialize master display list stuff */

void Init(void)
{
	u_int nQWC;

	// TJC - if this fires, youve called masterlist init twice in a row.
//#ifndef SHIP_IT
	if(IsInited())
	{
		printf("MasterList::Init called while MasterList already Init'ed\n");
		printf("Probably because of an earlier assert initializing it...\n");
		//ASSERT(!IsInited());
		return;
	}
//#endif

	// verify that we were shutdown correctly
	DBGASSERT(gFreeChunks == NULL);
	DBGASSERT(gRenderingChunks == NULL);
	DBGASSERT(gRenderingChunksEnd == NULL);
	DBGASSERT(gReadyChunks == NULL);
	DBGASSERT(gReadyChunksEnd == NULL);
	DBGASSERT(gNextFrameChunks == NULL);
	DBGASSERT(gNextFrameChunksEnd == NULL);

	// verify alignment issues
#ifdef USE_SPR_BUFFERS
	DBGASSERT(((u_int)&gMasterListSPRChunks[0] % 128) == 0);
	DBGASSERT(((u_int)&gMasterListSPRChunks[1] % 128) == 0);
#endif
	DBGASSERT(sizeof(MasterListChunk) == MASTERLIST_CHUNK_SIZE);
	DBGASSERT((MASTERLIST_CHUNK_SIZE % 128) == 0);

	// initialize the chunk counters
	gCurrAllocedChunks = 0;
	gCurrReadyChunks = 0;
	gCurrNextFrameChunks = 0;
	gCurrRenderingChunks = 0;
	gCurrFreeChunks = 0;
	gCurrChunkCountMomentum = 0.0f;
	gMostRecentMinFreeChunks = 0;

	// we jumpstart these..(its a little funny, but it works)
	gFrameBeingDisplayed	= 0;
	gFrameBeingRendered	= 0;
	gFrameBeingGenerated	= 0;

	// for now we force this during init.
#ifdef RENDER_WHILE_GENERATING
	gRenderWhileGenerating = true;
#endif

	//
	// these routines build the static packets in SPR that we dma into the display list.
	//

	// build path3 packet used to open/close the path3 gate
	nQWC = BuildPath3GatePacket();
	DBGASSERT(nQWC == MASTERLIST_PATH3FLUSHPACKET_SIZE);

	// start without an upload in progress
	gPath3Started = false;

	// dmas arent ours by default
	gManagedDMA = false;

	gDisplayVSynced = true;

	//
	// start the interrupt handler
	//

	gDmacHandlerID = AddDmacHandler(DMAC_VIF1, &DMAInterruptHandler, 0);
	if(gDmacHandlerID == -1)
	{
		DBGASSERT(!"ERROR! Could not register DMAC_VIF1 handler\n");
	}
	EnableDmac(DMAC_VIF1);

	DisableIntc(INTC_VBLANK_S);
	gVBlankHandlerID = AddIntcHandler(INTC_VBLANK_S, VSyncInterruptHandler, 0);
	EnableIntc(INTC_VBLANK_S);

#ifdef SHOW_INIT_CLEANUP_RESET
	DisableInterrupts();
	CMSG(TTY_RED, "ML::Init");
	EnableInterrupts();
#endif // SHOW_MSG

#ifdef USE_SPR_BUFFERS
	gMasterListSPRChunks[0].pNext = NULL;
	gMasterListSPRChunks[0].mVIFCode0 = createNOP();
	gMasterListSPRChunks[0].mVIFCode1 = createNOP();
	gMasterListSPRChunks[1].pNext = NULL;
	gMasterListSPRChunks[1].mVIFCode0 = createNOP();
	gMasterListSPRChunks[1].mVIFCode1 = createNOP();
#endif

#ifdef PAGRAB_ENABLED
	SetHWBreakpointTrigger();
#endif
}

//////////////////////////////////////////////////////////////////////////////

bool IsInited(void)
{
	return gDmacHandlerID != -1;
}

//////////////////////////////////////////////////////////////////////////////

static bool FlushConditional(void)
{
#ifdef SHOW_FLUSHCONDITIONAL
	DisableInterrupts();
	DUMPLISTHEADS(TTY_RED, "FlushConditional");
	EnableInterrupts();
#endif

	// wait for dmas to complete...
	DisableInterrupts();
	bool keepWaiting = 0
		|| gReadyChunks 
		|| gRenderingChunks
#ifdef RENDER_WHILE_GENERATING
		|| gNextFrameChunks
#endif
		;
	EnableInterrupts();
	return keepWaiting;
}

//////////////////////////////////////////////////////////////////////////////

static bool StallConditional(void)
{
#ifdef SHOW_STALLCONDITIONAL
	DisableInterrupts();
	CMSG(TTY_RED, "StallConditional");
	EnableInterrupts();
#endif

	// wait for dmas to complete...
	DisableInterrupts();
	bool keepWaiting = gFrameBeingRendered < gFrameBeingGenerated;
	EnableInterrupts();
	return keepWaiting;
}

//////////////////////////////////////////////////////////////////////////////

static bool FreeConditional(void)
{
#ifdef SHOW_FREECONDITIONAL
	DisableInterrupts();
	DUMPLISTHEADS(TTY_RED, "FreeConditional");
	EnableInterrupts();
#endif

	// wait for dmas to complete...
	DisableInterrupts();
	bool keepWaiting = gFreeChunks == NULL && ( gRenderingChunks != NULL || gReadyChunks != NULL || gNextFrameChunks != NULL);
	EnableInterrupts();
	return keepWaiting;
}

//////////////////////////////////////////////////////////////////////////////

static bool DMACompleteOrInProgressConditional(void)
{
	DisableInterrupts();
	bool keepWaiting = gReadyChunks != NULL || gNextFrameChunks != NULL;
	EnableInterrupts();
	return keepWaiting;
}

//////////////////////////////////////////////////////////////////////////////

void InterruptsAreDisabled(bool really)
{
	gInterruptsAreDisabledExternally = really;
}

//////////////////////////////////////////////////////////////////////////////

bool IsDMABlocked(void)
{
	return gDMABlocked;
}

//////////////////////////////////////////////////////////////////////////////

void BlockDMA(bool really)
{
	// TJC - we need to disable interrupts in such a way that there is
	// no pending interrupt possible for masterlist.  thats what this
	// little piece of tom foolery does..
	if(really)
	{
		do {
			EI();
			DMA_Wait(D1_CHCR);
			DI();
		} while(((*( (volatile u_int *) D1_CHCR)) & D_CHCR_STR_M) == D_CHCR_STR_M || gReadyChunks || gNextFrameChunks || gRenderingChunks );
#ifdef SHOW_WARNINGS
		CMSG(TTY_RED, "BlockON");
#endif
	}

	gDMABlocked = really;

	if(!really)
	{
		DMA_Wait(D1_CHCR);
#ifdef SHOW_WARNINGS
		CMSG(TTY_RED, "BlockOFF");
#endif
		EI();
	}
}

//////////////////////////////////////////////////////////////////////////////

void Flush(void)
{
	if(gDMABlocked)
	{
#ifdef SHOW_WARNINGS
		CMSG(TTY_RED, "Flush while DMABlocked");
#endif
		return;
	}


	gCurrentDMAWaitCycleCount += WaitForDMACompletion(FlushConditional);

	// TJC -  make sure all objects are freed...
	DeleteAfter2Frames();
	DeleteAfter2Frames();
	DeleteAfter2Frames();
}

//////////////////////////////////////////////////////////////////////////////

void LockChunkList(int count /* = -1 */ )
{
	Flush();

	DBGASSERT( gChunkListLocked == false );
	if ( count != -1 && count != (int)gCurrAllocedChunks)
	{
		int diff = count - gCurrAllocedChunks;

		while ( diff > 0 )
		{
			DisableInterrupts();
			AddChunkToFreeList( ReserveChunk() );
			EnableInterrupts();
			diff--;
		}

		while ( diff < 0 )
		{
			ASSERT ( gFreeChunks != NULL );

			DisableInterrupts();
			FreeChunk();
			EnableInterrupts();
			diff++;
		}
	}

	gChunkListLocked = true;
#ifdef RENDER_WHILE_GENERATING
//	gRenderWhileGenerating = false;
#endif
}

//////////////////////////////////////////////////////////////////////////////

void UnlockChunkList(void)
{
	DBGASSERT( gChunkListLocked == true );
	gChunkListLocked = false;
#ifdef RENDER_WHILE_GENERATING
//	gRenderWhileGenerating = true;
#endif
}

//////////////////////////////////////////////////////////////////////////////

static bool FreeChunk(void)
{
	DBGASSERT( gInterruptsAreDisabled );

	if(!gFreeChunks)
		return false;
	MasterListChunk* chunkToFree = gFreeChunks;
	MasterListChunk* ucChunk = UNCACHED_CHUNK( gFreeChunks );
	gFreeChunks = ucChunk->pNext;

	gCurrFreeChunks--;
	gCurrAllocedChunks--;
	if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
		scePrintf("f%d r%d n%d r%d == %d\n",  gCurrFreeChunks, gCurrRenderingChunks, gCurrNextFrameChunks, gCurrReadyChunks, gCurrAllocedChunks);
	DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);

	EnableInterrupts();
	PS2RendererFree(chunkToFree);
	DisableInterrupts();

	return true;
}

//////////////////////////////////////////////////////////////////////////////

static void FreeChunkList()
{
	while ( gFreeChunks )
	{
		FreeChunk();
	}
}

//////////////////////////////////////////////////////////////////////////////

void DisableInterruptHandling()
{
	if (gDmacHandlerID != -1)
	{
		DisableDmac(DMAC_VIF1);
		RemoveDmacHandler(DMAC_VIF1, gDmacHandlerID);
		gDmacHandlerID = -1;
	}

	if (gVBlankHandlerID != -1)
	{
		DisableIntc(INTC_VBLANK_S);
		RemoveIntcHandler(INTC_VBLANK_S, gVBlankHandlerID);
		gVBlankHandlerID = -1;
	}
}

//////////////////////////////////////////////////////////////////////////////

void Cleanup()
{
	// TJC - if this fires, youve called masterlist cleanup without
	// first calling init.
//#ifndef SHIP_IT
	if(!IsInited())
	{
		printf("MasterList::Cleanup called while MasterList not Init'ed\n");
		printf("Probably some interaction with the onscreen assert handler\n");
		return;
	}
//#endif

	// cant call between Start && End
	DBGASSERT(!CurrentlyActive());

#ifdef SHOW_INIT_CLEANUP_RESET
	DisableInterrupts();
	DUMPLISTHEADS(TTY_RED, "ML::Cleanup");
	EnableInterrupts();
#endif // SHOW_MSG

	// wait for dma completion
	Flush();

	// disable the interrupt handlers
	DisableInterruptHandling();

	// run through and free all the master list blocks...
	DisableInterrupts();
	FreeChunkList();
	EnableInterrupts();
	DBGASSERT( gCurrAllocedChunks == 0 );
	DBGASSERT( gCurrFreeChunks == 0 );
	DBGASSERT( gFreeChunks == NULL );
	DBGASSERT( gCurrRenderingChunks == 0 );
	DBGASSERT( gCurrReadyChunks == 0 );
	DBGASSERT( gCurrNextFrameChunks == 0 );
	gCurrAllocedChunks = 0;
	gCurrFreeChunks = 0;
	gFreeChunks = NULL ;
	gCurrRenderingChunks = 0 ;
	gCurrReadyChunks = 0 ;
	gCurrNextFrameChunks = 0;
	gCurrChunkCountMomentum = 0.0f;
	gMostRecentMinFreeChunks = 0;
}

//////////////////////////////////////////////////////////////////////////////

// drain out any list built so far and start it over again...
void Reset(void)
{
#ifdef SHOW_INIT_CLEANUP_RESET
	DisableInterrupts();
	DUMPLISTHEADS(TTY_RED, "ML::Reset");
	EnableInterrupts();
#endif

	if ( IsDMABlocked() )
	{
		#ifndef SHIPIT
		scePrintf("FORCING MasterList::BlockDMA(false)!  This is bad (unless its due to an assert)\n");
		#endif
		BlockDMA( false );
	}

	// to make asserts work, this has to be handled
	//ASSERT( gChunkListLocked == false );
	if ( gChunkListLocked )
	{
		UnlockChunkList( );
	}


	Flush();

	// musnt be interrupted...
	DisableInterrupts();

	if(gReadyChunks)
	{
		ASSERT(false);
		MasterListChunk* ucChunk = UNCACHED_CHUNK(gReadyChunksEnd);
		ucChunk->pNext = gFreeChunks;
		gFreeChunks = gReadyChunks;
		gReadyChunks = NULL;
		gReadyChunksEnd = NULL;
		gCurrFreeChunks += gCurrReadyChunks;
		gCurrReadyChunks = 0;
		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong C" );
		ASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
	}
	if(gNextFrameChunks)
	{
		ASSERT(false);
		MasterListChunk* ucChunk = UNCACHED_CHUNK(gNextFrameChunksEnd);
		ucChunk->pNext = gFreeChunks;
		gFreeChunks = gNextFrameChunks;
		gNextFrameChunks = NULL;
		gNextFrameChunksEnd = NULL;
		gCurrFreeChunks += gCurrNextFrameChunks;
		gCurrNextFrameChunks = 0;
		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong B" );
		ASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
	}
	if(gRenderingChunks)
	{
		ASSERT(false);
		MasterListChunk* ucChunk = UNCACHED_CHUNK(gRenderingChunks);
		ucChunk->pNext = gFreeChunks;
		gFreeChunks = gRenderingChunks;
		gRenderingChunks = NULL;
		gRenderingChunksEnd = NULL;
		gCurrFreeChunks += gCurrRenderingChunks;
		gCurrRenderingChunks = 0;
		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong A" );
		ASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
	}



	// free our chunks...
	FreeChunkList();
	DBGASSERT( gCurrAllocedChunks == 0 );
	DBGASSERT( gCurrFreeChunks == 0 );
	DBGASSERT( gFreeChunks == NULL );
	DBGASSERT( gCurrRenderingChunks == 0 );
	DBGASSERT( gCurrReadyChunks == 0 );
	DBGASSERT( gCurrNextFrameChunks == 0 );
	gCurrAllocedChunks = 0;
	gCurrFreeChunks = 0;
	gFreeChunks = NULL ;
	gCurrRenderingChunks = 0 ;
	gCurrReadyChunks = 0 ;
	gCurrNextFrameChunks = 0;
	gCurrChunkCountMomentum = 0.0f;
	gMostRecentMinFreeChunks = 0;

	// we jumpstart these..(its a little funny, but it works)
	gFrameBeingDisplayed	= 0;
	gFrameBeingRendered	= 0;
	gFrameBeingGenerated	= 0;

	EnableInterrupts();

	// mark that the vector unit is idle...
	SetFlushed(true);
	SetBase(0xFFFFFFFF);
	SetOffset(0xFFFFFFFF);

	gMasterListActiveThread = -1;
}

//////////////////////////////////////////////////////////////////////////////

// prepare master list for building enormous display list for everything that needs to be rendered
void Start(void)
{
	// we must not currently be between a Start() & End()
	DBGASSERT(!CurrentlyActiveAtAll());

	while(gDMABlocked == true)
	{
//		DelayThread( 3000 );
	}

	if(gCurrentSPRStart == NULL)
	{
#ifdef USE_SPR_BUFFERS
		gCurrentSPRStart = &gMasterListSPRChunks[gCurrentSPRChunk];
#else
		DisableInterrupts();
		gCurrentSPRStart = UNCACHED_CHUNK(AllocateChunk());
		EnableInterrupts();
#endif
		gCurrentSPRPos = (u_char*)gCurrentSPRStart;
		gCurrentSPREnd = (u_char*)&gCurrentSPRStart->mTag;
	}

	// mark that the vector unit is idle...
	SetFlushed(true);
	SetBase(0xFFFFFFFF);
	SetOffset(0xFFFFFFFF);

	// stall for the DMA to catch up...
	// this keeps the generation from getting more than
	// one frame ahead of the rendering..
	gCurrentDMAWaitCycleCount += WaitForDMACompletion(StallConditional);

	STATS();

	// we have completed the generation of this frame...
	DisableInterrupts();

	// make asserts work...
	gMasterListActiveThread = GetThreadId();

#ifdef SHOW_START_END
	CMSG(TTY_YELLOW, "ML::Start");
#endif

	gFrameBeingGenerated++;

	gMostRecentChunksPerFrame = gChunksThisFrame;
	gChunksThisFrame = 0;
	gMostRecentMinFreeChunks = gMinFreeChunks;
	gMinFreeChunks = gCurrFreeChunks;

#ifdef ADDITIONAL_ASSERTS
	DBGASSERT(gReadyChunks == NULL);
#endif

	if(gNextFrameChunks == NULL)
	{
		if(gFrameBeingRendered < gFrameBeingGenerated && gRenderingChunks == NULL)
		{
			// rendering of the last frame has already completed...
			gFrameBeingRendered++;
#ifdef SHOW_WARNINGS
			CMSG(TTY_RED, "Frame already complete");
#endif
		}
	}
	else
	{
		gReadyChunks = gNextFrameChunks;
		gReadyChunksEnd = gNextFrameChunksEnd;
		gNextFrameChunks = NULL;
		gNextFrameChunksEnd = NULL;
		gCurrReadyChunks = gCurrNextFrameChunks;
		gCurrNextFrameChunks = 0;
	}

#if 0
	if(
		// are we currently rendering?
		(!gRenderingChunks) &&
		// is it safe to start rendering?
		gFrameBeingDisplayed+1 == gFrameBeingRendered &&
		// is there anything left of the prior frame?
		(!RenderReadyChunks( false )))
	{
		// go ahead and start this frame up...
		RenderNextFrameChunks( false );
	}
#endif

	EnableInterrupts();
#ifdef __SNTUNER_H_INC__
	char buff[8] = "Gen";
	sprintf(&buff[3], "%d", gFrameBeingGenerated);
	snStartMarker(0, buff);
#endif
}

//////////////////////////////////////////////////////////////////////////////

// finishes the current display list, waits for prior one to complete
// and returns the time (in cpu cycles) that it waited.
u_int End(void)
{
	// we must currently be between a Start() & End()
	DBGASSERT(CurrentlyActive());

	while(gDMABlocked == true)
	{
//		DelayThread( 3000 );
	}

	// Complete the current tag
	CompleteChunk(createFLUSHA());

	DisableInterrupts();
#ifdef SHOW_START_END
	CMSG(TTY_YELLOW, "ML::End");
#endif

	if(
		// are we currently rendering?
		(!gRenderingChunks) &&
		// is it safe to start rendering?
		gFrameBeingDisplayed+1 == gFrameBeingRendered &&
		// is there anything left of the prior frame?
		(!RenderReadyChunks( false )))
	{
		// go ahead and start this frame up...
		RenderNextFrameChunks( false );
	}

	// make asserts work...
	gMasterListActiveThread = -1;

	// TJC - process chunk momentum.
	gCurrChunkCountMomentum -= gMostRecentMinFreeChunks * MASTERLIST_CHUNK_FREE_RATE;
	if(gCurrChunkCountMomentum < 0.0f)
		gCurrChunkCountMomentum = 0.0f;

	// bring us back down to reality.
	if(gCurrChunkCountMomentum > MASTERLIST_MAX_FREE_CHUNKS_BEFORE_FASTFREE)
		gCurrChunkCountMomentum = gMostRecentMinFreeChunks - (MASTERLIST_MINFREECHUNK_TARGET + 1);

	if(gCurrChunkCountMomentum+MASTERLIST_MINFREECHUNK_TARGET < gMostRecentMinFreeChunks &&
		gFreeChunks && 
		!gChunkListLocked )
	{
		FreeChunk();
		gCurrChunkCountMomentum = gMostRecentMinFreeChunks;
	}

	EnableInterrupts();

#ifdef __SNTUNER_H_INC__
	snStopMarker(0);
#endif

	// TJC - every frame we need to delete stuff thats old
	// enough.. this doesnt really belong in here.. but its in here for
	// now...
	DeleteAfter2Frames();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

static void AddChunkToFreeList( MasterListChunk* singleChunk )
{
	DBGASSERT( gInterruptsAreDisabled );

	if ( singleChunk == NULL )
		return;

	MasterListChunk* ucChunk = UNCACHED_CHUNK( singleChunk );

	ucChunk->pNext = gFreeChunks;
	gFreeChunks = singleChunk;
	gCurrFreeChunks++;
	if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
		CMSG( TTY_RED, "count wrong D" );
	DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
}

//////////////////////////////////////////////////////////////////////////////

static MasterListChunk* ReserveChunk( void )
{
	DBGASSERT( gInterruptsAreDisabled );

	EnableInterrupts();
	MasterListChunk* newChunk = (MasterListChunk*)PS2RendererMemAlign(MASTERLIST_CHUNK_ALIGNMENT, MASTERLIST_CHUNK_SIZE);
	DisableInterrupts();
	if ( !newChunk )
		return NULL;

	gCurrAllocedChunks++;

	// verify cache restrictions & alignment
	DBGASSERT(((u_int)newChunk & 0x0000007F) == 0);
	DBGASSERT(((u_int)MASTERLIST_CHUNK_SIZE & 0x0000007F) == 0);

	// invalidate the memory block...
	InvalidDCache(newChunk, &newChunk->mVIFCode1);

	// switch to uncached pointer
	return newChunk;
}

//////////////////////////////////////////////////////////////////////////////

static MasterListChunk* AllocateChunk(void)
{
	DBGASSERT( gInterruptsAreDisabled );

	MasterListChunk* newChunk = NULL;

	if(!gFreeChunks && 
		!gChunkListLocked)
	{
		EnableInterrupts();
		bool couldAlloc = PS2RendererCouldAlloc(MASTERLIST_CHUNK_ALIGNMENT, MASTERLIST_CHUNK_SIZE);
		DisableInterrupts();
		if (couldAlloc)
		{
			MasterListChunk* chunk = ReserveChunk();
			return chunk;
		}
	}

	// WOW!, a case where 'out of memory' is not fatal!!!!

	// double check that we even need to stall.. might have gotten one while
	// running the if() above
	if(!gFreeChunks)
	{
		EnableInterrupts();
		// we just stall here until something becomes available..
		gCurrentBlockWaitCycleCount += WaitForDMACompletion(FreeConditional);
		DisableInterrupts();
	}

	// out of memory!
	ASSERT( gFreeChunks );

#if !defined(DISABLE_EXCEPTIONS)
	if (!gFreeChunks)
	{
		// fatal: out of ram! give some info to the exception screen
		char buf[256];
		sprintf(buf, "OUT OF MEMORY, WHILE TRYING TO ALLOC CHUNK\n"
			          " chunks: free=%d rend=%d next=%d ready=%d alloc=%d\n",
						 gCurrFreeChunks, gCurrRenderingChunks, gCurrNextFrameChunks, gCurrReadyChunks, gCurrAllocedChunks);
		ExceptionHandler::SetExceptionMessage(buf);
		*(volatile int*)1 = 0;
	}
#endif

	// that'll do for the caller :)
	// grab a buffer
	newChunk = gFreeChunks;
	gFreeChunks = UNCACHED_CHUNK( newChunk )->pNext;
	gCurrFreeChunks--;

	return newChunk;
}

//////////////////////////////////////////////////////////////////////////////

void DisplayVSynced(bool enabled)
{
	gDisplayVSynced = enabled;
}

//////////////////////////////////////////////////////////////////////////////

bool IsDisplayVSynced()
{
	return gDisplayVSynced;
}

//////////////////////////////////////////////////////////////////////////////

void StallTillAllDMAIsCompleteOrInProgress()
{
	WaitForDMACompletion( DMACompleteOrInProgressConditional );
}

//////////////////////////////////////////////////////////////////////////////

void SPRFlush(void)
{
	DBGASSERT(((int)gCurrentSPRPos % 16) == 0);

	if(gCurrentSPRPos == (u_char*)gCurrentSPRStart)
		return;

	CompleteChunk(createNOP());
}

//////////////////////////////////////////////////////////////////////////////

#ifdef DETECT_MASTERLIST_ERRORS
static void ScanMasterListForErrors(MasterListChunk* head)
{
	struct TAG
	{
		struct
		{
			unsigned mQWC : 16;
			unsigned mUnused : 10;
			unsigned mPCD : 2;
			unsigned mID : 3;
			unsigned mIRQ : 1;
		};
		union
		{
			u_int mAddr;
			TAG* mNext;
		};
		u_int mVIFCode0;
		u_int mVIFCode1;
	};

	TAG* ASR[2];
	u_int ASP = 0;

	TAG* next = (TAG*) UNCACHED_CHUNK(head);
	TAG* curr;

	do
	{
		curr = next;
		if(
			curr->mID == TAGID_REF ||
			curr->mID == TAGID_REFS ||
			curr->mID == TAGID_REFE ||
			curr->mID == TAGID_CALL ||
			curr->mID == TAGID_NEXT)
		{
			if(curr->mAddr & 0x80000000)
			{
				if(curr->mAddr & 0x7FFFC000)
				{
					printf("Address out of range\n");
					DBGASSERT(false);
				}
			}
			else if((curr->mAddr & 0x78000000) != 0)
			{
				printf("Address out of range\n");
				DBGASSERT(false);
			}

			if((curr->mAddr & 0x0000000F) != 0)
			{
				printf("address unaligned\n");
				DBGASSERT(false);
			}
		}

		switch(curr->mID)
		{
		case TAGID_RET:
			if(ASP == 0)
			{
				printf("Appended too many RET tags\n");
				DBGASSERT(false);
			}
			else
				next = ASR[--ASP];
			break;
		case TAGID_CNT:
			next = curr + curr->mQWC + 1;
			break;
		case TAGID_NEXT:
			next = curr->mNext;
			break;
		case TAGID_CALL:
			next = curr->mNext;
			ASR[ASP++] = curr + curr->mQWC + 1;
			break;
		case TAGID_REF:
			next = curr + 1;
			break;
		case TAGID_REFS:
			next = curr + 1;
			printf("REFS tag found\n");
			DBGASSERT(false);
			break;
		case TAGID_REFE:
			printf("Appended a REFE!\n");
			DBGASSERT(false);
			break;
		}
		next = (TAG*)UNCACHED_CHUNK(next);
	} while
		(!(curr->mID == TAGID_REFE ||
		curr->mID == TAGID_END ||
		(curr->mID == TAGID_RET && ASP == 0)));
}
#endif

//////////////////////////////////////////////////////////////////////////////

static void CompleteChunk(u_int vif0)
{
	gChunksThisFrame++;

	MasterListChunk* newChunk;
	DisableInterrupts();
#ifdef USE_SPR_BUFFERS
	newChunk = AllocateChunk();
#else
	newChunk = CACHED_CHUNK(gCurrentSPRStart);
#endif

	// this succeeds only if the buffer being finished is exactly full...
	if(gCurrentSPRPos == gCurrentSPREnd)
	{
#ifdef HASH_MASTERLIST_CHUNKS
		// this case should never happen in this mode... in fact, it MUST not ever happen..
		// we use the 2 VIF code spots to store a hash and a valid size instead.
		asm("break 1");
#endif
		gCurrentSPRStart->mTag = TAGID_END << 28;
		gCurrentSPRStart->mVIFCode0 = vif0;
		gCurrentSPRStart->mVIFCode1 = createNOP();

#ifdef USE_SPR_BUFFERS
		// copy it down..
		DMA_FROM_SPR(gCurrentSPRStart, newChunk, MASTERLIST_QW_CHUNK_SIZE);
#endif
	}
	else
	{
		u_int offset = gCurrentSPREnd - gCurrentSPRPos;

		// TJC - this should probably be the only case ever where we directly
		// append a dma tag to the masterlist without using AppendDMATAG().
		Append(createDMATAG(0, TAGID_END));
		Append(vif0);
		Append(createNOP());

#ifdef USE_SPR_BUFFERS
		// copy it down..
		u_int qwc = gCurrentSPRPos - (u_char*)gCurrentSPRStart;
		qwc >>= 4;

		DMA_FROM_SPR(gCurrentSPRStart, newChunk, qwc);
		DMA_Wait(D8_CHCR);
#endif

		// in this case, the mTag is set to 0 to indicate that the next
		// tag needs to go somewhere different than the end...
		MasterListChunk* ucChunk = UNCACHED_CHUNK(newChunk);
		ucChunk->mTag = 0;
		ucChunk->pNext = 0;
		ucChunk->mVIFCode0 = offset;

#ifdef HASH_MASTERLIST_CHUNKS
		// generate the hash...
		int size = MASTERLIST_CHUNK_SIZE - ucChunk->mVIFCode0 - 16;
		ASSERT( size > 0 );
		int hashresult = Hash::CalcChecksum(gCurrentSPRStart, size );
		//scePrintf("HashChunk: %x  %x  %x\n", newChunk, size, hashresult);
		ucChunk->mVIFCode1 = hashresult;
#endif // HASH_MASTERLIST_CHUNKS
	}

#if defined(ADDITIONAL_ASSERTS) && defined(USE_SPR_BUFFERS)
	{
		DMA_Wait(D8_CHCR);
	//	FlushCache(0);
		MasterListChunk* ucChunk = UNCACHED_CHUNK(newChunk);
		if(ucChunk->mTag == 0)
		{
			DBGASSERT(ucChunk->mVIFCode0 != 0);
		}
	}
#endif

#ifdef DETECT_MASTERLIST_ERRORS
	DMA_Wait(D8_CHCR);
	ScanMasterListForErrors(newChunk);
#endif

	// link it into the right place...
	if(gNextFrameChunks)
	{
		MasterListChunk* ucChunk = UNCACHED_CHUNK(gNextFrameChunksEnd);
		if(ucChunk->mTag != 0)
		{
			ucChunk->pNext = newChunk;
			ucChunk->mTag = TAGID_NEXT << 28;
		}
		else
		{
			ucChunk->pNext = newChunk;
			u_char* pos = (u_char*)&ucChunk->mTag;
			pos -= ucChunk->mVIFCode0;
			Append(pos, createDMATAG(0, TAGID_NEXT, newChunk));
		}
	}
	else
	{
		gNextFrameChunks = newChunk;
	}
	gNextFrameChunksEnd = newChunk;
	gCurrNextFrameChunks++;

#ifdef SHOW_COMPLETECHUNKS
	CMSG(TTY_BLUE, "completechunk");
	scePrintf("\t" TTY_BLUE "@ 0x%x" TTY_RESET "\n", newChunk);
#endif

	if(
		// if we are not rendering right now
		gRenderingChunks == NULL &&
		// is it safe to start rendering this frame (are we displaying the prior)
		gFrameBeingDisplayed+1 == gFrameBeingRendered)
	{
		if(
		// if there are no ready chunks
		(!RenderReadyChunks( false )) &&
		// if the dma is up to the render...
		gFrameBeingRendered == gFrameBeingGenerated)
		{
#ifdef RENDER_WHILE_GENERATING
			if(gRenderWhileGenerating)
			if(!RenderNextFrameChunks( false ))
#ifdef SHOW_NOSTART
				CMSG(TTY_PURPLE, "nostart");
#else
				;
#endif
#endif
		}
	}
#ifdef SHOW_NOSTART
	else
		CMSG(TTY_PURPLE, "nostart");
#endif
	if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
		CMSG( TTY_RED, "count wrong E" );
	DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);

#ifdef USE_SPR_BUFFERS
	gCurrentSPRChunk ^= 1;
	gCurrentSPRStart = &gMasterListSPRChunks[gCurrentSPRChunk];
#else
	gCurrentSPRStart = UNCACHED_CHUNK(AllocateChunk());
#endif

	EnableInterrupts();

	gCurrentSPRPos = (u_char*)gCurrentSPRStart;
	gCurrentSPREnd = (u_char*)&gCurrentSPRStart->mTag;

}

//////////////////////////////////////////////////////////////////////////////

static void StartRender(MasterListChunk* head, bool inInterrupt )
{
	DBGASSERT(!gDMABlocked);
	gManagedDMA = true;

	// this is nasty... there has to be a better way...
	// the sprite code refs data that is not out of
	// the cache by the time we get here... ick..
	if( inInterrupt ) iFlushCache(0);
	else FlushCache(0);

	// we have another dma this frame :)
	gCurrentDMAsThisFrame++;

	DBGASSERT((*D1_CHCR & 0x100) == 0);

	// send the new master display list.
	DPUT_D1_QWC((u_int)0);
	DPUT_D1_TADR((u_int) head);
	asm volatile ("sync.l"); // dont remove these
	asm volatile ("sync.p"); // dont remove these
#ifdef A_COMPILER_SN
	// turn on T bit breaks in VU1
	// jlb - turn on D bit breaks in VU1
	unsigned int fbrst;
	asm("cfc2 %0, $vi28" : "=r" (fbrst));
#ifdef _DEBUG
	fbrst |= 0x0C00;
#else
	fbrst |= 0x0800;
#endif
	asm("ctc2 %0, $vi28" : : "r" (fbrst));

	// for debugging, we want to stop right here!!
//	asm(".global MasterListStopPoint; MasterListStopPoint:");
#endif

#ifdef IMMEDIATE_DMA_STALL
	DPUT_D1_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M );
	DMA_Wait(D1_CHCR);
	HandleDMAEnd(inInterrupt);
#else
	DPUT_D1_CHCR(D_CHCR_DIR_M | D_CHCR_STR_M | D_CHCR_SCHAIN_MODE_M | D_CHCR_TTE_M | D_CHCR_TIE_M);
#endif

#ifdef __SNTUNER_H_INC__
	char buff[8] = "Dma";
	sprintf(&buff[3], "%d", gFrameBeingRendered);
	snStartMarker(1, buff);
#endif

	asm("mfc0 %0, $9" : "=r" (gDMAStartTime));
}

//////////////////////////////////////////////////////////////////////////////

static void RemoveRenderingChunks(void)
{
	if(gRenderingChunks)
	{
#ifdef HASH_MASTERLIST_CHUNKS
		// we need to walk the chunks that just completed rendering to check their hashes...
		{
			MasterListChunk* cChunk = gRenderingChunks;
			do
			{
				MasterListChunk* ucChunk = UNCACHED_CHUNK( cChunk );

				int size = MASTERLIST_CHUNK_SIZE - ucChunk->mVIFCode0 - 16;
				ASSERT( size > 0 );
				unsigned int hashresult = Hash::CalcChecksum( ucChunk, size );
				//scePrintf("ReHashChunk: %x  %x  %x == %x\n",ucChunk, size, ucChunk->mVIFCode1, hashresult);
				//ASSERT( ucChunk->mVIFCode1 == hashresult );
				if(ucChunk->mVIFCode1 != hashresult)
				{
					asm("break 1");
				}

				cChunk = ucChunk->pNext;
			} while ( cChunk );
		}
#endif


		MasterListChunk* ucChunk = UNCACHED_CHUNK(gRenderingChunksEnd);
		ucChunk->pNext = gFreeChunks;
		gFreeChunks = gRenderingChunks;
		gRenderingChunks = NULL;
		gRenderingChunksEnd = NULL;
		gCurrFreeChunks += gCurrRenderingChunks;
		gCurrRenderingChunks = 0;
		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong F" );
		DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
	}
}

//////////////////////////////////////////////////////////////////////////////

static bool RenderNextFrameChunks( bool inInterrupt )
{
#ifdef ADDITIONAL_ASSERTS
	DBGASSERT(gRenderingChunks == NULL);
#endif

	if(gNextFrameChunks != NULL)
	{
		// we need to make sure we have it all..
		DMA_Wait(D8_CHCR);

#ifdef DETECT_MASTERLIST_ERRORS
		ScanMasterListForErrors(gNextFrameChunks);
#endif

		gRenderingChunks = gNextFrameChunks;
		gRenderingChunksEnd = gNextFrameChunksEnd;
		gNextFrameChunks = NULL;
		gNextFrameChunksEnd = NULL;
		gCurrRenderingChunks = gCurrNextFrameChunks;
		gCurrNextFrameChunks = 0;
		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong G" );
		DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
#ifdef SHOW_DMASTART
		CMSG(TTY_GREEN, "start nf");
#endif
		StartRender(gRenderingChunks, inInterrupt );
		return true;
	}

#ifdef SHOW_NOSTART
	CMSG(TTY_PURPLE, "none nf");
#endif

	return false;
}

//////////////////////////////////////////////////////////////////////////////

static bool RenderReadyChunks( bool inInterrupt )
{

#ifdef ADDITIONAL_ASSERTS
	DBGASSERT(gRenderingChunks == NULL);
#endif

	if(gReadyChunks != NULL)
	{
#ifdef DETECT_MASTERLIST_ERRORS
		ScanMasterListForErrors(gReadyChunks);
#endif

		gRenderingChunks = gReadyChunks;
		gRenderingChunksEnd = gReadyChunksEnd;
		gReadyChunks = NULL;
		gReadyChunksEnd = NULL;
		gCurrRenderingChunks = gCurrReadyChunks;
		gCurrReadyChunks = 0;

		if( gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks != gCurrAllocedChunks)
			CMSG( TTY_RED, "count wrong H" );
		DBGASSERT(gCurrFreeChunks + gCurrRenderingChunks + gCurrNextFrameChunks + gCurrReadyChunks == gCurrAllocedChunks);
#ifdef SHOW_DMASTART
		CMSG(TTY_GREEN, "start ready");
#endif
		StartRender(gRenderingChunks, inInterrupt );
		return true;
	}

#ifdef SHOW_NOSTART
	CMSG(TTY_PURPLE, "none ready");
#endif

	return false;
}

//////////////////////////////////////////////////////////////////////////////

static void HandleSwap()
{
	// How did these happen
	DBGASSERT(!gInterruptsAreDisabledExternally);
	DBGASSERT(!gInterruptsAreDisabled);

	// see if we are ready for a frame swap
	if(
		// are we still making the frame?
		gFrameBeingDisplayed+1 < gFrameBeingRendered &&
		// have the minimum number of vblanks expired?
		gNumVBlanks >= gMinVBlanks)
	{
#ifdef SHOW_SWAP
		CMSG(TTY_GREEN, "swap");
#endif

		// increase the displayed buffer index
		gFrameBeingDisplayed++;

		// swap the buffers
 		if (gSwapBufferCallback)
 			gSwapBufferCallback(gFrameBeingDisplayed & 1);

		#ifdef __SNTUNER_H_INC__
			snStopMarker(2);
			char buff[8] = "Dsp";
			sprintf(&buff[3], "%d", gFrameBeingDisplayed);
			snStartMarker(2, buff);
		#endif

		// reset vblank counter...
		gVBlanksLastFrame = gNumVBlanks;
		gNumVBlanks = 0;

		if(gDMABlocked)
			return;

		// the DMA had better have been waiting for us.. :)
		DBGASSERT(gRenderingChunks == NULL);

		// lets start it up...
		if(gFrameBeingRendered != gFrameBeingGenerated)
		{
			// now if the masterlist is active, then the chunks for the
			// currently rendering frame have been moved to the ready list,
			// but if the masterlist is currently inactive, then these
			// chunks can be found in the next frame list...
			if(!RenderReadyChunks( true ))
			{
				if(!CurrentlyActiveAtAll())
				{
					RenderNextFrameChunks( true );
				}
			}
		}
		else
		{
			// in this case, the masterlist is active, since MasterList::Start
			// has been called..  but, the ready list should be empty...
			// I have had it not be empty though, which points to some logic
			// problem in this code... I need to investigate it more...
			if(!RenderReadyChunks( true ))
			{
				#ifdef RENDER_WHILE_GENERATING

				if(gRenderWhileGenerating)
					RenderNextFrameChunks( true );
				else

				#endif

				if(!CurrentlyActiveAtAll())
					RenderNextFrameChunks( true );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////////

static int VSyncInterruptHandler( int n )
{
	// How did these happen
	DBGASSERT(!gInterruptsAreDisabledExternally);
	DBGASSERT(!gInterruptsAreDisabled);

	// increment the number of vblanks weve seen
	gNumVBlanks++;

	HandleSwap();

	// call game callback..
	if(gCallbackVBlankInterrupt)
		(*gCallbackVBlankInterrupt)();

	ExitHandler();
	return (0);
}

//////////////////////////////////////////////////////////////////////////////

static void HandleDMAEnd(bool inInterrupt)
{
	// this cant be called if !gManagedDMA
	// otherwise, there will be problems...
	DBGASSERT(gManagedDMA);

	// end of transfer... so remember that...
	// if ! busy.. someone else must be doing a transfer
	// so we ignore it...
	gManagedDMA = false;

#ifdef SHOW_DMAEND
	CMSG(TTY_GREEN, "DMAend" );
#endif

	u_int DMAEndTime;
	asm("mfc0 %0, $9" : "=r" (DMAEndTime));
	gCurrentCycleCount += DMAEndTime - gDMAStartTime;

	// we arent rendering..
	RemoveRenderingChunks();

#ifdef __SNTUNER_H_INC__
	snStopMarker(1);
#endif

	if(gDMABlocked)
		return;

	// are there any new transfers?
	if(RenderReadyChunks( inInterrupt ))
		return;

	if(gFrameBeingRendered > gFrameBeingGenerated)
		return;

	if(gFrameBeingRendered == gFrameBeingGenerated)
	{
		// we are still drawing a frame that is currently being
		// generated... (DMA is ahead of CPU)
		//
		// we dont need to check:
		//		// is it safe to start rendering this frame (are we displaying the prior)
		//		gFrameBeingDisplayed+1 < gFrameBeingRendered &&
		// because we cant get here unless we've already one at least one
		// packet with this expression true:
		//		gFrameBeingRendered == gFrameBeingGenerated

#ifdef RENDER_WHILE_GENERATING
		if(gRenderWhileGenerating)
		if(RenderNextFrameChunks( inInterrupt ))
			return;
#endif

		// we ran out of next frame chunks.
		// that could mean that we finished the next frame...
		// if so, we should be able to tell by checking the
		// masterlistactive flag...  If its active, we
		// will still probably have stuff to send up...
		if(CurrentlyActiveAtAll())
			return;
	}

#ifdef SHOW_NOSTART
	CMSG(TTY_PURPLE, "Outofwork");
#endif

	// we just completed DMAing a frame...
	// we dont start drawing the next frame until a vblank.. ;)
	// if we just started rendering here, we'd be writing to the frontbuffer!
	gFrameBeingRendered++;

	if(!gDisplayVSynced)
		HandleSwap( );

	// copy the cycle count into place..
	gMostRecentCycleCount = gCurrentCycleCount;
	gMostRecentDMAsThisFrame = gCurrentDMAsThisFrame;
	gMostRecentDMAWaitCycleCount = gCurrentDMAWaitCycleCount;
	gMostRecentBlockWaitCycleCount = gCurrentBlockWaitCycleCount;
	gCurrentDMAsThisFrame = 0;
	gCurrentCycleCount = 0;
	gCurrentDMAWaitCycleCount = 0;
	gCurrentBlockWaitCycleCount = 0;

	// call the callback...
	if(gCallbackEndOfDMA != NULL)
	{
		(*gCallbackEndOfDMA)();
	}
}

//////////////////////////////////////////////////////////////////////////////

// interrupt handler for DMA Ch 1 for PS2...
static int DMAInterruptHandler(int channel)
{
	// how did this happen?
	DBGASSERT(channel == 1);
	DBGASSERT(!gInterruptsAreDisabledExternally);
	DBGASSERT(!gInterruptsAreDisabled);

	if(!gManagedDMA)
	{
		// Must call this just before returning from handler
		ExitHandler();

		return 0;
	}

	u_int stat = DGET_D_STAT();

	// The interrupt could be caused by many things, but it's
	// most likely the end of transfer or a tag IRQ = 1.
#ifndef SHIP_IT
	// Note: This code doesn't check for CALL tags with IRQ set that cause
	// call stack overflows.

	// Bus errors are checked for by the system.

	if(stat & (1 << 14)) {					// Check MFIFO bit
		// well... using MFIFO are we?
		// (although... how did it get in here?)
		ReportAboutDMA("MasterList Encountered an error while in MFIFO Mode.  Why were we in mfifo mode?", false, true);
	} else if(stat & (1 << 13)) { 		// Check SIS bit
		// hurm... Stall for SIS...  using MFIFO are we?
		// (although... how did it get in here?)
		ReportAboutDMA("MasterList Encountered an error while in MFIFO Mode (SIS stall)", false, true);
	} else if((*D1_CHCR & D_CHCR_MOD_M) == 0) {	// end of Normal
		// must be a texture upload or setting or something...
		// just ignore it...
		// (although... how did it get in here?)
		ReportAboutDMA("MasterList ended by 'End of normal transfer'", false, true);
	} else if((*D1_CHCR & D_CHCR_MOD_M) >> D_CHCR_MOD_O == 2) {	// end of Interleave
		// interesting... end of an interleave transfer...
		// why are you doing an interleave transfer?
		// this RENDER_ASSERT is just to make sure you know what your
		// doing.  If you really want to do one, just remove the RENDER_ASSERT...
		ReportAboutDMA("MasterList Ended by 'End of interleaved transfer'", false, true);
	} else {
		u_int tag = (*D1_CHCR & D_CHCR_TAG_M) >> D_CHCR_TAG_O;
		u_int id = (tag >> 12) & 0x7;

		if(id == 7) {	 // Check for 'end'
			// make sure that END tag was the one that we created...
			// you can verify this through TADR...
			//DBGASSERT(*D1_TADR == (u_int)gRenderingChunks);

			HandleDMAEnd(true);
		} else if(id == 0){ // check for 'refe'
			// We were ended by a refe! who put that there?
			ReportAboutDMA("MasterList ended by a REFE tag", false, true);
		} else if(id == 6 && (*D1_CHCR & D_CHCR_ASP_M) == 0) {	// Check for 'ret'
			// TAGID_RET one time too many
			ReportAboutDMA("MasterList ended by a RET tag", false, true);
		} else if((!(tag & 0x8000)) && id == 5) { 	  // Check for call stack overflow
			// TAGID_CALL one time too many
			ReportAboutDMA("MasterList dma CALL too deep.", false, true);
		} else if(tag & 0x8000) {
			// interrupt....

			// Restart the DMA - NOTE!!! We must not do
			// this if this was the last tag in the chain, but there
			// is no reliable way of knowing this, so make sure
			// that you never set IRQ = 1 in the last tag of the chain.
			// as that tag is created by MasterList, its unlikely to ever
			// happen...
			if(gCallbackDmaInterrupt != NULL)
			{
				(*gCallbackDmaInterrupt)();
			}
			else
			{
				ReportAboutDMA("Unhandled DMA Interrupt encountered", false, true);
			}
			// resume DMA
			*D1_CHCR |= D_CHCR_STR_M;
		} else {
			// dont know why we were called...
			//char buffer[100];
			//sprintf(buffer, "STAT : %x   TAG: %x  ID: %x\n", stat, tag, id);
			//ReportAboutDMA(buffer, false, true);
		}
	}
#else
	if(*D1_CHCR & 0x80000000) { // Was the last tag's IRQ bit set?
		// interrupt....

		// Restart the DMA - NOTE!!! We must not do
		// this if this was the last tag in the chain, but there
		// is no reliable way of knowing this, so make sure
		// that you never set IRQ = 1 in the last tag of the chain.
		// as that tag is created by MasterList, its unlikely to ever
		// happen...
		if(gCallbackDmaInterrupt != NULL)
		{
			(*gCallbackDmaInterrupt)();
		}
		else
		{
			//ReportAboutDMA("Unhandled DMA Interrupt encountered", false, true);
		}
		*D1_CHCR |= D_CHCR_STR_M;
	} else {
		HandleDMAEnd(true);
	}
#endif

	// Must call this just before returning from handler
	ExitHandler();

	return 0;
}

//////////////////////////////////////////////////////////////////////////////

#ifndef SHIP_IT

// report status of dma channels to try to debug
void ReportAboutDMA(char* message, bool resetDma, bool inInterrupt)
{
	u_int u32Value;
	const char* string;
	char buffer[400];
	bool needsReset = false;

#ifdef PAGRAB_ON_DMAC_REPORT
   HWBreak();
#endif

	scePrintf("---------------------------------------\n");
	scePrintf("***************************************\n");
	scePrintf("---------------------------------------\n");
	scePrintf("%s\n", message);
	scePrintf("---------------------------------------\n");
	if(gRenderingChunks)
		scePrintf("Current chain @ 0x%x\n", gRenderingChunks);
	if(gReadyChunks)
		scePrintf("Ready chain @ 0x%x\n", gReadyChunks);
	if(gNextFrameChunks)
		scePrintf("Next Frame chain @ 0x%x\n", gNextFrameChunks);
	scePrintf("---------------------------------------\n");
	scePrintf("Generating Frame # %d\n", gFrameBeingGenerated);
	scePrintf("Rendering  Frame # %d\n", gFrameBeingRendered);
	scePrintf("Displaying Frame # %d\n", gFrameBeingDisplayed);
	scePrintf("---------------------------------------\n");

	if(!gRenderingChunks)
	{
		scePrintf("If the MasterList didn't complete in time, its\n");
		scePrintf("probably because it looks there isn't one running\n");
		if(gNextFrameChunks || gReadyChunks)
		{
			scePrintf("but one is ready to run, so there was probably\n");
			scePrintf("a syncronization error in the MasterList code.\n");
		}
		else
		{
			scePrintf("if the message above says that, then there are\n");
			scePrintf("ghosts in your PS2 or something...\n");
		}
		scePrintf("---------------------------------------\n");
	}

	u32Value = *D1_CHCR;
	scePrintf("D1_CHCR: 0x%x, STR=0x%x, ASP=0x%x\n", u32Value,
			 (u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O,
			 (u32Value & D_CHCR_ASP_M) >> D_CHCR_ASP_O);
	needsReset |= (((u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O) != 0);
	scePrintf("MADR=0x%x, QWC=0x%x, TADR=0x%x, ASR0=0x%x, ASR1=0x%x\n",
			 *D1_MADR,
			 *D1_QWC,
			 *D1_TADR,
			 *D1_ASR0,
			 *D1_ASR1);
	scePrintf("---------------------------------------\n");
	u32Value = *D2_CHCR;
	scePrintf("D2_CHCR: 0x%x, STR=0x%x, ASP=0x%x\n", u32Value,
			 (u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O,
			 (u32Value & D_CHCR_ASP_M) >> D_CHCR_ASP_O);
	scePrintf("MADR=0x%x, QWC=0x%x, TADR=0x%x, ASR0=0x%x, ASR1=0x%x\n",
			 *D2_MADR,
			 *D2_QWC,
			 *D2_TADR,
			 *D2_ASR0,
			 *D2_ASR1);
	scePrintf("---------------------------------------\n");
	u32Value = *D8_CHCR;
	scePrintf("D8_CHCR: 0x%x, STR=0x%x, ASP=0x%x\n", u32Value,
			 (u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O,
			 (u32Value & D_CHCR_ASP_M) >> D_CHCR_ASP_O);
	scePrintf("MADR=0x%x, QWC=0x%x, TADR=N/A, ASR0=N/A, ASR1=N/A\n",
			 *D8_MADR,
			 *D8_QWC);
	scePrintf("---------------------------------------\n");
	u32Value = *D9_CHCR;
	scePrintf("D9_CHCR: 0x%x, STR=0x%x, ASP=0x%x\n", u32Value,
			 (u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O,
			 (u32Value & D_CHCR_ASP_M) >> D_CHCR_ASP_O);
	scePrintf("MADR=0x%x, QWC=0x%x, TADR=0x%x, ASR0=N/A, ASR1=N/A\n",
			 *D9_MADR,
			 *D9_QWC,
			 *D9_TADR);
	scePrintf("---------------------------------------\n");
	u32Value = *VIF1_STAT;
	scePrintf("VIF1_STAT=0x%x, VIF1_CODE=0x%x\n",
			 u32Value,
			 *VIF1_CODE);
	scePrintf("VIF1_NUM=0x%x, VIF1_MARK=0x%x\n",
		*VIF1_NUM,
		*VIF1_MARK);
	string = &buffer[0];

	buffer[0] = '\0';
	switch((u32Value & VIF1_STAT_VPS_M) >> VIF1_STAT_VPS_O){
	case 0: strcat(buffer, "Idle\n\t\t"); break;
	case 1: strcat(buffer, "Awaiting Data From DMA\n\t\t"); needsReset = true; break;
	case 2: strcat(buffer, "Decoding VIFCode\n\t\t"); needsReset = true; break;
	case 3: strcat(buffer, "Decompressing/transferring data following VIFCode\n\t\t"); needsReset = true; break;
	}
	if(((u32Value & VIF1_STAT_VEW_M) >> VIF1_STAT_VEW_O) == 1){
		strcat(buffer, "Waiting for [E] bit from VU\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_VGW_M) >> VIF1_STAT_VGW_O) == 1){
		strcat(buffer, "Waiting for GIF to complete\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_VSS_M) >> VIF1_STAT_VSS_O) == 1){
		strcat(buffer, "Stopped because STOP\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_VFS_M) >> VIF1_STAT_VFS_O) == 1){
		strcat(buffer, "Stopped because ForceBreak\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_VFS_M) >> VIF1_STAT_VFS_O) == 1){
		strcat(buffer, "Stopped because ForceBreak\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_VIS_M) >> VIF1_STAT_VIS_O) == 1){
		strcat(buffer, "Stopped because VU Interrupt (not sure on this one.. [VIS bit])\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_INT_M) >> VIF1_STAT_INT_O) == 1){
		strcat(buffer, "Stopped because [I] bit Interrupt in VIFCode\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_ER0_M) >> VIF1_STAT_ER0_O) == 1){
		strcat(buffer, "Stopped because DMATag detected in the VIF Packet\n\t\t");
		needsReset = true;
	}

	if(((u32Value & VIF1_STAT_ER1_M) >> VIF1_STAT_ER1_O) == 1){
		strcat(buffer, "Stopped because Undefined VIFCode detected\n\t\t");
		needsReset = true;
	}

	if(buffer[0] != '\0')
	{
		u_int len = strlen(buffer);
		while(buffer[--len] == '\t') buffer[len] = '\0';
	}
	scePrintf("\tCurrent VIF Activity:\n\t\t%s", buffer);

	switch((u32Value & VIF1_STAT_MRK_M) >> VIF1_STAT_MRK_O){
	case 0: scePrintf("\tMARK not detected in VIF stream\n"); break;
	case 1: scePrintf("\tMARK detected in VIF stream\n"); break;
	}

	switch((u32Value & VIF1_STAT_DBF_M) >> VIF1_STAT_DBF_O){
	case 0: scePrintf("\tTOPS=BASE\n"); break;
	case 1: scePrintf("\tTOPS=BASE+OFFSET\n"); break;
	}

	switch((u32Value & VIF1_STAT_FDR_M) >> VIF1_STAT_FDR_O){
	case 0: scePrintf("\tTransfer direction: Memory/SPR -> VIF1\n"); break;
	case 1: scePrintf("\tTransfer direction: VIF1 -> Memory/SPR\n"); break;
	}

	scePrintf("---------------------------------------\n");
	u32Value = *GIF_STAT;
	scePrintf("GIF_STAT=0x%x\n", u32Value);

	buffer[0] = '\0';

	switch((u32Value & GIF_STAT_OPH_M) >> GIF_STAT_OPH_O){
	case 0: strcat(buffer, "Idle\n\t\t"); break;
	case 1: strcat(buffer, "Outputing Data to GS\n\t\t"); needsReset = true; break;
	}

	// this is incorrectly defined in the headers we use...
#undef GIF_STAT_APATH_M
#define GIF_STAT_APATH_M	(0x03<<10)

	switch((u32Value & GIF_STAT_APATH_M) >> GIF_STAT_APATH_O){
	case 0: strcat(buffer, "No Data Source Active\n\t\t"); break;
	case 1: strcat(buffer, "Data Source is PATH1\n\t\t"); needsReset = true; break;
	case 2: strcat(buffer, "Data Source is PATH2\n\t\t"); needsReset = true; break;
	case 3: strcat(buffer, "Data Source is PATH3\n\t\t"); needsReset = true; break;
	}

	if(buffer[0] != '\0')
	{
		u_int len = strlen(buffer);
		while(buffer[--len] == '\t') buffer[len] = '\0';
	}
	scePrintf("\tCurrent GIF Activity:\n\t\t%s", buffer);

	if(((u32Value & GIF_STAT_PSE_M) >> GIF_STAT_PSE_O) == 1){
		scePrintf("\tStopped because Temporary stop requested through PSE flag of GIF_MODE\n");
		needsReset = true;
	}

	if(((u32Value & GIF_STAT_P3Q_M) >> GIF_STAT_P3Q_O) == 1){
		scePrintf("\tRequest to wait for processing in PATH3 (Queued?)\n");
		needsReset = true;
	}
	if(((u32Value & GIF_STAT_P2Q_M) >> GIF_STAT_P2Q_O) == 1){
		scePrintf("\tRequest to wait for processing in PATH2 (Queued?)\n");
		needsReset = true;
	}
	if(((u32Value & GIF_STAT_P1Q_M) >> GIF_STAT_P1Q_O) == 1){
		scePrintf("\tRequest to wait for processing in PATH1 (Queued?)\n");
		needsReset = true;
	}

	if(((u32Value & GIF_STAT_M3R_M) >> GIF_STAT_M3R_O) == 1)
		scePrintf("\tPATH3 Masked by GIF_MODE.MR3\n");
	if(((u32Value & GIF_STAT_M3P_M) >> GIF_STAT_M3P_O) == 1)
		scePrintf("\tPATH3 Masked by VIF MASKPTH3 VIFCode\n");
	if(((u32Value & GIF_STAT_IP3_M) >> GIF_STAT_IP3_O) == 1){
		scePrintf("\tPATH3 Transfer Interrupted for another path\n");
		needsReset = true;
	}

	switch((u32Value & GIF_STAT_IMT_M) >> GIF_STAT_IMT_O){
	case 0: scePrintf("\tPATH3 In Continuous Transfer Mode\n"); break;
	case 1: scePrintf("\tPATH3 In Intermittent Transfer Mode\n"); break;
	}

	switch((u32Value & GIF_STAT_DIR_M) >> GIF_STAT_DIR_O){
	case 0: scePrintf("\tTransfer direction: EE -> GS\n"); break;
	case 1: scePrintf("\tTransfer direction: GS -> EE\n"); break;
	}

	scePrintf("\tQuad word count in GIF FIFO: 0x%X\n", (u32Value & GIF_STAT_FQC_M) >> GIF_STAT_FQC_O);

	scePrintf("\tLast GIFTag %x %x %x %x decoded:\n", *GIF_TAG3, *GIF_TAG2, *GIF_TAG1, *GIF_TAG0);
	u32Value = *GIF_TAG0;
	scePrintf("\t\tNLOOP=0x%x, EOP=0x%x, Unused(31:16)=0x%x\n",
		((tGIF_TAG0*)GIF_TAG0)->NLOOP,
		((tGIF_TAG0*)GIF_TAG0)->EOP,
		((tGIF_TAG0*)GIF_TAG0)->tag);
	u32Value = *GIF_TAG1;
	scePrintf("\t\tUnused(45:32)=0x%x, PRE=0x%x, PRIM=0x%x, FLG=0x%x, NREG=0x%x\n",
		((tGIF_TAG1*)GIF_TAG1)->tag,
		((tGIF_TAG1*)GIF_TAG1)->PRE,
		((tGIF_TAG1*)GIF_TAG1)->PRIM,
		((tGIF_TAG1*)GIF_TAG1)->FLG,
		((tGIF_TAG1*)GIF_TAG1)->NREG);
	scePrintf("\t\tRegisters: %x %x\n", *GIF_TAG3, *GIF_TAG2);

	scePrintf("\tGIF_CNT Decoded 0x%x\n", *GIF_CNT);
	scePrintf("\t\tCurrent loop counter (backwards from NLOOP):0x%x\n", ((tGIF_CNT*)GIF_CNT)->LOOPCNT);
	scePrintf("\t\tCurrent register:%d\n", ((tGIF_CNT*)GIF_CNT)->REGCNT == 0 ? 15 : ((tGIF_CNT*)GIF_CNT)->REGCNT - 1);
	scePrintf("\t\tAddress of VU Memory under transfer:0x%x\n", ((tGIF_CNT*)GIF_CNT)->VUADDR);

	scePrintf("---------------------------------------\n");
	asm (" cfc2    %0,$vi29" : "=r" (u32Value));
	scePrintf("VPU_STAT=0x%x, VU1_TPC=0x%x (VU1 Program Counter)\n", u32Value, sceDevVu1GetTpc());

	if((u32Value & (1<< 8)) >> 8){
		scePrintf("\tVU1 Executing Microcode\n");
		needsReset = true;
	}
	if((u32Value & (1 << 9)) >> 9){
		scePrintf("\tVU1 Stopped by D Bit\n");
		needsReset = true;
	}
	if((u32Value & (1 << 10)) >> 10){
		scePrintf("\tVU1 Stopped by T Bit\n");
		needsReset = true;
	}
	if((u32Value & (1 << 11)) >> 11){
		scePrintf("\tVU1 Stopped by Force Break\n");
		needsReset = true;
	}
	if((u32Value & (1 << 12)) >> 12){
		scePrintf("\tVU1 XGKICK Waiting for prior XGKICK\n");
		needsReset = true;
	}
	if((u32Value & (1 << 13)) >> 13){
		scePrintf("\tVU1 DIV unit busy\n");
		needsReset = true;
	}
	if((u32Value & (1 << 14)) >> 14){
		scePrintf("\tVU1 EFU unit busy\n");
		needsReset = true;
	}
	if((u32Value & (1<<8|1<<9|1<<10|1<<11|1<<12|1<<13|1<<14)) == 0)
		scePrintf("\tVU1 Idle\n");

	if((u32Value & (1<< 0)) >> 0)
		scePrintf("\tVU0 Executing Microcode\n");
	if((u32Value & (1 << 1)) >> 1)
		scePrintf("\tVU0 Stopped by D Bit\n");
	if((u32Value & (1 << 2)) >> 2)
		scePrintf("\tVU0 Stopped by T Bit\n");
	if((u32Value & (1 << 3)) >> 3)
		scePrintf("\tVU0 Stopped by Force Break\n");
	if((u32Value & (1 << 5)) >> 5)
		scePrintf("\tVU0 DIV unit busy\n");
	if((u32Value & (1 << 7)) >> 7)
		scePrintf("\tVU0 IBS bit set (whats this?)\n");

	scePrintf("---------------------------------------\n");
	scePrintf("***************************************\n");
	// make analysis...
	char analysis[ 1024 ];
	analysis[0] = 0;
	u32Value = *D1_CHCR;
	if((u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O)
	{
		u32Value = *VIF1_STAT;
		if(((u32Value & VIF1_STAT_VEW_M) >> VIF1_STAT_VEW_O) || ((u32Value & VIF1_STAT_VGW_M) >> VIF1_STAT_VGW_O))
		{
			u32Value = *GIF_STAT;
			if((u32Value & GIF_STAT_OPH_M) >> GIF_STAT_OPH_O)
			{
				if((u32Value & GIF_STAT_APATH_M) >> GIF_STAT_APATH_O == 1)
				{
					asm (" cfc2    %0,$vi29" : "=r" (u32Value));
					if((u32Value & (1<< 8)) >> 8 && (u32Value & (1 << 12)) >> 12)
					{
						strcpy(analysis, "Maybe the VU kicked invalid GIF Data (or its in an infinite loop kicking data)\n");
					}
					else
					{
						strcpy(analysis, "Maybe the VU code is really screwed?\n");
					}
				}
				else if((u32Value & GIF_STAT_APATH_M) >> GIF_STAT_APATH_O == 2)
				{
					strcpy(analysis, "Maybe the GIF got invalid VIF DIRECT information\n");
				}
				else if((u32Value & GIF_STAT_APATH_M) >> GIF_STAT_APATH_O == 3)
				{
					strcpy(analysis, "Maybe a GIF Path3 Invalid chain?\n");
				}
				else
				{
					strcpy(analysis, "Not really sure whats going on\n");
				}
			}
			else
			{
				asm (" cfc2    %0,$vi29" : "=r" (u32Value));
				if((u32Value & (1<< 8)) >> 8)
				{
					strcpy(analysis, "Maybe the VU is off running in an infinite loop?\n");
				}
				else
				{
					strcpy(analysis, "Not sure whats up... the VIF is waiting for the stopped VU\n");
				}
			}
		}
		else
		{
			strcpy(analysis, "Maybe the VIF encountered an error in its input\n");
		}
	}
	else
	{
		strcpy(analysis, "Maybe the DMAC encountered an error in the chain\n");
	}
	scePrintf( "%s", analysis );

	char* tempBuffer = (char*) (*GS_SIGLBLID >> 32);
	buffer[0] = '\0';
	if (tempBuffer != NULL && ((unsigned int)tempBuffer) < (128u*1024u*1024u))
	{
		strncpy(buffer, tempBuffer, 50);
		buffer[49] = '\0';
		scePrintf("---------------------------------------\n");
		scePrintf("Engine String: %s\n", buffer);
	}

	scePrintf("***************************************\n");

#if !defined(DISABLE_EXCEPTIONS)
	strcat( analysis, "LABEL=" );
	strcat( analysis, buffer );

	int* d1tadr = (int*)*D1_TADR;
#define _CHECK_PTR(x) (((int)(x) > 0x00100000 && ((int)(x) < 0x02000000) && (((int)(x) & 0x0000000f) == 0)))
	if(_CHECK_PTR(d1tadr))
		sprintf(buffer, "\nTADR=%x *TADR-1=%x %x %x %x\n",
				  (int)d1tadr, d1tadr[-1], d1tadr[0], d1tadr[1], d1tadr[2]);
	else
		sprintf(buffer, "\nTADR=%x (bad PTR)\n", (int)d1tadr);
	strcat( analysis, buffer );
	int* madr = (int*)*D1_MADR;
	if(_CHECK_PTR(madr))
		sprintf(buffer, "\nMADR=%x *MADR-1=%x %x %x %x\n",
				  (int)madr, madr[-1], madr[0], madr[1], madr[2]);
	else
		sprintf(buffer, "\nMADR=%x (bad PTR)\n", (int)madr);
	strcat( analysis, buffer );
#undef _CHECK_PTR
	sprintf(buffer, "D1_CHCR: 0x%x, STR=0x%x, ASP=0x%x\n", u32Value,
			  (u32Value & D_CHCR_STR_M) >> D_CHCR_STR_O,
			  (u32Value & D_CHCR_ASP_M) >> D_CHCR_ASP_O);
	strcat( analysis, buffer );
	sprintf(buffer, "QWC=0x%x, ASR0=0x%x, ASR1=0x%x\n",
			  *D1_QWC,
			  *D1_ASR0,
			  *D1_ASR1);
	strcat( analysis, buffer );
	sprintf(buffer, "Last GIFTag %x %x %x %x\n", *GIF_TAG3, *GIF_TAG2, *GIF_TAG1, *GIF_TAG0);
	strcat( analysis, buffer );
	strcat( analysis, message );
	ExceptionHandler::SetExceptionMessage( analysis );
#endif

#ifdef BREAK_ON_DMAC_REPORT
#if defined(PS2) && defined(SN)
	asm volatile ("break 0x1");
#else
	asm volatile ("breakc 0x0");
#endif
#endif

	if(resetDma && needsReset)
	{
		scePrintf("---------------------------------------\n");
		scePrintf("Resetting DMA Subsystem! - this may not work...\n");
		sceDmaReset(1);
		sceDevVif1Reset();
		sceDevVu1Reset();
		sceDevGifReset();
		sceGsResetPath();
		scePrintf("---------------------------------------\n");
	}
}
#endif //DEBUG

//////////////////////////////////////////////////////////////////////////////

// these just toggle the currently active state
// so that things that check it will think its not active
void Suspend(void)
{
	// we must not currently be Suspended()
	DBGASSERT(!CurrentlySuspended());

	// make asserts work...
	bSuspended = true;
}

//////////////////////////////////////////////////////////////////////////////

void Resume(void)
{
	// we must currently be Suspended()
	DBGASSERT(CurrentlySuspended());

	// make asserts work...
	bSuspended = false;
}

//////////////////////////////////////////////////////////////////////////////

bool TryToFreeMemory(u_int size)
{
	bool freedsome = false;
	bool success = true;
	bool tried = false;
	// only do half each time
	// plus one to deal with rounding down
	int blocks_to_free = ((size / MASTERLIST_CHUNK_SIZE) / 2) + 1;
	while(blocks_to_free-- && success && gCurrAllocedChunks > 4)
	{
		DisableInterrupts();
		success = FreeChunk();
		EnableInterrupts();
		freedsome |= success;
		tried = true;
	}
	blocks_to_free = ((size / MASTERLIST_CHUNK_SIZE) / 2) + 1;
	if(!freedsome && tried)
	{
		gCurrentBlockWaitCycleCount += WaitForDMACompletion(FreeConditional);
		while(blocks_to_free-- && success && gCurrAllocedChunks > 4)
		{
			DisableInterrupts();
			success = FreeChunk();
			EnableInterrupts();
			freedsome |= success;
		}
	}

	return freedsome;
}

//////////////////////////////////////////////////////////////////////////////

// function to call to use cpu cycles while we're waiting on dma's.
void SetDMAWaitCallback(void (*callback)(void))
{
	gWaitFunc = callback;
}

//////////////////////////////////////////////////////////////////////////////

// this callback gets called (if its !NULL) when the dma completes
void SetEndOfDMACallback(MasterListCallback* callback)
{
	gCallbackEndOfDMA = callback;
}

//////////////////////////////////////////////////////////////////////////////

// this callback gets called (if its !NULL) when a dmatag with the
// interrupt bit set COMPLETES executing.
void SetDMAInterruptCallback(MasterListCallback* callback)
{
	gCallbackDmaInterrupt = callback;
}

//////////////////////////////////////////////////////////////////////////////

// callback for whenever a vblank happens
void SetVBlankInterruptCallback(MasterListCallback* callback)
{
	gCallbackVBlankInterrupt = callback;
}

//////////////////////////////////////////////////////////////////////////////

MasterListCallback* GetVBlankInterruptCallback(void)
{
	return gCallbackVBlankInterrupt;
}

//////////////////////////////////////////////////////////////////////////////

void SetMinVBlanks(int minVBlanks)
{
	gMinVBlanks = minVBlanks;
}

//////////////////////////////////////////////////////////////////////////////

int GetMinVBlanks(void)
{
	return gMinVBlanks;
}

//////////////////////////////////////////////////////////////////////////////

void SetSwapBufferCallback(SwapBufferCallback callback)
{
	gSwapBufferCallback = callback;
}

//////////////////////////////////////////////////////////////////////////////

SwapBufferCallback GetSwapBufferCallback(void)
{
	return gSwapBufferCallback;
}

//////////////////////////////////////////////////////////////////////////////

void StartPath3(void)
{
	gPath3Started = true;

	// first, the changes to the master list... we insert a wait for this
	// texture to get up to texture mem...  the 2 flushes are
	// NECESSARY!  As is all the refed data!!!! PLEASE DONT MESS WITH
	// THIS WITHOUT FIRST TALKING TO Tyler!!! If you do, you are
	// flirting with a most horrible death!
	AppendDMATAG(MASTERLIST_PATH3FLUSHPACKET_SIZE, TAGID_REF, &u128Path3FlushPacket[0]);
	Append(createFLUSHA());
	Append(createFLUSH());

	// since we flushed above, mark it...
	SetFlushed(true);
}

//////////////////////////////////////////////////////////////////////////////

void Path3_UploadImageData(void* pData, u_int address, u_int width, u_int height, u_int format = SCE_GS_PSMCT32, bool bInterrupt = false)
{
	DBGASSERT(CurrentPath3DMAPointer());

	// make sure path3 uploads are initted...
	if(!gPath3Started)
	{
		StartPath3();
	}

	// now the changes to the path3 list
	// much simpler... in fact, its just like the path 2 list with a
	// little twist... no eop bit set
	AppendTextureUpload(CurrentPath3DMAPointer(), pData, address, width, height, format, bInterrupt, false);
}


//////////////////////////////////////////////////////////////////////////////

u_int GetRecentCycleCount(void)
{
	return gMostRecentCycleCount;
}

//////////////////////////////////////////////////////////////////////////////

u_int GetDMAWaitStallCycleCount(void)
{
	return gMostRecentDMAWaitCycleCount;
}
//////////////////////////////////////////////////////////////////////////////

u_int GetBlockWaitStallCycleCount(void)
{
	return gMostRecentBlockWaitCycleCount;
}

//////////////////////////////////////////////////////////////////////////////

u_int CurrAllocatedChunks(void)
{
	return gCurrAllocedChunks;
}

//////////////////////////////////////////////////////////////////////////////

u_int ChunksUsedLastFrame(void)
{
	return gMostRecentChunksPerFrame;
}

//////////////////////////////////////////////////////////////////////////////

u_int MinChunksFree(void)
{
	return gMostRecentMinFreeChunks;
}

//////////////////////////////////////////////////////////////////////////////

// this is a stolen routine from sce...
// dont ask me about the reason the integer parameters are passed as floats...
//static inline void _WatchVu1Reg(u_int i0, u_int r0, u_int* i1, PS2Vector4* r1)
//{
//	u_int i1temp=0;
//	asm __volatile__ ("
//#  WatchVu1Reg
//#  ---------------------------------------------------------------
//   ctc2            %1, $vi05
//   ctc2            %2, $vi06
//   vilwr.x         vi07, (vi05)
//   vlqi.xyzw       vf07xyzw, (vi06++)
//   nop
//   nop
//   nop
//   nop
//   cfc2            %0, $vi07
//   sqc2            vf07, 0x00(%3)
//
//   ": "=r" (i1temp) : "r" (i0) , "r" (r0), "r" (r1));
//	*i1 = i1temp;
//}

// interrupt handler for catching our VU Debugging stuff
// static int VUInterruptHandler(int /*cause*/)
// {
// 	// we know 'cause' will always be INTC_VU1, so we ignore it

// 	// some defines we use to make life easyer
// #define ASFLOAT(x,y)	(*(float*)&(((u_int*)&x)[y]))
// #define ASINT(x,y)	(((u_int*)&x)[y])
// 	// first, determine if it was a T bit interrupt that fired us off..
// 	u_int u32Value;
// 	u_int value;
// 	PS2Vector4 vec;
// 	asm (" cfc2    %0,$vi29" : "=r" (u32Value));
// 	if(u32Value & (1 << 10))
// 	{
// 		// VU1 was stopped by the T bit...
// 		// that means that this interrupt is for us!
// 		u_int* VUMem = (u_int*)0x1100c000;
// 		switch(VUMem[2])
// 		{
// 		case 1:
// 			// wait a bit to allow prior prints to flush...
// 			for(int i = 0; i < 4000000; ) i++;//sceGsSyncV(0);
// 			// get regs
// 			_WatchVu1Reg(0x420+VUMem[3], 0x400+VUMem[3], &value, &vec);
// 			// print them..
// 			scePrintf("VI%c%c %d 0x%x   VF%c%c :0x%x 0x%x 0x%x 0x%x\n",
// 						 VUMem[3]/10 + '0', VUMem[3]%10 + '0',
// 						 value, value,
// 						 VUMem[3]/10 + '0', VUMem[3]%10 + '0',
// 						 *(u_int*)&vec[0], *(u_int*)&vec[1], *(u_int*)&vec[2], *(u_int*)&vec[3]);
// 			break;
// 		default:
// 			DBGASSERT(!"Invalid TBit Interrupt triggered\n");
// 			break;
// 		}

// 		// wait for the GIF to be idle...
// 		// we have to do this because its bad to start the vu when the gif is not idle...
// 		// just the rules folks..
// 		do {
// 			// do nothing until its idle...
// 		} while((u32Value & GIF_STAT_OPH_M) >> GIF_STAT_OPH_O);

// 		// restart the VU
// #ifndef SHIP_IT
// 		sceDevVu1Exec(sceDevVu1GetTpc());
// #else
// 		// I know I know... it will never fire... but its accurate :)..
// 		DBGASSERT(false);
// #endif
// 	}

// 	ExitHandler();
// 	return 0;
// }

//////////////////////////////////////////////////////////////////////////////

static u_int WaitForDMACompletion(ConditionalFunction* conditionalFunction)
{
	u_int u32StartCycles = 0;
	u_int u32EndCycles = 0;

	// TODO: time this and report errors if it takes too long...
	// get current cycle counter...
	asm __volatile__("mfc0 %0,$9" : "=r" (u32StartCycles));	// value in cpu cycles (300MHz)

	bool stillWaiting;

	do
	{
		// in the debug build we track elapsed time so that we can
		// inform the user if things break down in the process.
		asm __volatile__("mfc0 %0,$9" : "=r" (u32EndCycles)); // value in cpu cycles (300MHz)

#if !defined(SHIP_IT) && defined(A_COMPILER_SN)
		u_int u32Value;
		asm ("cfc2    %0,$vi29" : "=r" (u32Value));
		if(u32Value & 0x200)
		{
			// [D] bit hit...
			#ifdef PAGRAB_ON_VU1_DBIT
			HWBreak();
			#else
			asm("break 1;");
			#endif

			// restart and get execution address
			asm("ctc2 %1, $vi01\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "vilwr.x $vi01, ($vi01)\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "viadd $vi31, $vi00, $vi01\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "cfc2 %0, $vi01" : "=r" (u32Value) : "r" (0x43A));

			u32StartCycles = u32EndCycles;
		}
		else if(u32Value & 0x400)
		{
			// [T] bit hit...
			// In this case, we display whats in VU1Mem @ QWord 1024 and restart it all up...
			u32StartCycles = u32EndCycles;

			// get last qword in vu1mem..
			static unsigned int lastval[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
			float* vumem = (float*)0x1100fff0;
			unsigned int* vumemi = (unsigned int*)0x1100fff0;
			if(lastval[0] != vumemi[0] || lastval[1] != vumemi[1] || lastval[2] != vumemi[2] || lastval[3] != vumemi[3])
			{
				scePrintf("VU1 %f %f %f %f (%8.8X %8.8X %8.8X %8.8X)\n",
						 vumem[0], vumem[1], vumem[2], vumem[3],
						 vumemi[0], vumemi[1], vumemi[2], vumemi[3]);
				lastval[0] = vumemi[0];
				lastval[1] = vumemi[1];
				lastval[2] = vumemi[2];
				lastval[3] = vumemi[3];
			}

			// restart and get execution address
			asm("ctc2 %1, $vi01\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "vilwr.x $vi01, ($vi01)\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "viadd $vi31, $vi00, $vi01\n"
				 "vnop\n"
				 "vnop\n"
				 "vnop\n"
				 "cfc2 %0, $vi01" : "=r" (u32Value) : "r" (0x43A));
		}
#endif //!defined(SHIP_IT) && defined(A_COMPILER_SN)

		// have we met our exit criteria?
		stillWaiting = (*conditionalFunction)();

		if (stillWaiting)
		{
			// watch for bad timeout...
			if (u32EndCycles - u32StartCycles > MAX_WAIT_CYCLES)
			{
				#ifdef SHIP_IT
				sceDmaReset(1);
				sceDevVif1Reset();
				sceDevVu1Reset();
				sceDevGifReset();
				sceGsResetPath();
				#else
				ReportAboutDMA("MasterList failed to complete in a reasonable time", RESET_DMAC_ON_ERROR, false);
				#endif

				gWaitFunc = NULL; // stop doing extra stuff (like servicing the network!)
				DisableInterrupts();
				HandleDMAEnd(false);
				EnableInterrupts();
				break;
			}

			if (gWaitFunc && (conditionalFunction == StallConditional))
			{
				gWaitFunc();
			}
		}
	} while(stillWaiting);

	// get current cycle counter...
	asm __volatile__("mfc0 %0,$9" : "=r" (u32EndCycles));	// value in cpu cycles (300MHz)

#ifndef SHIP_IT
// 	sceDevVu1PutTBit(0);
// 	DisableIntc(INTC_VU1);
// 	RemoveIntcHandler(INTC_VU1, gIntCVU1Handler);
#endif

	// get total time.. because these are unsigned, it doesnt matter if the timer wrapped,
	// as long as a frame doesnt take more than 14 seconds or so, we should be fine.
	return u32EndCycles - u32StartCycles;

}

//////////////////////////////////////////////////////////////////////////////

static void AppendTextureUpload(u_char *&list, void* pData, u_int address, u_int width, u_int height, u_int format, bool bInterrupt, bool eop)
{
	if(width == 0) return;
	if(height == 0) return;
	DBGASSERT(width > 0 && height > 0);

	// this code doesnt handle all cases.  Most cases should be removed
	// by the toolchain before the texture ever gets here though, so
	// this is juts a double-check...
#ifdef _DEBUG
	u_int u32PageWidth;
	u_int u32PageHeight;
	switch(format)
	{
	case SCE_GS_PSMT4:
		u32PageWidth = 128;
		u32PageHeight = 128;
		break;
	case SCE_GS_PSMT8:
		u32PageWidth = 128;
		u32PageHeight = 64;
		break;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
		u32PageWidth = 64;
		u32PageHeight = 64;
		break;
	case SCE_GS_PSMCT32:
	case SCE_GS_PSMCT24:
		u32PageWidth = 64;
		u32PageHeight = 32;
		break;
	default:
		DBGASSERT(FALSE);
		break;
	}
	if ((width < u32PageWidth) && (height > u32PageHeight))
		DBGASSERT(FALSE);
#endif //_DEBUG

	// just do the upload in the master list....
	if(list == CurrentDMAPointer())
		AppendDMATAG(5, TAGID_CNT, 0, bInterrupt);
	else
		Append(list, createDMATAG(5, TAGID_CNT, 0, bInterrupt));
//	{static int i = 0;Append(list, createMARK(i++));}
	Append(list, createNOP());
	Append(list, createDIRECT(5));
	Append(list, createGIFTAG(4, 1, 0xeeeeeeeeeeeeeeee, false));

	Append(list, createAD_BITBLTBUF(
				 0,						// sbp-- source buffer base pointer, vram word addr / 64 (vram block addr)
				 0,						// sbw-- source buffer width, pixels / 64
				 0,						// spsm-- source pixel format
				 address,				// dbp-- dest buffer base pointer, vram word addr / 64 (vram block addr)
				 width / 64,			// dbw-- dest buffer width, pixels / 64
				 format));				// dpsm-- dest pixel format

	Append(list, createAD_TRXPOS(
				 0,	// ssax-- source upper left xpos
				 0,	// ssay-- source upper left ypos
				 0,	// dsax-- dest upper left xpos
				 0,	// dsay-- dest upper left ypos
				 0));	// dir-- pixel transmission order (active in local -> local transmission only)

	Append(list, createAD_TRXREG(width, height));
	Append(list, createAD_TRXDIR(0));

	// calculate the size of the data...
	u_int nQWC = width * height;
	switch(format)
	{
	case SCE_GS_PSMCT32:
	case SCE_GS_PSMCT24:
		nQWC /= 4;	// 4 pixels / qword
		break;
	case SCE_GS_PSMCT16:
	case SCE_GS_PSMCT16S:
		nQWC /= 8;	// 8 pixels / qword
		break;
	case SCE_GS_PSMT8:
		nQWC /= 16;	// 16 pixels / qword
		break;
	case SCE_GS_PSMT4:
		nQWC /= 32;	// 32 pixels / qword
		break;
	default:
		DBGASSERT(0);
		break;
	}
	if (nQWC == 0)
		nQWC = 1;	// must be at least one quad word for dma (very small textures may be less than 16 bytes)

	// if no eop bit was set, we want to remember where the tag is
	// incase the caller wants to change his mind.
	if(!eop)
		pLastFalseEOPTextureUpload = list;

	// now ref the data...
	while(nQWC > 0)
	{
		u_int thistime = nQWC;
		if(thistime > 0x7F00)
			thistime = 0x7F00;
		if(list == CurrentDMAPointer())
			AppendDMATAG(1, TAGID_CNT, 0, ((thistime == nQWC) ? bInterrupt : 0));
		else
			Append(list, createDMATAG(1, TAGID_CNT, 0, ((thistime == nQWC) ? bInterrupt : 0)));
//		Append(list, createMARK(thistime));
		Append(list, createNOP());
		Append(list, createDIRECT(1));
		Append(list, createGIFTAG(thistime, 1, 0, ((thistime == nQWC) ? eop : 0), GIFTag::FLG_IMAGE));
		if(list == CurrentDMAPointer())
			AppendDMATAG(thistime, TAGID_REF, pData);
		else
			Append(list, createDMATAG(thistime, TAGID_REF, pData));
		Append(list, createNOP());
		Append(list, createDIRECT(thistime));
		pData = (void*)((u_int)pData + thistime * 16);
		nQWC -= thistime;
	}
}

//////////////////////////////////////////////////////////////////////////////

// this is the core of UploadPath3Data if there is no path 3 list...
void Path2_UploadImageData(void* pData, u_int address, u_int width, u_int height, u_int format = SCE_GS_PSMCT32, bool bInterrupt = false)
{
	AppendTextureUpload(CurrentDMAPointer(), pData, address, width, height, format, bInterrupt, true);
}

//////////////////////////////////////////////////////////////////////////////

void DoneWithImageData(void)
{
	// remember that we need a new gate-open for the next block...
	gPath3Started = false;

	// patch up the last upload to include an EOP bit to mark the end
	// of the uploads..  14 is the byte index of the byte that contains
	// the EOP bit in a gif tag.
	if(pLastFalseEOPTextureUpload)
		pLastFalseEOPTextureUpload[14] |= 0x80;
}

//////////////////////////////////////////////////////////////////////////////

void UploadImageData(void* pData, u_int address, u_int width, u_int height, u_int format, bool bInterrupt)
{
	if(CurrentPath3DMAPointer() != NULL)	Path3_UploadImageData(pData, address, width, height, format, bInterrupt);
	else Path2_UploadImageData(pData, address, width, height, format, bInterrupt);
}

//////////////////////////////////////////////////////////////////////////////

static u_int BuildPath3GatePacket(void)
{
	u_char* spr = (u_char*)&u128Path3FlushPacket[0];
	u_int nQWC = 0;


	// first QW is a ref to the rest of the data...  only
	// the first QW gets copied into the master list...
	Append(spr, createDMATAG(24, TAGID_REF, (void*)(((u_int)&u128Path3FlushPacket[1]) | DMA_ADDR_FROM_SPR)));
	Append(spr, createMSKPATH3(0)); // open the path3 gate

	// 24 QW of VIF nops...
	// 1 NOP is in the dma tag... but thats good...
	while(nQWC < 24)
	{
		Append(spr, createNOP());
		Append(spr, createNOP());
		Append(spr, createNOP());
		Append(spr, createNOP());
		nQWC++;
	}

	// this makes up for the 1 nop that went into the dma tag
	// finish last qw by closing the path3 gate...
	Append(spr, createMSKPATH3(1)); // close the path3 gate

	DBGASSERT(((int)spr - (int)&u128Path3FlushPacket[0]) == (MASTERLIST_PATH3FLUSHPACKET_SIZE * sizeof(u_long128)));

	return MASTERLIST_PATH3FLUSHPACKET_SIZE;
}

// TJC - this is really a bad thing to do.. but for a quick fix,
// its probably best to just hack it into masterlist instead of
// putting in a proper "masterlist tells us when frames go by"
// type fix.
// TJC - tell master list to delete this for us soon...
static DDMAObject* DeleteAfter3FramesList = NULL;
static DDMAObject* DeleteAfter2FramesList = NULL;
static DDMAObject* DeleteAfter1FramesList = NULL;
//////////////////////////////////////////////////////////////////////////////

void DeleteAfter2Frames(DDMAObject* obj)
{
	DDMAObject* o = obj;
	while(o->next() != NULL)
	{
		o = o->next();
	}
	o->next(DeleteAfter3FramesList);
	DeleteAfter3FramesList = obj;
}
//////////////////////////////////////////////////////////////////////////////

static void DeleteAfter2Frames(void)
{
	DDMAObject* o = DeleteAfter1FramesList;
	while(o)
	{
		DDMAObject* obj = o->next();

		delete o;

		o = obj;
	}
	DeleteAfter1FramesList = DeleteAfter2FramesList;
	DeleteAfter2FramesList = DeleteAfter3FramesList;
	DeleteAfter3FramesList = NULL;
}
//////////////////////////////////////////////////////////////////////////////
bool DisplayListDeletesPending()
{
	return ( DeleteAfter1FramesList != NULL ||
            DeleteAfter2FramesList != NULL ||
				DeleteAfter3FramesList != NULL );
}

#ifdef PAGRAB_ENABLED
//////////////////////////////////////////////////////////////////////////////

void HWBreak(void)
{
}

//////////////////////////////////////////////////////////////////////////////

void SetHWBreakpointTrigger (void)
{
    unsigned int rg;

    asm volatile("sync.l");
    rg = 0x87828000;
    asm volatile("mtbpc %0" : : "r" (rg));// set breakpoint command register
    rg = (unsigned int)MasterList::HWBreak;
    asm volatile("mtiab %0" : : "r" (rg));// set breakpoint address
    rg = 0xffffffff;
    asm volatile("mtiabm %0" : : "r" (rg));// set breakpoint mask
    asm volatile("sync.p");
}
#endif


//#define SEMAPHORE_BASED
#define OS_DELAY_THREAD_BASED

#ifndef OS_DELAY_THREAD_BASED
static void DelayThreadCallbackX(int alarmID, u_short time, void* arg)
{
	#ifdef SEMAPHORE_BASED
	
	iSignalSema((int)arg);

	#else
	
	iWakeupThread((int)arg);

	#endif
}
#endif

static void DelayThread(u_int time)
{
	#ifdef OS_DELAY_THREAD_BASED
	::DelayThread(time*16);
	#else
		#ifdef SEMAPHORE_BASED
	
		SemaParam semaParam;
		semaParam.initCount = 1;
		semaParam.maxCount = 1;
		semaParam.option = 'MLst';
		semaParam.attr = 0;
		int delaySemaphoreID = CreateSema(&semaParam);
		SetAlarm(16*time, DelayThreadCallbackX, (void*)delaySemaphoreID);
		WaitSema(delaySemaphoreID);
		DeleteSema(delaySemaphoreID);
	
		#else
	
		SetAlarm(16*time, DelayThreadCallbackX, (void*)GetThreadId());
		SleepThread();
	
		#endif
	#endif
}


} // namespace MasterList
