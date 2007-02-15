/***************************************************************************/
// A simple n linked list (n doubly linked lists)
//
// Class T must already contain next/prev pointers
// i.e.: (public: T *next[n]; T *prev[n];)
//
// No initialization of the next/prev pointers is assumed
//
// No delete's are EVER called
/***************************************************************************/
#ifndef NLLIST_H
#define NLLIST_H

#ifndef NULL
#define NULL	0
#endif

// Define this to help track down problems
//#define NOTRUSTNLLLIST

template <class T, uint n> class NLinkedList
{
public:
	// The constructor
	inline NLinkedList(void);

	// We do nothing
	inline ~NLinkedList(void) {}

	// Add an item at the head/tail of the list
	// When you add an item the *LIST* controls the destructor.
	inline void AddHead(T *newItem, uint forN);
	inline void AddTail(T *newItem, uint forN);

	// Unlink an object from the list, but don't destroy it
	inline void Unlink(T *item, uint forN);
	inline void Unlink(T *item);
	inline void UnlinkAll(uint forN);
	inline void UnlinkAll(void);

	// Get at the head
	inline T *Head(uint forN);

	// Get at the tail
	inline T *Tail(uint forN);

	// Is the list empty?
	inline bool IsEmpty(uint forN);
	inline bool IsEmpty(void);

	// Swap a and b (assumes they are both in the list)
	inline void Swap(T *a, T *b, uint forN);

	// Insert insertMe before/after e, (insertMe is not in the list!)
	inline void InsertBefore(T *e, T *insertMe, uint forN);
	inline void InsertAfter(T *e, T *insertMe, uint forN);

	// Move moveMe before/after e, moveMe is in the list
	inline void MoveBefore(T *e, T *moveMe, uint forN);
	inline void MoveAfter(T *e, T *moveMe, uint forN);

	// Insert this item in the list, requires that < > are implmented
	inline void InsertAscending(T *newItem, uint forN);
	inline void InsertAscending(T *newItem);

	inline void InsertDescending(T *newItem, uint forN);
	inline void InsertDescending(T *newItem);

	// Sort the list using a bubble sort...a special note here:
	// bubble sorts are horrible inefficient and naive, however
	// if the list is "almost in order" than it can be the fastest
	// sort.  Given I'm implementing this for a 2D sort of
	// physical objects frame-to-frame, this is the best sort.  Please
	// add some other sorts that are more efficient if you need them.
	// requires int Compare(T &a, T &b, uint forN) returns:
	//	< 0: a < b
	//   0: a == b
	//	> 0: a > b
	// returns the # of swaps made (for profiling reasons).
	inline int BubbleSortAscending(uint forN);
	inline int BubbleSortDescending(uint forN);

	inline int BubbleSortAscending(void);
	inline int BubbleSortDescending(void);

	// For debugging, returns false if broken
	inline bool Debug(uint forN);
	inline bool Debug(void);

private:
	T		*head[n], *tail[n];		// head/tail of the list

#ifdef NOTRUSTNLLLIST
	uint	debugCount;
#endif
};

// Implementation
#include "container/nllist.hpp"

#endif
