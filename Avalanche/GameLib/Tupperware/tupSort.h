////////////////////////////////////////////////////////////////////////////
//
// TupSort
//
// A class for doing quick sorting - taken from the gamelib container project
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPERWARE_SORT_H
#define TUPPERWARE_SORT_H

// This parameter defines the cutoff between using quick sort and
// insertion sort for arrays; arrays with lengths shorter or equal to the
// below value use insertion sort.
// testing shows that this is good value
#define QSORT_CUTOFF	8

// Note <class T> must support:
// operator=
// operator<=
// operator>=
// operator<
// operator>
template <class T> class TupSort
{
public:
	// The quick sorts
	static void SortAscending(T *list, int items);
	static void SortDescending(T *list, int items);

	// The quick sorts for list of pointers to objects
	static void SortAscending(T **list, int items);
	static void SortDescending(T **list, int items);

	// Handy little insertion sorts
	static void	InsertionSortAscending(T *list, int items)
		{ InsertionSortAscending(list, 0, items - 1); }
	static void	InsertionSortDescending(T *list, int items)
		{ InsertionSortDescending(list, 0, items - 1); }

	// Handy little insertion sorts for list of pointers to objects
	static void	InsertionSortAscending(T **list, int items)
		{ InsertionSortAscending(list, 0, items - 1); }
	static void	InsertionSortDescending(T **list, int items)
		{ InsertionSortDescending(list, 0, items - 1); }

private:
	// Internals
	static void	InsertionSortAscending(T *list, int lo, int hi);
	static void	InsertionSortDescending(T *list, int lo, int hi);
	static void Swap(T *list, int e1, int e2);

	// For lists of pointers
	static void	InsertionSortAscending(T **list, int lo, int hi);
	static void	InsertionSortDescending(T **list, int lo, int hi);
	static void Swap(T **list, int e1, int e2);
};

// Include the implementation
#include "tupSort.hpp"

#endif
