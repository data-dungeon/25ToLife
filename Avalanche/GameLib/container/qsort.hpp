//***************************************************************************
// Quck sort, adapted from standard C library
//***************************************************************************

//***************************************************************************
// Do the sort
//***************************************************************************
template <class T>
inline void QuickSort<T>::SortAscending(
T *list,
int items)
{
	int lo, hi; 					// ends of sub-array currently sorting
	int mid;							// points to middle of subarray
	int loguy, higuy;				// traveling pointers for partition step
	unsigned size;					// size of the sub-array
	int lostk[30], histk[30];
	int stkptr = 0; 					// stack for saving sub-array to be processed

	// Note: the number of stack entries required is no more than
	// 1 + log2(size), so 30 is sufficient for any array

	// Duh
	if (items < 2)
		return;

	// Set the extents
	lo = 0;
	hi = items - 1;

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// prserved, locals aren't, so we preserve stuff on the stack
recurse:

	// number of el's to sort
	size = (hi - lo) + 1;

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= QSORT_CUTOFF)
		InsertionSortAscending(list, lo, hi);
	else
	{
		// First we pick a partititioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the
		// median of the values, but also that we select one fast.  Using
		// the first one produces bad performace if the array is already
		// sorted, so we use the middle one, which would require a very
		// wierdly arranged array for worst case performance.  Testing shows
		// that a median-of-three algorithm does not, in general, increase
		// performance.
		mid = lo + (size / 2); 				// find middle element
		Swap(list, mid, lo);					// swap it to beginning of array

		// We now wish to partition the array into three pieces, one
		// consisiting of elements <= partition element, one of elements
		// equal to the parition element, and one of element >= to it.  This
		// is done below; comments indicate conditions established at every
		// step.
		loguy = lo;
		higuy = hi + 1;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;)
		{
		   // lo <= loguy < hi, lo < higuy <= hi + 1,
		   //  A[i] <= A[lo] for lo <= i <= loguy,
	   	//  A[i] >= A[lo] for higuy <= i <= hi
		   do {
				++loguy;
   		} while (loguy <= hi && list[loguy] <= list[lo]);

	  		// lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
	     	// either loguy > hi or A[loguy] > A[lo]
	   	do {
	         --higuy;
   		} while (higuy > lo && list[higuy] >= list[lo]);

	   	// lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
	      // either higuy <= lo or A[higuy] < A[lo]
		   if (higuy < loguy)
				break;

	   	// if loguy > hi or higuy <= lo, then we would have exited, so
	      // A[loguy] > A[lo], A[higuy] < A[lo],
	      // loguy < hi, highy > lo
		   Swap(list, loguy, higuy);

	   	// A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
	      // of loop is re-established
		}

		// A[i] >= A[lo] for higuy < i <= hi,
		// A[i] <= A[lo] for lo <= i < loguy,
		// higuy < loguy, lo <= higuy <= hi
		// implying:
		// A[i] >= A[lo] for loguy <= i <= hi,
		// A[i] <= A[lo] for lo <= i <= higuy,
		// A[i] = A[lo] for higuy < i < loguy
 		Swap(list, lo, higuy);		// put partition element in place

		// OK, now we have the following:
		// A[i] >= A[higuy] for loguy <= i <= hi,
		// A[i] <= A[higuy] for lo <= i < higuy
		// A[i] = A[lo] for higuy <= i < loguy

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy-1] and [loguy, hi].
	   // We do the smaller one first to minimize stack usage.
	   // We only sort arrays of length 2 or more.
		if (higuy - 1 - lo >= hi - loguy)
		{
			// save big recursion for later
		   if (lo + 1 < higuy)
			{
	         lostk[stkptr] = lo;
	         histk[stkptr] = higuy - 1;
	         ++stkptr;
	  		}

			// do small recursion
			if (loguy < hi)
			{
		      lo = loguy;
	         goto recurse;
		   }
		}
		else
		{
			// save big recursion for later
	   	if (loguy < hi)
			{
	         lostk[stkptr] = loguy;
	         histk[stkptr] = hi;
	         ++stkptr;
	  		}

			// do small recursion
	   	if (lo + 1 < higuy)
			{
	         hi = higuy - 1;
	         goto recurse;
	  		}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;
	if (stkptr >= 0)
	{
		// pop subarray from stack */
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;
	}
}

//***************************************************************************
// Do the sort
//***************************************************************************
template <class T>
inline void QuickSort<T>::SortDescending(
T *list,
int items)
{
	int lo, hi; 					// ends of sub-array currently sorting
	int mid;							// points to middle of subarray
	int loguy, higuy;				// traveling pointers for partition step
	unsigned size;					// size of the sub-array
	int lostk[30], histk[30];
	int stkptr = 0; 					// stack for saving sub-array to be processed

	// Note: the number of stack entries required is no more than
	// 1 + log2(size), so 30 is sufficient for any array

	// Duh
	if (items < 2)
		return;

	// Set the extents
	lo = 0;
	hi = items - 1;

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// prserved, locals aren't, so we preserve stuff on the stack
recurse:

	// number of el's to sort
	size = (hi - lo) + 1;

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= QSORT_CUTOFF)
		InsertionSortDescending(list, lo, hi);
	else
	{
		// First we pick a partititioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the
		// median of the values, but also that we select one fast.  Using
		// the first one produces bad performace if the array is already
		// sorted, so we use the middle one, which would require a very
		// wierdly arranged array for worst case performance.  Testing shows
		// that a median-of-three algorithm does not, in general, increase
		// performance.
		mid = lo + (size / 2); 				// find middle element
		Swap(list, mid, lo);					// swap it to beginning of array

		// We now wish to partition the array into three pieces, one
		// consisiting of elements <= partition element, one of elements
		// equal to the parition element, and one of element >= to it.  This
		// is done below; comments indicate conditions established at every
		// step.
		loguy = lo;
		higuy = hi + 1;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;)
		{
		   // lo <= loguy < hi, lo < higuy <= hi + 1,
		   //  A[i] <= A[lo] for lo <= i <= loguy,
	   	//  A[i] >= A[lo] for higuy <= i <= hi
		   do {
				++loguy;
   		} while (loguy <= hi && list[loguy] >= list[lo]);

	  		// lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
	     	// either loguy > hi or A[loguy] > A[lo]
	   	do {
	         --higuy;
   		} while (higuy > lo && list[higuy] <= list[lo]);

	   	// lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
	      // either higuy <= lo or A[higuy] < A[lo]
		   if (higuy < loguy)
				break;

	   	// if loguy > hi or higuy <= lo, then we would have exited, so
	      // A[loguy] > A[lo], A[higuy] < A[lo],
	      // loguy < hi, highy > lo
		   Swap(list, loguy, higuy);

	   	// A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
	      // of loop is re-established
		}

		// A[i] >= A[lo] for higuy < i <= hi,
		// A[i] <= A[lo] for lo <= i < loguy,
		// higuy < loguy, lo <= higuy <= hi
		// implying:
		// A[i] >= A[lo] for loguy <= i <= hi,
		// A[i] <= A[lo] for lo <= i <= higuy,
		// A[i] = A[lo] for higuy < i < loguy
 		Swap(list, lo, higuy);		// put partition element in place

		// OK, now we have the following:
		// A[i] >= A[higuy] for loguy <= i <= hi,
		// A[i] <= A[higuy] for lo <= i < higuy
		// A[i] = A[lo] for higuy <= i < loguy

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy-1] and [loguy, hi].
	   // We do the smaller one first to minimize stack usage.
	   // We only sort arrays of length 2 or more.
		if (higuy - 1 - lo >= hi - loguy)
		{
			// save big recursion for later
		   if (lo + 1 < higuy)
			{
	         lostk[stkptr] = lo;
	         histk[stkptr] = higuy - 1;
	         ++stkptr;
   		}

			// do small recursion
			if (loguy < hi)
			{
		      lo = loguy;
	         goto recurse;
		   }
		}
		else
		{
			// save big recursion for later
	   	if (loguy < hi)
			{
	         lostk[stkptr] = loguy;
	         histk[stkptr] = hi;
	         ++stkptr;
   		}

			// do small recursion
	   	if (lo + 1 < higuy)
			{
	         hi = higuy - 1;
	         goto recurse;
   		}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;
	if (stkptr >= 0)
	{
		// pop subarray from stack */
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;
	}
}

//***************************************************************************
// Do the sort
//***************************************************************************
template <class T>
inline void QuickSort<T>::SortAscending(
T **list,
int items)
{
	int lo, hi; 					// ends of sub-array currently sorting
	int mid;							// points to middle of subarray
	int loguy, higuy;				// traveling pointers for partition step
	unsigned size;					// size of the sub-array
	int lostk[30], histk[30];
	int stkptr = 0; 					// stack for saving sub-array to be processed

	// Note: the number of stack entries required is no more than
	// 1 + log2(size), so 30 is sufficient for any array

	// Duh
	if (items < 2)
		return;

	// Set the extents
	lo = 0;
	hi = items - 1;

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// prserved, locals aren't, so we preserve stuff on the stack
recurse:

	// number of el's to sort
	size = (hi - lo) + 1;

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= QSORT_CUTOFF)
		InsertionSortAscending(list, lo, hi);
	else
	{
		// First we pick a partititioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the
		// median of the values, but also that we select one fast.  Using
		// the first one produces bad performace if the array is already
		// sorted, so we use the middle one, which would require a very
		// wierdly arranged array for worst case performance.  Testing shows
		// that a median-of-three algorithm does not, in general, increase
		// performance.
		mid = lo + (size / 2); 				// find middle element
		Swap(list, mid, lo);					// swap it to beginning of array

		// We now wish to partition the array into three pieces, one
		// consisiting of elements <= partition element, one of elements
		// equal to the parition element, and one of element >= to it.  This
		// is done below; comments indicate conditions established at every
		// step.
		loguy = lo;
		higuy = hi + 1;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;)
		{
		   // lo <= loguy < hi, lo < higuy <= hi + 1,
		   //  A[i] <= A[lo] for lo <= i <= loguy,
	   	//  A[i] >= A[lo] for higuy <= i <= hi
		   do {
				++loguy;
   		} while (loguy <= hi && *(list[loguy]) <= *(list[lo]));

	  		// lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
	     	// either loguy > hi or A[loguy] > A[lo]
	   	do {
	         --higuy;
   		} while (higuy > lo && *(list[higuy]) >= *(list[lo]));

	   	// lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
	      // either higuy <= lo or A[higuy] < A[lo]
		   if (higuy < loguy)
				break;

	   	// if loguy > hi or higuy <= lo, then we would have exited, so
	      // A[loguy] > A[lo], A[higuy] < A[lo],
	      // loguy < hi, highy > lo
		   Swap(list, loguy, higuy);

	   	// A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
	      // of loop is re-established
		}

		// A[i] >= A[lo] for higuy < i <= hi,
		// A[i] <= A[lo] for lo <= i < loguy,
		// higuy < loguy, lo <= higuy <= hi
		// implying:
		// A[i] >= A[lo] for loguy <= i <= hi,
		// A[i] <= A[lo] for lo <= i <= higuy,
		// A[i] = A[lo] for higuy < i < loguy
 		Swap(list, lo, higuy);		// put partition element in place

		// OK, now we have the following:
		// A[i] >= A[higuy] for loguy <= i <= hi,
		// A[i] <= A[higuy] for lo <= i < higuy
		// A[i] = A[lo] for higuy <= i < loguy

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy-1] and [loguy, hi].
	   // We do the smaller one first to minimize stack usage.
	   // We only sort arrays of length 2 or more.
		if (higuy - 1 - lo >= hi - loguy)
		{
			// save big recursion for later
		   if (lo + 1 < higuy)
			{
	         lostk[stkptr] = lo;
	         histk[stkptr] = higuy - 1;
	         ++stkptr;
   		}

			// do small recursion
			if (loguy < hi)
			{
		      lo = loguy;
	         goto recurse;
		   }
		}
		else
		{
			// save big recursion for later
	   	if (loguy < hi)
			{
	         lostk[stkptr] = loguy;
	         histk[stkptr] = hi;
	         ++stkptr;
   		}

			// do small recursion
	   	if (lo + 1 < higuy)
			{
	         hi = higuy - 1;
	         goto recurse;
   		}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;
	if (stkptr >= 0)
	{
		// pop subarray from stack */
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;
	}
}

//***************************************************************************
// Do the sort
//***************************************************************************
template <class T>
inline void QuickSort<T>::SortDescending(
T **list,
int items)
{
	int lo, hi; 					// ends of sub-array currently sorting
	int mid;							// points to middle of subarray
	int loguy, higuy;				// traveling pointers for partition step
	unsigned size;					// size of the sub-array
	int lostk[30], histk[30];
	int stkptr = 0; 					// stack for saving sub-array to be processed

	// Note: the number of stack entries required is no more than
	// 1 + log2(size), so 30 is sufficient for any array

	// Duh
	if (items < 2)
		return;

	// Set the extents
	lo = 0;
	hi = items - 1;

	// this entry point is for pseudo-recursion calling: setting
	// lo and hi and jumping to here is like recursion, but stkptr is
	// prserved, locals aren't, so we preserve stuff on the stack
recurse:

	// number of el's to sort
	size = (hi - lo) + 1;

	// below a certain size, it is faster to use a O(n^2) sorting method
	if (size <= QSORT_CUTOFF)
		InsertionSortDescending(list, lo, hi);
	else
	{
		// First we pick a partititioning element.  The efficiency of the
		// algorithm demands that we find one that is approximately the
		// median of the values, but also that we select one fast.  Using
		// the first one produces bad performace if the array is already
		// sorted, so we use the middle one, which would require a very
		// wierdly arranged array for worst case performance.  Testing shows
		// that a median-of-three algorithm does not, in general, increase
		// performance.
		mid = lo + (size / 2); 				// find middle element
		Swap(list, mid, lo);					// swap it to beginning of array

		// We now wish to partition the array into three pieces, one
		// consisiting of elements <= partition element, one of elements
		// equal to the parition element, and one of element >= to it.  This
		// is done below; comments indicate conditions established at every
		// step.
		loguy = lo;
		higuy = hi + 1;

		// Note that higuy decreases and loguy increases on every iteration,
		// so loop must terminate.
		for (;;)
		{
		   // lo <= loguy < hi, lo < higuy <= hi + 1,
		   //  A[i] <= A[lo] for lo <= i <= loguy,
	   	//  A[i] >= A[lo] for higuy <= i <= hi
		   do {
				++loguy;
   		} while (loguy <= hi && *(list[loguy]) >= *(list[lo]));

	  		// lo < loguy <= hi+1, A[i] <= A[lo] for lo <= i < loguy,
	     	// either loguy > hi or A[loguy] > A[lo]
	   	do {
	         --higuy;
   		} while (higuy > lo && *(list[higuy]) <= *(list[lo]));

	   	// lo-1 <= higuy <= hi, A[i] >= A[lo] for higuy < i <= hi,
	      // either higuy <= lo or A[higuy] < A[lo]
		   if (higuy < loguy)
				break;

	   	// if loguy > hi or higuy <= lo, then we would have exited, so
	      // A[loguy] > A[lo], A[higuy] < A[lo],
	      // loguy < hi, highy > lo
		   Swap(list, loguy, higuy);

	   	// A[loguy] < A[lo], A[higuy] > A[lo]; so condition at top
	      // of loop is re-established
		}

		// A[i] >= A[lo] for higuy < i <= hi,
		// A[i] <= A[lo] for lo <= i < loguy,
		// higuy < loguy, lo <= higuy <= hi
		// implying:
		// A[i] >= A[lo] for loguy <= i <= hi,
		// A[i] <= A[lo] for lo <= i <= higuy,
		// A[i] = A[lo] for higuy < i < loguy
 		Swap(list, lo, higuy);		// put partition element in place

		// OK, now we have the following:
		// A[i] >= A[higuy] for loguy <= i <= hi,
		// A[i] <= A[higuy] for lo <= i < higuy
		// A[i] = A[lo] for higuy <= i < loguy

		// We've finished the partition, now we want to sort the subarrays
		// [lo, higuy-1] and [loguy, hi].
	   // We do the smaller one first to minimize stack usage.
	   // We only sort arrays of length 2 or more.
		if (higuy - 1 - lo >= hi - loguy)
		{
			// save big recursion for later
		   if (lo + 1 < higuy)
			{
	         lostk[stkptr] = lo;
	         histk[stkptr] = higuy - 1;
	         ++stkptr;
	  		}

			// do small recursion
			if (loguy < hi)
			{
		      lo = loguy;
	         goto recurse;
		   }
		}
		else
		{
			// save big recursion for later
	   	if (loguy < hi)
			{
	         lostk[stkptr] = loguy;
	         histk[stkptr] = hi;
	         ++stkptr;
   		}

			// do small recursion
	   	if (lo + 1 < higuy)
			{
	         hi = higuy - 1;
	         goto recurse;
   		}
		}
	}

	// We have sorted the array, except for any pending sorts on the stack.
	// Check if there are any, and do them.
	--stkptr;
	if (stkptr >= 0)
	{
		// pop subarray from stack */
		lo = lostk[stkptr];
		hi = histk[stkptr];
		goto recurse;
	}
}

//***************************************************************************
// Insertion sort for short lists
//***************************************************************************
template <class T>
inline void QuickSort<T>::InsertionSortAscending(
T *list,
int lo,
int hi)
{
	int p, max;

	// Note: in assertions below, i and j are alway inside original bound of
	// array to sort.
	while (hi > lo)
	{
		// A[i] <= A[j] for i <= j, j > hi
		max = lo;
		for (p = lo + 1; p <= hi; p++)
		{
	   	// A[i] <= A[max] for lo <= i < p
	   	if (list[p] > list[max])
	         max = p;

	   	// A[i] <= A[max] for lo <= i <= p
		}

		// A[i] <= A[max] for lo <= i <= hi
		Swap(list, max, hi);

		// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
		--hi;

		// A[i] <= A[j] for i <= j, j > hi, loop top condition established
	}

	// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
	// so array is sorted
}

//***************************************************************************
// Insertion sort for short lists
//***************************************************************************
template <class T>
inline void QuickSort<T>::InsertionSortDescending(
T *list,
int lo,
int hi)
{
	int p, max;

	// Note: in assertions below, i and j are alway inside original bound of
	// array to sort.
	while (hi > lo)
	{
		// A[i] <= A[j] for i <= j, j > hi
		max = lo;
		for (p = lo + 1; p <= hi; p++)
		{
	   	// A[i] <= A[max] for lo <= i < p
	   	if (list[p] < list[max])
	         max = p;

	   	// A[i] <= A[max] for lo <= i <= p
		}

		// A[i] <= A[max] for lo <= i <= hi
		Swap(list, max, hi);

		// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
		--hi;

		// A[i] <= A[j] for i <= j, j > hi, loop top condition established
	}

	// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
	// so array is sorted
}

//***************************************************************************
// Swap the two elements
// The <class T> must support the = operator
//***************************************************************************
template <class T>
inline void QuickSort<T>::Swap(
T *list,
int e1,
int e2)
{
	T temp = list[e1];
	list[e1] = list[e2];
	list[e2] = temp;
}

//***************************************************************************
// Insertion sort for short lists
//***************************************************************************
template <class T>
inline void QuickSort<T>::InsertionSortAscending(
T **list,
int lo,
int hi)
{
	int p, max;

	// Note: in assertions below, i and j are alway inside original bound of
	// array to sort.
	while (hi > lo)
	{
		// A[i] <= A[j] for i <= j, j > hi
		max = lo;
		for (p = lo + 1; p <= hi; p++)
		{
	   	// A[i] <= A[max] for lo <= i < p
	   	if (*(list[p]) > *(list[max]))
	         max = p;

	   	// A[i] <= A[max] for lo <= i <= p
		}

		// A[i] <= A[max] for lo <= i <= hi
		Swap(list, max, hi);

		// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
		--hi;

		// A[i] <= A[j] for i <= j, j > hi, loop top condition established
	}

	// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
	// so array is sorted
}

//***************************************************************************
// Insertion sort for short lists
//***************************************************************************
template <class T>
inline void QuickSort<T>::InsertionSortDescending(
T **list,
int lo,
int hi)
{
	int p, max;

	// Note: in assertions below, i and j are alway inside original bound of
	// array to sort.
	while (hi > lo)
	{
		// A[i] <= A[j] for i <= j, j > hi
		max = lo;
		for (p = lo + 1; p <= hi; p++)
		{
	   	// A[i] <= A[max] for lo <= i < p
	   	if (*(list[p]) < *(list[max]))
	         max = p;

	   	// A[i] <= A[max] for lo <= i <= p
		}

		// A[i] <= A[max] for lo <= i <= hi
		Swap(list, max, hi);

		// A[i] <= A[hi] for i <= hi, so A[i] <= A[j] for i <= j, j >= hi
		--hi;

		// A[i] <= A[j] for i <= j, j > hi, loop top condition established
	}

	// A[i] <= A[j] for i <= j, j > lo, which implies A[i] <= A[j] for i < j,
	// so array is sorted
}

//***************************************************************************
// Swap the two elements
// The <class T> must support the = operator
//***************************************************************************
template <class T>
inline void QuickSort<T>::Swap(
T **list,
int e1,
int e2)
{
	T *temp = list[e1];
	list[e1] = list[e2];
	list[e2] = temp;
}



