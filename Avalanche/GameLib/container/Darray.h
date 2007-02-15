/***************************************************************************/
// A dynamic array template
//
// Do not use this for objects unless you are explicitly going to
// manage the constructor/destructors. if you want something that
// will automatically call destructors, use DynList
/***************************************************************************/
#ifndef DARRAY_H
#define DARRAY_H

template <class T> class DynamicArray
{
public:
	// Create the array
	// Space for 'initialItems' will be allocated, and every time the array
	// needs to grow it will grow by 'growBy' items.  If zeroFill is set,
	// new array items will be zeroed out.
	inline DynamicArray(int initialItems = 8, int growBy = 8, bool zeroFill = false);

	// Destroy the array
	inline ~DynamicArray();

	// Clear all the items in the array  (does not shrink it down, or free list memory)
	inline void Clear();

	// ASSERT that array is empty, and free list memory buffer
	inline void Reset();

	// Return the number of items in the list
	inline int Count() const	
		{ return items; }

	// what is the current available size of the list before it needs to expand?
	inline int Total() const	
		{ return total; }

	// lock this array after allocation-- for memory leak purposes, permanent arrays must be allocated large enough
	// that they never need to grow
	inline void Lock()	
		{ locked = true; }
	inline bool IsLocked() const	
		{ return locked; }

	// Add an item to the array (pos of -1 or > size adds to the end)
	// Returns TRUE or FALSE based on success or failure
	inline bool Add(const T &newItem, int pos = -1);

	// Expand the array by 1 (at the end) and return the object
	// NULL means something went awry
	inline T *Expand();

	// Delete the item from the array (pos of -1 deletes from end)
	// Returns TRUE or FALSE based on success or failure
	// -1 for end
	inline bool Delete(int pos);

	// Replace the item at 'pos' with the new item
	// -1 for end
	inline bool Replace(int pos, T &newItem);

	// Get an item from the array
	// Returns NULL on failure 
	// -1 for end
	inline T *Get(int pos) const;

	// Get the pointer to the top of the array
	inline T *GetPtr() const;

	// Get the item from the array POS MUST BE VALID
	inline T &operator[](int pos) const;

private:
   T					*list;			// Pointer to the list
   int 				items;			// Current number of valid items in array
	int				total;			// total number of items in the array
   int 				grow;				// How many items to expand by when growing
   bool				zero;				// are we supposed to zero fill?
	bool				locked;			// for memory leak purposes, some pools are allocated big enough to never need to grow
};

// The all important implementation
#include "container/darray.hpp"

#endif
