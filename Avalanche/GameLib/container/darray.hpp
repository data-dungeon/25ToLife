/***************************************************************************/
// A dynamic array template
//
// Do not use this for objects unless you are explicitly going to
// manage the constructor/destructors. if you want something that
// will automatically call destructors, use DynList
/***************************************************************************/
#include <string.h>
#include <stdlib.h>

#if !defined(NOLAYERS)
#include "Layers/Memory.h"
#endif

/***************************************************************************/
// Create the array
// Space for 'initialItems' will be allocated, and every time the array
// needs to grow it will grow by 'growBy' items.  If zeroFill is set,
// new array items will be zeroed out. it is legal for initialItems to be 0
/***************************************************************************/
template <class T>
inline DynamicArray<T>::DynamicArray(
int initialItems,
int growBy,
bool zeroFill)
{
	// Allocate the inital array
	if (initialItems != 0)
		list = (T *)new char[initialItems * sizeof(T)];
	else
		list = NULL;

	// Fill out the structure
	items = 0;
	total = initialItems;
	grow = growBy;
	zero = zeroFill;
	locked = false;

	// Setup the items
	if (zero && list)
		memset(list, 0, initialItems * sizeof(T));
}

/***************************************************************************/
// Destroy the array (hDyn is then invalid)
/***************************************************************************/
template <class T>
inline DynamicArray<T>::~DynamicArray()
{
	if (list)
#if defined( PS2) || defined(GCN)
		delete list;	// hack! it seems that in spots the compiler is assuming that new [] is not overridden, and that default new [] leaves some header space in front of buffer, so it subtracts 16 from buffer address!
#else //PS2
		delete [] list;
#endif //PS2
}

/***************************************************************************/
// Clear all the items in the array
/***************************************************************************/
template <class T>
inline void DynamicArray<T>::Clear()
{
	// Clear the items
	if (zero && list)
		memset(list, 0, total * sizeof(T));

	// No more items
	items = 0;
}

/***************************************************************************/
// free the array's memory, for memory leak control
/***************************************************************************/
template <class T>
inline void DynamicArray<T>::Reset()
{
	// a list that gets reset does not have to be locked, since the program is called reset to release memory-- it's
	// probably not permanent memory
	ASSERT(locked == false);

	// free memory
	if (list)
#if defined( PS2) || defined(GCN)
		delete list;	// hack! it seems that in spots the compiler is assuming that new [] is not overridden, and that default new [] leaves some header space in front of buffer, so it subtracts 16 from buffer address!
#else //PS2
		delete [] list;
#endif //PS2

	// reset
	items = 0;
	total = 0;
	list = NULL;
}

/***************************************************************************/
// Add an item to the array (-1 or > size adds to the end)
// Returns true or false based on success or failure
/***************************************************************************/
template <class T>
inline bool DynamicArray<T>::Add(
const T &newItem,
int pos)
{
   // Setup pos
   if (pos < 0 || pos > items)
      pos = items;

   // Do we have to make the array bigger?
   if (items == total)
   {
      // How big will it be?
      int oldSize = total * sizeof(T);
      int newSize = (total + grow) * sizeof(T);

      // We have to make more space
		ASSERT(!locked);	// if this hits, then original allocation should have been bigger...can continue past this ASSERT
		T *tempList = (T *)new char[newSize];
		if (!tempList)
			return false;
		if (list)	// old list may be 0-length
			memcpy(tempList, list, oldSize);

		// swap list ptr (before we free, better thread safety)
		T *oldList = list;
		list = tempList;

		// delete old list
		if (oldList)
#if defined(PS2) || defined(GCN)
			delete oldList;	// hack! it seems that in spots the compiler is assuming that new [] is not overridden, and that default new [] leaves some header space in front of buffer, so it subtracts 16 from buffer address!
#else //PS2
			delete [] oldList;
#endif //PS2

      // Setup the totals correctly
      total += grow;

      // Zero fill?
      if (zero)
         memset((char *)list + oldSize, 0, newSize - oldSize);
	}

   // Make space to insert the item
   char *item = (char *)list + pos * sizeof(T);
	int toMove = items - pos;
   if (toMove > 0)
      memmove(item + sizeof(T), item, toMove * sizeof(T));

   // Insert the item
   memcpy(item, &newItem, sizeof(T));
   items++;

   return true;
}

/***************************************************************************/
// Expand the array by 1 (at the end) and return the object
/***************************************************************************/
template <class T>
inline T *DynamicArray<T>::Expand()
{
   // Do we have to make the array bigger?
   if (items == total)
   {
      // How big will it be?
      int oldSize = total * sizeof(T);
      int newSize = (total + grow) * sizeof(T);

      // We have to make more space
		ASSERT(!locked);	// if this hits, then original allocation should have been bigger...can continue past this ASSERT
		T *tempList = (T *)new char[newSize];
		if (!tempList)
			return NULL;
		if (list)	// old list may be 0-length
			memcpy(tempList, list, oldSize);

		// swap list ptr (before we free, better thread safety)
		T *oldList = list;
		list = tempList;

		// delete old list
		if (oldList)
#if defined(PS2) || defined(GCN)
			delete oldList;	// hack! it seems that in spots the compiler is assuming that new [] is not overridden, and that default new [] leaves some header space in front of buffer, so it subtracts 16 from buffer address!
#else //PS2
			delete [] oldList;
#endif //PS2

      // Setup the totals correctly
      total += grow;

      // Zero fill?
      if (zero)
         memset((char *)list + oldSize, 0, newSize - oldSize);
	}

	// The end!
   char *item = (char *)list + items * sizeof(T);
   items++;
	return (T *)item;
}

/***************************************************************************/
// Delete the item from the array
// Returns true or false based on success or failure
/***************************************************************************/
template <class T>
inline bool DynamicArray<T>::Delete(
int pos)
{
	/*** NOTE: this routine does NOT free the memory pointed to by the list entry. ***/
	/*** also, to clean up a dynamic array, you will want to loop through it       ***/
	/*** BACKWARDS,since this routine compacts the list and decrements the count   ***/
	// Setup pos
	if (pos < 0)
		pos = items - 1;
	if (pos < 0 || pos >= items)
		return false;

   // Remove the item
   char *item = (char *)list + pos * sizeof(T);
	int toMove = (items - 1 - pos);
   if (toMove > 0)
      memmove(item, item + sizeof(T), toMove * sizeof(T));

   // One less item in the list
   --items;

   // Do we have to zero fill the end?
   if (zero)
      memset(&list[items], 0, sizeof(T));

   return true;
}

/***************************************************************************/
// Replace the item at 'pos' with the new item
/***************************************************************************/
template <class T>
inline bool DynamicArray<T>::Replace(
int pos,
T &newItem)
{
   // Setup pos
	if (pos < 0)
		pos = items - 1;
	if (pos < 0 || pos >= items)
		return false;

   // Do the copy
   memcpy(&list[pos], &newItem, sizeof(T));

   return true;
}

/***************************************************************************/
// Get an item from the array
// Returns NULL on failure
/***************************************************************************/
template <class T>
inline T *DynamicArray<T>::Get(
int pos) const
{
   // Setup pos
	if (pos < 0)
		pos = items - 1;
	if (pos < 0 || pos >= items)
      return NULL;

   // Give them back the item ptr
   return &list[pos];
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T *DynamicArray<T>::GetPtr() const
{
   return list;
}

/***************************************************************************/
// Another type of get
/***************************************************************************/
template <class T>
inline T &DynamicArray<T>::operator[](int pos) const
{
   // Give them back the item ptr
	ASSERT(pos >= 0 && pos < items);
   return list[pos];
}


