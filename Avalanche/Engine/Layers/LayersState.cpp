/*************************************/
/*                                   */
/*   LayersState.cpp                 */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   layers state and init           */
/*                                   */
/*************************************/

/******************* includes ****************************/

#include "Layers/LayersPCH.h"
#include "Layers/MemoryConfig.h"
#include "Layers/MemPool.h"

#include "platform/gameapp.h"

#ifdef GCN
#include "dolphin/vm.h"
#endif

#include "Math/fastrand.h"

/******************* defines *****************************/

#define DEFAULT_DEBUG_START_LEVEL			1
#define DEFAULT_JOYSTICK_COUNT				1
#define DEFAULT_MAX_OPEN_FILES				1
#define DEFAULT_MAX_HOG_VOLUMES				8
#define DEFAULT_MAX_TASK_CONTROL_BLOCKS	32
#define DEFAULT_MAX_PROFILE_ENTRIES			50

#if !defined(CDROM)
//#define MEMORY_LOGGING
#endif

/******************* macros ******************************/

/******************* structures **************************/

//ATB moved ts_LayersState definition to l_LayersState.h

/******************* externs *****************************/

/******************* global variables ********************/

/******************* local variables *********************/

ts_LayersState LayersState = {
	
	/* memory */

	NULL,										// u8				*pu8LocalMemory;
	INVALID_HANDLE,						// t_Handle		MainMemoryHandle;
	INVALID_HANDLE,						// t_Handle		SecondaryMemoryHandle;
#ifdef GCN
	INVALID_HANDLE,						// t_Handle		ARAMMemoryHandle;
#endif

	/* debug system */

	DEFAULT_DEBUG_START_LEVEL,			// u8				u8DebugStartLevel;

	/* joystick */

	DEFAULT_JOYSTICK_COUNT,				// u32			u32JoystickCount;

	/* media (file i/o) */

	DEFAULT_MAX_OPEN_FILES,				// u32			u32MaxOpenFiles;
	DEFAULT_MAX_HOG_VOLUMES,			//	u32			u32MaxHogVolues;

	/* tasking */

	DEFAULT_MAX_TASK_CONTROL_BLOCKS,	// u32			u32MaxTaskControlBlocks;

	/* profiler */

	DEFAULT_MAX_PROFILE_ENTRIES,		// u32			u32MaxProfileEntries;

	/* init status, and to make sure we don't get out of synch with structure definition */

	LAYERS_NO_INIT							//	u32			u32InitStatus;
};

ts_MemoryConfig g_MemoryConfig =
{
	0,			// u32			Size;
	0,			// u32			OSReserve;
	0,			// u32			Secondary;
	0,			// u32			Handles;
	NULL,		// poolCfg*		MainRamPools;
	NULL,		// poolCfg*		AudioRamPools;
};

/******************* local prototypes ********************/

/******************* functions ***************************/

/* initialize layers stuff. if you want to change any of the default values, use the macros provided in l_State.h before
calling GameEngine__Init (which calls this routine) */

bool Layers__Init(void)
{
#ifdef WIN32
#ifndef CONSUMER_BUILD
	/* this assert is intended to make sure that this module is compiled with the same MMX_OK setting as the math */
	/* module. the mmx instructions require some alignment of matrices and vectors (__declspec(align(SIMD_ALIGN))). */
	/* if one module assumes alignment and another doesn't, all hell breaks loose. all modules that use math must */
	/* be compiled with same MMX_OK setting. the MMX_OK setting is done through project->settings->c/c++: the */
	/* preprocessor definitions should include MMX_OK=$(MMX_OK), and you should have the environment variable MMX_OK */
	/* set to 1 if your machine can handle SIMD, and 0 if not (older athlons) */

#if !defined(_XBOX)
	ASSERT(Math::MMXOK() == MMX_OK);
#endif	// XBOX
#endif //CONSUMER_BUILD
#endif //WIN32

	/* bail if already inited. there are situations where you want to initialize layers separately from the game */
	/* engine (to get file i/o up and running, for example) */

	if (LayersState.u32InitStatus == LAYERS_INIT)
		return(TRUE);

	/* make sure that our declaration of LayersState values matches struct definition */

	LayersState.u32InitStatus = LAYERS_INIT;

	/* initialize memory */

	if( !Layers__CreateHeap( ))
	{
		return( FALSE);
	}

	/* debug system-- not currently used (or indeed understood!!!) */

	dbgInit(LayersState.u8DebugStartLevel, TRUE);

	/* joystick */

	joyInit(LayersState.u32JoystickCount);

	// Tell the load profile stuff

	LoadProfile::Start();

	/* media (file i/o) */

	Hog_Initialize(LayersState.u32MaxHogVolumes, LayersState.u32MaxOpenFiles, GameApp::Application()->HogCheckLocal());
	Media::Init(LayersState.u32MaxOpenFiles);

	/* tasking system (not currently used) */

	tskInit(LayersState.u32MaxTaskControlBlocks);

	/* timer */

	EngineTimer__Init();

	/* init profiler */

#ifndef CONSUMER_BUILD
	Profiler__Init(LayersState.u32MaxProfileEntries);
#endif //CONSUMER_BUILD

	/* success */

	return(TRUE);
}

u32 Layers__SystemRAMAvailable( u32 max )
{
	#if defined( _XBOX )

		MEMORYSTATUS stat;
		GlobalMemoryStatus( &stat );
		u32 avail = stat.dwAvailPhys;
		if ( avail > max )
			avail = max;
		return avail;

	#elif defined( GCN )

		u32 avail = (u32)OSGetArenaHi() - (u32)OSGetArenaLo();
		if ( avail > max )
			avail = max;
		return avail;

	#else //PS2, PC

		// first try for max, in debug usually succeeds
		char* maxAlloc = (char*) malloc( max );
		if ( maxAlloc )
		{
			free( maxAlloc );
			return max;
		}
		else
		{
			// use binary search to compute memory available
			u32 lo = 1;
			u32 hi = max;
			while ( lo <= hi )
			{
				u32 m      = ( lo + hi ) / 2;
				char* buf  = (char*) malloc( m );
				if ( buf )
				{
					free( buf );
					lo = m + 1;
				}
				else
					hi = m - 1;
			}
			return hi;
		}

	#endif
}

void* Layers__SystemRAMAlloc( u32 size )
{
	#if defined( GCN )

		void* buf = (void*) OSGetArenaLo();
		OSSetArenaLo( (u32)buf + size );
		return buf;

	#else

		void* buf = (void*) malloc( size );
		ASSERT_PTR( buf );
		return buf;

	#endif
}

bool Layers__CreateHeap( void)
{
	// only create once
	if ( !Layers__HeapCreated() )
	{
		// force to use system memory manager?
		#ifdef MEM_USE_WIN32_MANAGER

		#ifdef PARANOIDHEAP
		// Get current flag
		int tmpFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );

		// Turn on leak-checking bit
		tmpFlag |= _CRTDBG_ALLOC_MEM_DF;
		tmpFlag |= _CRTDBG_CHECK_CRT_DF;
		tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;

		// Set flag to the new value
		_CrtSetDbgFlag( tmpFlag );
		#endif

		return true;
		#endif //MEM_USE_WIN32_MANAGER

		// fill in the default memory structure
		GetApplicationMemoryConfig( &g_MemoryConfig );

		// if app didn't fill in a size, assume most we can handle.
		if ( g_MemoryConfig.Size == 0 )
			g_MemoryConfig.Size = 128 * 1024 * 1024;

		// limit it by the most actually available
		g_MemoryConfig.Size = Layers__SystemRAMAvailable( g_MemoryConfig.Size );

		// set back by the amount reserved for the OS
		g_MemoryConfig.Size -= g_MemoryConfig.OSReserve;

		// final clamp to our memory manager maximum size
		if ( g_MemoryConfig.Size > 128 * 1024 * 1024 )
			g_MemoryConfig.Size = 128 * 1024 * 1024;

		// round down to even number
		g_MemoryConfig.Size &= ~15;
		ASSERT( g_MemoryConfig.Size > 0 );

		// get our buffer
		LayersState.pu8LocalMemory = (u8*) Layers__SystemRAMAlloc( g_MemoryConfig.Size );
		ASSERT_PTR( LayersState.pu8LocalMemory );

		int size1 = g_MemoryConfig.Size - g_MemoryConfig.Secondary;
		int size2 = g_MemoryConfig.Secondary;
		ASSERT( size1 > 0 );

		// create our heap
		memInit();
		LayersState.MainMemoryHandle = memAddRegion(
			LayersState.pu8LocalMemory,		// unsigned char *pStart
			size1,									// u32 u32Length
			0,											// u32 u32Flags
			"MAIN",									// char *pdesc
			g_MemoryConfig.Handles,				// u32 u32Handles
			FIT_FIRST,								// unsigned short usfitmethod
			g_MemoryConfig.MainRamPools );	// poolCfg *pPoolConfig

		if ( size2 > 0 )
		{
			static CMemPool s_secondaryPools[] =
			{
				POOLDEF( 16, 32 ),
				POOLDEF( 32, 32 ),
				POOLDEF( 64, 16 ),
				POOLDEF( 96, 16 ),
				POOLDEF(  0,  0 )
			};
			ASSERT( size2 > CMemPoolSet::Size( s_secondaryPools ) );
			LayersState.SecondaryMemoryHandle = memAddRegion(
				LayersState.pu8LocalMemory + size1,		// unsigned char *pStart
				size2,									// u32 u32Length
				0,											// u32 u32Flags
				"SECONDARY",							// char *pdesc
				g_MemoryConfig.Handles,				// u32 u32Handles
				FIT_FIRST,								// unsigned short usfitmethod
				s_secondaryPools );	// poolCfg *pPoolConfig
		}

		memSetActiveHandle( LayersState.MainMemoryHandle );

		// create secondary ram heap
		#ifdef GCN
		#if __option(cats)
			LayersState.ARAMMemoryHandle = LayersState.MainMemoryHandle;
		#else
			u32	vmStart = GCNApp::IAm( )->GetVMBase( );
			u32	vmSize = GCNApp::IAm( )->GetVMSize( );
			VMAlloc( vmStart, vmSize);
			LayersState.ARAMMemoryHandle = memAddRegion(
				(unsigned char *)vmStart,				// unsigned char *pStart
				vmSize,										// u32 u32Length
				0,												// u32 u32Flags
				"VIRTUAL",									// char *pdesc
				3072,											// u32 u32Handles
				FIT_LAST,	//FIT_BEST,					// unsigned short usfitmethod
				g_MemoryConfig.AudioRamPools)			// poolCfg *pPoolConfig
		#endif //cats
		#endif //gcn

		// show info
#ifndef CONSUMER_BUILD
		dbgPrint("Layers heap: 0x%08x to 0x%08x, %d bytes, %d handles, %d bytes available for malloc\n",
			(int) LayersState.pu8LocalMemory,
			(int) LayersState.pu8LocalMemory + g_MemoryConfig.Size - 1,
			g_MemoryConfig.Size,
			g_MemoryConfig.Handles,
			memGetRegionLength(LayersState.MainMemoryHandle));
#endif
	}

	return true;
}

/* reset */

void Layers__Reset(void)
{
#ifndef CONSUMER_BUILD
	Profiler__Reset();
#endif //CONSUMER_BUILD
	tskReset();
//	EngineTimer__Reset();
//yuch-- need to write reset routines for commented-out systems
//	medReset();
//	joyReset();
//	dbgReset();

	// Tell the load profile stuff
	LoadProfile::Reset();
}

/* shutdown */

void Layers__Shutdown(void)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_INIT);	// if this hits, it means Layers__Init was never called by GameEngine__Init
	LayersState.u32InitStatus = LAYERS_NO_INIT;

	/* shutdown systems */

#ifndef CONSUMER_BUILD
	Profiler__Shutdown();
#endif //CONSUMER_BUILD
	EngineTimer__Shutdown();
	tskShutdown();
//yuch-- need to write shutdown routines for commented-out systems
//	medShutdown();
//	joyShutdown();
//	dbgShutdown();

	/* Close the hog */
	Hog_Terminate();

	/* can be useful for debugging memory leaks */

//	Layers__ShowMemoryState();

	/* free heap. do not set bHeapCreated to FALSE here-- it is used to check if the heap was ever created, */
	/* (in Layers__HeapEverCreated) to check whether it is currently in existence */

#ifndef CONSUMER_BUILD
	Layers__CheckPermanentMemory();
#endif

// don't shut down right now, it needs to hang on while the deconstructors are called
//	if( LayersState.pu8LocalMemory)
//	{
//		free( LayersState.pu8LocalMemory);
//		LayersState.pu8LocalMemory = NULL;
//	}
//	LayersState.MainMemoryHandle = INVALID_HANDLE;

	Media::Terminate();
}

/* routines to update default initialization values before init is called */

void Layers__SetDebugStartLevel(u8 u8DebugStartLevel)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u8DebugStartLevel = u8DebugStartLevel;
}
void Layers__SetJoystickCount(u32 u32JoystickCount)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u32JoystickCount = u32JoystickCount;
}
void Layers__SetMaxOpenFiles(u32 u32MaxOpenFiles)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u32MaxOpenFiles = u32MaxOpenFiles;
}
void Layers__SetMaxOpenHogs(u32 u32MaxHogVolumes)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u32MaxHogVolumes= u32MaxHogVolumes;
}
void Layers__SetMaxTaskControlBlocks(u32 u32MaxTaskControlBlocks)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u32MaxTaskControlBlocks = u32MaxTaskControlBlocks;
}
void Layers__SetMaxProfileEntries(u32 u32MaxProfileEntries)
{
	ASSERT(LayersState.u32InitStatus == LAYERS_NO_INIT);	// if this hits, then Layers__Init has already been called, and the update has no effect
	LayersState.u32MaxProfileEntries = u32MaxProfileEntries;
}

/* get free and allocated memory stats for Heap Info debug menu item */

void Layers__GetMemoryStats(int *pnFreeBytes, int *pnFreeHandles, int* pnLargestFree, int *pnAllocBytes, int *pnAllocHandles)
{
	memGetRegionStats(LayersState.MainMemoryHandle, pnFreeBytes, pnFreeHandles, pnLargestFree, pnAllocBytes, pnAllocHandles);
}

/* show memory manager state */

#ifndef CONSUMER_BUILD
void Layers__ShowMemoryState(void)
{
	/* have to wrap this into LayersState since MainMemoryHandle is not publically available */

	if (LayersState.MainMemoryHandle != INVALID_HANDLE)
		memShowRegionInfo(LayersState.MainMemoryHandle);
#ifdef GCN
	if( LayersState.ARAMMemoryHandle != INVALID_HANDLE)
		memShowRegionInfo( LayersState.ARAMMemoryHandle);
#endif
}

void Layers__BriefShowMemoryState(void)
{
#ifdef MEM_DEBUG_BLOCKS
	/* have to wrap this into LayersState since MainMemoryHandle is not publically available */

	if (LayersState.MainMemoryHandle != INVALID_HANDLE)
		memShowRegionBriefInfo(LayersState.MainMemoryHandle);
#ifdef GCN
	if( LayersState.ARAMMemoryHandle != INVALID_HANDLE)
		memShowRegionBriefInfo(LayersState.ARAMMemoryHandle);
#endif
	// TJC - this doesnt really apply to the new texture manager currently
	// but probably will in the future.
#endif
}

void Layers__ShowFreeMemory(void)
{
#ifdef MEM_DEBUG_BLOCKS
	/* have to wrap this into LayersState since MainMemoryHandle is not publically available */

	if (LayersState.MainMemoryHandle != INVALID_HANDLE)
		memShowRegionFreeSize(LayersState.MainMemoryHandle);
#endif
}

void Layers__ShowAllocMemory(void)
{
#ifdef MEM_DEBUG_BLOCKS
	/* have to wrap this into LayersState since MainMemoryHandle is not publically available */

	if (LayersState.MainMemoryHandle != INVALID_HANDLE)
		memShowRegionAllocSize(LayersState.MainMemoryHandle);
#endif
}

// DEBUG DEBUG DEBUG
int sessionID = 0;
void FreeSpaceCheck()
{
#if defined(MEMORY_LOGGING)
	// block dump
	static int count = 0;
	char filename[ 32 ];
	if (sessionID == 0)
		sessionID = g_random.InRange(10000, 99999);
	sprintf( filename, "%d-perm-%d", sessionID, count++ );
	extern void memDumpBlockInfo( const char* filename );
	memDumpBlockInfo( filename );

	// free space check
	static int lastFreeB = 0, lastFreeMax = 0, lastFreeH = 0;
	int freeB, freeMax, freeH, allocB, allocH;
	memGetRegionStats(LayersState.MainMemoryHandle, &freeB, &freeH, &freeMax, &allocB, &allocH);
	dbgPrint("FreeSpaceCheck: FREE: %d  LARGEST:%d  HANDLES:%d\n", freeB, freeMax, freeH);
	if ((lastFreeB && freeB != lastFreeB) || (lastFreeMax && freeMax != lastFreeMax))
	{
		dbgPrint("    last      : FREE: %d  LARGEST:%d  HANDLES:%d\n", lastFreeB, lastFreeMax, lastFreeH);
		ASSERTF(false, ("lost memory between checks: free(%d,%d) was(%d,%d)", freeB, freeMax, lastFreeB, lastFreeMax));
	}
	lastFreeB   = freeB;
	lastFreeH   = freeH;
	lastFreeMax = freeMax;
#endif
}
// DEBUG DEBUG DEBUG

bool Layers__MarkPermanentMemory(void)
{
	/* have to wrap this into LayersState since MainMemoryHandle is not publically available */

	// Tell the load profile stuff
	LoadProfile::DonePermanent();

#ifdef GCN
	if (LayersState.ARAMMemoryHandle != INVALID_HANDLE)
		memMarkPermanentMemory(LayersState.ARAMMemoryHandle);
#endif
	if (LayersState.MainMemoryHandle != INVALID_HANDLE)
		memMarkPermanentMemory(LayersState.MainMemoryHandle);

	FreeSpaceCheck();

	return(TRUE);
}

bool Layers__CheckPermanentMemory(void)
{
	bool retVal = true;
	if ( LayersState.MainMemoryHandle != INVALID_HANDLE )
	{
		retVal = memCheckPermanentMemory( LayersState.MainMemoryHandle );
	}

#ifdef GCN
	if ( LayersState.ARAMMemoryHandle != INVALID_HANDLE &&
		  LayersState.ARAMMemoryHandle != LayersState.MainMemoryHandle )
	{
		// if first permanent memory check failed, we want to return false even if this one succeeds
		retVal &= memCheckPermanentMemory( LayersState.ARAMMemoryHandle);
	}
#endif

	FreeSpaceCheck();

#ifdef SOMEDAYTHISWILLWORKAGAIN
	ASSERTS(retVal, "Permanent memory check failed, see debug output");
#endif

	return(retVal);
}

static u32 MaxLayersUsage=0, MaxLayersTotal=0, MaxCHeapUsage=0, MaxCHeapTotal=0, MaxHandles=0;
void Layers__CheckMemoryState(int nPrintRequest)
{
#ifndef CDROM
	u32 LayersUsage=0, LayersTotal=0, CHeapUsage=0, CHeapTotal=0, Handles=0;
	bool bNewMax=false;

	memCheckHeapUsage(LayersState.MainMemoryHandle, &LayersUsage, &LayersTotal, &CHeapUsage, &CHeapTotal, &Handles);

	if(LayersUsage > MaxLayersUsage)
	{
		MaxLayersUsage = LayersUsage;
		bNewMax = true;
	}

	if(LayersTotal > MaxLayersTotal)
	{
		MaxLayersTotal = LayersTotal;
		bNewMax = true;
	}

	if(CHeapUsage > MaxCHeapUsage)
	{
		MaxCHeapUsage = CHeapUsage;
		bNewMax = true;
	}

	if(CHeapTotal > MaxCHeapTotal)
	{
		MaxCHeapTotal = CHeapTotal;
		bNewMax = true;
	}

	if(Handles > MaxHandles)
	{
		MaxHandles = Handles;
		bNewMax = true;
	}

	/* print request? */

	bool bPrint = false;
	switch(nPrintRequest)
	{
	case CHECK_MEMORY_STATE_PRINT_ALWAYS:
		bPrint = true;
		break;
	case CHECK_MEMORY_STATE_PRINT_IF_HIGHER:
		bPrint = bNewMax;
		break;
	}
	if (bPrint)
	{
		printf("New Max Heap: LayersUse = 0x%x, LayersTotal = 0x%x, CHeapUse = 0x%x, CHeapTotal = 0x%x, Handles = %d\n",
						MaxLayersUsage, MaxLayersTotal, MaxCHeapUsage, MaxCHeapTotal, MaxHandles);
		dbgPrint("New Max Heap: LayersUse = 0x%x, LayersTotal = 0x%x, CHeapUse = 0x%x, CHeapTotal = 0x%x, Handles = %d\n",
						MaxLayersUsage, MaxLayersTotal, MaxCHeapUsage, MaxCHeapTotal, MaxHandles);
	}
#endif //CDROM
}

/* get values calculated by Layers__CheckMemoryState */

void Layers__GetMemoryMaxStats(int *pnMaxLayersUsage, int *pnMaxLayersTotal, int *pnMaxLayersHandles, int *pnMaxCHeapUsage, int *pnMaxCHeapTotal)
{
	*pnMaxLayersUsage = MaxLayersUsage;
	*pnMaxLayersTotal = MaxLayersTotal;
	*pnMaxCHeapUsage = MaxCHeapUsage;
	*pnMaxCHeapTotal = MaxCHeapTotal;
	*pnMaxLayersHandles = MaxHandles;
}

/* turn memory high water check on/off */

void Layers__TrackMemoryHighWater(bool bState)
{
	memTrackHighWater(LayersState.MainMemoryHandle, bState);
}

/* get high water check status */

bool Layers__MemoryHighWaterOn(void)
{
	return(memHighWaterOn(LayersState.MainMemoryHandle));
}

/* reset high water test before each world is loaded */

void Layers__ResetMemoryHighWater(void)
{
	memResetHighWater(LayersState.MainMemoryHandle);
}

/* get high water marks */

void Layers__GetMemoryHighWaterStats(int *pnMinBytes, int *pnMaxBytes)
{
	memGetHighWater(LayersState.MainMemoryHandle, pnMinBytes, pnMaxBytes);
}

/* get real available size of heap, excluding memory handle blocks */

int Layers__GetMemorySize()
{
	return(memGetRegionLength(LayersState.MainMemoryHandle));
}
#endif //CONSUMER_BUILD
