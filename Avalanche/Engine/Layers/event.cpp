/* -----------------7/19/98 10:55PM------------------
 * Include Files
 * --------------------------------------------------*/

/* system includes */

#include <stdio.h>

/* engine includes */

#include "Layers/Debug.h"
#include "Layers/Event.h"
#include "layers/slllist.h"
#include "layers/sllutil.h"
#include "Layers/Memory.h"

/* -----------------7/19/98 10:55PM------------------
 * Local Defines
 * --------------------------------------------------*/
#define __SYS__			"EVT"

#define EVT_TYPE_MASK	0xFF

struct ts_Event
{
   ts_Event			*pNext;
   t_evtfunc		EventFunc;
   void				*pEventData;
	u32				u32Priority;
   u32				u32EventFlags;
	short				sWait;
	short				sWaitCount;
	short				sRunCount;
};

struct ts_EventList
{
	ts_EventList		*pNext;
	ts_Event				*pEventList;
	s32					s32EventID;
};

/* -----------------7/19/98 10:56PM------------------
 * Local Data
 * --------------------------------------------------*/
u32					u32EventsAlloced;

ts_Event				*pEventMem;
ts_Event          *pFreeEventList;

ts_EventList		*pEventList;

/* -----------------7/19/98 10:55PM------------------
 * Local Prototypes
 * --------------------------------------------------*/
ts_EventList *		_evtFindEventList(s32 s32ID);
ts_Event *			_evtFindEvent( t_Handle Handle);
void 					_evtFree( ts_Event *pEvent);
ts_Event *			_evtGetEvent( void);
t_Handle 			_evtGetEventHandle( ts_Event *pEvent);
ts_EventList *		_evtNewEventList(s32 s32ID);
bool 					_evtPriorityCompare( ts_voidListSll *pPrev, ts_voidListSll *pCur, ts_voidListSll *pIns);
bool 					_evtProcess( ts_Event *pEvent);

/* -----------------7/19/98 10:56PM------------------
 * Exported Functions
 * --------------------------------------------------*/
void evtInit( int iEventHandles)
{
   ASSERT( iEventHandles != 0);

	MEM_SET_ALLOC_NAME("EventMgr list");
   pEventMem = (ts_Event *) memAlloc( iEventHandles * sizeof( ts_Event));

	sllBlockMemLink( (ts_voidListSll *) pEventMem, iEventHandles, sizeof( ts_Event));

	pFreeEventList = pEventMem;
   pEventList = NULL;

	u32EventsAlloced = (u32) iEventHandles;
}


t_Handle evtAdd( s32 s32ID, t_evtfunc Func, void *pData, u32 u32Priority,
					short sFlags, short sWait, short sRunCount)
{
   ts_Event          *pNewEvent;
	ts_EventList		*pList;

	u32					u32RetVal;

   ASSERT( Func != NULL);

	if( sWait == 0)
		sWait = 1;

   pNewEvent = _evtGetEvent( );

	if( pNewEvent == NULL)
	{
		dbgPrint( "%s: Out of Event Handles.\n", __SYS__);
		dbgPrint( "%s: ID: %lu Func Addr: %08x.\n", s32ID, Func);
		CRT_ERROR( "HALT!");
	}

	pNewEvent->u32Priority = u32Priority;
   pNewEvent->EventFunc = Func;
   pNewEvent->pEventData = pData;
   pNewEvent->u32EventFlags = sFlags;
	pNewEvent->sWait = sWait;
	pNewEvent->sWaitCount = sWait;
	pNewEvent->sRunCount = sRunCount;

	pList = _evtFindEventList( s32ID);

	if( pList == NULL)
		pList = _evtNewEventList( s32ID);

	sllInsertwCallback( (ts_voidListSll **) &pList->pEventList, (ts_voidListSll *) pNewEvent,
								SLL_INSERT_MIDDLE, _evtPriorityCompare, &u32RetVal);

	return _evtGetEventHandle( pNewEvent);
}


void evtFlagSet( t_Handle Handle, u32 u32Flag)
{
	ts_Event				*pEvent;

	pEvent = _evtFindEvent(Handle);

	u32Flag &= ~EVT_TYPE_MASK;

	pEvent->u32EventFlags |= u32Flag;

}

void evtFlagUnSet( t_Handle Handle, u32 u32Flag)
{
	ts_Event				*pEvent;

	pEvent = _evtFindEvent( Handle);

	u32Flag &= ~EVT_TYPE_MASK;
	u32Flag = ~u32Flag;

	pEvent->u32EventFlags &= u32Flag;
}

void evtRemove( t_Handle Handle)
{

}


void evtProcessEvent(s32 s32EventID)
{

	bool				bFreeFlag;
	ts_EventList	*pThisEvent;
	ts_Event			**ppList;
	ts_Event			*pFreeEvent;


	pThisEvent = _evtFindEventList(s32EventID);

	if( pThisEvent == NULL)
		return;

	ppList = &pEventList->pEventList;

	while( *ppList)
	{
		if( (*ppList)->u32EventFlags & EVT_DISABLE)
			bFreeFlag = FALSE;
		else
			bFreeFlag = _evtProcess( *ppList);

		if( bFreeFlag == FALSE)
			ppList = &(*ppList)->pNext;
		else
		{
			pFreeEvent = *ppList;
			*ppList = (*ppList)->pNext;
			_evtFree( pFreeEvent);
		}
	}

}
/* -----------------7/19/98 10:56PM------------------
 * Internal Functions
 * --------------------------------------------------*/
ts_Event *_evtGetEvent( void)
{
	ts_Event				*pEvent;

	if( pFreeEventList == NULL)
		return NULL;

	pEvent = pFreeEventList;
	pFreeEventList = pEvent->pNext;

	return pEvent;
}

bool _evtProcess( ts_Event *pEvent)
{
	short					sType;
	bool				bFreeFlag;

		sType = (short) pEvent->u32EventFlags & EVT_TYPE_MASK;

		bFreeFlag = FALSE;

		switch( sType)
		{
			case EVT_ONE_TIME:
				pEvent->EventFunc( pEvent->pEventData);
				bFreeFlag = TRUE;
				break;

			default:
			case EVT_CONTINUOUS:
				if( --pEvent->sWait <= 0)
				{
					if( pEvent->EventFunc( pEvent->pEventData) == TRUE)
						bFreeFlag = TRUE;
					else
						pEvent->sWait = pEvent->sWaitCount;
				}
				break;
			case EVT_COUNT:
				if( --pEvent->sWait <= 0)
				{
					if( pEvent->EventFunc( pEvent->pEventData) == TRUE)
						bFreeFlag = TRUE;

					if( --(pEvent->sRunCount) <= 0)
						bFreeFlag = TRUE;
					else
						pEvent->sWait = pEvent->sWaitCount;
				}
				break;
		}

	return bFreeFlag;
}

void _evtFree( ts_Event *pEvent)
{
	pEvent->pNext = pFreeEventList;
	pFreeEventList = pEvent;
}


ts_Event *_evtFindEvent( t_Handle Handle)
{
	ASSERT((u32) Handle < u32EventsAlloced);

	return pEventMem + Handle;
}

t_Handle _evtGetEventHandle( ts_Event *pEvent)
{
	return (t_Handle) ((pEvent - pEventMem) / sizeof( ts_Event));
}

bool _evtPriorityCompare( ts_voidListSll *pPrev, ts_voidListSll *pCur, ts_voidListSll *pIns)
{
	u32 u32PrevPri, u32CurPri, u32InsPri;

	if( pPrev == NULL)
		u32PrevPri = EVT_PRIORITY_MAX;
	else
		u32PrevPri = ((ts_Event *) pPrev)->u32Priority;

	if( pCur == NULL)
		u32CurPri = EVT_PRIORITY_MIN;
	else
		u32CurPri = ((ts_Event *) pCur)->u32Priority;

	u32InsPri = ((ts_Event *) pIns)->u32Priority;

	return(((u32InsPri <= u32PrevPri) && (u32InsPri >= u32CurPri)) ? (bool) TRUE : (bool) FALSE);
}

ts_EventList *_evtFindEventList(s32 s32ID)
{
	ts_EventList		*pList;

	pList = pEventList;

	while ( pList)
	{
		if( pList->s32EventID == s32ID)
			return pList;

		pList = pList->pNext;
	}

	return NULL;
}

ts_EventList *_evtNewEventList(s32 s32ID)
{
ts_EventList			*pNewEvent;

	MEM_SET_ALLOC_NAME("EventMgr new");
	pNewEvent = (ts_EventList *) memAlloc( sizeof( ts_EventList));

	pNewEvent->s32EventID = s32ID;
	pNewEvent->pEventList = NULL;
	pNewEvent->pNext = pEventList;
	pEventList = pNewEvent;

	return pNewEvent;
}
