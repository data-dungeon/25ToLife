/*************************************/
/*                                   */
/*   l_Timing.h                      */
/*   new engine  03/20/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   layers timing module            */
/*                                   */
/*************************************/

#ifndef __L_TIMING_H
#define __L_TIMING_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* defines *****************************/

/* first profile slots are reserved for entire loop, unaccounted time, render time */

#define PROFILE_ENTIRE_LOOP			0
#define PROFILE_RENDER_ASYNC			1	// only valid on ps2
#define PROFILE_VSYNC					2
#define PROFILE_RENDER_IMMEDIATE		3
#define PROFILE_COLLISION				4
#define PROFILE_SPRITE_RENDER			5
#define PROFILE_PARTICLE_RENDER		6
#define PROFILE_WALK_SCENE_TREE		7
#define PROFILE_RENDER_BUCKETS		8
#define PROFILE_RENDER_SHADOWS		9
#define PROFILE_CREATE_SHADOWS		10
#define PROFILE_BUILD_MASTER_LIST	11
#define PROFILE_COPY_BONES				12
#define PROFILE_MOTION_PROCESSING	13
#define PROFILE_SCRIPT_PROCESSING	14
#define PROFILE_ANIHF					15
#define PROFILE_FILE_LOAD				16
#define PROFILE_DBL_PARSE				17
#define PROFILE_NETWORK_UPDATE		18
#define PROFILE_VOICE_UPDATE			19
#define PROFILE_ENGINE_ADVANCE		20
#define PROFILE_RENDER_MANAGER		21
#define PROFILE_GAME_LOGIC				22
#define PROFILE_UNRESERVED_START		23	// first entry after reserved slots
#define PROFILE_INVALID_SLOT			0xffffffff

/* profile types. normal types can be pushed and popped on stack. async ones leave the stack alone--
they just use a Profiler__Start/Profiler__Stop pair to record a delta time, no matter what other
calls to Profiler__Start or Profiler__Stop are in between. useful for getting the entire loop time,
and also for asynchronous rendering on the ps2 */

#define PROFILE_NORMAL				0
#define PROFILE_ASYNC				1

/******************* macros ******************************/

/******************* structures **************************/

/******************* global variables ********************/

extern float SecondsPerFrame;

/******************* global prototypes *******************/

void EngineTimer__Init(void);
void EngineTimer__Reset(void);
void EngineTimer__Shutdown(void);
void EngineTimer__Advance(void);
u32 EngineTimer__Time(void);
void EngineTimer__CheckWrap(void);
u32 EngineTimer__GameTime(u32 *hours, u32 *minutes, u32 *seconds, u32 *thousandths);
void EngineTimer__StartSleep();
void EngineTimer__EndSleep();


#define SUPPORT_PROFILING

// logic here is:
// we want profiling if its not a PS2/SN release build
// we never want profiling in a CONSUMER_BUILD build..
// but sometimes I want SNs profiler support in the PS2/SN Release build..
#if defined(CONSUMER_BUILD) || defined(CONSUMER_BUILDxx)  
#undef SUPPORT_PROFILING
#else // CONSUMER_BUILD
#if defined(PS2) && defined(SN)
#ifndef DEBUG
#if defined(__SNTUNER_H_INC__) && 0
#define Profiler__Start(index) snStartMarker(index, __PRETTY_FUNCTION__)
#define Profiler__Stop(index) snStopMarker(index)
#endif // defined(__SNTUNER_H_INC__) && 0
//#undef SUPPORT_PROFILING
#endif // !DEBUG
#endif // defined(PS2) && defined(SN)
#endif // CONSUMER_BUILD

#ifdef SUPPORT_PROFILING
void Profiler__Init(u32 u32EntryCount);
void Profiler__Reset(void);
void Profiler__Shutdown(void);
void Profiler__StartLoop(void);
void Profiler__StopLoop(void);
void Profiler__Start(int nSlotIndex);
void Profiler__Stop(int nSlotIndex);
int Profiler__CurrentSlot();
bool Profiler__ReserveSlot(int nSlotIndex, char *pName, u32 u32Type);
float Profiler__GetAverage(int nSlotIndex);
char *Profiler__GetAverageString(int nSlotIndex);
int Profiler__GetActiveEntryCount(void);
ts_bRGBA *Profiler__GetBarColor(int nIndex);
float Profiler__GetBarFraction(int nIndex);
void Profiler__WriteLog(bool bDebugOutput = false);

#else // SUPPORT_PROFILING

#define Profiler__Init(u32EntryCount)
#define Profiler__Reset()
#define Profiler__Shutdown()
#define Profiler__StartLoop()
#define Profiler__StopLoop()
#define Profiler__Start(u32SlotIndex)
#define Profiler__Stop(u32SlotIndex)
#define Profiler__ReserveSlot(u32SlotIndex, pName, u32Type) (true)
#define Profiler__GetAverage(u32SlotIndex) (0.0f)
#define Profiler__GetAverageString(u32SlotIndex) ("")
#define Profiler__GetActiveEntryCount() (0)
#define Profiler__GetBarColor(nIndex) ((ts_bRGBA*)NULL)
#define Profiler__GetBarFraction(nIndex) (0.0f)
#define Profiler__WriteLog(bDebugOutput)

#endif // SUPPORT_PROFILING



#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __L_TIMING_H

