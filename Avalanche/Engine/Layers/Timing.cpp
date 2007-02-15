/*************************************/
/*                                   */
/*   Timing.cpp                      */
/*   new engine  03/20/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   layers timing module            */
/*                                   */
/*************************************/

/* some simple timing stuff, meant to allow a sort of profiling analysis. real game timing code should also go in here */

/******************* includes ****************************/

#include "Layers/LayersPCH.h"

#include "game/managers/scenemgr.h"

#include "cmdcon/cmdcon.h"
#include "EngineHelper/filestream.h"
#if defined (WIN32) && !defined (_XBOX)
#include <wtypes.h>	//for LARGE_INTEGER
#endif

/******************* defines *****************************/

/* engine timer */

#define ENGINE_TIMER_ENTRIES		15					// number of entries to average over
#define MAX_SECS_PER_FRAME			(1.0f / 10.0f)	// cap frame rate scaler

/* for profiling */

#define MAX_PROFILE_NAME_LENGTH	64
#define PROFILE_AVERAGE_ENTRIES	15
#define PROFILE_HISTORY_ENTRIES	100
#define PROFILE_STACK_DEPTH		10

#ifdef PS2
/* for sony conversion from cpu ticks to hblanks */

#define CPU_CYCLES_PER_SECOND		294912000.0f	// roughly 300 mhz
#define VBLANKS_PER_SECOND			59.94f
#define HBLANKS_PER_VBLANK			262.5f
#define HBLANKS_PER_SECOND			(HBLANKS_PER_VBLANK * VBLANKS_PER_SECOND)
#define CPU_CYCLES_PER_HBLANK		(CPU_CYCLES_PER_SECOND / HBLANKS_PER_SECOND)

#endif //PS2

#ifdef WIN32
#define BAR_ALPHA	128
#elif defined(PS2)
#define BAR_ALPHA	64
#elif defined(GCN)
#define BAR_ALPHA	128
#endif //WIN32/PS2

/******************* macros ******************************/

/* for profiling-- ps2 uses high resolution cpu tick counter */

#ifdef WIN32
#define CURRENT_TICKS		u64Ticks.QuadPart
#define LAST_TICKS			u64LastTicks.QuadPart
#define TICKS_PER_SECOND	u64Frequency.QuadPart
#define START_TICKS			u64StartTicks.QuadPart
#elif defined(PS2)
#define CURRENT_TICKS		u64Ticks
#define LAST_TICKS			u64LastTicks
#define TICKS_PER_SECOND	CPU_CYCLES_PER_SECOND
#elif defined(GCN)
#define CURRENT_TICKS		u64Ticks
#define LAST_TICKS			u64LastTicks
#define TICKS_PER_SECOND	OSSecondsToTicks( 1)
#endif //WIN32/PS2

/******************* structures **************************/

struct ts_ProfileEntry
{
	u32	u32Ticks[PROFILE_AVERAGE_ENTRIES];			// the last PROFILE_AVERAGE_ENTRIES entries for moving average
	u32	u32HistoryTicks[PROFILE_HISTORY_ENTRIES];	// the last PROFILE_HISTORY_ENTRIES entries for history report
#ifdef WIN32
	LARGE_INTEGER	u64LastTicks;										// ticks value when this slot becomes active in profiling
#else
	u64	u64LastTicks;										// ticks value when this slot becomes active in profiling
#endif
	u32	u32Type;												// currently PROFILE_NORMAL or PROFILE_ASYNC
	char	Name[MAX_PROFILE_NAME_LENGTH];				// name of this collection entry

	/* for log file */

	u32	u32MinTicks;
	u32	u32MaxTicks;
	float	fAverageTicks;
};

/******************* externs *****************************/

/******************* global variables ********************/

// JMH - kind of hacky - setting up for a generic demo mode
bool g_demoPlaybackMode = false;
u32 g_demoPlaybackTime = 0;

float SecondsPerFrame = (1.0f / 60.0f);	// this is actually an average over the last ENGINE_TIMER_ENGINE frames, and capped at MAX_SECS_PER_FRAME
#if defined(PS2) && !defined(CONSUMER_BUILD)
static u32 sleepStart;
#endif
/******************* local variables *********************/

/* engine timer */

static u32 u32EngineTimerIndex;
static u32 u32EngineTimer[ENGINE_TIMER_ENTRIES];
u32 u32Milliseconds = 0;
#ifdef WIN32
static LARGE_INTEGER u64Ticks;		// for high-accuracy profiling
static LARGE_INTEGER u64Frequency;	// for pc, ps2 is at a fixed frequency
static LARGE_INTEGER u64StartTicks;	
#else
static u64 u64Ticks;		// for high-accuracy profiling
static u64 u64Frequency;	// for pc, ps2 is at a fixed frequency
static u64 u64StartTicks;	
#endif
static u32 u32GameStartTime;	//time since the game started
#ifdef PS2
static u32 u32LastCPUCycles = 0;
#endif //PS2

/* profiler */

#ifndef CONSUMER_BUILD
static ts_ProfileEntry *pProfileList = NULL;
static u32 *pu32EntryTranslate = NULL;
static int nMaxProfileEntryCount = 0;					// record passed parameter to Profiler__Init
static int nHighSlotIndex = 0;							// the highest value passed to Profiler__ReserveSlot
static int nActiveProfileEntryCount;					// number of active slots
static bool	bProfilingOn;
static int ProfileStack[PROFILE_STACK_DEPTH];
static int nProfileStackIndex;							// index into slot stack-- Profiler__Start pushed, Profile__Stop pops
static int nTicksIndex = 0;								// which slot in u32Ticks array we are filling this time
static int nHistoryTicksIndex = 0;						// which slot in u32HistoryTicks array we are filling this time
static bool bHistoryIndexWrapped = FALSE;				// whether history index has wrapped yet
static bool bListReservedSlots = FALSE;
static ts_bRGBA BarColorList[6] =
{
	{255,   0, 255, BAR_ALPHA},
	{255,   0,   0, BAR_ALPHA},
	{  0, 255,   0, BAR_ALPHA},
	{  0,   0, 255, BAR_ALPHA},
	{255, 255,   0, BAR_ALPHA},
	{  0, 255, 255, BAR_ALPHA},
};
static char *BarColorName[6] = 
{
	"purple  ",
	"red     ",
	"green   ",
	"blue    ",
	"yellow  ",
	"lt. blue",
};

#endif //CONSUMER_BUILD

/******************* local prototypes ********************/

static void Profiler__ReserveFirstSlots(void);
static void Profiler__GetDeltaTicks(int nSlotIndex);

/******************* functions ***************************/

/* initialize the engine timer. registers a function to be called once per loop */

void EngineTimer__Init(void)
{
	int i;
	static bool bFirstInit = TRUE;

#ifdef WIN32
	QueryPerformanceCounter((LARGE_INTEGER *) &CURRENT_TICKS);
	ASSERT(CURRENT_TICKS != 0);	// result = 0 means no performance counter, we assume there is one
	START_TICKS = CURRENT_TICKS;
	QueryPerformanceFrequency(&u64Frequency);
#elif defined(PS2)
	CURRENT_TICKS = 0;
#elif defined(GCN)
	CURRENT_TICKS = OSGetTime( );
#endif //PS2

	u32Milliseconds = EngineTimer__Time();

	if (bFirstInit)
	{
		u32GameStartTime = u32Milliseconds;
		bFirstInit = FALSE;
	}

	/* initialize timer array */

	u32EngineTimerIndex = 0;
	for (i = 0; i < ENGINE_TIMER_ENTRIES; i++)
		u32EngineTimer[i] = u32Milliseconds;
}

/* returns number of miliseconds since the game started */
u32 EngineTimer__GameTime(u32 *hours, u32 *minutes, u32 *seconds, u32 *thousandths)
{
	u32 curTime = EngineTimer__Time();
	u32 gameTime = curTime - u32GameStartTime;

	*hours = gameTime / (60*60*1000);
	gameTime -= *hours * (60*60*1000);
	*minutes = gameTime / (60*1000);
	gameTime -= *minutes * (60*1000);
	*seconds = gameTime / (1000);
	gameTime -= *seconds * 1000;
	*thousandths = gameTime;

	return (curTime - u32GameStartTime);
}

/* reset */

void EngineTimer__Reset(void)
{
	/* just re-init it */

	EngineTimer__Init();
}

/* shutdown */

void EngineTimer__Shutdown(void)
{
	// nothing to do
}

/* a routine to get the current time in milliseconds. also sets the u32Milliseconds value for use without getting
current time */

u32 EngineTimer__Time(void)
{
	if ( g_demoPlaybackMode )
	{
		return g_demoPlaybackTime;
	}

	#if defined( WIN32 )
	
		QueryPerformanceCounter((LARGE_INTEGER *) &CURRENT_TICKS);
		LARGE_INTEGER adjustedTime;
		adjustedTime.QuadPart = CURRENT_TICKS - START_TICKS;
		u32Milliseconds = (int) ((double) adjustedTime.QuadPart * 1000.0 / (double) TICKS_PER_SECOND);

	#elif defined( PS2 )

		u32 u32CPUCycles = 0;
		asm __volatile__("mfc0 %0,$9" : "=r" (u32CPUCycles));	// value in cpu cycles (300MHz)

		#if 0
			// TJC - the code below uses aliasing which is rarely a good idea.
			// TJC - this does a 64 bit divide, which is quite expensive in such a low level function.
			static u32 *pu32Ticks = (u32 *) &CURRENT_TICKS;	// break the 64-bit word up into 2 32-bit words
			if (u32CPUCycles < u32LastCPUCycles)	//wrapped? (once every 14.3 seconds or so)
				pu32Ticks[1] += 1;	// increment high word
			u32LastCPUCycles = u32CPUCycles;
			pu32Ticks[0] = u32CPUCycles;	// replace low word
			u32Milliseconds = (unsigned long)(CURRENT_TICKS * 1000) / (unsigned long) TICKS_PER_SECOND;
		#else // 0
			// this version just relys on the fact that a 32 bit unsigned subtraction handles
			// the wraparound case for us.
			CURRENT_TICKS += u32CPUCycles - u32LastCPUCycles;
			u32LastCPUCycles = u32CPUCycles;
			// instead of dividing I reduce the 18 bits of precision we're going to loose right off the 
			// top with the divide (18 because TICKS_PER_SECOND / 2^18 is still a whole number)
			u64 bigticks = (CURRENT_TICKS >> 18);
			// this makes it so that we are now only dividing by 1125, but we also want to multiply by 1000 to get
			// milliseconds.  so 
			// 1000/1125 is .88888888   Im going to move that into a 0.32 fixed point number which is 0xE38E38E3.  
			// Multiply by that and I get a fixed point number back 
			bigticks *= 0xE38E38E3;
			// again 32 bits of fraction... (since my original was 32.0, and the multiplier was
			// 0.32, the result is 0.32)
			// well, I dont really need the fraction... :)
			bigticks >>= 32;
			// ok then what I have left should be the milliseconds...
			u32Milliseconds = (u32)bigticks;
		#endif // 0

	#elif defined( GCN )

		CURRENT_TICKS = OSGetTime( );
		u32Milliseconds = OSTicksToMilliseconds( CURRENT_TICKS);

	#endif

	return(u32Milliseconds);
}

/* quick check for ps2 timer wrapping, called from vblank interrupt to handle accurate timing during long file loads */

#ifdef PS2
void EngineTimer__CheckWrap(void)
{
	u32 u32CPUCycles = 0;
	asm __volatile__("mfc0 %0,$9" : "=r" (u32CPUCycles));	// value in cpu cycles (300MHz)
	if (u32CPUCycles < u32LastCPUCycles)	//wrapped? (once every 14.3 seconds or so)
		EngineTimer__Time();	// handle wrapping by calling normal routine
}
#endif //PS2

/* this routine is called once per loop, and calculates the average frame time over the last ENGINE_TIMER_ENTRIES frames */

void EngineTimer__Advance(void)
{
	/* grab current milliseconds value (load u32Milliseconds) */

	u32 currentTime = EngineTimer__Time();

	/* calculate capped average frame rate */

	SecondsPerFrame = (float) (currentTime - u32EngineTimer[u32EngineTimerIndex]) / ((float) ENGINE_TIMER_ENTRIES * 1000.0f);

//#ifdef WIN32 //djr: the following cap is a problem on PS2, especially when measuring performance, so I set it to WIN32 only
	if (SecondsPerFrame > MAX_SECS_PER_FRAME)
		SecondsPerFrame = MAX_SECS_PER_FRAME;
//#endif

	/* record current time in current slot. this slot will be used again in seconds-per-frame calculation */
	/* in ENGINE_TIMER_ENTRIES frames */

	u32EngineTimer[u32EngineTimerIndex] = currentTime;
	if (++u32EngineTimerIndex == ENGINE_TIMER_ENTRIES)
		u32EngineTimerIndex = 0;

	/* maybe spit out in text form */

//#define TEXT_FRAME_RATE
#ifdef CONSUMER_BUILD
#undef TEXT_FRAME_RATE
#endif //CONSUMER_BUILD
#ifdef TEXT_FRAME_RATE
{
	static int u32LastMilliseconds = 0;
	if (currentTime - u32LastMilliseconds > 2000)
	{
		char acString[20];
		sprintf(acString, "%.2f Hz\n", 1.0f / SecondsPerFrame);
		conPuts(acString);
		u32LastMilliseconds = currentTime;
	}
}
#endif //TEXT_FRAME_RATE
}

/* prepare for sleeping */
void EngineTimer__StartSleep()
{
#if defined(PS2) && !defined(CONSUMER_BUILD)
	asm __volatile__("mfc0 %0,$9" : "=r" (sleepStart));	// value in cpu cycles (300MHz)
#endif
}

/* done sleeping */
void EngineTimer__EndSleep()
{
#if defined(PS2) && !defined(CONSUMER_BUILD)
	u32 sleepEnd;
	asm __volatile__("mfc0 %0,$9" : "=r" (sleepEnd));	// value in cpu cycles (300MHz)
	SceneMgr::GetStats().IncProcessSleepuSec( (sleepEnd - sleepStart) / 295 );	//record cpu cycles slept
#endif
}

#ifndef CONSUMER_BUILD

/* initialize the "profiler"-- not really a profiler, just a way of timing things */

#ifndef Profiler__Init
void Profiler__Init(u32 u32EntryCount)
{
	ASSERT(pProfileList == NULL);	// this routine should only be called once
	if (u32EntryCount == 0)
		return;
	if (u32EntryCount < PROFILE_UNRESERVED_START)
		nMaxProfileEntryCount = PROFILE_UNRESERVED_START;
	else
		nMaxProfileEntryCount = u32EntryCount;
	MEM_SET_ALLOC_NAME("Profile");
	pProfileList = (ts_ProfileEntry *) memAlloc(nMaxProfileEntryCount * sizeof(ts_ProfileEntry));
	memset(pProfileList, 0, nMaxProfileEntryCount * sizeof(ts_ProfileEntry));
	MEM_SET_ALLOC_NAME("Profiler Entry Array");
	pu32EntryTranslate = (u32 *) memAlloc(nMaxProfileEntryCount * sizeof(u32));
	nActiveProfileEntryCount = 0;
	bProfilingOn = FALSE;
	bListReservedSlots = FALSE;
	nTicksIndex = 0;
	nHistoryTicksIndex = 0;
	bHistoryIndexWrapped = FALSE;

	/* first entries are reserved */

	Profiler__ReserveFirstSlots();
}
#endif

/* reset */

#ifndef Profiler__Reset
void Profiler__Reset(void)
{
	ASSERT(pProfileList != NULL);	// must have been inited first

	/* clear list */

	memset(pProfileList, 0, nMaxProfileEntryCount * sizeof(ts_ProfileEntry));
	nActiveProfileEntryCount = 0;
	bProfilingOn = FALSE;
	bListReservedSlots = FALSE;
	nTicksIndex = 0;
	nHistoryTicksIndex = 0;
	bHistoryIndexWrapped = FALSE;

	/* first entries are reserved */

	Profiler__ReserveFirstSlots();
}
#endif // Profiler__Reset

static void Profiler__ReserveFirstSlots(void)
{
	nHighSlotIndex = 0;
	Profiler__ReserveSlot(PROFILE_ENTIRE_LOOP, "Entire loop", PROFILE_ASYNC);
#ifdef GCN
	Profiler__ReserveSlot(PROFILE_RENDER_ASYNC, "Render Async", PROFILE_NORMAL);
#else
	Profiler__ReserveSlot(PROFILE_RENDER_ASYNC, "Render Async", PROFILE_ASYNC);
#endif
	Profiler__ReserveSlot(PROFILE_VSYNC, "Vsync", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_RENDER_IMMEDIATE, "Render (immediate)", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_COLLISION, "Collision tests", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_SPRITE_RENDER, "Sprite Render", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_PARTICLE_RENDER, "Particle Render", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_WALK_SCENE_TREE, "Walk scene tree", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_RENDER_BUCKETS, "Render buckets", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_RENDER_SHADOWS, "Render shadows", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_CREATE_SHADOWS, "Create shadows", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_BUILD_MASTER_LIST, "Master list", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_COPY_BONES, "Copy bones", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_MOTION_PROCESSING, "Motion Processing", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_SCRIPT_PROCESSING, "Script Processing", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_ANIHF, "Animated height fields", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_FILE_LOAD, "File load", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_DBL_PARSE, "DBL parse", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_NETWORK_UPDATE, "Network Update", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_VOICE_UPDATE, "Voice Update", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_ENGINE_ADVANCE, "Engine advance", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_RENDER_MANAGER, "Render manager", PROFILE_NORMAL);
	Profiler__ReserveSlot(PROFILE_GAME_LOGIC, "Game logic", PROFILE_NORMAL);
}

/* shutdown */

#ifndef Profiler__Shutdown
void Profiler__Shutdown(void)
{
	ASSERT(pProfileList);	// if this hits, Profiler__Init was never hit
	if (pProfileList)
	{
		memFree(pProfileList);
		pProfileList = NULL;
	}
	if (pu32EntryTranslate)
	{
		memFree(pu32EntryTranslate);
		pu32EntryTranslate = NULL;
	}
}
#endif // Profiler__Shutdown

/* start profiling */

#ifndef Profiler__StartLoop
void Profiler__StartLoop(void)
{
	int i;

	/* maybe nothing to do */

	if (pProfileList == NULL)
		return;

	/* spit out bar colors for reference. do it here so that slots only get printed out once, and not at */
	/* all if profiler is not being used */

	if (bListReservedSlots)	// gets set by Profiler__ReserveSlot when a new slot is reserved
	{
		dbgPrint("Profiler timing bars:\n");
		g_console.Echo("Profiler timing bars:");
		for (i = 0; i < (signed) nActiveProfileEntryCount; i++)
		{
			dbgPrint("   %s: %s\n", BarColorName[i % 6], pProfileList[pu32EntryTranslate[i]].Name);
			g_console.Echo("   %s: %s", BarColorName[i % 6], pProfileList[pu32EntryTranslate[i]].Name);
		}
		bListReservedSlots = FALSE;
	}

	/* finish previous loop, if any */

	if (bProfilingOn)
	{
		ASSERT(nProfileStackIndex == 0);		// if this hits, then somewhere there is an unbalanced Profile_Start or Profile__Stop call
		Profiler__Stop(PROFILE_ENTIRE_LOOP);	// async, no stack effect

		/* copy current slot tick counts to history */

		for (i = 0; i < (signed) nHighSlotIndex; i++)
			pProfileList[i].u32HistoryTicks[nHistoryTicksIndex] = pProfileList[i].u32Ticks[nTicksIndex];
		nHistoryTicksIndex++;
		if (nHistoryTicksIndex >= PROFILE_HISTORY_ENTRIES)
		{
			nHistoryTicksIndex = 0;
			bHistoryIndexWrapped = TRUE;
		}
	}
	bProfilingOn = TRUE;

	/* update slot to use in timing entries */

	nTicksIndex++;
	if (nTicksIndex >= PROFILE_AVERAGE_ENTRIES)
		nTicksIndex = 0;
	for (i = 0; i < (signed) nHighSlotIndex; i++)
		pProfileList[i].u32Ticks[nTicksIndex] = 0;

	/* reset stack, start entire-loop timer */

	nProfileStackIndex = 0;
	Profiler__Start(PROFILE_ENTIRE_LOOP);	// async, no stack effect
}
#endif // Profiler__StartLoop

/* force profiling to stop, typically, once it's on it stays on, but for profiling SimLoop::Initialize, we'
profile to get timing data then turn it off and wait for user to request it through debug menus */


#ifndef Profiler__StopLoop
void Profiler__StopLoop()
{
	if (bProfilingOn)
	{
		/* take care of recording measurements taken so far */

		Profiler__StartLoop();

		/* and stop */

		bProfilingOn = false;
	}
}
#endif // Profiler__StopLoop

/* reserve a profiler slot */

#ifndef Profiler__ReserveSlot
bool Profiler__ReserveSlot(int nSlotIndex, char *pName, u32 u32Type)
{
	/* maybe nothing to do */

	if (pProfileList == NULL)
		return(FALSE);

	if (nSlotIndex >= nMaxProfileEntryCount)
	{
		ASSERT(FALSE);	// beyond max slots
		return(FALSE);
	}
	if (strlen(pProfileList[nSlotIndex].Name) != 0)
	{	// slot already taken
		if ((strcmp(pProfileList[nSlotIndex].Name, pName) == 0) && (pProfileList[nSlotIndex].u32Type == u32Type))
		{	// trying to reserve the slot again - no big deal
			return(TRUE);
		}
		else
		{	// slot already taken, and new slot is different
			ASSERT(FALSE);
			return(FALSE);
		}
	}
	strncpy(pProfileList[nSlotIndex].Name, pName, MAX_PROFILE_NAME_LENGTH);
	ASSERT((u32Type == PROFILE_NORMAL) || (u32Type == PROFILE_ASYNC));
	pProfileList[nSlotIndex].u32Type = u32Type;

	/* keep track of order slots were reserved so i can spit them out in order for timing bars */

	pu32EntryTranslate[nActiveProfileEntryCount++] = nSlotIndex;

	/* record highest index found to cut down loop iterations */

	if (nSlotIndex + 1 > nHighSlotIndex)
		nHighSlotIndex = nSlotIndex + 1;

	/* spit out bar colors for reference */

	bListReservedSlots = TRUE;	// will get printed next time Profiler__StartLoop is called

	/* done */

	return(TRUE);
}
#endif // Profiler__ReserveSlot

/* start timing for passed slot, push previous slot onto stack */

#ifndef Profiler__Start
void Profiler__Start(int nSlotIndex)
{
	/* maybe nothing to do */

	if ((pProfileList == NULL) || (nSlotIndex == PROFILE_INVALID_SLOT))
		return;
	if (!bProfilingOn)	// haven't called Profiler__StartLoop yet
		return;

	/* must have an index pointer */

	ASSERT(nSlotIndex < nMaxProfileEntryCount);
	ASSERT(strlen(pProfileList[nSlotIndex].Name) != 0);	// if this hits, slot was not reserved

	/* call EngineTimer__Time to get current time */

	EngineTimer__Time();

	/* async slots just record start time and leave-- no stack manipulation */

	if (pProfileList[nSlotIndex].u32Type == PROFILE_ASYNC)
	{
		pProfileList[nSlotIndex].LAST_TICKS = CURRENT_TICKS;
		return;
	}

	/* record delta time for currently active slot on stack if there is one */

	if (nProfileStackIndex > 0)
		Profiler__GetDeltaTicks(ProfileStack[nProfileStackIndex - 1]);

	/* add new slot to stack, record current time */

	ASSERT(nProfileStackIndex < PROFILE_STACK_DEPTH);
	ProfileStack[nProfileStackIndex++] = nSlotIndex;
	pProfileList[nSlotIndex].LAST_TICKS = CURRENT_TICKS;
}
#endif // Profiler__Start

/* finish timing for current slot, pop stack */

#ifndef Profiler__Stop
void Profiler__Stop(int nSlotIndex)
{
	/* maybe nothing to do */

	if ((pProfileList == NULL) || (nSlotIndex == PROFILE_INVALID_SLOT))
		return;
	if (!bProfilingOn)	// haven't called Profiler__StartLoop yet
		return;

	/* must have an index pointer */

	ASSERT(nSlotIndex < nMaxProfileEntryCount);
	ASSERT(strlen(pProfileList[nSlotIndex].Name) != 0);	// if this hits, slot was not reserved

	/* check stack integrity */

#ifdef _DEBUG
	if (pProfileList[nSlotIndex].u32Type == PROFILE_NORMAL)
	{
		ASSERT(nProfileStackIndex > 0);	// if this hits there is an unbalanced Profiler__Start/Profiler__Stop
		ASSERT(nSlotIndex == ProfileStack[nProfileStackIndex - 1]);	// if this hits, then last Profiler__Start call was passed different slot index-- unbalanced?
	}
#endif //_DEBUG

	/* call EngineTimer__Time to get current time */

	EngineTimer__Time();

	/* calculate time since current slot started (or was popped back into active status) */

	Profiler__GetDeltaTicks(nSlotIndex);

	/* async slots just record time and leave-- no stack manipulation */

	if (pProfileList[nSlotIndex].u32Type == PROFILE_ASYNC)
		return;
	
	/* pop stack, record current time */

	nProfileStackIndex--;
	if (nProfileStackIndex > 0)
		pProfileList[ProfileStack[nProfileStackIndex - 1]].LAST_TICKS = CURRENT_TICKS;
}
#endif // Profiler__Stop

/* get current profiling slot-- can be useful for checking if a lower-level routine is going into the right bucket
without having to wrap the routine in a start/stop pair */

int Profiler__CurrentSlot()
{
	if (!bProfilingOn)
		return(PROFILE_INVALID_SLOT);
	if (nProfileStackIndex == 0)
		return(PROFILE_INVALID_SLOT);
	return(ProfileStack[nProfileStackIndex - 1]);
}

/* get delta ticks since last call to this routine */

static void Profiler__GetDeltaTicks(int nSlotIndex)
{
	int nDeltaTicks;

	/* calculate delta ticks from current time (EngineTimer__Time has already been called) to last */
	/* time recorded in slot */

	ASSERT(nSlotIndex < nMaxProfileEntryCount);
	nDeltaTicks = (int) (CURRENT_TICKS - pProfileList[nSlotIndex].LAST_TICKS);
	if (nDeltaTicks < 0)
	{
//		ASSERT(0);
		nDeltaTicks = 0;
	}
	pProfileList[nSlotIndex].u32Ticks[nTicksIndex] += nDeltaTicks;
	// don't need to set LAST_TICKS here because will be reset if slot is popped
}

/* retrieve moving average for requested entry */

#ifndef Profiler__GetAverage
float Profiler__GetAverage(int nSlotIndex)
{
	int i;
	u32 u32TotalTicks;
	ts_ProfileEntry *pEntry;

	/* maybe nothing to do */

	if (pProfileList == NULL)
		return(0.0f);

	ASSERT(nSlotIndex < nMaxProfileEntryCount);
	pEntry = &pProfileList[nSlotIndex];
	ASSERT(strlen(pEntry->Name) != 0);	// if this hits, slot was not reserved
	u32TotalTicks = 0;
	for (i = 0; i < PROFILE_AVERAGE_ENTRIES; i++)
	{
		/* don't include current index-- it gets cleared in Profiler__StartLoop, so it skews results */

		if (i != (signed) nTicksIndex)
			u32TotalTicks += pEntry->u32Ticks[i];
	}

	/* and return average, in milliseconds. "- 1" is because we discard current tick slot in calculating total ticks */

	return((float) (u32TotalTicks * 1000.0f) / ((float) (PROFILE_AVERAGE_ENTRIES - 1) * TICKS_PER_SECOND));
}
#endif // Profiler__GetAverage

#ifndef Profiler__GetAverageString
char *Profiler__GetAverageString(int nSlotIndex)
{
	static char String[20];	// returns a pointer to this, so make it static

	sprintf(String, "%.1f ", Profiler__GetAverage(nSlotIndex));
	return(String);
}
#endif // Profiler__GetAverageString

#ifndef Profiler__GetActiveEntryCount
int Profiler__GetActiveEntryCount(void)
{
	if (!bProfilingOn)
		return(0);
	return(nActiveProfileEntryCount);
}
#endif // Profiler__GetActiveEntryCount

#ifndef Profiler__GetBarColor
ts_bRGBA *Profiler__GetBarColor(int nIndex)
{
	return(&BarColorList[nIndex % 6]);
}
#endif // Profiler__GetBarColor

#ifndef Profiler__GetBarFraction
float Profiler__GetBarFraction(int nIndex)
{
	/* return value as a fraction of the bar that would be drawn for the entire loop with game running at 10 hz */

	return(Profiler__GetAverage(pu32EntryTranslate[nIndex]) / (1000.0f / 10.0f));
}
#endif // Profiler__GetBarFraction

/* dump log of results on request */

void Profiler__WriteLog(bool bDebugOutput /*= false*/)
{
#ifdef PS2
#define PrintIt	printf	// i want this to get spit out in cdrom builds
#else //WIN32/PS2
#define PrintIt	dbgPrint
#endif //WIN32/PS2
#if defined(WIN32) || defined(PS2)
	int i;
	int j;
	int nStartIndex;
	int nTickEntryCount;
	u32 u32TotalTicks;
	char Message[200];

	if ((nHistoryTicksIndex == 0) && (bHistoryIndexWrapped == false))
	{
		PrintIt("No profile data.");
		return;
	}

	if (pProfileList == NULL)
	{
		ASSERT(FALSE);		// if this hits, no profiling data to report-- thought you'd like to know
		return;
	}

	/* open log file */

	FileStream *pLogFile = NULL;
	if (bDebugOutput == false)
	{
		Media::EnableNameMangling(false);		// don't mess with requested file name
#ifdef WIN32
		pLogFile = FileStream::NewAndOpen("ProfileData.log", FileStream::WRITE);
#elif defined(PS2)
		pLogFile = FileStream::NewAndOpen("host0:ProfileData.log", FileStream::WRITE);
#endif //WIN32/PS2
		Media::EnableNameMangling(true);			// restore
		if (pLogFile == NULL)
		{
			ASSERT(FALSE);	// couldn't open file for output
			return;
		}
	}

	/* header */

#ifdef WIN32
	sprintf(Message, "ProfileData.log\nProfile data from game engine\nPC build\n");
#elif defined(PS2)
	sprintf(Message, "ProfileData.log\nProfile data from game engine\nPS2 build\n");
#endif //WIN32/PS2
	if (bDebugOutput)
		PrintIt("%s", Message);
	else
		pLogFile->Print("%s", Message);
#ifdef WIN32
	{
		struct tm when;
		time_t now;
		time( &now );
		when = *localtime( &now );
		if (bDebugOutput)
			PrintIt("%s", asctime( &when ));
		else
			pLogFile->Print("%s", asctime( &when ));
	}
#endif //WIN32
	if (bDebugOutput)
		PrintIt("\n");
	else
		pLogFile->Print("\n");

	/* analyze data */

	if (bHistoryIndexWrapped)
	{
		nStartIndex = nHistoryTicksIndex + 1;
		nTickEntryCount = PROFILE_HISTORY_ENTRIES;
	}
	else
	{
		nStartIndex = 0;
		nTickEntryCount = nHistoryTicksIndex;
		ASSERT(nTickEntryCount);	// should never call this before the first entry is recorded
		if (nTickEntryCount == 0)
			nTickEntryCount = 1;	// for safety
	}
	for (i = 0; i < (signed) nHighSlotIndex; i++)
	{
		pProfileList[i].u32MinTicks = MAXUINT;
		pProfileList[i].u32MaxTicks = 0;
		u32TotalTicks = 0;

		/* loop through tick entries-- if necessary, j wraps back to 0 once it increments off the end of the list */

		for (j = nStartIndex; j != (signed) nHistoryTicksIndex; j++)
		{
			if (j >= PROFILE_HISTORY_ENTRIES)
				j = 0;	// wrap
			u32TotalTicks += pProfileList[i].u32HistoryTicks[j];
			if (pProfileList[i].u32HistoryTicks[j] < pProfileList[i].u32MinTicks)
				pProfileList[i].u32MinTicks = pProfileList[i].u32HistoryTicks[j];
			if (pProfileList[i].u32HistoryTicks[j] > pProfileList[i].u32MaxTicks)
				pProfileList[i].u32MaxTicks = pProfileList[i].u32HistoryTicks[j];
		}
		pProfileList[i].fAverageTicks = (float) u32TotalTicks / (float) nTickEntryCount;
	}

	/* write data */

	float fTotalAverageTicks = 0.0f;
	if (bDebugOutput)
	{
		PrintIt("Number of tick entries: %d\n\n", nTickEntryCount);
		PrintIt("Slot      Avg     Min     Max   Name\n");
		PrintIt("----      ---     ---     ---   ----\n");
	}
	else
	{
		pLogFile->Print("Number of tick entries: %d\n\n", nTickEntryCount);
		pLogFile->Print("Slot      Avg     Min     Max   Name\n");
		pLogFile->Print("----      ---     ---     ---   ----\n");
	}
	for (i = 0; i < (signed) nHighSlotIndex; i++)
	{
		/* skip inactive entries */

		if (strlen(pProfileList[i].Name) == 0)
			continue;
		sprintf(Message, "%4d  %7.1f %7.1f %7.1f   %s\n",
				  i,
				  (pProfileList[i].fAverageTicks * 1000.0f) / (float) TICKS_PER_SECOND,
				  ((float) pProfileList[i].u32MinTicks * 1000.0f) / (float) TICKS_PER_SECOND,
				  ((float) pProfileList[i].u32MaxTicks * 1000.0f) / (float) TICKS_PER_SECOND,
				  pProfileList[i].Name);
		if (bDebugOutput)
			PrintIt("%s", Message);
		else
			pLogFile->Print("%s", Message);
		fTotalAverageTicks += pProfileList[i].fAverageTicks;
	}
	sprintf(Message, "\nTotal %7.1f milliseconds (%.3f seconds)\n", (fTotalAverageTicks * 1000.0f) / (float) TICKS_PER_SECOND, fTotalAverageTicks / (float) TICKS_PER_SECOND);
	if (bDebugOutput)
		PrintIt("%s", Message);
	else
		pLogFile->Print("%s", Message);

	/* done */

	if (pLogFile)
	{
		pLogFile->Close();
		delete pLogFile;
	}

#endif //WIN32 || PS2
}

#endif //CONSUMER_BUILD
