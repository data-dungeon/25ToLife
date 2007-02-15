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

/* system includes */

#include <stdio.h>

/* engine includes */

#include "l_SLLList.h"

/******************************************************************************
 * Function:	sllAddToList
 * Date:	   	6/29/98
 *
 * Purpose:		Insert a new link into either the head or tail of a list.
 * Parameters: ppVoidList	Address of a pointer to a list head or list entry
 * 								where the new link is to be added.
 * 				pLink			Link or list to be added to ppVoidList
 * 				u32Flags		Flags which determine how pLink is added to ppVoidList.
 * 								SLL_ADD_TO_HEAD	Adds pLink to the head of ppVoidList.
 * 								SLL_ADD_TO_TAIL	Adds pLink to the tail of ppVoidList.
 * 								SLL_ADD_LIST		OR'ed with one of the above flags,
 * 														this indicates that pLink is a list
 * 														and should be linked into ppVoidList.
 * 				pu32Return		Set to error type if function fails with FALSE.
 * 								SLL_INVALID_TYPE	0x00000001
 *
 * Returns:		bool			FALSE				on failure.
 * 								TRUE				on success.
 *
 *****************************************************************************/
bool sllAddToList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags, u32 *pu32Return)
{
  ts_voidListSll	**ppTempListPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(pLink       != NULL);
  ASSERT(u32Flags      != 0);

  if(u32Flags & SLL_ADD_TO_HEAD)
  {
		if(u32Flags & SLL_ADD_LIST)
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
  else if (u32Flags & SLL_ADD_TO_TAIL)
  {
  	for(ppTempListPtr = ppVoidList; *ppTempListPtr != NULL; ppTempListPtr = &(*ppTempListPtr)->next)
			continue;

		if(!(u32Flags & SLL_ADD_LIST))
			pLink->next = NULL;

	  *ppTempListPtr = pLink;
	   return(TRUE);
  }

 *pu32Return = SLL_INVALID_TYPE;
  return(FALSE);
}

/******************************************************************************
 * Function:	sllGetFromList
 * Date:	   	6/29/98
 *
 * Purpose:		Returns a link from the head or tail of a linked list.
 * Parameters: ppVoidList	Points to a list head.
 *					u32Flags 		Flags which determine what link to remove.
 *									SLL_GET_HEAD	Returns the list head and adjusts
 * 													ppVoidList to point at the next link.
 *									SLL_GET_TAIL	Returns the last link on the list and
 * 													sets the previous link as the tail.
 * 				pu32Return		Set to error type if function fails with FALSE.
 *									SLL_INVALID_TYPE	0x00000001	unknown u32Flags.
 *
 * Returns:		bool		FALSE				on failure.
 * 								TRUE				on success.
 *****************************************************************************/
 bool sllGetFromList(ts_voidListSll **ppVoidList, u32 u32Flags, ts_voidListSll **ppLink, u32 *pu32Return)
{
  ts_voidListSll **ppTempListPtr;
  ts_voidListSll **ppPrevPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(ppLink != NULL);
  ASSERT(u32Flags      != 0);

	if(u32Flags & SLL_GET_HEAD)
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
	else if(u32Flags & SLL_GET_TAIL)
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

  *pu32Return = SLL_INVALID_TYPE;
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
 *					u32Flags			Included for future use.  Use SLL_INSERT_MIDDLE.
 *					sllInsCallback	A user defined callback function which is called
 * 									for each link in ppVoidList.  If sllInsCallback
 * 									returns TRUE the link is inserted after the
 * 									previous and before the current link.
 *					pu32Return			Set to error tpe if function fails with FALSE.
 *										SLL_CALLBACK_INSERT_FAILED		0x00000004
 *
 * Returns:		bool			FALSE				on failure.
 * 									TRUE				on success.
 *****************************************************************************/
bool sllInsertwCallback(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags,
 											 bool (*sllInsCallback)(ts_voidListSll *prevLink, ts_voidListSll *curLink, ts_voidListSll *insLink),
											 u32 *pu32Return)
{
	ts_voidListSll *pTempListPtr;
	ts_voidListSll *pPrevPtr;

  ASSERT(ppVoidList  != NULL);
  ASSERT(sllInsCallback != NULL);
  ASSERT(pu32Return != NULL);

	if( *ppVoidList == NULL)
	{
		*ppVoidList = pLink;
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

  *pu32Return = SLL_CALLBACK_INSERT_FAILED;
	return(FALSE);
}


/******************************************************************************
 * Function:	sllGetwCallback
 * Date:	   	6/29/98
 *
 * Purpose:		Runs the list at the address of the pointer ppVoidList, where
 * 				each link in the list is passed to the user function
 * 				ssllGetCallback(). If sllGetCallback returns TRUE the link
 * 				will be returned to the user. Depending on the u32Flags flag
 * 				either the first or all of the links which return TRUE from
 * 				sllGetCallback can be returned.  See the u32Flags description below
 *  				for each flags behavior.
 *
 * Parameters:	ppVoidList			Address of a pointer to a list head.
 *					ppLink				Address of a pointer to a link pointer which can
 * 										return an individual link or a linked list.
 * 										See the u32Flags description for the possible
 * 										return behavior of ppLink.
 *					u32Flags				Flag determines if the link is to be removed
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
 * 										on u32Flags.
 *
 *					pu32Return				Set to error type if function fails with FALSE.
 *											SLL_LINK_NOT_FOUND	0x00000002
 *
 * Returns:		bool				FALSE				on failure.
 * 										TRUE				on success.
 *****************************************************************************/
bool sllGetwCallback(ts_voidListSll **ppVoidList, ts_voidListSll **ppLink, u32 u32Flags,
 										 bool (*sllGetCallback)(ts_voidListSll *curLink, void *pData),  void *pData, u32 *pu32Return)
{
  ts_voidListSll 		*pTempListPtr;
  ts_voidListSll 		*pPrevPtr;
  ts_voidListSll 		*pTempListHead;
  u32	rCode;

  ASSERT(pu32Return != NULL);
  ASSERT(ppLink != NULL);
  ASSERT(u32Flags & (SLL_GET_FIRST | SLL_GET_ALL));
  ASSERT(ppVoidList  != NULL);
  ASSERT(sllGetCallback != NULL);

	for(pTempListHead = pPrevPtr = NULL, pTempListPtr = *ppVoidList; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		if(sllGetCallback(pTempListPtr, pData) == TRUE)
		{
			if((u32Flags & (SLL_GET_NO_REMOVE | SLL_GET_FIRST | SLL_GET_ALL)) != (SLL_GET_NO_REMOVE | SLL_GET_FIRST))
			{
				if(pPrevPtr == NULL)
					*ppVoidList = pTempListPtr->next;
				else
					pPrevPtr->next = pTempListPtr->next;
			}

			if(u32Flags & SLL_GET_FIRST)
       	{
				*ppLink = pTempListPtr;
				if(!(u32Flags & SLL_GET_NO_REMOVE))
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
 * 				function returns FALSE with pu32Return set to the return code of
 * 				sllListCallback plRCode.
 * Parameters:	ppVoidLIst 			Points to a list head or starting list position.
 *					sllListCallback	Function pointer which will be called for each
 * 										link found in ppVoidList.
 *					pu32Return 			Set to error return of pListFunction?,
 * 										if pListFunction fails.
 *
 * Returns:		bool				FALSE				on failure.
 * 										TRUE				on success.
 *****************************************************************************/
bool sllListCallback(ts_voidListSll *pVoidList, bool (*sllListCallback)(ts_voidListSll *curLink, u32 *pu32RCode),
										 u32 *pu32Return)
{
	u32	rCode;
	ts_voidListSll		*pTempListPtr;

  ASSERT(sllListCallback != NULL);
  ASSERT(pu32Return != NULL);

	for(pTempListPtr = pVoidList; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
	{
		if(sllListCallback(pTempListPtr, &rCode) == FALSE)
		{
			*pu32Return = rCode;
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
void sllGetListLength(ts_voidListSll *pVoidList, u32 *pu32Len)
{
	ts_voidListSll *pTempListPtr;
	u32 lCount;

	ASSERT(pu32Len != NULL);
	for(pTempListPtr = pVoidList, lCount = 0; pTempListPtr != NULL; pTempListPtr = pTempListPtr->next)
		lCount++;
	*pu32Len = lCount;
}

/******************************************************************************
 * Function:	sllFindInList
 * Date:	   	7/01/98
 *
 * Purpose:		This function runs a linked list comparess the input address
 * 				link.  A found link can either be removed or left on the list.
 * Parameters: ppVoidList	Address of a pointer to a list head
 *					pLink			Address of a link to be removed.
 *					u32Flags      Set either to remove a found link or leave it on
 *									the list.
 *									SLI_GET_LINK	0x00000001
 *									SLI_LEAVE_LINK	0x00000002
 *				  	pu32Return    Set to error on failure.
 *
 *Returns:		TRUE on success.
 *					FALSE on failure.
 ******************************************************************************/
bool sllFindInList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags, u32 *pu32Return)
{
	ts_voidListSll **ppNextPtr;
	ts_voidListSll **ppCurrPtr;

	ASSERT(ppVoidList != NULL);
	ASSERT(pLink != NULL);
	ASSERT(u32Flags != 0);
	ASSERT(pu32Return != NULL);

	for(ppCurrPtr = ppVoidList, ppNextPtr = &(*ppVoidList)->next; *ppCurrPtr != NULL; ppNextPtr = &(*ppNextPtr)->next)
	{
		if(pLink == (ts_voidListSll *)ppNextPtr)
		{
			if(u32Flags & SLL_GET_LINK)
				*ppCurrPtr = *ppNextPtr;
			return(TRUE);

		}
		ppCurrPtr = ppNextPtr;
	}

	*pu32Return = SLL_LINK_NOT_FOUND;
	return(FALSE);
}