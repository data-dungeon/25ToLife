/***************************************************************************/
// A template to manage a fixed size pool of objects.
// Designed to be very fast.
/***************************************************************************/
#ifndef FPOOL_H
#define FPOOL_H

#include "platform/BaseType.h"

// An object pool template
template <class T, uint size> class FixedPool
{
public:
	// Use 0 if you don't want the list to automatically grow, shrink is not
	// yet supported and the heap will not be released until destroyed.
#ifndef _DEBUG
	inline FixedPool(bool callTheDestructors = true);
#else
	inline FixedPool(char *name, bool callTheDestructors = true);
#endif
	inline ~FixedPool(void);

	// Get a free item
	inline T *New(void);

	// Get a ptr to an area for the free item (call new (ptr) Class(parms);)
	// Use this when you need to call a constructor that has parameters
	inline void *NewPtr(void);

	// Let go of an item
	inline void Delete(T *item);

	// Clear the allocated objects (calls the destructors if you set it above)
	inline void Clear(void);

	// Is the pool full (won't assert)
	inline bool Full(void);

private:
	// Forward referenece
	struct FixedPoolItem
	{
		FixedPoolItem *next;
	};

#ifdef _DEBUG
	char						name[32];
#endif

	bool						callDestructors;
	FixedPoolItem	  		*freeHead;			// Free list pointer
	unsigned char 			*block[size * sizeof(T)]; // The data

#ifdef _DEBUG
	int						count, maxCount;

	// Memfill for debugging
	inline void MemFill(void *ptr, unsigned long data, uint bytes);
	inline void FillItem(T *item)
		{ MemFill(item, 0xf00df00d, sizeof(T)); }
#endif

	// Make the free chain
	inline void MakeFreeChain(void);
};

// Include the implementation file
#include "container/fpool.hpp"

#endif
