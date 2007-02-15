/***************************************************************************/
// A simple doubly linked list
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline NLinkedList<T, n>::NLinkedList(void)
{
	// Init the pointers
	for (uint i = 0; i < n; i++)
	{
		head[i] = NULL;
		tail[i] = NULL;
	}

#ifdef NOTRUSTNLLLIST
	debugCount = 0;
#endif
}

/***************************************************************************/
// Add an item to the start of the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::AddHead(
T *newItem,
uint forN)
{
	ASSERT(forN < n);

	// Insert it into the list
	if (IsEmpty(forN))
	{
		head[forN] = tail[forN] = newItem;
		newItem->next[forN] = NULL;
		newItem->prev[forN] = NULL;
	}
	else
	{
		newItem->next[forN] = head[forN];
		newItem->prev[forN] = NULL;
		head[forN]->prev[forN] = newItem;
		head[forN] = newItem;
	}

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Add an item to the end of the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::AddTail(
T *newItem,
uint forN)
{
	ASSERT(forN < n);

	// Insert it into the list
	if (IsEmpty(forN))
	{
		head[forN] = tail[forN] = newItem;
		newItem->next[forN] = NULL;
		newItem->prev[forN] = NULL;
	}
	else
	{
		newItem->next[forN] = NULL;
		newItem->prev[forN] = tail[forN];
		tail[forN]->next[forN] = newItem;
		tail[forN] = newItem;
	}

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Remove this item from the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::Unlink(
T *item,
uint forN)
{
	ASSERT(forN < n);

	// Removing the head?
	if (head[forN] == item)
		head[forN] = item->next[forN];
	else
		item->prev[forN]->next[forN] = item->next[forN];

	// Removing the tail?
	if (tail[forN] == item)
		tail[forN] = item->prev[forN];
	else
		item->next[forN]->prev[forN] = item->prev[forN];

	// Not in the list any more
	item->next[forN] = item->prev[forN] = NULL;

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Remove this item from the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::Unlink(
T *item)
{
	for (uint i = 0; i < n; i++)
		Unlink(item, i);
}

/***************************************************************************/
// Unlink all the items from the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::UnlinkAll(
uint forN)
{
	ASSERT(forN < n);

	while (Head(forN))
		Unlink(Head(forN));
}

/***************************************************************************/
// Unlink all the items from the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::UnlinkAll(void)
{
	for (uint i = 0; i < n; i++)
		Unlink(forN);
}

/***************************************************************************/
// Get at the head
/***************************************************************************/
template <class T, uint n>
inline T *NLinkedList<T, n>::Head(
uint forN)
{
	ASSERT(forN < n);
	return(head[forN]);
}

/***************************************************************************/
// Get at the tail
/***************************************************************************/
template <class T, uint n>
inline T *NLinkedList<T, n>::Tail(
uint forN)
{
	ASSERT(forN < n);
	return(tail[forN]);
}

/***************************************************************************/
// Is the list empty?
/***************************************************************************/
template <class T, uint n>
inline bool NLinkedList<T, n>::IsEmpty(
uint forN)
{
	ASSERT(forN < n);
	return(head[forN] == NULL);
}

/***************************************************************************/
// Are all the lists empty?
/***************************************************************************/
template <class T, uint n>
inline bool NLinkedList<T, n>::IsEmpty(void)
{
	for (uint i = 0; i < n; i++)
	{
		if (head[forN])
			return(false);
	}

	return(true);
}

/***************************************************************************/
// Swap a and b (assumes they are both in the list)
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::Swap(
T *a,
T *b,
uint forN)
{
	ASSERT(forN < n);

	// Special case, they are right next to each other
	if (a->next[forN] == b)
	{
		b->prev[forN] = a->prev[forN];
		if (b->prev[forN] != NULL)
			b->prev[forN]->next[forN] = b;
		else
			head[forN] = b;

		a->next[forN] = b->next[forN];
		if (a->next[forN] != NULL)
			a->next[forN]->prev[forN] = a;
		else
			tail[forN] = a;

		b->next[forN] = a;
		a->prev[forN] = b;
	}
	else if (b->next[forN] == a)
	{
		a->prev[forN] = b->prev[forN];
		if (a->prev[forN] != NULL)
			a->prev[forN]->next[forN] = a;
		else
			head[forN] = a;

		b->next[forN] = a->next[forN];
		if (b->next[forN] != NULL)
			b->next[forN]->prev[forN] = b;
		else
			tail[forN] = b;

		a->next[forN] = b;
		b->prev[forN] = a;
	}
	else // They are far apart in the list
	{
		// save off a's prev and next
		T *tempPrev = a->prev[forN];
		T *tempNext = a->next[forN];

		// Put a in b's place
		a->prev[forN] = b->prev[forN];
		if (a->prev[forN] != NULL)
			a->prev[forN]->next[forN] = a;
		else
			head[forN] = a;

		a->next[forN] = b->next[forN];
		if (a->next[forN] != NULL)
			a->next[forN]->prev[forN] = a;
		else
			tail[forN] = a;

		// put b in a's place
		b->prev[forN] = tempPrev;
		if (b->prev[forN] != NULL)
			b->prev[forN]->next[forN] = b;
		else
			head[forN] = b;

		b->next[forN] = tempNext;
		if (b->next[forN] != NULL)
			b->next[forN]->prev[forN] = b;
		else
			tail[forN] = b;
	}

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Insert insertMe before/after e, (insertMe is not in the list!)
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertBefore(
T *e,
T *insertMe,
uint forN)
{
	ASSERT(forN < n);

	// Are we at the head?
	if (e->prev[forN] != NULL)
		e->prev[forN]->next[forN] = insertMe;
	else
		head[forN] = insertMe;

	// Insert into the list
	insertMe->prev[forN] = e->prev[forN];
	insertMe->next[forN] = e;
	e->prev[forN] = insertMe;

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertAfter(
T *e,
T *insertMe,
uint forN)
{
	ASSERT(forN < n);

	// Are we at the tail?
	if (e->next[forN] != NULL)
		e->next[forN]->prev[forN] = insertMe;
	else
		tail[forN] = insertMe;

	// Insert into the list
	insertMe->next[forN] = e->next[forN];
	insertMe->prev[forN] = e;
	e->next[forN] = insertMe;

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Move moveMe before/after e, moveMe is in the list
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::MoveBefore(
T *e,
T *moveMe,
uint forN)
{
	ASSERT(forN < n);

	Unlink(moveMe, forN);
	InsertBefore(e, moveMe, forN);

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::MoveAfter(
T *e,
T *moveMe,
uint forN)
{
	ASSERT(forN < n);

	Unlink(moveMe, forN);
	InsertAfter(e, moveMe, forN);

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Insert this item in the list, requires that < > are implmented
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertAscending(
T *newItem,
uint forN)
{
	ASSERT(forN < n);

	// is there a head?
	if (head[forN] == NULL)
		AddHead(newItem, forN);
	else
	{
		T *e = head[forN];
		while (e != NULL && T::Compare(*e, *newItem, forN) > 0)
			e = e->next[forN];

		if (e == NULL)
			AddTail(newItem, forN);
		else
			InsertBefore(e, newItem, forN);
	}

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertDescending(
T *newItem,
uint forN)
{
	ASSERT(forN < n);

	// is there a head?
	if (head[forN] == NULL)
		AddHead(newItem, forN);
	else
	{
		T *e = head[forN];
		while (e != NULL && T::Compare(*e, *newItem, forN) < 0)
			e = e->next[forN];

		if (e == NULL)
			AddTail(newItem, forN);
		else
			InsertBefore(e, newItem, forN);
	}

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
}

/***************************************************************************/
// Insert this item in the list, requires that < > are implmented
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertAscending(
T *newItem)
{
	for (uint i = 0; i < n; i++)
		InsertAscending(newItem, i);
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::InsertDescending(
T *newItem)
{
	for (uint i = 0; i < n; i++)
		InsertDescending(newItem, i);
}

/***************************************************************************/
// Sort the list using a bubble sort...a special note here:
// bubble sorts are horrible inefficient and naive, however
// if the list is "almost in order" than it can be the fastest
// sort.  Given I'm implementing this for a 2D sort of
// physical objects frame-to-frame, this is the best sort.  Please
// add some other sorts that are more efficient if you need them.
/***************************************************************************/
template <class T, uint n>
inline int NLinkedList<T, n>::BubbleSortAscending(
uint forN)
{
	ASSERT(forN < n);
	int swaps = 0;

	bool sorted;
	do {
		sorted = true;
		T *e = head[forN];
		while (e != NULL && e->next[forN] != NULL)
		{
			if (T::Compare(*e, *e->next[forN], forN) > 0)
			{
				swaps++;
				Swap(e, e->next[forN], forN);
				sorted = false;
			}
			else
				e = e->next[forN];
		}
	} while (!sorted);

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
	return swaps;
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline int NLinkedList<T, n>::BubbleSortDescending(
uint forN)
{
	int swaps = 0;
	ASSERT(forN < n);

	bool sorted;
	do {
		sorted = true;
		T *e = head[forN];
		while (e != NULL && e->next[forN] != NULL)
		{
			if (T::Compare(*e, *e->next[forN], forN) < 0)
			{
				swaps++;
				Swap(e, e->next[forN], forN);
				sorted = false;
			}
			else
				e = e->next[forN];
		}
	} while (!sorted);

#ifdef NOTRUSTNLLLIST
	ASSERT(Debug(forN));
#endif
	return swaps;
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline int NLinkedList<T, n>::BubbleSortAscending(void)
{
	int swaps = 0;
	for (uint i = 0; i < n; i++)
		swaps += BubbleSortAscending(i);
	return swaps;
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline int NLinkedList<T, n>::BubbleSortDescending(void)
{
	int swaps = 0;
	for (uint i = 0; i < n; i++)
		swaps += BubbleSortDescending(i);
	return swaps;
}

/***************************************************************************/
// For debugging, returns false if the list is corrupted
/***************************************************************************/
template <class T, uint n>
inline bool NLinkedList<T, n>::Debug(
uint forN)
{
#ifdef NOTRUSTNLLLIST
	int count;
	T *t;

	++debugCount;

	ASSERT(forN < n);

	// Basics
	if (head[forN] == NULL && tail[forN] == NULL)
		return(true);

	if (head[forN] == NULL && tail[forN] != NULL)
		goto maddjunk;

	if (tail[forN] == NULL && head[forN] != NULL)
		goto maddjunk;

	if (head[forN]->prev[forN] != NULL)
		goto maddjunk;

	if (tail[forN]->next[forN] != NULL)
		goto maddjunk;

	t = head[forN];
	while (t)
	{
		if (t->next[forN] == t)
			goto maddjunk;

		if (t->prev[forN] == t)
			goto maddjunk;

		if (t->prev[forN] == NULL && head[forN] != t)
			goto maddjunk;

		if (t->next[forN] == NULL && tail[forN] != t)
			goto maddjunk;

		t = t->next[forN];
	}

	t = tail[forN];
	while (t)
	{
		if (t->next[forN] == t)
			goto maddjunk;

		if (t->prev[forN] == t)
			goto maddjunk;

		if (t->prev[forN] == NULL && head[forN] != t)
			goto maddjunk;

		if (t->next[forN] == NULL && tail[forN] != t)
			goto maddjunk;
	
		t = t->prev[forN];
	}

	// verify item numbers
	count = 0;
  	t = head[forN];
	while (t)
	{
		++count;
		t = t->next[forN];
	}
	t = tail[forN];
	while (t)
	{
		--count;
		t = t->prev[forN];
	}
	if (count != 0)
		goto maddjunk;
#endif

	// okay
	return(true);

maddjunk:
	return(false);
}

/***************************************************************************/
/***************************************************************************/
template <class T, uint n>
inline bool NLinkedList<T, n>::Debug(void)
{
#ifdef NOTRUSTNLLLIST
	for (uint i = 0; i < n; i++)
	{
		if (!Debug(i))
			return(false);
	}
#endif

	return(true);
}

#ifdef NOTRUSTNLLLIST
#include <stdio.h>

/***************************************************************************/
// Hello
/***************************************************************************/
template <class T, uint n>
inline void NLinkedList<T, n>::Dump(
char *file, 
char *title,
uint forN)
{
	ASSERT(forN < n);

	FILE *fh = fopen(file, "at");
	if (!fh)	
		return;

	fprintf(fh, "[%d] %s (debug = %d)\n\n", forN, title, debugCount);
	T *t = head[forN];
	while (t)
	{
		T::Dump(fh, t, forN);
		t = t->next[forN];
	}
	fprintf(fh, "\n");

	fclose(fh);
}

#endif
