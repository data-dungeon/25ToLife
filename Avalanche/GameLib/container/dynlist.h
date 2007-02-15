/***************************************************************************/
// Dynamic array template.
// You must implement operator= to use this array.
/***************************************************************************/
#ifndef DYNLIST_H
#define DYNLIST_H

// We are going to use a dyn array to keep track of a list of obj pointers
#include "container/Darray.h"
#include "platform/BaseType.h"

template <class T> class DynList
{
public:
   inline DynList(int startSize = 8, int growSize = 8, bool callTheDestructors = true);
   inline ~DynList();

	// delete all list items, and free list memory buffer
	inline void Reset();

	// Flag to set whether or not destructors are called
	inline void CallDestructors(int callThem)
		{ callDestructors = callThem; }

   // Add an item to the list
   // Note that you must use 'new' to create the object, it
   // is not copied and therefor can not be a local variable
   inline bool Add(T *newItem, int pos = -1);

   // How many items are in the list?
   inline int Count() const
		{ return list->Count(); }

	// what is the current available size of the list before it needs to expand?
   inline int Total() const
		{ return list->Total(); }
	
	// lock this array after allocation-- for memory leak purposes, permanent arrays must be allocated large enough
	// that they never need to grow
	inline void Lock() 
		{ list->Lock(); }

	// access to dynamic array address-- just for memory block naming
	inline void *ArrayAddress() const
		{ return(list); }

   // Get an item from the list
   inline T *Get(int pos) const;
   inline T *operator[](int pos) const;

   // Delete an item from the list
   inline bool Delete(int pos);

   // Swap two items in the list
   inline bool Swap(int pos1, int pos2);

   // Find an item in the list (-1 for not found)
   inline int Find(T const *lookFor) const;

   // Find an item in the list (-1 for not found)
   inline int Find(T const &lookFor) const;

	// Clear the list
	inline void Clear();

	// Sort the list
	inline void QSort(int (CDECL *compare)(const void *elem1, const void *elem2));

private:
   // Here's how we keep track of all the objects
   DynamicArray<T *>	*list;
	int					callDestructors;

	inline void CallDestructors();
};

// Include the implementation
#include "container/dynlist.hpp"

#endif
