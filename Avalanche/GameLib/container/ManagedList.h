/**********************************************/
/*                                            */
/* ManagedList.h                              */
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

#ifndef MANAGEDLIST_H
#define MANAGEDLIST_H

/* system includes */

/* engine includes */

#include "container/pool.h"
#include "container/dllist.h"

/******************* template *******************************/

template <class T> class ManagedList
{
public:

	/* constructor/destructor */

	inline ManagedList(unsigned int startingSize = 16, unsigned int automaticGrowSize = 16);
	inline ~ManagedList();

	/* get a record from the free list (pool will grow if necessary) and add to active list */

	inline T *New();

	/* return a record from active list to the free list */

	inline void Delete(T *item);

	/* access to the doubly linked list */

	inline T *ActiveHead()		{ return(activeList.Head()); }
	inline T *ActiveTail()		{ return(activeList.Tail()); }
	inline int IsActiveEmpty()	{ return(activeList.IsEmpty()); }
	inline int ActiveItems()	{ return(activeList.Items()); }

	/* lock this array after allocation-- for memory leak purposes, permanent arrays must be allocated large enough */
	/* that they never need to grow */

	inline void Lock()			{ pool.Lock(); }

private:

	/* the pool and doubly linked list */

   DoublyLinkedList<T>	activeList;
   Pool<T>					pool;
};

// Include the implementation file
#include "container/ManagedList.hpp"

#endif //MANAGEDLIST_H
