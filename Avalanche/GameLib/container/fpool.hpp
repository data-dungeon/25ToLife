/***************************************************************************/
// Implementation for fixed sized pool
/***************************************************************************/
#include <new.h>
#ifdef _DEBUG
#include <stdio.h>
#include "Layers/Debug.h"
#endif
#include "platform/BaseType.h"

/***************************************************************************/
// Use 0 if you don't want the list to automatically grow,
// shrink is not supported and the heap will not be released
// until the program exits (at least not in this implementation)
/***************************************************************************/
template <class T, uint size>
inline FixedPool<T, size>::FixedPool(
#ifdef _DEBUG
char *name,
#endif
bool callTheDestructors)
{
#ifdef _DEBUG
	// Save the name
	strncpy(this->name, name, sizeof(this->name) - 1);
	this->name[sizeof(this->name) - 1] = '\0';
#endif

	// Save this for delete's
	callDestructors = callTheDestructors;

	// Make the free chain
	MakeFreeChain();

#ifdef _DEBUG
	count = 0;
	maxCount = 0;
#endif
}

/***************************************************************************/
// Free up all of our blockLists
/***************************************************************************/
template <class T, uint size>
inline FixedPool<T, size>::~FixedPool(void)
{
	Clear();
#ifdef _DEBUG
	char temp[256];
	sprintf(temp, "Fixed pool %s reached a maximum of %d items.\n", name, maxCount);
	conPuts(temp);
#endif
}

/***************************************************************************/
// Get a free item
/***************************************************************************/
template <class T, uint size>
inline T *FixedPool<T, size>::New(void)
{
	// Get sapce
	void *space = NewPtr();
	if (!space)
		return(NULL);

	// Call the constructor
	T *newItem = new (space) T;

	// Return the pointer to they guy
	return(newItem);
}

/***************************************************************************/
// Get a ptr to an area for the free item (call new (ptr) Class(parms);)
// Use this when you need to call a constructor that has parameters
/***************************************************************************/
template <class T, uint size>
inline void *FixedPool<T, size>::NewPtr(void)
{
	// Is there space?
	if (!freeHead)
	{
#ifdef _DEBUG
	char temp[256];
	sprintf(temp, "Fixed pool %s filled up with %d items.\n", name, maxCount);
	conPuts(temp);
#endif
		ASSERT(freeHead);
		return(NULL);
	}

	// Get the item off the top of the free list
	FixedPoolItem *space = freeHead;
	freeHead = space->next;

#ifdef _DEBUG
	if (++count > maxCount)
		maxCount = count;
#endif

	// Return a pointer to the object data
	return(space);
}

/***************************************************************************/
// Let go of an item
/***************************************************************************/
template <class T, uint size>
inline void FixedPool<T, size>::Delete(T *item)
{
	// Make sure this is okay
	if (item == NULL)
		return;

	// Call the detructor
	if (callDestructors)
		item->~T();

	// Get a pointer to the item header
	FixedPoolItem *kill = (FixedPoolItem *)item;

#ifdef _DEBUG
	FillItem((T *)kill);
#endif

	// Put it back in the free list
	kill->next = freeHead;
	freeHead = kill;

#ifdef _DEBUG
	--count;
	ASSERT(count >= 0);
#endif
}

/***************************************************************************/
// Clear the allocated objects (will NOT call destructors)
/***************************************************************************/
template <class T, uint size>
inline void FixedPool<T, size>::Clear(void)
{
	// Call the destructors?
	if (!callDestructors)
		MakeFreeChain();
	else
	{
		// This is hella complicated

		// What is on the free chain?
		bool onFreeChain[size];
		uint i;
		for (i = 0; i < size; i++)
			onFreeChain[i] = false;

		// Traverse the free chain, marking the list
		FixedPoolItem *item = freeHead;
		while (item)
		{
			uint index = ((uint)item - (uint)block) / sizeof(T);
			onFreeChain[index] = true;
			item = item->next;
		}

		// Re-traverse the array, freeing the non-freechain'd items
		uint itemIndex = 0;
		for (i = 0; i < size; i++, itemIndex += sizeof(T))
		{
			if (!onFreeChain[i])
			{
				// Call the destructor
//yuch-- vc was acting as though block was an array of ints, not unsigned chars, so we were getting every 4th item...
//				T *killMe = (T *)&block[itemIndex];
				T *killMe = (T *) ((int) block + itemIndex);
				killMe->~T();

				// Get a pointer to the item header
				FixedPoolItem *item = (FixedPoolItem *)killMe;

#ifdef _DEBUG
				FillItem((T *)item);
#endif

				// Put it back in the free list
				item->next = freeHead;
				freeHead = item;

#ifdef _DEBUG
				--count;
				ASSERT(count >= 0);
#endif
			}
		}
	}
}

/***************************************************************************/
// Is the pool full (won't assert)
/***************************************************************************/
template <class T, uint size>
inline bool FixedPool<T, size>::Full(void)
{
	return(freeHead == NULL);
}

#ifdef _DEBUG
/***************************************************************************/
// Fill a hunk of memory
/***************************************************************************/
template <class T, uint size>
inline void FixedPool<T, size>::MemFill(
void *ptr,
unsigned long data,
uint bytes)
{
	// Block it out with the word value
	uint words = bytes / sizeof(u32);
	u32 *word = (u32 *)ptr;
	uint i;
	for (i = 0; i < words; i++)
		word[i] = data;

	// What was the residual
	uint leftover = bytes - (words * sizeof(u32));
	unsigned char *cptr = (unsigned char *)ptr;
	unsigned char *dptr = (unsigned char *)&data;
	for (i = 0; i < leftover; i++)
		cptr[i] = dptr[i];
}
#endif

/***************************************************************************/
// Create the block list
/***************************************************************************/
template <class T, uint size>
inline void FixedPool<T, size>::MakeFreeChain(void)
{
	// Get the first item
	FixedPoolItem *firstItem = (FixedPoolItem *)&block[0];

	// The the freehead to the item
	freeHead = firstItem;

	// Enumerate the items
	FixedPoolItem *item = firstItem;
	for (uint i = 0; i < size - 1; i++)
	{
#ifdef _DEBUG
		FillItem((T *)item);
#endif
		// Setup the next pointer
		item->next = (FixedPoolItem *)((unsigned char *)item + sizeof(T));
		item = item->next;
	}

	// make sure the lastitem next is null
	item->next = NULL;
}

