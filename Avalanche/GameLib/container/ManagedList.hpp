/**********************************************/
/*                                            */
/* ManagedList.hpp                            */
/* big juju  11/07/02                         */
/* ned martin  avalanche software             */
/* visual c++ 6.0                             */
/* a managed list of records                  */
/*                                            */
/**********************************************/

/* this template combines the Pool and the DoublyLinkedList templates to provide an easy-to-use list of records with
an active and a free list. the list grows dynamically, as needed, but does not shrink until destroyed. for example, i plan
to use this for naplam blobs-- a pool of CNapalm records with two linked lists running through it, a free and an active list.
the Pool template provides the memory for the list of records, and the free list. the DoublyLinkedList template provides
the active list

the record that uses this template must contain next/prev pointers, i.e.: (public: T *next; T *prev;) */

/* constructor */

template <class T>
inline ManagedList<T>::ManagedList(
unsigned int startingSize /*= 16*/,
unsigned int automaticGrowSize /*= 16*/) :
activeList(false),	// must be false, see comment in ~ManagedList
pool(startingSize, automaticGrowSize)
{
}

/* destructor-- cleans up pool and doubly-linked list */

template <class T>
inline ManagedList<T>::~ManagedList()
{
	/* this template sets DoublyLinkedList::callDestructors to false-- it has to, because the records are not malloc'd */
	/* individually, but come out of the pool buffers, and DoublyLinkedList calls delete if callDestructors is true. */
	/* so go through the active list and clean up each item, so that all records are in the free list, and presumably */
	/* clean */

	while (ActiveHead())
		Delete(ActiveHead());
}

/* get a record from the free list (pool will grow if necessary) and add to active list */

template <class T>
inline T *ManagedList<T>::New()
{
	/* get a record from the pool */

	T *item = pool.New();

	/* add to active list */

	if (item)
		activeList.AddTail(item);

	/* return value */

	return(item);
}

/* return a record to the free list */

template <class T>
inline void ManagedList<T>::Delete(T *item)
{
	/* remove from active list */

	activeList.Unlink(item);

	/* add to free list. also calls item's destructor */

	pool.Delete(item);
}

