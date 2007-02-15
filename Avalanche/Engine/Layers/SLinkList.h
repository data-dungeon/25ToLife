/*************************************/
/*                                   */
/*   l_SLinkList.h                   */
/*   new engine  07/12/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   singly-linked list processing   */
/*                                   */
/*************************************/

#ifndef __L_SLINKLIST_H
#define __L_SLINKLIST_H

/******************* includes ****************************/

/* system includes */

#include <stdio.h>

/* engine includes */

#include "platform/BaseType.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

/******************* defines *****************************/

/******************* macros ******************************/

/******************* structures **************************/

/* basic singly-linked list */

struct ts_SLinkList
{
	void *pHead;
	u32 u32NextOffset;			// offset of pNext pointer in records that list will link together

	// TJC - I added a pointer to the tail of the list, to make AddLinkToTail run at a decent speed.
	// so this means that a singly linked list can still be used to add to tail (LRU caches :)...
	void *pTail;
};



/******************* global variables ********************/

/******************* global prototypes *******************/

void SLinkList__Init(ts_SLinkList *pList, u32 u32NextOffset);
void SLinkList__LinkBlock(ts_SLinkList *pList, void *pBuffer, int nCount, int nRecordSize);
#if defined(CONSUMER_BUILD)
inline int SLinkList__Check(ts_SLinkList *pList) { return 0; }
#else
int SLinkList__Check(ts_SLinkList *pList);
#endif
bool SLinkList__InList(ts_SLinkList *pList, void *pCheckRecord);
void SLinkList__RemoveLink(ts_SLinkList *pList, void *pRecord);
void SLinkList__RemoveUnknownLink(ts_SLinkList **ppList, int nListCount, void *pRecord);
void *SLinkList__GetAndRemoveHead(ts_SLinkList *pList);
void SLinkList__AddLinkToHead(ts_SLinkList *pList, void *pRecord);
void SLinkList__AddLinkToTail(ts_SLinkList *pList, void *pRecord);
void SLinkList__SwapLinks(ts_SLinkList *pList, void *pRecord1, void *pRecord2);
void SLinkList__JoinLists(ts_SLinkList *pList1, ts_SLinkList *pList2);

/* TJC - with addition of tail pointer, it becomes possible to get the tail entry quickly.. */
/* its just slow to remove :) */
void *SLinkList__GetTail(ts_SLinkList *pList);

/* TJC - this is still kind of slow because removelink is slow */
inline void SLinkList__MoveToTail(ts_SLinkList *pList, void *pRecord){ SLinkList__RemoveLink(pList, pRecord); SLinkList__AddLinkToTail(pList, pRecord); }

#if defined(__cplusplus)
}
#endif // __cplusplus


/* -----------------9/19/2001 2:41PM-----------------
 * JTM: Below are the original SLL list functions with
 * call back list processing capibility.
 * --------------------------------------------------*/
 #if defined(__cplusplus)
extern "C" {
#endif

typedef struct voidListSll
{
	struct voidListSll *next;
}ts_voidListSll;


// sllAddToList function flags
#define SLL_ADD_TO_HEAD						0x00000001
#define SLL_ADD_TO_TAIL						0x00000002
#define SLL_ADD_LIST							0x00000004

// sllGetFromList function flags
#define SLL_GET_HEAD							0x00000001
#define SLL_GET_TAIL							0x00000002

// sllGetwCallback function flags
#define SLL_GET_FIRST						0x00000001
#define SLL_GET_ALL							0x00000002
#define SLL_GET_NO_REMOVE					0x00000004

// sllInsertwCallback function flags.
#define SLL_INSERT_MIDDLE					0x00000001

// sllFindInList function flags
#define SLL_GET_LINK							0x00000001
#define SLL_LEAVE_LINK						0x00000002

// Error returns
#define SLL_INVALID_TYPE					0x00000001
#define SLL_LINK_NOT_FOUND					0x00000002
#define SLL_CALLBACK_INSERT_FAILED 		0x00000004




// Function prototypes
extern bool 		sllGetFromList(ts_voidListSll **ppVoidList, unsigned long flType, ts_voidListSll **ppLink, unsigned long *plReturn);
extern bool 		sllAddToList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType, unsigned long *plReturn);
extern bool 		sllInsertwCallback(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType,
							  						bool (*sllInsCallback)(ts_voidListSll *prevLink, ts_voidListSll *curLink, ts_voidListSll *insLink), unsigned long *plReturn);
extern bool 		sllGetwCallback(ts_voidListSll **ppVoidList, ts_voidListSll **pLink, unsigned long flType,
 				 			  	  					bool (*sllGetCallback)(ts_voidListSll *curLink, void *pData), void *pData, unsigned long *plReturn);
extern bool 		sllListCallback(ts_voidListSll *pVoidList, bool (*sllListCallback)(ts_voidListSll *curLink, unsigned long *plRCode), unsigned long *plReturn);
extern void 		sllGetListLength(ts_voidListSll *pVoidList, unsigned long *plLen);
extern bool			sllFindInList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, unsigned long flType, unsigned long *plReturn);

#if defined(__cplusplus)
}
#endif


















#endif // __L_SLINKLIST_H
