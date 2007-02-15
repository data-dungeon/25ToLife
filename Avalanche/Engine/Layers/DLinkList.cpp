/*************************************/
/*                                   */
/*   DLinkList.cpp                   */
/*   new engine  07/12/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   doubly-linked list processing   */
/*                                   */
/*************************************/

/******************* includes ****************************/

#include "Layers/LayersPCH.h"



/******************* defines *****************************/

/* turn this on for more list integrity checks */

//#define CHECK_LIST_INTEGRITY
#undef CHECK_LIST_INTEGRITY
#ifndef _DEBUG
#undef CHECK_LIST_INTEGRITY
#endif //_DEBUG

/******************* macros ******************************/

/* this handles having the link pointers in each record being at arbitrary offsets within the record. basically,
point to that offset within the record and treat it as a void pointer */

#define NEXT_PTR(pList,pRecord)	*((void **) ((char *) pRecord + pList->u32NextOffset))
#define PREV_PTR(pList,pRecord)	*((void **) ((char *) pRecord + pList->u32PrevOffset))

/******************* structures **************************/

/******************* externs *****************************/

/******************* local variables ********************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* initialize a linked list record. provide offsets of next and prev fields in records that this list will link together.
currently assumed to be 0 and 4 */

void DLinkList__Init(ts_DLinkList *pList, u32 u32NextOffset, u32 u32PrevOffset)
{
	pList->pHead = NULL;
	pList->pTail = NULL;
	pList->u32NextOffset = u32NextOffset;
	pList->u32PrevOffset = u32PrevOffset;
}

/* link a block of records into a list. buffer is nCount records of nRecordSize, each has pNext and pPrev fields at
specified offset */

void DLinkList__LinkBlock(ts_DLinkList *pList, void *pBuffer, int nCount, int nRecordSize)
{
	int i;
	void *pRecord;

	ASSERT((pList->u32NextOffset != 0) || (pList->u32PrevOffset != 0));	// list must have been inited by DLinkList__Init
	pRecord = pBuffer;
	for (i = 0; i < nCount; i++)
	{
		if (i == 0)
		{
			/* first record */

			pList->pHead = pRecord;
			PREV_PTR(pList, pRecord) = NULL;
			NEXT_PTR(pList, pRecord) = (char *) pRecord + nRecordSize;
		}
		else if (i == nCount - 1)
		{
			/* last record */

			pList->pTail = pRecord;
			PREV_PTR(pList, pRecord) = (char *) pRecord - nRecordSize;
			NEXT_PTR(pList, pRecord) = NULL;
		}
		else
		{
			/* interior record */

			PREV_PTR(pList, pRecord) = (char *) pRecord - nRecordSize;
			NEXT_PTR(pList, pRecord) = (char *) pRecord + nRecordSize;
		}

		/* to next record */

		pRecord = (char *) pRecord + nRecordSize;
	}
#ifdef CHECK_LIST_INTEGRITY
	DLinkList__Check(pList);
#endif //CHECK_LIST_INTEGRITY
}

/* check the integrity of a list. return the length of the list */

#ifndef CONSUMER_BUILD
int DLinkList__Check(ts_DLinkList *pList)
{
	int nCount;
	void *pRecord;

	/* check list's integrity and return its length */

	if (pList->pHead == NULL)
	{
		ASSERT(pList->pTail == NULL);	// if list is empty, both head and tail should be null
		return(0);
	}
	nCount = 1;
	pRecord = pList->pHead;
	ASSERT(PREV_PTR(pList, pRecord) == NULL);	// head of list should have NULL prev pointer
	while (NEXT_PTR(pList, pRecord))
	{
		ASSERT(PREV_PTR(pList, NEXT_PTR(pList, pRecord)) == pRecord);	// check integrity of chain
		nCount++;
		pRecord = NEXT_PTR(pList, pRecord);
	}
	ASSERT(pList->pTail = pRecord);	// tail should point to last record
	return(nCount);
}
#endif //CONSUMER_BUILD

/* check if the passed record is actually in the passed list */

#ifndef CONSUMER_BUILD
bool DLinkList__InList(ts_DLinkList *pList, void *pCheckRecord)
{
	void *pRecord;

	pRecord = pList->pHead;
	while (pRecord)
	{
		if (pRecord == pCheckRecord)
			return(TRUE);
		pRecord = NEXT_PTR(pList, pRecord);
	}
	return(FALSE);
}
#endif //CONSUMER_BUILD

/* remove a link from a list */

void DLinkList__RemoveLink(ts_DLinkList *pList, void *pRecord)
{
	ASSERTS(pList, "DLinkList__RemoveLink: NULL list pointer");
#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(pList, pRecord));
#endif //CHECK_LIST_INTEGRITY

	/* current head of list? */

	if (pList->pHead == pRecord)
	{
		/* head of list, update list's head pointer */

		ASSERT(PREV_PTR(pList, pRecord) == NULL);	// confirm that this is the head
		pList->pHead = NEXT_PTR(pList, pRecord);	// may be NULL if this is only record in list
		if (pList->pHead)									// but if there is a new head
			PREV_PTR(pList, pList->pHead) = NULL;	// then new head of list gets NULL prev pointer
	}
	else
	{
		/* not head of list, just tell previous record to skip around it */

		NEXT_PTR(pList, PREV_PTR(pList, pRecord)) = NEXT_PTR(pList, pRecord);	// may be NULL if record is current tail of list
	}

	/* current tail of list? */

	if (pList->pTail == pRecord)
	{
		/* tail of list, update list's tail pointer */

		ASSERT(NEXT_PTR(pList, pRecord) == NULL);	// confirm that this is the tail
		pList->pTail = PREV_PTR(pList, pRecord);	// may be NULL if this is only record in list
		if (pList->pTail)									// but if there is a new tail
			NEXT_PTR(pList, pList->pTail) = NULL;	// then new tail of list gets NULL next pointer
	}
	else
	{
		/* not tail of list, just tell next record to skip around it */

		PREV_PTR(pList, NEXT_PTR(pList, pRecord)) = PREV_PTR(pList, pRecord);	// may be NULL if record is current head of list
	}
}

/* remove a link from a list, but we don't know which list. for example, when removing a group from a scene, we
want to remove all it's instances. the instances may be active or inactive, we're not sure which list they're in.
pList points to an array of candidate lists. if the link isn't a head or tail, then we can remove it without
caring which list it is in */

void DLinkList__RemoveUnknownLink(ts_DLinkList **ppList, int nListCount, void *pRecord)
{
	int i;

	ASSERT(nListCount > 0);	// must have a list so i know pointer offsets

	/* if link is not head or tail of a list, we don't have to update the list pointers, so we don't care */
	/* what list it is in */

	if ((NEXT_PTR(ppList[0], pRecord) != NULL) && (PREV_PTR(ppList[0], pRecord) != NULL))
	{
		/* join previous and next records */

		NEXT_PTR(ppList[0], PREV_PTR(ppList[0], pRecord)) = NEXT_PTR(ppList[0], pRecord);
		PREV_PTR(ppList[0], NEXT_PTR(ppList[0], pRecord)) = PREV_PTR(ppList[0], pRecord);
	}
	else if (PREV_PTR(ppList[0], pRecord) == NULL)
	{
		/* head of list. find the list that points to it */

		i = 0;
		while ((i < nListCount) && (ppList[i]->pHead != pRecord))
			i++;
		ASSERT(i < nListCount);	// if this hits, record is not in any of the candidate lists
		if (i < nListCount)
			DLinkList__RemoveLink(ppList[i], pRecord);
	}
	else
	{
		/* tail of list. find the list that points to it */

		ASSERT(NEXT_PTR(ppList[0], pRecord) == NULL);	// must be tail, nothing else is left
		i = 0;
		while ((i < nListCount) && (ppList[i]->pTail != pRecord))
			i++;
		ASSERT(i < nListCount);	// if this hits, record is not in any of the candidate lists
		if (i < nListCount)
			DLinkList__RemoveLink(ppList[i], pRecord);
	}
}

/* get the head of the list, removing it from the list */

void *DLinkList__GetAndRemoveHead(ts_DLinkList *pList)
{
	void *pRecord;

	if ((pRecord = pList->pHead) == NULL)
		return(NULL);
	DLinkList__RemoveLink(pList, pRecord);
	return(pRecord);
}

/* add a link to the head of a list */

void DLinkList__AddLinkToHead(ts_DLinkList *pList, void *pRecord)
{
#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(pList, pRecord) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	/* new record points to current head of list, if any */

	NEXT_PTR(pList, pRecord) = pList->pHead;
	if (pList->pHead)
		PREV_PTR(pList, NEXT_PTR(pList, pRecord)) = pRecord;

	/* new record is now head of list */

	PREV_PTR(pList, pRecord) = NULL;

	/* list points to new head */

	pList->pHead = pRecord;

	/* if list was previously empty, new record is tail as well */

	if (pList->pTail == NULL)
		pList->pTail = pRecord;
}

/* add a link to the tail of a list */

void DLinkList__AddLinkToTail(ts_DLinkList *pList, void *pRecord)
{
#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(pList, pRecord) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	/* new record points to current tail of list, if any */

	PREV_PTR(pList, pRecord) = pList->pTail;
	if (pList->pTail)
		NEXT_PTR(pList, PREV_PTR(pList, pRecord)) = pRecord;

	/* new record is now tail of list */

	NEXT_PTR(pList, pRecord) = NULL;

	/* list points to new tail */

	pList->pTail = pRecord;

	/* if list was previously empty, new record is head as well */

	if (pList->pHead == NULL)
		pList->pHead = pRecord;
}

/* add one link before another link */

void DLinkList__InsertLinkBefore(ts_DLinkList *pList, void *pRecordToAdd, void *pRecordAfterIt)
{
	ASSERT( CHECK_PTR( pRecordAfterIt ) && CHECK_PTR( pRecordToAdd ) && CHECK_PTR( pList ) );
	ASSERT( pRecordToAdd != pRecordAfterIt );
	if ( !pRecordAfterIt || !pRecordToAdd || !pList )
		return;

#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(pList, pRecordAfterIt) == TRUE);
	ASSERT(DLinkList__InList(pList, pRecordToAdd) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	if ( pList->pHead == pRecordAfterIt )
	{
		DLinkList__AddLinkToHead( pList,  pRecordToAdd );
		return;
	}

	void *prev = PREV_PTR(pList, pRecordAfterIt);
	NEXT_PTR(pList, pRecordToAdd) = pRecordAfterIt;
	PREV_PTR(pList, pRecordToAdd) = prev;
	NEXT_PTR(pList, prev) = pRecordToAdd;
	PREV_PTR(pList, pRecordAfterIt) = pRecordToAdd;
}

/* add one link after another link */

void DLinkList__InsertLinkAfter(ts_DLinkList *pList, void *pRecordToAdd, void *pRecordBeforeIt)
{
	ASSERT( CHECK_PTR( pRecordBeforeIt ) && CHECK_PTR( pRecordToAdd ) && CHECK_PTR( pList ) );
	ASSERT( pRecordToAdd != pRecordBeforeIt );
	if ( !pRecordBeforeIt || !pRecordToAdd || !pList )
		return;

#ifdef CHECK_LIST_INTEGRITY
	ASSERT(DLinkList__InList(pList, pRecordBeforeIt) == TRUE);
	ASSERT(DLinkList__InList(pList, pRecordToAdd) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	if ( pList->pTail == pRecordBeforeIt )
	{
		DLinkList__AddLinkToTail( pList,  pRecordToAdd );
		return;
	}

	void *next = NEXT_PTR(pList, pRecordBeforeIt);
	NEXT_PTR(pList, pRecordToAdd) = next;
	PREV_PTR(pList, pRecordToAdd) = pRecordBeforeIt;
	PREV_PTR(pList, next) = pRecordToAdd;
	NEXT_PTR(pList, pRecordBeforeIt) = pRecordToAdd;
}

/* swap the order of two links */

void DLinkList__SwapLinks(ts_DLinkList *pList, void *pRecord1, void *pRecord2)
{
	ASSERT(NEXT_PTR(pList, pRecord1) == pRecord2);
	ASSERT(PREV_PTR(pList, pRecord2) == pRecord1);

	/* record 2 points to record 1's previous record */

	PREV_PTR(pList, pRecord2) = PREV_PTR(pList, pRecord1);

	/* was record 1 head of list? */

	if (PREV_PTR(pList, pRecord2))
	{
		/* record 1 wasn't head of list */

		NEXT_PTR(pList, PREV_PTR(pList, pRecord2)) = pRecord2;
	}
	else
	{
		/* record 1 was head of list */

		ASSERT(pList->pHead == pRecord1);
		pList->pHead = pRecord2;
	}

	/* record 1 points to record 2's next record */

	NEXT_PTR(pList, pRecord1) = NEXT_PTR(pList, pRecord2);

	/* was record 2 tail of list? */

	if (NEXT_PTR(pList, pRecord1))
	{
		/* record 2 wasn't tail of list */

		PREV_PTR(pList, NEXT_PTR(pList, pRecord1)) = pRecord1;
	}
	else
	{
		/* record 2 was tail of list */

		ASSERT(pList->pTail == pRecord2);
		pList->pTail = pRecord1;
	}

	/* and records point to each other */

	NEXT_PTR(pList, pRecord2) = pRecord1;
	PREV_PTR(pList, pRecord1) = pRecord2;
}

/* join two lists together */

void DLinkList__JoinLists(ts_DLinkList *pList1, ts_DLinkList *pList2)
{
	ASSERT(pList1->u32NextOffset == pList2->u32NextOffset);	// can't join lists whose members have link pointers in different places
	ASSERT(pList1->u32PrevOffset == pList2->u32PrevOffset);	// can't join lists whose members have link pointers in different places

	if (pList2->pHead == NULL)
		return;	// second list is empty, nothing to do
	if (pList1->pHead == NULL)
	{
		/* first list is empty, just copy head and tail from second list */

		*pList1 = *pList2;
	}
	else
	{
		/* neither list is empty. join first list's tail with second list's head */

		NEXT_PTR(pList1, pList1->pTail) = pList2->pHead;
		PREV_PTR(pList2, pList2->pHead) = pList1->pTail;

		/* and update tail pointer of first list */

		pList1->pTail = pList2->pTail;
	}
#ifdef CHECK_LIST_INTEGRITY
	DLinkList__Check(pList1);
#endif //CHECK_LIST_INTEGRITY
}

