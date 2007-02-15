/*************************************/
/*                                   */
/*   SLinkList.cpp                   */
/*   new engine  07/12/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   singly-linked list processing   */
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

/* this handles having the link pointer in each record being at an arbitrary offset within the record. basically,
point to that offset within the record and treat it as a void pointer */

#define NEXT_PTR(pList,pRecord)	*((void **) ((char *) pRecord + pList->u32NextOffset))

/******************* structures **************************/

/******************* externs *****************************/

/******************* local variables ********************/

/******************* global variables ********************/

/******************* local variables *********************/

/******************* local prototypes ********************/

/******************* functions ***************************/

/* initialize a linked list record. provide offsets of next field in records that this list will link together.
currently assumed to be 0 */

void SLinkList__Init(ts_SLinkList *pList, u32 u32NextOffset)
{
	pList->pHead = NULL;
	pList->u32NextOffset = u32NextOffset;
	pList->pTail = NULL;
}

/* link a block of records into a list. buffer is nCount records of nRecordSize, each has pNext field at
specified offset */

void SLinkList__LinkBlock(ts_SLinkList *pList, void *pBuffer, int nCount, int nRecordSize)
{
	int i;
	void *pRecord;

	pRecord = pBuffer;
	for (i = 0; i < nCount; i++)
	{
		if (i == 0)
		{
			/* first record */

			pList->pHead = pRecord;

			/* may be last, too */

			if (i == nCount - 1)
				NEXT_PTR(pList, pRecord) = NULL;	// only record in list
			else
				NEXT_PTR(pList, pRecord) = (char *) pRecord + nRecordSize;
		}
		else if (i == nCount - 1)
		{
			/* last record */

			NEXT_PTR(pList, pRecord) = NULL;

			/* TJC - remember the end */
			pList->pTail = pRecord;
		}
		else
		{
			/* interior record */

			NEXT_PTR(pList, pRecord) = (char *) pRecord + nRecordSize;
		}

		/* to next record in buffer */

		pRecord = (char *) pRecord + nRecordSize;
	}
#ifdef CHECK_LIST_INTEGRITY
	SLinkList__Check(pList);
#endif //CHECK_LIST_INTEGRITY
}

/* check the integrity of a list. return the length of the list */

#ifndef CONSUMER_BUILD
int SLinkList__Check(ts_SLinkList *pList)
{
	int nCount;
	void *pRecord;

	/* check list's integrity and return its length */

	if (pList->pHead == NULL)
		return(0);
	nCount = 1;
	pRecord = pList->pHead;
	while (NEXT_PTR(pList, pRecord))
	{
		/* TJC - to verify integrity of the tail pointer, we need to make sure
			that we either have another record after this one, or this record is
			pointed to by pTail */
		ASSERT(NEXT_PTR(pList, pRecord) || pList->pTail == pRecord);

		nCount++;
		pRecord = NEXT_PTR(pList, pRecord);
	}
	return(nCount);
}
#endif //CONSUMER_BUILD

/* check if the passed record is actually in the passed list */

#ifndef CONSUMER_BUILD
bool SLinkList__InList(ts_SLinkList *pList, void *pCheckRecord)
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

void SLinkList__RemoveLink(ts_SLinkList *pList, void *pRecord)
{
	void *pSearchRecord;
	void *pNextRecord;

#ifdef CHECK_LIST_INTEGRITY
	ASSERT(SLinkList__InList(pList, pRecord));
#endif //CHECK_LIST_INTEGRITY

	/* current head of list? */

	if (pList->pHead == pRecord)
	{
		/* head of list, update list's head pointer */

		pList->pHead = NEXT_PTR(pList, pRecord);	// may be NULL if this is only record in list

		// TJC - if this record was the tail of the list as well, we need to update the tail pointer
		if(pList->pTail == pRecord)
			pList->pTail = NULL;
	}
	else
	{
		/* not head of list, just tell previous record to skip around it. have to find it in list */

		pSearchRecord = pList->pHead;
		ASSERT(pSearchRecord);	// list can't be empty if we're trying to remove a link from it
		pNextRecord = NEXT_PTR(pList, pSearchRecord);	// since we want to know previous link when we find it
		while ((pNextRecord != pRecord) && (pNextRecord != NULL))
		{
			pSearchRecord = pNextRecord;
			pNextRecord = NEXT_PTR(pList, pSearchRecord);
		}
		ASSERT(pNextRecord == pRecord);
		if (pNextRecord == pRecord)
		{
			/* TJC - update tail */
			if(pList->pTail == pRecord)
				pList->pTail = pSearchRecord;

			NEXT_PTR(pList, pSearchRecord) = NEXT_PTR(pList, pRecord);	// may be NULL if record is current tail of list
		}
	}
}

/* remove a link from a list, but we don't know which list. for example, when removing a group from a scene, we
want to remove all it's instances. the instances may be active or inactive, we're not sure which list they're in.
pList points to an array of candidate lists. if the link isn't a head or tail, then we can remove it without
caring which list it is in */

void SLinkList__RemoveUnknownLink(ts_SLinkList **ppList, int nListCount, void *pRecord)
{
	int i;
	ts_SLinkList *pList;
	void *pSearchRecord;
	void *pNextRecord;

	ASSERT(nListCount > 0);	// must have a list so i know pointer offsets

	/* start looking for link in the candidate lists */

	i = 0;
	while (i < nListCount)
	{
		pList = ppList[i];

		/* current head of list? */

		if (pList->pHead == pRecord)
		{
			/* TJC - update tail */
			if(pList->pTail == pRecord)
				pList->pTail = NULL;
			
			pList->pHead = NEXT_PTR(pList, pRecord);	// may be NULL if this is only record in list
			return;
		}
		else
		{
			/* not head of list, just tell previous record to skip around it. have to find it in list */

			pSearchRecord = pList->pHead;
			ASSERT(pSearchRecord);	// list can't be empty if we're trying to remove a link from it
			pNextRecord = NEXT_PTR(pList, pSearchRecord);	// since we want to know previous link when we find it
			while ((pNextRecord != pRecord) && (pNextRecord != NULL))
			{
				pSearchRecord = pNextRecord;
				pNextRecord = NEXT_PTR(pList, pSearchRecord);
			}
			if (pNextRecord == pRecord)
			{
				/* TJC - update tail */
				if(pList->pTail == pRecord)
					pList->pTail = pSearchRecord;
			
				NEXT_PTR(pList, pSearchRecord) = NEXT_PTR(pList, pRecord);	// may be NULL if record is current tail of list
				return;
			}
		}

		/* to next candidate list */

		i++;
	}

	/* not found in any of candidate lists! */

	ASSERT(FALSE);
}

/* get the head of the list, removing it from the list */

void *SLinkList__GetAndRemoveHead(ts_SLinkList *pList)
{
	void *pRecord;

	if ((pRecord = pList->pHead) == NULL)
		return(NULL);
	SLinkList__RemoveLink(pList, pRecord);
	return(pRecord);
}

/* add a link to the head of a list */

void SLinkList__AddLinkToHead(ts_SLinkList *pList, void *pRecord)
{
#ifdef CHECK_LIST_INTEGRITY
	ASSERT(SLinkList__InList(pList, pRecord) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	/* new record points to current head of list, if any */

	NEXT_PTR(pList, pRecord) = pList->pHead;

	/* list points to new head */

	pList->pHead = pRecord;

	/* TJC - update tail */
	if(pList->pTail == NULL)
		pList->pTail = pRecord;
}

/* add a link to the tail of a list */

void SLinkList__AddLinkToTail(ts_SLinkList *pList, void *pRecord)
{
	void* pOldTail;

#ifdef CHECK_LIST_INTEGRITY
	ASSERT(SLinkList__InList(pList, pRecord) == FALSE);
#endif //CHECK_LIST_INTEGRITY

	/* empty list? */

	if (pList->pHead == NULL)
	{
		pList->pHead = pRecord;
	}
	else
	{
		pOldTail = pList->pTail;
		NEXT_PTR(pList, pOldTail) = pRecord;
	}

	/* record is new tail */

	pList->pTail = pRecord;
	NEXT_PTR(pList, pRecord) = NULL;
}

/* swap the order of two links */

void SLinkList__SwapLinks(ts_SLinkList *pList, void *pRecord1, void *pRecord2)
{
	void *pSearchRecord;
	void *pNextRecord;

	ASSERT(NEXT_PTR(pList, pRecord1) == pRecord2);

	/* TJC - update tail pointer */
	if(pRecord1 == pList->pTail)
		pList->pTail = pRecord2;
	else if(pRecord2 == pList->pTail)
		pList->pTail = pRecord1;

	/* was record 1 head of list? */

	if (pList->pHead == pRecord1)
	{
		pList->pHead = pRecord2;
		NEXT_PTR(pList, pRecord1) = NEXT_PTR(pList, pRecord2);
		NEXT_PTR(pList, pRecord2) = pRecord1;
	}
	else
	{
		/* record 1 wasn't head of list. find record that points to it */

		pSearchRecord = pList->pHead;
		ASSERT(pSearchRecord);	// list can't be empty
		pNextRecord = NEXT_PTR(pList, pSearchRecord);
		while ((pNextRecord != pRecord1) && (pNextRecord != NULL))
		{
			pSearchRecord = pNextRecord;
			pNextRecord = NEXT_PTR(pList, pSearchRecord);
		}
		ASSERT(pNextRecord == pRecord1);	// if this hits, record isn't in list!
		if (pNextRecord == pRecord1)
		{
			NEXT_PTR(pList, pSearchRecord) = pRecord2;
			NEXT_PTR(pList, pRecord1) = NEXT_PTR(pList, pRecord2);
			NEXT_PTR(pList, pRecord2) = pRecord1;
		}
	}
}

/* join two lists together */

void SLinkList__JoinLists(ts_SLinkList *pList1, ts_SLinkList *pList2)
{
	void *pLastRecord;
	void *pNextRecord;

	ASSERT(pList1->u32NextOffset == pList2->u32NextOffset);	// can't join lists whose members have link pointers in different places

	if (pList2->pHead == NULL)
		return;	// second list is empty, nothing to do
	if (pList1->pHead == NULL)
	{
		/* first list is empty, just copy from second list */

		*pList1 = *pList2;
	}
	else
	{
		/* neither list is empty. join first list's tail with second list's head. */
		/* first find last record in list */

		pLastRecord = pList1->pHead;
		pNextRecord = NEXT_PTR(pList1, pLastRecord);
		while (pNextRecord)
		{
			pLastRecord = pNextRecord;
			pNextRecord = NEXT_PTR(pList1, pLastRecord);
		}

		/* and join the lists */

		NEXT_PTR(pList1, pLastRecord) = pList2->pHead;

		/* TJC - update tail of list 1 */
		pList1->pTail = pList2->pTail;

	}
#ifdef CHECK_LIST_INTEGRITY
	SLinkList__Check(pList1);
#endif //CHECK_LIST_INTEGRITY
}


/* TJC - with addition of tail pointer, it become possible to get the tail entry quickly.. */
/* its just slow to remove :) */
void *SLinkList__GetTail(ts_SLinkList *pList)
{
	return pList->pTail;
}


/******************************************************************************
 * Module:	slllist.c
 * Date:	   6/29/98
 *
 * Creator:	Jon Matsukawa
 * Purpose:	Implementation of re-usable singly linked list code
 *
 * 			(C) 1998 Avalanche Software
 *
 *****************************************************************************/

/******************************************************************************
 * Function:	sllAddToList
 * Date:	   	6/29/98
 *
 * Purpose:		Insert a new link into either the head or tail of a list.
 * Parameters: ppVoidList	Address of a pointer to a list head or list entry
 * 								where the new link is to be added.
 * 				pLink			Link or list to be added to ppVoidList
 * 				flType		Flags which determine how pLink is added to ppVoidList.
 * 								SLL_ADD_TO_HEAD	Adds pLink to the head of ppVoidList.
 * 								SLL_ADD_TO_TAIL	Adds pLink to the tail of ppVoidList.
 * 								SLL_ADD_LIST		OR'ed with one of the above flags,
 * 														this indicates that pLink is a list
 * 														and should be linked into ppVoidList.
 * 				plReturn		Set to error type if function fails with FALSE.
 * 								SLL_INVALID_TYPE	0x00000001
 *
 * Returns:		bool		FALSE				on failure.
 * 								TRUE				on success.
 *
 *****************************************************************************/
bool sllAddToList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType, unsigned long *plReturn)
{
  ts_voidListSll	**ppTempListPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(pLink       != NULL);
  ASSERT(flType      != 0);

  if(flType & SLL_ADD_TO_HEAD)
  {
		if(flType & SLL_ADD_LIST)
     {
		  	for(ppTempListPtr = &pLink->next; *ppTempListPtr != NULL; ppTempListPtr = &(*ppTempListPtr)->next)
				continue;

		  	*ppTempListPtr = *ppVoidList;
     }
     else
     {
		  	pLink->next = *ppVoidList;
     }

     *ppVoidList = pLink;
     return(TRUE);
  }
  else if(flType & SLL_ADD_TO_TAIL)
  {
  	for(ppTempListPtr = ppVoidList; *ppTempListPtr != NULL; ppTempListPtr = &(*ppTempListPtr)->next)
			continue;

		if(!(flType & SLL_ADD_LIST))
			pLink->next = NULL;

	  *ppTempListPtr = pLink;
	   return(TRUE);
  }

 *plReturn = SLL_INVALID_TYPE;
  return(FALSE);
}

/******************************************************************************
 * Function:	sllGetFromList
 * Date:	   	6/29/98
 *
 * Purpose:		Returns a link from the head or tail of a linked list.
 * Parameters: ppVoidList	Points to a list head.
 *					flType 		Flags which determine what link to remove.
 *									SLL_GET_HEAD	Returns the list head and adjusts
 * 													ppVoidList to point at the next link.
 *									SLL_GET_TAIL	Returns the last link on the list and
 * 													sets the previous link as the tail.
 * 				plReturn		Set to error type if function fails with FALSE.
 *									SLL_INVALID_TYPE	0x00000001	unknown flType.
 *
 * Returns:		bool		FALSE				on failure.
 * 								TRUE				on success.
 *****************************************************************************/
 bool sllGetFromList(ts_voidListSll **ppVoidList, unsigned long flType, ts_voidListSll **ppLink, unsigned long *plReturn)
{
  ts_voidListSll **ppTempListPtr;
  ts_voidListSll **ppPrevPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(ppLink != NULL);
  ASSERT(flType      != 0);

	if(flType & SLL_GET_HEAD)
	{
	  *ppLink = *ppVoidList;
     if(*ppVoidList != NULL)
     {
	     *ppVoidList = (*ppVoidList)->next;
			return(TRUE);
     }
     else
     	return(FALSE);
	}
	else if(flType & SLL_GET_TAIL)
	{
		if(*ppVoidList == NULL)
		{
	     *ppLink = *ppVoidList;
   	   return(FALSE);
		}

		for(ppPrevPtr = ppVoidList, ppTempListPtr = &(*ppVoidList)->next; *ppTempListPtr != NULL; ppTempListPtr = &(*ppTempListPtr)->next)
   	{
			ppPrevPtr = ppTempListPtr;
			continue;
   	}

		*ppPrevPtr = NULL;
		*ppLink  = (ts_voidListSll *)ppTempListPtr;
		return(TRUE);
  }

  *plReturn = SLL_INVALID_TYPE;
  return(FALSE);
}


/******************************************************************************
 * Function:	sllInsertwCallback
 * Date:	   	6/29/98
 *
 * Purpose:		Runs the list at the address of the pointer ppVoidList, where a
 * 				current link and previous link pointer are passed to the user
 * 				defined sllInsCallback.  If sllInsCallback returns TRUE pLink
 *  				is inserted after the previous and before the current link.
 *
 *	Parameters:	ppVoidList		Address of a pointer to a list head.
 *					pLink				Pointer to a link to be inserted if sllInsCallback
 * 									returns TRUE.
 *					flType			Included for future use.  Use SLL_INSERT_MIDDLE.
 *					sllInsCallback	A user defined callback function which is called
 * 									for each link in ppVoidList.  If sllInsCallback
 * 									returns TRUE the link is inserted after the
 * 									previous and before the current link.
 *					plReturn			Set to error tpe if function fails with FALSE.
 *										SLL_CALLBACK_INSERT_FAILED		0x00000004
 *
 * Returns:		bool			FALSE				on failure.
 * 									TRUE				on success.
 *****************************************************************************/
bool sllInsertwCallback(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType,
 											 bool (*sllInsCallback)(ts_voidListSll *prevLink, ts_voidListSll *curLink, ts_voidListSll *insLink),
											 unsigned long *plReturn)
{
	ts_voidListSll *pTempListPtr;
	ts_voidListSll *pPrevPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(sllInsCallback != NULL);
  ASSERT(plReturn != NULL);

	if( *ppVoidList == NULL)
	{
		*ppVoidList = pLink;
		pLink->next = NULL;
		return( TRUE);
	}

	for(pPrevPtr = NULL, pTempListPtr = *ppVoidList; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		if(sllInsCallback(pPrevPtr, pTempListPtr, pLink) == TRUE)
		{
			if(pPrevPtr == NULL)
			{
				pLink->next = pTempListPtr;
				*ppVoidList = pLink;
			}
			else
			{
				pPrevPtr->next = pLink;
				pLink->next    = pTempListPtr;
			}
			return(TRUE);
		}

     pPrevPtr = pTempListPtr;
	}

	if( sllInsCallback( pPrevPtr, NULL, pLink) == TRUE)
	{
		pPrevPtr->next = pLink;
		pLink->next = NULL;
		return(TRUE);
	}

  *plReturn = SLL_CALLBACK_INSERT_FAILED;
	return(FALSE);
}


/******************************************************************************
 * Function:	sllGetwCallback
 * Date:	   	6/29/98
 *
 * Purpose:		Runs the list at the address of the pointer ppVoidList, where
 * 				each link in the list is passed to the user function
 * 				ssllGetCallback(). If sllGetCallback returns TRUE the link
 * 				will be returned to the user. Depending on the flType flag
 * 				either the first or all of the links which return TRUE from
 * 				sllGetCallback can be returned.  See the flType description below
 *  				for each flags behavior.
 *
 * Parameters:	ppVoidList			Address of a pointer to a list head.
 *					ppLink				Address of a pointer to a link pointer which can
 * 										return an individual link or a linked list.
 * 										See the flType description for the possible
 * 										return behavior of ppLink.
 *					flType				Flag determines if the link is to be removed
 * 										if found.
 *											SLL_GET_FIRST	Returns the first link where
 * 															sllGetCallback returns TRUE.
 *											SLL_GET_ALL		ppLink returns the address of a
 * 															pointer to a linked list
 * 															containing all links for which
 * 															sllGetCallback returned TRUE.
 * 			  *only with FIRST*  SLL_GET_NO_REMOVE Returns the link or links in
 * 									 						the list that the call back specifies
 * 															but does not remove the links from the list.
 * 															ONLY WORKS WITH SLL_GET_FIRST!!!!
 *					sllGetCallback		A user defined callback function which is called
 * 										for each link in ppVoidList.  If sllGetCallback
 * 										returns TRUE the link is either returned
 * 										immediately or linked on a return list depending
 * 										on flType.
 *
 *					plReturn				Set to error type if function fails with FALSE.
 *											SLL_LINK_NOT_FOUND	0x00000002
 *
 * Returns:		bool				FALSE				on failure.
 * 										TRUE				on success.
 *****************************************************************************/
bool sllGetwCallback(ts_voidListSll **ppVoidList, ts_voidListSll **ppLink, unsigned long flType,
 										 bool (*sllGetCallback)(ts_voidListSll *curLink, void *pData),  void *pData, unsigned long *plReturn)
{
  ts_voidListSll 		*pTempListPtr;
  ts_voidListSll 		*pPrevPtr;
  ts_voidListSll 		*pTempListHead;
  unsigned long	rCode;

  ASSERT(plReturn != NULL);
  ASSERT(ppLink != NULL);
  ASSERT(flType & (SLL_GET_FIRST | SLL_GET_ALL));
  ASSERT(ppVoidList  != NULL);
  ASSERT(sllGetCallback != NULL);

	for(pTempListHead = pPrevPtr = NULL, pTempListPtr = *ppVoidList; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		if(sllGetCallback(pTempListPtr, pData) == TRUE)
		{
			if((flType & (SLL_GET_NO_REMOVE | SLL_GET_FIRST | SLL_GET_ALL)) != (SLL_GET_NO_REMOVE | SLL_GET_FIRST))
			{
				if(pPrevPtr == NULL)
					*ppVoidList = pTempListPtr->next;
				else
					pPrevPtr->next = pTempListPtr->next;
			}

			if(flType & SLL_GET_FIRST)
       	{
				*ppLink = pTempListPtr;
				if(!(flType & SLL_GET_NO_REMOVE))
					(*ppLink)->next = NULL;

				return(TRUE);
			}
			else
			{
				sllAddToList((ts_voidListSll **)&pTempListHead, pTempListPtr, SLL_ADD_TO_HEAD, &rCode);
				if(pPrevPtr != NULL)
					pTempListPtr = pPrevPtr;
				else
           	pTempListPtr = *ppVoidList;
           continue;
			}
		}

     pPrevPtr = pTempListPtr;
	}

	if(pTempListHead != NULL)
	{
		*ppLink = pTempListHead;
  		return(TRUE);
	}
	else
		return(FALSE);
}

/******************************************************************************
 * Function:	sllListCallback
 * Date:	   	6/29/98
 *
 * Purpose:		Runs the list at the address of the pointer ppVoidList, where
 * 				each link in the list is passed to the user function
 * 				ssllListCallback(). If sllListCallback returns FALSE the
 * 				function returns FALSE with plReturn set to the return code of
 * 				sllListCallback plRCode.
 * Parameters:	ppVoidLIst 			Points to a list head or starting list position.
 *					sllListCallback	Function pointer which will be called for each
 * 										link found in ppVoidList.
 *					plReturn 			Set to error return of pListFunction?,
 * 										if pListFunction fails.
 *
 * Returns:		bool				FALSE				on failure.
 * 										TRUE				on success.
 *****************************************************************************/
bool sllListCallback(ts_voidListSll *pVoidList, bool (*sllListCallback)(ts_voidListSll *curLink, unsigned long *plRCode),
										 unsigned long *plReturn)
{
	unsigned long	rCode;
	ts_voidListSll		*pTempListPtr;

  ASSERT(sllListCallback != NULL);
  ASSERT(plReturn != NULL);

	for(pTempListPtr = pVoidList; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		if(sllListCallback(pTempListPtr, &rCode) == FALSE)
		{
			*plReturn = rCode;
			return(FALSE);
		}
	}
  return(TRUE);
}

/******************************************************************************
 * Function:	sllGetListLength
 * Date:	   	6/29/98
 *
 * Purpose:		This exists mainly for debug purposes.  This function simply runs
 * 				the linked list and returns the number of links in pVoidList.
 * Parameters: pVoidList	Points to a list head.
 *					plLen			Returns the number of links found in pVoidList.
 *
 * Returns:		none
 ******************************************************************************/
void sllGetListLength(ts_voidListSll *pVoidList, unsigned long *plLen)
{
	ts_voidListSll		*pTempListPtr;
	unsigned long	lCount;

  ASSERT(plLen != NULL);

	for(pTempListPtr = pVoidList, lCount = 0; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		lCount++;
	}
	*plLen = lCount;
}


/******************************************************************************
 * Function:	sllFindInList
 * Date:	   	7/01/98
 *
 * Purpose:		This function runs a linked list comparess the input address
 * 				link.  A found link can either be removed or left on the list.
 * Parameters: ppVoidList	Address of a pointer to a list head
 *					pLink			Address of a link to be removed.
 *					flType      Set either to remove a found link or leave it on
 *									the list.
 *									SLI_GET_LINK	0x00000001
 *									SLI_LEAVE_LINK	0x00000002
 *				  	plReturn    Set to error on failure.
 *
 *Returns:		TRUE on success.
 *					FALSE on failure.
 ******************************************************************************/
bool sllFindInList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType, unsigned long *plReturn)
{
	ts_voidListSll **ppNextPtr;
	ts_voidListSll **ppCurrPtr;

	ASSERT(ppVoidList  != NULL);
	ASSERT(pLink != NULL);
	ASSERT(flType      != 0);

	for(ppCurrPtr = ppVoidList, ppNextPtr = &(*ppVoidList)->next; *ppCurrPtr != NULL; ppNextPtr = &(*ppNextPtr)->next)
	{
		if(pLink == (ts_voidListSll *)ppNextPtr)
		{
			if(flType & SLL_GET_LINK)
				*ppCurrPtr = *ppNextPtr;
			return(TRUE);

		}
		ppCurrPtr = ppNextPtr;
	}

	*plReturn = SLL_LINK_NOT_FOUND;
	return(FALSE);
}

