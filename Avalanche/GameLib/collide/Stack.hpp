/*
**
**  File:   Stack.hpp
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   A simple stack for transient objects used in collision testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/collide/Stack.hpp $
**  $Revision: #21 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#ifdef STACK_HPP
#undef STACK_HPP

// System includes //
#include <new.h>
#include <string.h>

// Engine includes //
#include "Layers/Memory.h"
#ifdef DEBUG_STACK
#include "Layers/Debug.h"
#include "EngineHelper/timer.h"
#endif

#define BOTTOM_LOCKED 0x80000000

#ifdef DEBUG_STACK
// The code will ASSERT if a hole is created in the stack that is larger than
// DEBUG_ALLOWED_HOLE unless DEBUG_ALLOWED_HOLE is less than zero or
// d_dumpChanges is true. d_dumpChanges should be set to true in the debugger
// and a good time to do that is when execution is stopped due to an ASSERT
// from a large hole.
#define DEBUG_ALLOWED_HOLE -1
#endif

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Reset()
 *	Access:      public
 *	Description: Resets the stack. Any items that were on the stack are
 *              no longer valid. Also, none of their destructors are
 *              called.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::Reset(void)
{
//	ASSERT(d_currentLock == &d_baseLock);
	d_end = d_currentLock->d_end;
	d_lastItem = d_currentLock->d_lastItem;
	d_bottom = d_size;
#ifdef DEBUG_STACK
	d_currentReset = true;
	if (d_currentLock == &d_baseLock)
		d_firstItem = 0;
#endif
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Initialize()
 *	Access:      public
 *	Description: Allocates the stack and sets it to unused. This must
 *              be called before the stack can be used.
 *	Arguments:
 *    i_size  - The size in bytes of the stack.
 *	Return:      True for success, false otherwise. This will fail if
 *              the requested amount of memory cannot be allocated.
 *              or if a different stack has already been associated
 *              with the CCollisionStackItem class.
 *
 **********************************************************************/
inline bool CCollisionStack::Initialize(uint32 i_size)
{
	if ((d_stack != NULL) && d_ownsStack)
		memFree((void*)d_stack);
	MEM_SET_ALLOC_NAME("CCollisionStack");
	d_stack = (byte*)memAlloc(i_size * sizeof(byte));
	ASSERT(d_stack != NULL);
	d_ownsStack = true;
	d_size = i_size;
	d_bottom = i_size;
	d_bottomKey = 0;
	Reset();

#ifdef DEBUG_STACK
	d_peakSize = 0;
	d_peakName[0] = '\0';
	d_peakName[STACK_NAME_LENGTH - 1] = '\0';
	d_currentName[0] = '\0';
	d_currentName[STACK_NAME_LENGTH - 1] = '\0';
	d_dumpChanges = false;
#endif

	return(d_stack != NULL);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Initialize()
 *	Access:      public
 *	Description: Uses a specifed chunk of memory for the stack and sets
 *              it to unused. This must be called before the stack can
 *              be used.
 *	Arguments:
 *    i_stack - The location of the stack.
 *    i_size  - The size in bytes of the stack.
 *	Return:      True for success, false otherwise. This will fail if
 *              the requested amount of memory cannot be allocated.
 *              or if a different stack has already been associated
 *              with the CCollisionStackItem class.
 *
 **********************************************************************/
inline bool CCollisionStack::Initialize(byte* i_stack, uint32 i_size)
{
	if ((d_stack != NULL) && d_ownsStack)
		memFree((void*)d_stack);
	d_stack = i_stack;
	d_ownsStack = false;
	d_size = i_size;
	d_bottom = i_size;
	d_bottomKey = 0;
	Reset();

#ifdef DEBUG_STACK
	d_peakSize = 0;
	d_peakName[0] = '\0';
	d_peakName[STACK_NAME_LENGTH - 1] = '\0';
	d_currentName[0] = '\0';
	d_currentName[STACK_NAME_LENGTH - 1] = '\0';
	d_dumpChanges = false;
#endif

	return(d_stack != NULL);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Deallocate()
 *	Access:      public
 *	Description: Releases memory back to the stack. If the deallocated
 *              item is at the end of the stack, its memory is freed
 *              for other uses. If it is not at the end of the stack,
 *              the memory remains tied up; however, it will be freed
 *              after all following items on the stack are deallocated.
 *	Arguments:
 *    i_item  - A pointer to the item to be deallocated.
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::Deallocate(CCollisionStackItem* i_item)
{
/*
 *	Make sure stack has been initialized and that the item is in the
 *	valid stack range.
 */
	ASSERT((d_stack != NULL) && ((void*)i_item >= (void*)d_stack) &&
			((void*)i_item < (void*)(d_stack + d_end)));
/*
 *	Deallocate the item if it at the end of the stack.
 */
	if (i_item->d_itemEnd == d_end)
	{
		d_lastItem = i_item->d_previousItem;
		if (d_lastItem == (uint32)~0)
			d_end = 0;
		else
		{
			CCollisionStackItem* endItem =
					(CCollisionStackItem*)(&d_stack[d_lastItem]);
			d_end = endItem->d_itemEnd;
			endItem->d_nextItem = d_end;
		}
#ifdef DEBUG_STACK
		if (d_lastItem == (uint32)~0)
			d_firstItem = 0;

		if (d_dumpChanges)
		{
			char msg[256];
			sprintf(msg, "Stack Deallocate From End - "
					"Start = %u, End = %u, Size = %u\n",
					i_item->d_itemStart, i_item->d_itemEnd,
					i_item->d_itemEnd - i_item->d_itemStart);
			conPuts(msg);

			sprintf(msg, "\tStack End = %u\n", d_end);
			conPuts(msg);
		}
#endif
	}
/*
 *	Deallocate the item if is not at the end of the stack. Reset pointers
 *	to previous and next items so the memory associated with this item can
 *	be freed when all following items are deallocated.
 */
	else
	{
/*
 *		Point the next item to this item's previous item.
 */
		((CCollisionStackItem*)(&d_stack[i_item->d_nextItem]))->d_previousItem =
				i_item->d_previousItem;
/*
 *		If this item is not at the beginning of the stack, point the previous
 *		item to this item's next item.
 */
		if (i_item->d_previousItem != (uint32)~0)
		{
			((CCollisionStackItem*)(&d_stack[i_item->d_previousItem]))->d_nextItem
					= i_item->d_nextItem;
		}
#ifdef DEBUG_STACK
		else
			d_firstItem = i_item->d_nextItem;
#endif

#ifdef DEBUG_STACK
		uint32 holeStart;
		if (i_item->d_previousItem != (uint32)~0)
		{
			holeStart = ((CCollisionStackItem*)
					(&d_stack[i_item->d_previousItem]))->d_itemEnd;
		}
		else
			holeStart = 0;
		uint32 holeSize = i_item->d_nextItem - holeStart;
		ASSERT(d_dumpChanges ||
				(DEBUG_ALLOWED_HOLE < 0) || (holeSize <= DEBUG_ALLOWED_HOLE));

		if (d_dumpChanges)
		{
			char msg[256];
			sprintf(msg, "Stack Deallocate From Middle - "
					"Start = %u, End = %u, Size = %u\n",
					i_item->d_itemStart, i_item->d_itemEnd,
					i_item->d_itemEnd - i_item->d_itemStart);
			conPuts(msg);

			if (((CCollisionStackItem*)(&d_stack[holeStart]) == i_item) &&
					(i_item->d_nextItem == i_item->d_itemEnd))
				conPuts("\tStack Hole Created");
			else
				conPuts("\tStack Hole Enlarged");
			sprintf(msg, " - Size = %u, Start = %u, End = %u\n",
					holeSize, holeStart, i_item->d_nextItem);
			conPuts(msg);
		}
#endif
	}
#ifdef DEBUG_STACK
//	ValidateStack();
#endif
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Unlock()
 *	Access:      public
 *	Description: Establishes a lock at the current end of the stack.
 *	Arguments:
 *    i_lock - The lock object.
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::Unlock(CCollisionStackLock* i_lock)
{
	ASSERT((i_lock != NULL) && (i_lock->d_prev != NULL) &&
			(i_lock->d_item != NULL));

	if (i_lock == d_currentLock)
		d_currentLock = i_lock->d_prev;

	i_lock->d_prev->d_next = i_lock->d_next;
	if (i_lock->d_next)
		i_lock->d_next->d_prev = i_lock->d_prev;

	i_lock->d_item->~CCollisionStackItem();
	Deallocate(i_lock->d_item);
}

/**********************************************************************
 *
 *	Class:       CCollisionLock
 *	Function:    Release()
 *	Access:      public
 *	Description: Release a lock (if any).
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStackLock::Release(void)
{
	if (d_item != NULL)
	{
		ASSERT(d_item->d_stack != NULL);
		d_item->d_stack->Unlock(this);
		d_item = NULL;
		d_prev = NULL;
		d_next = NULL;
		d_end = 0;
		d_lastItem = ~0;
	}
}
/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Terminate()
 *	Access:      public
 *	Description: Deallocates the stack and puts it into a state that
 *              can be initialized again.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::Terminate(void)
{
	while(d_baseLock.d_next != NULL)
		d_baseLock.d_next->Release();

	if ((d_stack != NULL) && d_ownsStack)
		memFree((void*)d_stack);
	d_stack = NULL;
	d_size = 0;
	d_bottom = 0;
	Reset();

#ifdef DEBUG_STACK
	char msg[STACK_NAME_LENGTH + 35];
	if (d_peakReset)
		sprintf(msg, "Stack Peak Size = %d (%s - reset)\n", d_peakSize,
				d_peakName);
	else
		sprintf(msg, "Stack Peak Size = %d (%s)\n", d_peakSize, d_peakName);
	conPuts(msg);
#endif
}



/**********************************************************************
 *
 *	Class:       CCollisionLock
 *	Function:    ~CCollisionLock()
 *	Access:      public
 *	Description: Destruct a lock by making sure no lock is set.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline CCollisionStackLock::~CCollisionStackLock()
{
	Release();
}

/**********************************************************************
 *
 *	Class:       CCollisionLock
 *	Function:    CCollisionLock()
 *	Access:      public
 *	Description: Construct a lock and initialize it as if it were the
 *              base lock item for a stack.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline CCollisionStackLock::CCollisionStackLock()
{
	d_item = NULL;
	d_prev = NULL;
	d_next = NULL;
	d_end = 0;
	d_lastItem = ~0;
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionStack::CCollisionStack()
{
	d_stack = NULL;
	d_currentLock = &d_baseLock;
	d_size = 0;
	d_bottom = 0;
	Reset();
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionStack::CCollisionStack(uint32 i_size)
{
	d_stack = NULL;
	d_currentLock = &d_baseLock;
	Initialize(i_size);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionStack::CCollisionStack(byte* i_stack, uint32 i_size)
{
	d_stack = NULL;
	d_currentLock = &d_baseLock;
	Initialize(i_stack, i_size);
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionStack::~CCollisionStack()
{
	Terminate();
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Allocate()
 *	Access:      public
 *	Description: Reserves the requested amount of memory at the end
 *              of the stack.
 *	Arguments:
 *    i_size  - The size in bytes of the requested stack item.
 *	Return:      A pointer to the allocated item if successful, NULL
 *              otherwise. This will fail if there is not enough room
 *              left on the stack.
 *
 **********************************************************************/
inline CCollisionStackItem* CCollisionStack::Allocate(uint32 i_size)
{
#ifdef PS2
	// TJC -  enforce VU alignment... (VU_ALIGN)
	if(i_size & (VU_ALIGN-1)) 
		i_size += VU_ALIGN - (i_size & (VU_ALIGN-1));
	ASSERT(IS_VU_ALIGNED(i_size));
	ASSERT(IS_VU_ALIGNED(d_end));
#endif

/*
 *	Make sure the stack has been initialized and that the size of the
 *	requeseted memory is at least big enough to contain a stack item
 *	base class object.
 */
	ASSERT((d_stack != NULL) && (i_size >= sizeof(CCollisionStackItem)));
/*
 *	See if there is enough room left on the stack.
 */
	if ((d_end + i_size) >= d_bottom)
	{
#ifdef DEBUG_STACK
		DumpStack();
#endif
		ASSERTF(false, ("Collision Stack Out Of Space: (%d + %d) >= %d",
				d_end, i_size, d_bottom));
		return(NULL);
	}
/*
 *	Reserve the memory on the stack, set up the item and return success.
 */
	CCollisionStackItem* item = (CCollisionStackItem*)(&d_stack[d_end]);
#ifdef DEBUG_STACK
	item->d_itemStart = d_end;
#endif
	item->d_previousItem = d_lastItem;
	d_lastItem = d_end;
	d_end += i_size;
	item->d_nextItem = d_end;
	item->d_itemEnd = d_end;

#ifdef PS2
	// TJC - enforce VU alignment
	ASSERT(IS_VU_ALIGNED(d_end));
	ASSERT(IS_VU_ALIGNED(item));
#endif

#ifdef DEBUG_STACK
	CheckPeak();
//	ValidateStack();

	if (d_dumpChanges)
	{
		char msg[256];
		sprintf(msg, "Stack Allocate - Start = %u, End = %u, Size = %u\n",
				item->d_itemStart, item->d_itemEnd,
				item->d_itemEnd - item->d_itemStart);
		conPuts(msg);
	}
#endif
	return(item);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Top()
 *	Access:      public
 *	Description: Gets the top of the stack.
 *	Arguments:   None
 * Return:      A pointer to the memory at the top of the stack.
 *
 **********************************************************************/
inline void* CCollisionStack::Top(void)
{
	return((void*)&d_stack[d_end]);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Expand()
 *	Access:      public
 *	Description: Extends the end of the last item on the stack to give
 *              it more room.
 *	Arguments:
 *    i_size        - The size in bytes of the requested expansion.
 *    i_arrayHeader - The item that contains the array.
 *	Return:      A pointer to the newly aquired memory. If there is not
 *              enough room left on the stack or if a valid key is not
 *              provided, NULL is returned.
 *
 **********************************************************************/
inline void* CCollisionStack::Expand(uint32 i_size,
		CCollisionStackItem* i_arrayHeader)
{
#ifdef PS2
	// TJC -  enforce VU alignment... (VU_ALIGN)
	if(i_size & (VU_ALIGN-1)) 
		i_size += VU_ALIGN - (i_size & (VU_ALIGN-1));
	ASSERT(IS_VU_ALIGNED(i_size));
	ASSERT(IS_VU_ALIGNED(d_end));
#endif

/*
 *	Make sure stack has been initialized, that the header is in the valid
 *	stack range and that the last item is being expanded.
 */
	ASSERT((d_stack != NULL) && ((void*)i_arrayHeader >= (void*)d_stack) &&
			((void*)i_arrayHeader < (void*)(d_stack + d_end)) &&
			(i_arrayHeader->d_itemEnd == d_end));
/*
 *	See if there is enough room left on the stack.
 */
	if ((d_end + i_size) >= d_bottom)
	{
#ifdef DEBUG_STACK
		DumpStack();
#endif
		ASSERTF(false, ("Collision Stack Out Of Space: (%d + %d) >= %d", d_end, i_size, d_bottom));
		return(NULL);
	}
/*
 *	Move the end of the stack the requested amount.
 */
	uint32 expansion = d_end;
	d_end += i_size;
	i_arrayHeader->d_nextItem = d_end;
	i_arrayHeader->d_itemEnd = d_end;

#ifdef PS2
	// TJC - enforce vu alignment
	ASSERT(IS_VU_ALIGNED(d_end));
#endif

#ifdef DEBUG_STACK
	if (d_dumpChanges)
	{
		char msg[256];
		sprintf(msg, "Stack Expand - Start = %u, End = %u, Size = %u\n",
				expansion, i_arrayHeader->d_itemEnd,
				i_arrayHeader->d_itemEnd - expansion);
		conPuts(msg);

		sprintf(msg, "\tModified Item - Start = %u, End = %u, Size = %u\n",
				i_arrayHeader->d_itemStart, i_arrayHeader->d_itemEnd,
				i_arrayHeader->d_itemEnd - i_arrayHeader->d_itemStart);
		conPuts(msg);
	}
#endif

	return((void*)&d_stack[expansion]);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Contract()
 *	Access:      public
 *	Description: Retracts the end of the last item on the stack to give
 *              room back to the stack.
 *	Arguments:
 *    i_size        - The size in bytes of the requested contraction.
 *    i_arrayHeader - The item that contains the array.
 *	Return:      True for success. Fails if a valid key is not provided
 *              or if the collision item is not big enough to support
 *              the contraction.
 *
 **********************************************************************/
inline bool CCollisionStack::Contract(uint32 i_size,
		CCollisionStackItem* i_arrayHeader)
{
#ifdef PS2
	// TJC -  enforce VU alignment... (VU_ALIGN)
	if(i_size & (VU_ALIGN-1)) 
		i_size -= (i_size & (VU_ALIGN-1));
	ASSERT(IS_VU_ALIGNED(i_size));
	ASSERT(IS_VU_ALIGNED(d_end));
#endif

/*
 *	Make sure stack has been initialized and that the header is in the
 *	valid stack range.
 */
	ASSERT((d_stack != NULL) && ((void*)i_arrayHeader >= (void*)d_stack) &&
			((void*)i_arrayHeader < (void*)(d_stack + d_end)));
/*
 *	See if there is enough room left in the item.
 */
	if ((void*)&d_stack[i_arrayHeader->d_itemEnd - i_size -
			sizeof(CCollisionStackItem)] < (void*)i_arrayHeader)
	{
		ASSERT(false);
		return(false);
	}
/*
 *	If the item is at the end of the stack, move the end of the stack the
 *	requested amount; otherwise, just move the end of the item.
 */
	if (i_arrayHeader->d_itemEnd == d_end)
	{
		d_end -= i_size;
		i_arrayHeader->d_nextItem = d_end;
		i_arrayHeader->d_itemEnd = d_end;

#ifdef DEBUG_STACK
		if (d_dumpChanges)
		{
			char msg[256];
			sprintf(msg, "Stack Contract From End - "
					"Start = %u, End = %u, Size = %u\n", i_arrayHeader->d_itemEnd,
					i_arrayHeader->d_itemEnd + i_size, i_size);
			conPuts(msg);

			sprintf(msg, "\tModified Item - Start = %u, End = %u, Size = %u\n",
					i_arrayHeader->d_itemStart, i_arrayHeader->d_itemEnd,
					i_arrayHeader->d_itemEnd - i_arrayHeader->d_itemStart);
			conPuts(msg);
		}
#endif
	}
	else
	{
		i_arrayHeader->d_itemEnd -= i_size;

#ifdef DEBUG_STACK
		uint32 holeSize = i_arrayHeader->d_nextItem - i_arrayHeader->d_itemEnd;
		ASSERT(d_dumpChanges ||
				(DEBUG_ALLOWED_HOLE < 0) || (holeSize <= DEBUG_ALLOWED_HOLE));

		if (d_dumpChanges)
		{
			char msg[256];
			sprintf(msg, "Stack Contract From Middle - "
					"Start = %u, End = %u, Size = %u\n", i_arrayHeader->d_itemEnd,
					i_arrayHeader->d_itemEnd + i_size, i_size);
			conPuts(msg);

			sprintf(msg, "\tModified Item - Start = %u, End = %u, Size = %u\n",
					i_arrayHeader->d_itemStart, i_arrayHeader->d_itemEnd,
					i_arrayHeader->d_itemEnd - i_arrayHeader->d_itemStart);
			conPuts(msg);

			if ((i_arrayHeader->d_itemEnd + i_size) == i_arrayHeader->d_nextItem)
				conPuts("\tStack Hole Created");
			else
				conPuts("\tStack Hole Enlarged");
			sprintf(msg, " - Size = %u, Start = %u, End = %u\n",
					holeSize, i_arrayHeader->d_itemEnd, i_arrayHeader->d_nextItem);
			conPuts(msg);
		}
#endif
	}

#ifdef PS2
	// TJC - enforce vu alignment
	ASSERT(IS_VU_ALIGNED(d_end));
#endif

	return(true);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    LockBottom()
 *	Access:      public
 *	Description: Use of the bottom of the stack is fairly primitive
 *              and is meant to be used as very temporary storage.
 *              The bottom lock mechanism is an attempt to detect use
 *              of the bottom of the stack from multiple places.
 *	Arguments:   None
 * Return:      The key value passed to other routines that manipulate
 *              the bottom. If it matches the current key, access is
 *              authorized. If the bottom is currently locked, this
 *              lock attempt fails and zero is returned.
 *
 **********************************************************************/
inline uint32 CCollisionStack::LockBottom(void)
{
	if ((d_bottomKey & BOTTOM_LOCKED) != 0)
	{
		ASSERT(false);
		return(0);
	}

	d_bottomKey |= BOTTOM_LOCKED;
	return(d_bottomKey);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    UnlockBottom()
 *	Access:      public
 *	Description: Releases the lock on the bottom of the stack.
 *	Arguments:
 *    i_key - The bottom key. If it does not match the current key,
 *            no action is taken.
 * Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::UnlockBottom(uint32 i_key)
{
	if ((i_key == 0) || (i_key != d_bottomKey))
	{
		ASSERT(i_key == 0);
		return;
	}

	d_bottomKey &= ~BOTTOM_LOCKED;
	d_bottomKey++;
	if (d_bottomKey >= BOTTOM_LOCKED)
		d_bottomKey = 0;
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    IsBottomLocked()
 *	Access:      public
 *	Description: Checks to see if the bottom is locked.
 *	Arguments:   None
 * Return:      True if successful.
 *
 **********************************************************************/
inline bool CCollisionStack::IsBottomLocked(void)
{
	return((d_bottomKey & BOTTOM_LOCKED) != 0);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    IsBottomKey()
 *	Access:      public
 *	Description: Checks to see if the bottom is locked and the given key
 *              matches the current key.
 *	Arguments:
 *    i_key - The test key.
 * Return:      True if successful.
 *
 **********************************************************************/
inline bool CCollisionStack::IsBottomKey(uint32 i_key)
{
	return(((d_bottomKey & BOTTOM_LOCKED) != 0) && (i_key == d_bottomKey));
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    ResetBottom()
 *	Access:      public
 *	Description: Resets the bottom to the end of the stack space.
 *	Arguments:
 *    i_key - The bottom key. If it does not match the current key,
 *            no action is taken.
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStack::ResetBottom(uint32 i_key)
{
	if (i_key != d_bottomKey)
	{
		ASSERT(false);
		return;
	}

	d_bottom = d_size;
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    ExpandBottom()
 *	Access:      public
 *	Description: Reserves room at the bottom of the stack space.
 *	Arguments:   
 *    i_size - The size of the space to reserve.
 *    i_key -  The bottom key. If it does not match the current key,
 *             action fails.
 *	Return:      Pointer to the newly reserved space. NULL for failure.
 *
 **********************************************************************/
inline void* CCollisionStack::ExpandBottom(uint32 i_size, uint32 i_key)
{
	if (i_key != d_bottomKey)
	{
		ASSERT(false);
		return(NULL);
	}

	d_bottom -= i_size;
	if (d_bottom < d_end)
	{
		d_bottom += i_size;
		ASSERT(false);
		return(NULL);
	}
	return((void*)&d_stack[d_bottom]);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    ContractBottom()
 *	Access:      public
 *	Description: Releases reserved room at the bottom of the stack space.
 *	Arguments:   
 *    i_size - The size of the space to release.
 *    i_key -  The bottom key. If it does not match the current key,
 *             action fails.
 *	Return:      Pointer to the new bottom. NULL for failure.
 *
 **********************************************************************/
inline void* CCollisionStack::ContractBottom(uint32 i_size, uint32 i_key)
{
	if (i_key != d_bottomKey)
	{
		ASSERT(false);
		return(NULL);
	}

	d_bottom += i_size;
	if (d_bottom > d_size)
	{
		d_bottom -= i_size;
		ASSERT(false);
		return(NULL);
	}
	return((void*)&d_stack[d_bottom]);
}

/**********************************************************************
 *
 *	Class:       CCollisionStack
 *	Function:    Lock()
 *	Access:      public
 *	Description: Establishes a lock at the current end of the stack.
 *	Arguments:
 *    i_lock - The lock object.
 *	Return:      True for success; false otherwise.
 *
 **********************************************************************/
inline bool CCollisionStack::Lock(CCollisionStackLock* i_lock)
{
	ASSERT(i_lock != NULL);

	uint32 lastItem = d_end;
	CCollisionStackItem* item = Allocate(sizeof(CCollisionStackItem));
	if (item == NULL)
		return(false);

	new(item) CCollisionStackItem(*this);
	i_lock->d_item = item;
	i_lock->d_prev = d_currentLock;
	i_lock->d_next = NULL;
	i_lock->d_end = d_end;
	i_lock->d_lastItem = lastItem;

	d_currentLock->d_next = i_lock;
	d_currentLock = i_lock;

	return(true);
}

/**********************************************************************
 *
 *	Class:       CCollisionLock
 *	Function:    Set()
 *	Access:      public
 *	Description: Establish a lock on a stack.
 *	Arguments:
 *    i_stack - The stack to lock.
 *	Return:      True if successful; fals otherwise.
 *
 **********************************************************************/
inline bool CCollisionStackLock::Set(CCollisionStack &i_stack)
{
	Release();
	return(i_stack.Lock(this));
}

/**********************************************************************
 *
 *	Class:       CCollisionStackItem
 *	Function:    Delete()
 *	Access:      public
 *	Description: Delete the item.
 *	Arguments:   None
 *	Return:      None
 *
 **********************************************************************/
inline void CCollisionStackItem::Delete(CCollisionStackItem* i_item)
{
	if (i_item != NULL)
	{
		if (i_item->d_stack != NULL)
		{
			i_item->~CCollisionStackItem();
			i_item->d_stack->Deallocate(i_item);
		}
		else
			delete i_item;
	}
}

/**********************************************************************
 **********************************************************************/
inline uint32 CCollisionStack::TopSize(void)
{
	return(d_end);
}

/**********************************************************************
 **********************************************************************/
inline uint32 CCollisionStack::BottomSize(void)
{
	return(d_size - d_bottom);
}

/**********************************************************************
 **********************************************************************/
inline uint32 CCollisionStack::UsedSize(void)
{
	return(d_size - (d_bottom - d_end));
}

/**********************************************************************
 **********************************************************************/
inline uint32 CCollisionStack::UnusedSize(void)
{
	return(d_bottom - d_end);
}

/**********************************************************************
 **********************************************************************/
inline uint32 CCollisionStack::NumLocks(void)
{
	uint32 count = 0;
	CCollisionStackLock* lock = d_baseLock.d_next;
	while (lock != NULL)
	{
		count++;
		lock = lock->d_next;
	}
	return(count);
}

/**********************************************************************
 **********************************************************************/
inline bool CCollisionStack::IsEmpty(void)
{
	return((d_currentLock->d_end == 0) && ((d_bottomKey & BOTTOM_LOCKED) == 0));
}

#ifdef DEBUG_STACK

/**********************************************************************
 **********************************************************************/
inline void CCollisionStack::SetName(const char* i_name)
{
	strncpy(d_currentName, i_name, STACK_NAME_LENGTH - 1);
	d_currentReset = false;
}

/**********************************************************************
 **********************************************************************/
inline void CCollisionStack::CheckPeak(void)
{
	if (d_end > d_peakSize)
	{
		d_peakSize = d_end;
		strcpy(d_peakName, d_currentName);
		d_peakReset = d_currentReset;
	}
}

/**********************************************************************
 **********************************************************************/
inline void CCollisionStackLock::SetName(const char* i_name)
{
	strncpy(d_name, i_name, STACK_NAME_LENGTH - 1);
}

#endif

/**********************************************************************/
/**********************************************************************/
inline CCollisionStackItem::CCollisionStackItem() :
	d_stack(NULL)
{
#ifdef DEBUG_STACK
	d_time = g_timer.GetOSTime();
	d_frame = g_timer.GetFrameCount();
#endif
}

/**********************************************************************/
/**********************************************************************/
inline CCollisionStackItem::CCollisionStackItem(CCollisionStack &i_stack) :
	d_stack(&i_stack)
{
#ifdef DEBUG_STACK
	d_time = g_timer.GetOSTime();
	d_frame = g_timer.GetFrameCount();
#endif
}

#endif // STACK_HPP
