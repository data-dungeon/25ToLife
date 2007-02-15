//***************************************************************************
// I'm sick of C run-time's QSort (function call per comparison)
//***************************************************************************
#ifndef QSORT_H
#define QSORT_H

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
template <class T> class QuickSort
{
public:
	// Do nothing
	inline QuickSort(void) {}
	inline ~QuickSort(void) {}

	// The quick sorts
	inline void SortAscending(T *list, int items);
	inline void SortDescending(T *list, int items);

	// The quick sorts for list of pointers to objects
	inline void SortAscending(T **list, int items);
	inline void SortDescending(T **list, int items);

	// Handy little insertion sorts
	inline void	InsertionSortAscending(T *list, int items)
		{ InsertionSortAscending(list, 0, items - 1); }
	inline void	InsertionSortDescending(T *list, int items)
		{ InsertionSortDescending(list, 0, items - 1); }

	// Handy little insertion sorts for list of pointers to objects
	inline void	InsertionSortAscending(T **list, int items)
		{ InsertionSortAscending(list, 0, items - 1); }
	inline void	InsertionSortDescending(T **list, int items)
		{ InsertionSortDescending(list, 0, items - 1); }

private:
	// Internals
	inline void	InsertionSortAscending(T *list, int lo, int hi);
	inline void	InsertionSortDescending(T *list, int lo, int hi);
	inline void Swap(T *list, int e1, int e2);

	// For lists of pointers
	inline void	InsertionSortAscending(T **list, int lo, int hi);
	inline void	InsertionSortDescending(T **list, int lo, int hi);
	inline void Swap(T **list, int e1, int e2);
};

// Include the implementation
#include "container/qsort.hpp"

#endif
