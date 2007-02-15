/***************************************************************************/
// Implementation for the pool
/***************************************************************************/
#include <new.h>
#define NOAVALANCHE
#ifndef NOAVALANCHE
#include "Layers/Memory.h"
#endif

/***************************************************************************/
// Use 0 if you don't want the list to automatically grow,
// shrink is not supported and the heap will not be released
// until the program exits (at least not in this implementation)
/***************************************************************************/
template <class T>
inline Pool<T>::Pool(
unsigned int startingSize,
unsigned int automaticGrowSize,
unsigned int allocationSize)
{
	// How many items allocate after clearing
	clearSize = startingSize;

	// How many items to we grow by
	autoGrowSize = automaticGrowSize;

	// no free chain yet
	freeHead = NULL;

	// not locked-- permanent pools must be locked by hand
	locked = false;
	usedItems = 0;

	if ( allocationSize == 0 )
	{
		allocSize = sizeof( T );
	}
	else
	{
		ASSERT( allocationSize >= sizeof( T ) );
		allocSize = allocationSize;
		/* ? also make sure the allocSize is %4==0? */
	}

	// Create the list of data block pointers
	if (startingSize == 0)
		blockList = NULL;
	else
		CreateBlockList(startingSize);
}

/***************************************************************************/
// Free up all of our blockLists
/***************************************************************************/
template <class T>
inline Pool<T>::~Pool(void)
{
	DestroyBlockList();
}

/***************************************************************************/
// Get a free item
/***************************************************************************/
template <class T>
inline T *Pool<T>::New(void)
{
	// Get sapce
	void *space = NewPtr();
	if (!space)
		return(NULL);

	// Call the constructor
	MEM_SET_ONE_SHOT_NEW_NAME("Pool::New");
	T *newItem = new (space) T;
	++usedItems;

	// Return the pointer to they guy
	return(newItem);
}

/***************************************************************************/
// Get a ptr to an area for the free item (call new (ptr) Class(parms);)
// Use this when you need to call a constructor that has parameters
/***************************************************************************/
template <class T>
inline void *Pool<T>::NewPtr(void)
{
	// Have we created the list yet?
	if (blockList == NULL)
		CreateBlockList(autoGrowSize);

	// Make sure there is space
	if (freeHead == NULL && (autoGrowSize == 0 || !Grow(autoGrowSize)))
		return(NULL);

	// Get the item off the top of the free list
	PoolItem *space = freeHead;
	freeHead = space->next;
	++usedItems;

	// Return a pointer to the object data
	return(space);
}

/***************************************************************************/
// Let go of an item
/***************************************************************************/
template <class T>
inline void Pool<T>::Delete(T *item)
{
	// Make sure this is okay
	if (item == NULL)
		return;

	// Call the destructor
	item->~T();
	ASSERT(usedItems > 0);
	--usedItems;

	// Get a pointer to the item header
	PoolItem *kill = (PoolItem *)item;

	// Put it back in the free list
	kill->next = freeHead;
	freeHead = kill;
}

/***************************************************************************/
// Grow the list
/***************************************************************************/
template <class T>
inline bool Pool<T>::Grow(
unsigned int growBy)
{
	// This would be bad
	ASSERT(blockList != NULL);
	ASSERT(locked == false);	// if this hits, then original allocation should have been bigger...can continue past this ASSERT

	// Create more space
	unsigned int size = /*sizeof(T)*/ allocSize * growBy;
#ifdef NOAVALANCHE
	MEM_SET_ONE_SHOT_NEW_NAME("Pool::Grow");
	void *temp = (void *)new unsigned char[size];
#else
	MEM_SET_ALLOC_NAME("Pool::Grow");
	void *temp = (void *)memAlloc(size);
#endif

	// Did it work?
	if (temp == NULL || !blockList->Add(temp))
		return(false);

	// Set it up
	PoolItem *item;
	PoolItem *firstItem = NULL;
	PoolItem *lastItem = NULL;
	unsigned int i;
	for (i = 0, item = (PoolItem *)temp; i < growBy; i++)
		{
		// Last/first item?
		if (i == 0)
			firstItem = item;
		if (i == growBy - 1)
			lastItem = item;

		// Setup next and advance
		item->next = (PoolItem *)((char *)item + allocSize/*sizeof(T)*/);
		item = item->next;
		}

	// Insert it into the free chain
	lastItem->next = freeHead;
	freeHead = firstItem;

	// Stuff worked
	return(true);
}

/***************************************************************************/
// Grow the list
/***************************************************************************/
template <class T>
inline bool Pool<T>::Shrink(void)
{
	// This shit is so weak
	if (usedItems != 0 || locked)
		return false;

	DestroyBlockList();
	return true;
}

/***************************************************************************/
// Clear the allocated objects (will NOT call destructors)
/***************************************************************************/
template <class T>
inline void Pool<T>::Clear(void)
{
	// Get rid of everything
	DestroyBlockList();

	// Create the list of data block pointers
	if (clearSize == 0)
		blockList = NULL;
	else
		CreateBlockList(clearSize);
}

/***************************************************************************/
// Create the block list
/***************************************************************************/
template <class T>
inline void Pool<T>::CreateBlockList(
unsigned int startSize)
{
	// Create the list of pointers
	MEM_SET_ONE_SHOT_NEW_NAME("Pool blockList");
	if (autoGrowSize == 0)
		blockList = new DynamicArray<void *>(1, 0, false);
	else
		blockList = new DynamicArray<void *>(8, 4, false);
	ASSERT(blockList);

	// Setup the initial pool
	Grow(startSize);
}

/***************************************************************************/
// Destroy the block list
/***************************************************************************/
template <class T>
inline void Pool<T>::DestroyBlockList(void)
{
	// Were we ever created?
	if (blockList == NULL)
		return;

	// Free it up
	int count = blockList->Count();
	for (int i = 0; i < count; i++)
	{
		void *freeMe = (*blockList)[i];
#ifdef NOAVALANCHE
		delete[] (unsigned char*)freeMe;
#else
		memFree(freeMe);
#endif
	}

	// Now get rid of the list of blockLists
	delete blockList;
	blockList = NULL;

	// Make sure we have no freelist any more
	freeHead = NULL;
	usedItems = 0;
}
