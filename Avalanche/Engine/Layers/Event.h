#ifndef __EVENT_H
#define __EVENT_H
/* -----------------7/20/98 5:30PM-------------------
 * Includes
 * --------------------------------------------------*/

/* system includes */

/* engine includes */

#include "Layers/LayersStruct.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* -----------------7/20/98 5:30PM-------------------
 * Exported Defines
 * --------------------------------------------------*/
#define EVT_PRIORITY_MAX	MAXUINT
#define EVT_PRIORITY_MIN	0

enum EventTypes
{
	EVT_ONE_TIME = 0,
	EVT_CONTINUOUS,
	EVT_COUNT
} ;

enum EventFlags
{
	EVT_DISABLE = 0x100
} ;

typedef bool (*t_evtfunc)( void *);

#define evtDisable(lid)	evtFlagSet( lid, EVT_DISABLE)
#define evtEnable(lid)	evtFlagUnSet( lid, EVT_DISABLE)

/* -----------------7/20/98 5:30PM-------------------
 * Exported Functions
 * --------------------------------------------------*/
void 					evtInit( int iHandles);
t_Handle				evtAdd( s32 s32EventID, t_evtfunc Func, void *pData, u32 u32Priority,
							short sFlags, short sWait, short sRunCount);

void 					evtFlagSet( t_Handle lEventID, u32 u32Flag);
void 					evtFlagUnSet( t_Handle lEventID, u32 u32Flag);

void 					evtRemove( t_Handle lEventHandle);

void 					evtProcessEvent( s32 s32EventID);

#if defined(__cplusplus)
}
#endif

#endif
