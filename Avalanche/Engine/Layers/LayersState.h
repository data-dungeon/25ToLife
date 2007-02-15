/*************************************/
/*                                   */
/*   l_LayersState.h                 */
/*   new engine  04/11/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   layers state and init           */
/*                                   */
/*************************************/

#ifndef __L_LAYERSSTATE_H
#define __L_LAYERSSTATE_H

/******************* includes ****************************/

/* system includes */

/* engine includes */

#include "platform/BaseType.h"
#include "Layers/LayersStruct.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

struct poolCfg;					// in Layers/Memory.h

/******************* defines *****************************/

/* these values serve two purposes. first, nInitStatus is set to NO_INIT in the record declaration. this can be checked
to make sure that the record declaration doesn't get out of synch with the structure definition (i.e. a new field is
added to the structure but not to the record). second, the macros to set the default values can only be called before
the engine initialization-- they have no effect afterwards */

#define LAYERS_NO_INIT	0x12345678
#define LAYERS_INIT		0x87654321

/* for calls to Layers__CheckMemoryState */

#define CHECK_MEMORY_STATE_PRINT_ALWAYS		0
#define CHECK_MEMORY_STATE_PRINT_IF_HIGHER	1
#define CHECK_MEMORY_STATE_PRINT_NEVER			2

/******************* macros ******************************/

/******************* structures **************************/

/* record that tracks layers attributes. many values are filled in with default values for initialization.
macros are provided above for editing them before initialization call. other values are for tracking things that get
set during initialization and maybe game play */

struct ts_LayersState
{
	/* memory */

	u8				*pu8LocalMemory;
	t_Handle		MainMemoryHandle;
	t_Handle		SecondaryMemoryHandle;
#ifdef GCN
	t_Handle		ARAMMemoryHandle;
#endif //GCN

	/* debug system */

	u8				u8DebugStartLevel;

	/* joystick */

	u32			u32JoystickCount;

	/* media (file i/o) */

	u32			u32MaxOpenFiles;
	u32			u32MaxHogVolumes;

	/* tasking */

	u32			u32MaxTaskControlBlocks;

	/* profiler */

	u32			u32MaxProfileEntries;

	/* init flag, also used to make sure record declaration doesn't get out of synch with structure definition */

	u32			u32InitStatus;
};

/******************* global variables ********************/

extern ts_LayersState LayersState;

/******************* global prototypes *******************/

bool Layers__Init(void);
bool Layers__CreateHeap( void);

u32   Layers__SystemRAMAvailable( u32 max );
void* Layers__SystemRAMAlloc( u32 size );

inline bool Layers__HeapCreated( void)
{
	return( LayersState.MainMemoryHandle != INVALID_HANDLE );
}

void Layers__Reset(void);
void Layers__Shutdown(void);
void Layers__SetDebugStartLevel(u8 u8DebugStartLevel);
void Layers__SetJoystickCount(u32 u32JoystickCount);
void Layers__SetMaxOpenFiles(u32 u32MaxOpenFiles);
void Layers__SetMaxOpenHogs(u32 u32MaxHogVolumes);
void Layers__SetMaxTaskControlBlocks(u32 u32MaxTaskControlBlocks);
void Layers__SetMaxProfileEntries(u32 u32MaxProfileEntries);

#ifdef GCN
inline t_Handle Layers__GetVirtualHeap( void)
{
	return( LayersState.ARAMMemoryHandle);
}
#endif

inline t_Handle Layers__GetMainHeap( void)
{
	return( LayersState.MainMemoryHandle);
}

#ifndef CONSUMER_BUILD
void Layers__ShowMemoryState(void);
void Layers__BriefShowMemoryState(void);
void Layers__ShowFreeMemory(void);
void Layers__ShowAllocMemory(void);
bool Layers__MarkPermanentMemory(void);
bool Layers__CheckPermanentMemory(void);
void Layers__CheckMemoryState(int nPrintRequest);
void Layers__GetMemoryMaxStats(int *pnMaxLayersUsage, int *pnMaxLayersTotal, int *pnMaxLayersHandles, int *pnMaxCHeapUsage, int *pnMaxCHeapTotal);
void Layers__TrackMemoryHighWater(bool bState);
bool Layers__MemoryHighWaterOn(void);
void Layers__ResetMemoryHighWater(void);
void Layers__GetMemoryHighWaterStats(int *pnMinBytes, int *pnMaxBytes);
int Layers__GetMemorySize();
#endif //CONSUMER_BUILD

void Layers__GetMemoryStats(int *pnFreeBytes, int *pnFreeHandles, int *pnLargestFree, int *pnAllocBytes, int *pnAllocHandles);
#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __L_LAYERSSTATE_H
