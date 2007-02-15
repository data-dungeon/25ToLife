/***************************************************************************/
// A simple doubly linked list
//
// Class T must already contain next/prev pointers
// i.e.: (public: T *next; T *prev;)
//
// No initialization of the next/prev pointers is assumed
/***************************************************************************/
#ifndef DLLIST_H
#define DLLIST_H

#ifndef NULL
#define NULL	0
#endif

template <class T> class DoublyLinkedList
{
public:
	// The constructor
	inline DoublyLinkedList(bool callTheDestructors = true);

	// This will delete all the items in the list
	inline ~DoublyLinkedList(void);

	inline void CallDestructors(bool callTheDestructors)
		{ callDestructors = callTheDestructors; }

	// Add an item at the head/tail of the list
	// When you add an item the *LIST* controls the destructor.
	inline void AddHead(T *newItem);
	inline void AddTail(T *newItem);

	// Remove an item from the list
	// Will call the object destructor (if callTheDestructors is set)
	inline void Remove(T *item);
	inline void RemoveAll(void);

	// Unlink an object from the list, but don't destroy it
	inline void Unlink(T *item);
	inline void UnlinkAll(void);

	// Get at the head
	inline T *Head(void);

	// Get at the tail
	inline T *Tail(void);

	// Is the list empty?
	inline int IsEmpty(void);

	// Does the list contain the item?
	inline bool Contains(T *item);

	// How many items are in the list
	inline int Items(void);

	// Swap a and b (assumes they are both in the list)
	inline void Swap(T *a, T *b);

	// Insert insertMe before/after e, (insertMe is not in the list!)
	inline void InsertBefore(T *e, T *insertMe);
	inline void InsertAfter(T *e, T *insertMe);

	// Move moveMe before/after e, moveMe is in the list
	inline void MoveBefore(T *e, T *moveMe);
	inline void MoveAfter(T *e, T *moveMe);

	// Insert this item in the list, requires that < > are implmented
	inline void InsertAscending(T *newItem);
	inline void InsertDescending(T *newItem);

	// Sort the list using a bubble sort...a special note here:
	// bubble sorts are horrible inefficient and naive, however
	// if the list is "almost in order" than it can be the fastest
	// sort.  Given I'm implementing this for a 2D sort of
	// physical objects frame-to-frame, this is the best sort.  Please
	// add some other sorts that are more efficient if you need them.
	// requires that < > are implemented
	inline void BubbleSortAscending(void);
	inline void BubbleSortDescending(void);

	// For debugging, returns false if broken
	inline bool Debug(void);

private:
	T		*head, *tail;		// head/tail of the list
	bool	callDestructors;	// Call the destructors
	int	items;            // Items in the list
};

// Implementation
#include "container/dllist.hpp"

#endif
