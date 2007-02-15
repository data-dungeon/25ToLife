/*
**
**  File:   Stack.cpp
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   A simple stack for transient objects used in collision testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/Stack.cpp $
**  $Revision: 4 $
**      $Date: 7/25/03 11:42a $
**    $Author: Bcollard $
**
*/

#include "collide/Stack.h"

DeclareSingleton(CCollisionStack);

#ifdef DEBUG_STACK

#include <string.h>
#include "Game/Managers/SpriteMgr.h"

/**********************************************************************/
/**********************************************************************/
void CCollisionStack::ValidateStack(void)
{
	uint32 previousIndex = (uint32)~0;
	uint32 currentIndex = d_firstItem;
	while(currentIndex < d_end)
	{
		CCollisionStackItem* currentItem =
				(CCollisionStackItem*)(&d_stack[currentIndex]);
		ASSERT(currentItem->d_previousItem == previousIndex);
		ASSERT(currentItem->d_nextItem >= currentItem->d_itemEnd);
		previousIndex = currentIndex;
		currentIndex = currentItem->d_nextItem;
	}
	ASSERT(d_end == currentIndex);
	ASSERT(d_lastItem == previousIndex);
	int foo = 0;
	if (foo != 0)
		DumpStack();
}

/**********************************************************************/
/**********************************************************************/
void CCollisionStack::DumpStack(void)
{
/*
 *	Stack stats.
 */
	char msg[256];
	sprintf(msg, "\nStack Size = %u (Top = %u, Bottom = %u)\n", d_size, d_bottom,
			d_size - d_bottom);
	conPuts(msg);
/*
 *	Peak size.
 */
	if (d_peakReset)
		sprintf(msg, "Stack Peak Size = %u (%s - reset)\n\n", d_peakSize,
				d_peakName);
	else
		sprintf(msg, "Stack Peak Size = %u (%s)\n\n", d_peakSize, d_peakName);
	conPuts(msg);
/*
 *	Display title.
 */
	sprintf(msg, " Time    Frame   Size   Description\n");
	conPuts(msg);
	sprintf(msg, "------  ------  ------  -----------\n");
	conPuts(msg);
/*
 *	Display base lock.
 */
	sprintf(msg, "                        Lock 0 - Base Lock\n");
	conPuts(msg);
/*
 *	Display any leading free space.
 */
	if (d_firstItem > 0)
	{
		sprintf(msg, "                %6u  Free space\n", d_firstItem);
		conPuts(msg);
	}
/*
 *	Loop through each entry on the stack from the beginning.
 */
	uint32 previousIndex = (uint32)~0;
	uint32 currentIndex = d_firstItem;
	while(currentIndex < d_end)
	{
/*
 *		Find the next lock.
 */
		uint32 nextLockItem = d_end;
		uint lockIndex = 0;
		CCollisionStackLock* lock = d_baseLock.d_next;
		CCollisionStackLock* nextLock = NULL;
		while (lock != NULL)
		{
			lockIndex++;
			if ((lock->d_lastItem >= currentIndex) &&
					(lock->d_lastItem < nextLockItem))
			{
				nextLockItem = lock->d_lastItem;
				nextLock = lock;
			}
			lock = lock->d_next;
		}
/*
 *		Loop through each entry until the next lock entry is found.
 */
		while(currentIndex < nextLockItem)
		{
			CCollisionStackItem* currentItem =
					(CCollisionStackItem*)(&d_stack[currentIndex]);
			sprintf(msg, "%6u  %6u  %6u  %s", currentItem->d_time,
					currentItem->d_frame, currentItem->d_itemEnd - currentIndex,
					currentItem->GetName());
			currentItem->AppendDescription(msg);
			strcat(msg, "\n");
			conPuts(msg);
			ASSERT(currentItem->d_previousItem == previousIndex);
			ASSERT(currentItem->d_nextItem >= currentItem->d_itemEnd);
			if (currentItem->d_nextItem > currentItem->d_itemEnd)
			{
				sprintf(msg, "                %6u  Free space\n",
						currentItem->d_nextItem - currentItem->d_itemEnd);
				conPuts(msg);
			}
			previousIndex = currentIndex;
			currentIndex = currentItem->d_nextItem;
			ASSERT(currentIndex <= nextLockItem);
		}
/*
 *		Display info about the next lock entry.
 */
		if (nextLockItem < d_end)
		{
			CCollisionStackItem* currentItem =
					(CCollisionStackItem*)(&d_stack[currentIndex]);
			sprintf(msg, "%6u  %6u  %6u  Lock %u - %s\n", currentItem->d_time,
					currentItem->d_frame, currentItem->d_itemEnd - currentIndex,
					lockIndex, nextLock->Name());
			conPuts(msg);
			ASSERT(currentItem->d_previousItem == previousIndex);
			ASSERT(currentItem->d_nextItem >= currentItem->d_itemEnd);
			if (currentItem->d_nextItem > currentItem->d_itemEnd)
			{
				sprintf(msg, "                %6u  Free space\n",
						currentItem->d_nextItem - currentItem->d_itemEnd);
				conPuts(msg);
			}
			previousIndex = currentIndex;
			currentIndex = currentItem->d_nextItem;
		}
	}
	ASSERT(d_end == currentIndex);
	ASSERT(d_lastItem == previousIndex);
}

/**********************************************************************/
/**********************************************************************/
void CCollisionStack::PlotStack(void)
{
	float size = (float)d_size;

	// Ref
	SpriteMgr__SetPlotBar(0, 1.0f);

	// Bottom
	SpriteMgr__SetPlotBar(1, (float)(d_size - d_bottom) / size);

	// Peak
	SpriteMgr__SetPlotBar(2, (float)d_peakSize / size);

	// Reset peak
	d_peakSize = 0;
}

/**********************************************************************/
/**********************************************************************/
const char* CCollisionStackItem::GetName(void)
{
	return("BaseItem");
}

/**********************************************************************/
/**********************************************************************/
void CCollisionStackItem::AppendDescription(char* io_msg)
{
}

#endif
