/****************************************************************************/
// Simple hashed list
//
// For type T there must exist a function that looks like:
// uint HashKey(T &t);
// This is not expected to be a member function of T, it is just called
// directly.
/****************************************************************************/
#ifndef HASHLIST_H
#define HASHLIST_H

#include "container/pool.h"
#include "container/Darray.h"

template <class T, unsigned long powerOf2Size> class HashList
{
public:
	HashList(void);
	~HashList(void);

	// Add an item, returning it's index in the flat list
	inline unsigned long Add(T &t);

	// Find an item (return it's index)
	inline bool Find(T &t, unsigned long &index);

	// Load 0-100%.  The closer this is to 100, the
	// better your key function is
	inline unsigned long Load(void);

	// Get at the list
	inline T *Array(void)
		{ return(list.GetPtr()); }
	inline unsigned long Count(void)
		{ return(list.Count()); }
	inline T *Get(unsigned long index)
		{ return(list.Get(index)); }

private:
	// the ist
	DynamicArray<T>		list;

	// for the hash
	struct Element
	{
		unsigned long		index;
		Element				*next;
	};

	// the array of items we are keeping
	Element			*hashTable[1 << powerOf2Size];
	unsigned long	hashMask;

	// A fast way of allocating elements
	Pool<Element>			pool;
	inline Element *NewElement(void);
	inline void DeleteElement(Element *e);
};

// the implmentation
#include "container/hashlist.hpp"

#endif
