/***************************************************************************/
// Dynamic array template.
/***************************************************************************/

/***************************************************************************/
// Create the list of initial size
/***************************************************************************/
template <class T>
inline DynList<T>::DynList(
int startSize,
int growSize,
bool callTheDestructors)
{
   // Create the dyn list
   list = new DynamicArray<T *>(startSize, growSize, false);

	// Should we call the destructors?
	callDestructors = callTheDestructors;
}

/***************************************************************************/
// Delete the list and all it's items
/***************************************************************************/
template <class T>
inline DynList<T>::~DynList()
{
	// clear the list-- calls destructors if appropriate, and removes all items from list
	Clear();

   // Now destroy the list
   delete list;
}

/***************************************************************************/
// free the array's memory, for memory leak control
/***************************************************************************/
template <class T>
inline void DynList<T>::Reset()
{
	// clear the list-- calls destructors if appropriate, and removes all items from list
	Clear();

	// reset the DynamicArray list
	list->Reset();
}

/***************************************************************************/
// Insert an item into the list
/***************************************************************************/
template <class T>
inline bool DynList<T>::Add(T *newItem, int pos)
{
   // Add the item
   return(list->Add(newItem, pos));
}

/***************************************************************************/
// Get an item from the list
/***************************************************************************/
template <class T>
inline T *DynList<T>::Get(
int pos) const
{
   // Get the item
   return(*list->Get(pos));
}

/***************************************************************************/
// Get an item from the list
/***************************************************************************/
template <class T>
inline T *DynList<T>::operator[](
int pos) const
{ 
   return(Get(pos));
}

/***************************************************************************/
// Delete an item from the list
/***************************************************************************/
template <class T>
inline bool DynList<T>::Delete(
int pos)
{
	if (callDestructors)
	{
		T **item = list->Get(pos);
		if (item)
			delete *item;
	}

   return(list->Delete(pos));
}

/***************************************************************************/
// Swap two items
/***************************************************************************/
template <class T>
inline bool DynList<T>::Swap(
int pos1,
int pos2)
{
   T **t1 = list->Get(pos1);
   T **t2 = list->Get(pos2);

   // Did they blow it?
   if (t1 == NULL || t2 == NULL)
      return(false);

   // Do the swap
   list->Replace(pos1, *t2);
   list->Replace(pos2, *t1);

   return(true);
}

/***************************************************************************/
// Find the item in the list (returns -1 if not found)
/***************************************************************************/
template <class T>
inline int DynList<T>::Find(
T const &lookFor) const
{
   // Is there even a list?
   if (list == NULL)
      return(-1);

   // Scan the list sequentially
	unsigned int i, count;
   for (i = 0, count = list->Count(); i < count; i++)
      if (list[i] == lookFor)
         return(i);

   // Not found
   return(-1);
}

/***************************************************************************/
// Find the item in the list (returns -1 if not found)
/***************************************************************************/
template <class T>
inline int DynList<T>::Find(
const T *lookFor) const
{
   // Is there even a list?
   if (list == NULL)
      return(-1);

   // Scan the list sequentially
	unsigned int i, count;
   for (i = 0, count = list->Count(); i < count; i++)
      if (*list->Get(i) == lookFor)
         return(i);

   // Not found
   return(-1);
}

/***************************************************************************/
// Clear the list
/***************************************************************************/
template <class T>
inline void DynList<T>::Clear()
{
	// Make sure destructors get called
	CallDestructors();

   // Now clear the list
   list->Clear();
}

/***************************************************************************/
// Sort the list
/***************************************************************************/
template <class T>
inline void DynList<T>::QSort(
int (CDECL *compare)(const void *elem1, const void *elem2))
{
	unsigned int count = list->Count();
	if (count)
		qsort(list->Get(0), count, sizeof(T *), compare);
}

/***************************************************************************/
// Clear the list
/***************************************************************************/
template <class T>
inline void DynList<T>::CallDestructors()
{
   // Make sure all the destructors get called
	if (callDestructors)
	{
	   unsigned int i, count;
   	for (i = 0, count = list->Count(); i < count; i++)
		{
			T **p = list->Get(i);
      	delete *p;
		}
	}
}
