#ifndef __SLL_LIST_H
#define __SLL_LIST_H

#include <Platform/BaseType.h>

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
extern bool 		sllGetFromList(ts_voidListSll **ppVoidList, u32 u32Flags, ts_voidListSll **ppLink, u32 *pu32Return);
extern bool 		sllAddToList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags, u32 *pu32Return);
extern bool 		sllInsertwCallback(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags,
							  			bool (*sllInsCallback)(ts_voidListSll *prevLink, ts_voidListSll *curLink, ts_voidListSll *insLink),
							  			u32 *pu32Return);
extern bool 		sllGetwCallback(ts_voidListSll **ppVoidList, ts_voidListSll **pLink, u32 u32Flags,
 				 			  	  bool (*sllGetCallback)(ts_voidListSll *curLink, void *pData), void *pData, u32 *pu32Return);
extern bool 		sllListCallback(ts_voidListSll *pVoidList, bool (*sllListCallback)(ts_voidListSll *curLink, u32 *pu32RCode),
							  	  u32 *pu32Return);
extern void 		sllGetListLength(ts_voidListSll *pVoidList, u32 *pu32Len);
extern bool		sllFindInList(ts_voidListSll **ppVoidList, ts_voidListSll *pLink, u32 u32Flags, u32 *pu32Return);

#if defined(__cplusplus)
}
#endif

#endif
