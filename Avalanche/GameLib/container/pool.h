/***************************************************************************/
// A template to manage a pool of objects.
// Designed to be very fast.
/***************************************************************************/
#ifndef POOL_H
#define POOL_H

#include "Platform/BaseType.h"
#include "container/darray.h"

// An object pool template
template <class T> class Pool
{
public:
	// Use 0 in starting size if you want all allocation deferred until
	// the fist call to New/NewPtr
	// Use 0 in automaticGrowSize if you don't want the list to automatically
	// grow.
	// Use 0 in allocationSize to only allocate enough room for the object, otherwise,
	// make sure allocationSize is *larger* than the object size - allows allocating extra space per object.
	inline Pool(unsigned int startingSize = 16, unsigned int automaticGrowSize = 16, unsigned int allocationSize = 0 );
	inline ~Pool(void);

	// Get a free item
	inline T *New(void);

	// Get a ptr to an area for the free item (call new (ptr) Class(parms);)
	// Use this when you need to call a constructor that has parameters
	inline void *NewPtr(void);

	// Let go of an item
	inline void Delete(T *item);

	// lock this array after allocation-- for memory leak purposes, permanent arrays
	// must be allocated large enough that they never need to grow
	inline void Lock(void)
		{ locked = true; }

	// Grow the list
	inline bool Grow(unsigned int growBy);

	// Try the best you can to shrink
	inline bool Shrink(void);

	// Clear the allocated objects (will NOT call destructors)
	inline void Clear(void);

private:
	// Forward reference
	typedef struct tagPoolItem
	{
		struct tagPoolItem *next;
	} PoolItem;

	PoolItem					*freeHead;			// Free list pointer
	unsigned int			clearSize;			// How many items allocate after clearing
	unsigned int			autoGrowSize;		// How many items to grow by
	unsigned int			allocSize;			// Allow for allocating extra space per object
	DynamicArray<void *>	*blockList;			// Data block pointer list
	bool						locked;				// for memory leak purposes, some pools are allocated big enough to never need to grow
	unsigned int			usedItems;			// How many used items do we have

	// Create/destroy the block list
	inline void CreateBlockList(unsigned int startSize);
	inline void DestroyBlockList(void);
};

// Include the implementation file
#include "container/pool.hpp"

#endif
