/*************************************/
/*                                   */
/*   l_DLinkList.h                   */
/*   new engine  07/12/00            */
/*   ned martin  avalanche software  */
/*   visual c++ 6.0                  */
/*   doubly-linked list processing   */
/*                                   */
/*************************************/

#ifndef __L_DLINKLIST_H
#define __L_DLINKLIST_H

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

/* basic doubly-linked list */

struct ts_DLinkList
{
	void *pHead;
	void *pTail;
	u32 u32NextOffset;			// offset of pNext pointer in records that list will link together
	u32 u32PrevOffset;			// offset of pPrev pointer in records that list will link together
};

/******************* global variables ********************/

/******************* global prototypes *******************/

void DLinkList__Init(ts_DLinkList *pList, u32 u32NextOffset, u32 u32PrevOffset);
void DLinkList__LinkBlock(ts_DLinkList *pList, void *pBuffer, int nCount, int nRecordSize);
#if defined(CONSUMER_BUILD)
inline int DLinkList__Check(ts_DLinkList *pList) { return 0; }
#else
int DLinkList__Check(ts_DLinkList *pList);
#endif
bool DLinkList__InList(ts_DLinkList *pList, void *pCheckRecord);
void DLinkList__RemoveLink(ts_DLinkList *pList, void *pRecord);
void DLinkList__RemoveUnknownLink(ts_DLinkList **ppList, int nListCount, void *pRecord);
void *DLinkList__GetAndRemoveHead(ts_DLinkList *pList);
void DLinkList__AddLinkToHead(ts_DLinkList *pList, void *pRecord);
void DLinkList__AddLinkToTail(ts_DLinkList *pList, void *pRecord);
void DLinkList__SwapLinks(ts_DLinkList *pList, void *pRecord1, void *pRecord2);
void DLinkList__JoinLists(ts_DLinkList *pList1, ts_DLinkList *pList2);
void DLinkList__InsertLinkBefore(ts_DLinkList *pList, void *pRecordToAdd, void *pRecordAfterIt);
void DLinkList__InsertLinkAfter(ts_DLinkList *pList, void *pRecordToAdd, void *pRecordBeforeIt);


#if defined(__cplusplus)
}
#endif // __cplusplus

#endif // __L_DLINKLIST_H
