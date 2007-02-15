/******************************************************************************
 * Module:	tasking.c
 * Date:	   7/23/98
 *
 * Creator:	Todd Blackburn
 * Purpose:	Implementation of re-usable task switching code
 *
 * 			(C) 1998 Avalanche Software
 *
 *****************************************************************************/

/******************* includes ****************************/

#include "Layers/LayersPCH.h"





static ts_Context		DispatchContext;		//context structure used by tskDispatchLoop

static ts_Context		*pContextArray;		//pointer to task context array

static ts_TCB			*pTCBArray = NULL;	//pointer to array of TCBs

ts_TCB					*pCurrentTask;			//current task pointer

static DoublyLinkedList<ts_TCB> ActiveList( false);	//active task list pointer
static DoublyLinkedList<ts_TCB> FreeList( false);		//free task list pointer
static DoublyLinkedList<ts_TCB> MortList( false);

/******************************************************************************
 *****************************************************************************/
t_Error tskInit( s32 s32NumTCBs)
{

	int nIndex;

	ASSERTS(pTCBArray == NULL, "tskInit memory leak");
	MEM_SET_ALLOC_NAME( "tsk TCB array");
	pTCBArray = (ts_TCB *)memAlloc( s32NumTCBs * sizeof(ts_TCB));
	ASSERT( pTCBArray);

	/* set up linked lists */
	for( nIndex = 0; nIndex < s32NumTCBs; nIndex++)
	{
		FreeList.AddHead( pTCBArray + nIndex);
	}

	pCurrentTask = NULL;

	return( 0);
}

/******************************************************************************
 *****************************************************************************/
void tskReset()
{
	/* all tasks to the free list */

	while (ActiveList.IsEmpty() == false)
	{
		ts_TCB *pTask = ActiveList.Head();
		memFree(pTask->pLocalData);
		pTask->pLocalData = NULL;
		ActiveList.Unlink(pTask);
		FreeList.AddHead(pTask);
	}
	while (MortList.IsEmpty() == false)
	{
		ts_TCB *pTask = MortList.Head();
		memFree(pTask->pLocalData);
		pTask->pLocalData = NULL;
		MortList.Unlink(pTask);
		FreeList.AddHead(pTask);
	}
	pCurrentTask = NULL;
}

/******************************************************************************
 *****************************************************************************/
void tskShutdown()
{
	/* reset to clean up all tasks on active and dead lists */

	tskReset();

	/* clear out free list */

	FreeList.UnlinkAll();

	/* free memory */

	if (pTCBArray)
	{
		memFree(pTCBArray);
		pTCBArray = NULL;
	}
}

/******************************************************************************
 *****************************************************************************/
ts_TCB * tskCreate(	void (*pTaskEntryPoint)( void ),
							void (*pTaskDieFunction)(ts_TCB *pTCB),
							s32 s32TaskID,
							s32 s32TaskPriority,
							s32 s32TaskStackSize,
							s32 s32TaskLocalDataSize,
							u32 u32Flags)
{

	ts_TCB *pNewTCB;

	pNewTCB = FreeList.Head( );
	ASSERT( pNewTCB != NULL);

	if ( pNewTCB == NULL )
		return NULL;

#ifdef WIN32
	// Windows can use a large amount of stack space for Win API calls
	// Pad WIN32 tasks automatically,  shouldn't be necissary on consoles.
	// Jon M.
	s32TaskStackSize += (32 * 1024);
#endif

	pNewTCB->s32TaskID = s32TaskID;
	pNewTCB->s32TaskPriority = s32TaskPriority;
	pNewTCB->s32TaskSleepCount = 0;
	pNewTCB->fSleepTime = 0.0f;
	pNewTCB->pTaskDieFunction = pTaskDieFunction;
	pNewTCB->s32TaskStackSize = s32TaskStackSize;
	pNewTCB->s32TaskLocalDataSize = s32TaskLocalDataSize;
	pNewTCB->u32Flags = u32Flags;
	
	MEM_SET_ALLOC_NAME("tsk local data");
	pNewTCB->pLocalData = memAlloc( s32TaskStackSize + s32TaskLocalDataSize );
	ASSERT( pNewTCB->pLocalData);

	memset( pNewTCB->pLocalData, 0x00, ( s32TaskStackSize + s32TaskLocalDataSize ));
	ctxInit( &pNewTCB->Context, pTaskEntryPoint, pNewTCB->pLocalData, s32TaskStackSize, s32TaskLocalDataSize);

	FreeList.Unlink( pNewTCB);
	ActiveList.InsertDescending( pNewTCB);

	return( pNewTCB);
}

#if 0
/******************************************************************************
 *****************************************************************************/
ts_TCBList *tskCreateListEntry(void)
{
	ts_TCBList		*pNewTCBList;

	pNewTCBList = (ts_TCBList *) SLinkList__GetAndRemoveHead(&FreeList);
#ifdef DEBUG
	if (pNewTCBList == NULL)
	{
		dbgPrint ("Out of TCBs");
		return(NULL);
	}
#endif

	SLinkList__AddLinkToHead(&ActiveList, pNewTCBList);

	return(pNewTCBList);
}
#endif

/******************************************************************************
 *****************************************************************************/
void tskSleep( int iTaskSleepCount)
{
//	/* Make sure this is a c task */
//	ASSERT(((u32) pCurrentTaskList->pTCB & 0x01) == 0);

	pCurrentTask->s32TaskSleepCount = (s32)iTaskSleepCount;
	pCurrentTask->u32Flags &= ~TASKF_SLEEPMASK;
	pCurrentTask->u32Flags |= TASKF_SLEEPCOUNT;
	ctxSwitch( &DispatchContext, &pCurrentTask->Context);
}

/******************************************************************************
 *****************************************************************************/
void tskSleep( float fSleepTime)
{
//	/* Make sure this is a c task */
//	ASSERT(((u32) pCurrentTaskList->pTCB & 0x01) == 0);

	pCurrentTask->fSleepTime = fSleepTime;
	pCurrentTask->u32Flags &= ~TASKF_SLEEPMASK;
	pCurrentTask->u32Flags |= TASKF_SLEEPTIME;
	ctxSwitch( &DispatchContext, &pCurrentTask->Context);
}

/******************************************************************************
 *****************************************************************************/
void tskTransfer( ts_TCB *pTCB, void ( *pTaskEntryPoint)( void))
{
//	/* Make sure this is a c task */
//	ASSERT(((u32) pTCBList->pTCB & 0x01) == 0);

	ctxInit( &pTCB->Context, pTaskEntryPoint, pTCB->pLocalData, pTCB->s32TaskStackSize, pTCB->s32TaskLocalDataSize);
	pTCB->s32TaskSleepCount = 1;
	pCurrentTask->u32Flags &= ~TASKF_SLEEPMASK;
	pCurrentTask->u32Flags |= TASKF_SLEEPCOUNT;
}

/******************************************************************************
 *****************************************************************************/
void tskDie( void)
{

	ts_TCB		*pMyTCB = pCurrentTask;

	if( pMyTCB->pTaskDieFunction != NULL)
	{
		pMyTCB->pTaskDieFunction( pMyTCB);
	}

//	/* Make sure this is a c task */
//	ASSERT(((u32) pMyTCBList->pTCB & 0x01) == 0);

	/* move it to the dead list */
	ActiveList.Unlink( pMyTCB);
	MortList.AddHead( pMyTCB);

#ifndef _XBOX
	ctxKillSwitch( &DispatchContext);
#endif
}

/******************************************************************************
 *****************************************************************************/
void tskDispatchLoop( float fElapsedRealTime, float fElapsedGameTime, bool bInPause)
{

	ts_TCB		*pTCBNext;

	pCurrentTask = ActiveList.Head( );

	while( pCurrentTask)
	{
		pTCBNext = pCurrentTask->next;

		if( bInPause & !( pCurrentTask->u32Flags & TASKF_ACTIVE_DURING_PAUSE))
		{
			pCurrentTask = pTCBNext;
			continue;
		}

		if( pCurrentTask->u32Flags & TASKF_SLEEPCOUNT)
		{
			pCurrentTask->s32TaskSleepCount--;
			if( pCurrentTask->s32TaskSleepCount <= 0)
			{
				pCurrentTask->s32TaskSleepCount = 0;
				ctxSwitch ( &pCurrentTask->Context, &DispatchContext);
			}
		}
		else
		{
			if( pCurrentTask->u32Flags & TASKF_REALTIME)
			{
				pCurrentTask->fSleepTime -= fElapsedRealTime;
			}
			else
			{
				pCurrentTask->fSleepTime -= fElapsedGameTime;
			}
			if( pCurrentTask->fSleepTime <= 0.0f)
			{
				pCurrentTask->fSleepTime = 0.0f;
				ctxSwitch ( &pCurrentTask->Context, &DispatchContext);
			}
		}
		pCurrentTask = pTCBNext;
	}
#if 0
//yuch-- temporary
#ifdef WIN32
			else
			{
				/* This is a c++ task */
				u32 **classPointer = (u32 **)pCurrentTask;
				u32 *vftable = *classPointer;
				u32 runPtr = vftable[1];
				_asm
				{
					mov 	ecx, classPointer
					call 	[runPtr]
				}
			}
#endif //WIN32
#endif
	while( !MortList.IsEmpty( ))
	{
		pCurrentTask = MortList.Head( );
		memFree( pCurrentTask->pLocalData);
		pCurrentTask->pLocalData = NULL;
		
		MortList.Unlink( pCurrentTask);
		FreeList.AddHead( pCurrentTask);
	}

	pCurrentTask = NULL;
}

/******************************************************************************
 *****************************************************************************/
void tskKill( ts_TCB *pTCB)
{
	if( ( pTCB == NULL) || ( pTCB == pCurrentTask))
	{
		return;
	}

	if( pTCB->u32Flags & TASKF_UNSTOPPABLE)
	{
		return;
	}
	
	if( pTCB->pTaskDieFunction != NULL)
	{
		pTCB->pTaskDieFunction( pTCB);
	}
	memFree( pTCB->pLocalData);
	pTCB->pLocalData = NULL;

	ActiveList.Unlink( pTCB);
	FreeList.AddHead( pTCB);
}

/******************************************************************************
 *****************************************************************************/
void tskKillClass( s32 s32Class, s32 s32ClassMask)
{

	ts_TCB		*pTCB,
			 		*pTCBKill;

	s32Class &= s32ClassMask;

	pTCB = ActiveList.Head( );

	while( pTCB != NULL)
	{
		if( pTCB == pCurrentTask)
		{
			pTCB = pTCB->next;
			continue;
		}
		if( s32Class != ( pTCB->s32TaskID & s32ClassMask))
		{
			pTCB = pTCB->next;
			continue;
		}

		pTCBKill = pTCB;
		pTCB = pTCB->next;

		tskKill( pTCBKill);
	}
}

/******************************************************************************
 *****************************************************************************/
ts_TCB * tskExist( s32 s32Class, s32 s32ClassMask, ts_TCB *pPrevTCB)
{

	ts_TCB		*pTCB;

	s32Class &= s32ClassMask;

	if( pPrevTCB == NULL)
	{
		pTCB = ActiveList.Head( );
	}
	else
	{
		pTCB = pPrevTCB->next;
	}

	while( pTCB != NULL)
	{
		if( pTCB != pCurrentTask)
		{
			if( s32Class == ( pTCB->s32TaskID & s32ClassMask))
			{
				return pTCB;
			}
		}
		pTCB = pTCB->next;
	}

	return( NULL);
}

