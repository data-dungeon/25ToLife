/***************************************************************************/
// A simple doubly linked list
/***************************************************************************/

/***************************************************************************/
/***************************************************************************/
template <class T>
inline DoublyLinkedList<T>::DoublyLinkedList(
bool callTheDestructors)
{
	// Init the pointers
	head = tail = NULL;

	// Call the destructors?
	callDestructors = callTheDestructors;

	// No items yet
	items = 0;
}

/***************************************************************************/
// Destroy everything
/***************************************************************************/
template <class T>
inline DoublyLinkedList<T>::~DoublyLinkedList(void)
{
	// Clean the list?
	if (callDestructors)
	{
		while (!IsEmpty())
			Remove(Head());
	}
}

/***************************************************************************/
// Add an item to the start of the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::AddHead(
T *newItem)
{
	// Insert it into the list
	if (IsEmpty())
	{
		head = tail = newItem;
		newItem->next = NULL;
		newItem->prev = NULL;
	}
	else
	{
		newItem->next = head;
		newItem->prev = NULL;
		head->prev = newItem;
		head = newItem;
	}

	// One more item
	++items;
}

/***************************************************************************/
// Add an item to the end of the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::AddTail(
T *newItem)
{
	// Insert it into the list
	if (IsEmpty())
	{
		head = tail = newItem;
		newItem->next = NULL;
		newItem->prev = NULL;
	}
	else
	{
		newItem->next = NULL;
		newItem->prev = tail;
		tail->next = newItem;
		tail = newItem;
	}

	// One more item
	++items;
}

/***************************************************************************/
// Remove this item from the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::Remove(
T *item)
{
	// Removing the head?
	if (head == item)
		head = item->next;
	else
		item->prev->next = item->next;

	// Removing the tail?
	if (tail == item)
		tail = item->prev;
	else
		item->next->prev = item->prev;

	// Delete this bad boy
	if (callDestructors)
		delete item;
	// Not in the list any more
	else
		item->next = item->prev = NULL;
	--items;
}

/***************************************************************************/
// Remove all the items
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::RemoveAll(void)
{
	while (Head())
		Remove(Head());
}

/***************************************************************************/
// Remove this item from the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::Unlink(
T *item)
{
	// Removing the head?
	if (head == item)
		head = item->next;
	else
		item->prev->next = item->next;

	// Removing the tail?
	if (tail == item)
		tail = item->prev;
	else
		item->next->prev = item->prev;

	// Not in the list any more
	item->next = item->prev = NULL;

	// One less item
	--items;
}

/***************************************************************************/
// Unlink all the items from the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::UnlinkAll(void)
{
	while (Head())
		Unlink(Head());
}

/***************************************************************************/
// Get at the head
/***************************************************************************/
template <class T>
inline T *DoublyLinkedList<T>::Head(void)
{
	return(head);
}

/***************************************************************************/
// Get at the tail
/***************************************************************************/
template <class T>
inline T *DoublyLinkedList<T>::Tail(void)
{
	return(tail);
}

/***************************************************************************/
// Is the list empty?
/***************************************************************************/
template <class T>
inline int DoublyLinkedList<T>::IsEmpty(void)
{
	return(head == NULL);
}

/***************************************************************************/
// Is the given item in the list?
/***************************************************************************/
template <class T>
inline bool DoublyLinkedList<T>::Contains(T *item)
{
	T *curr = head;
	while (curr != NULL)
	{
		if (curr == item)
		{
			return(true);
		}
		curr = curr->next;
	}

	return(false);
}

/***************************************************************************/
// How many items are in the list
/***************************************************************************/
template <class T>
inline int DoublyLinkedList<T>::Items(void)
{
	return(items);
}

/***************************************************************************/
// Swap a and b (assumes they are both in the list)
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::Swap(
T *a,
T *b)
{
	// Special case, they are right next to each other
	if (a->next == b)
	{
		b->prev = a->prev;
		if (b->prev != NULL)
			b->prev->next = b;
		else
			head = b;

		a->next = b->next;
		if (a->next != NULL)
			a->next->prev = a;
		else
			tail = a;

		b->next = a;
		a->prev = b;
	}
	else if (b->next == a)
	{
		a->prev = b->prev;
		if (a->prev != NULL)
			a->prev->next = a;
		else
			head = a;

		b->next = a->next;
		if (b->next != NULL)
			b->next->prev = b;
		else
			tail = b;

		a->next = b;
		b->prev = a;
	}
	else // They are far apart in the list
	{
		// save off a's prev and next
		T *tempPrev = a->prev;
		T *tempNext = a->next;

		// Put a in b's place
		a->prev = b->prev;
		if (a->prev != NULL)
			a->prev->next = a;
		else
			head = a;

		a->next = b->next;
		if (a->next != NULL)
			a->next->prev = a;
		else
			tail = a;

		// put b in a's place
		b->prev = tempPrev;
		if (b->prev != NULL)
			b->prev->next = b;
		else
			head = b;

		b->next = tempNext;
		if (b->next != NULL)
			b->next->prev = b;
		else
			tail = b;
	}
}

/***************************************************************************/
// Insert insertMe before/after e, (insertMe is not in the list!)
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::InsertBefore(
T *e,
T *insertMe)
{
	// Are we at the head?
	if (e->prev != NULL)
		e->prev->next = insertMe;
	else
		head = insertMe;

	// Insert into the list
	insertMe->prev = e->prev;
	insertMe->next = e;
	e->prev = insertMe;

	// One more item
	++items;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::InsertAfter(
T *e,
T *insertMe)
{
	// Are we at the tail?
	if (e->next != NULL)
		e->next->prev = insertMe;
	else
		tail = insertMe;

	// Insert into the list
	insertMe->next = e->next;
	insertMe->prev = e;
	e->next = insertMe;

	// One more item
	++items;
}

/***************************************************************************/
// Move moveMe before/after e, moveMe is in the list
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::MoveBefore(
T *e,
T *moveMe)
{
	Unlink(moveMe);
	InsertBefore(e, moveMe);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::MoveAfter(
T *e,
T *moveMe)
{
	Unlink(moveMe);
	InsertAfter(e, moveMe);
}

/***************************************************************************/
// Insert this item in the list, requires that < > are implmented
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::InsertAscending(
T *newItem)
{
	// is there a head?
	if (head == NULL)
		AddHead(newItem);
	else
	{
		T *e = head;
		while (e != NULL && *e < *newItem)
			e = e->next;

		if (e == NULL)
			AddTail(newItem);
		else
			InsertBefore(e, newItem);
	}
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::InsertDescending(
T *newItem)
{
	// is there a head?
	if (head == NULL)
		AddHead(newItem);
	else
	{
		T *e = head;
		while (e != NULL && *e > *newItem)
			e = e->next;

		if (e == NULL)
			AddTail(newItem);
		else
			InsertBefore(e, newItem);
	}
}

/***************************************************************************/
// Sort the list using a bubble sort...a special note here:
// bubble sorts are horrible inefficient and naive, however
// if the list is "almost in order" than it can be the fastest
// sort.  Given I'm implementing this for a 2D sort of
// physical objects frame-to-frame, this is the best sort.  Please
// add some other sorts that are more efficient if you need them.
// requires that < > are implemented
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::BubbleSortAscending(void)
{
	bool sorted;
	do {
		sorted = true;
		T *e = head;
		for (int i = 0; i < items - 1; i++)
		{
			if (*e > *e->next)
			{
				Swap(e, e->next);
				sorted = false;
			}
			else
				e = e->next;
		}
	} while (!sorted);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void DoublyLinkedList<T>::BubbleSortDescending(void)
{
	bool sorted;
	do {
		sorted = true;
		T *e = head;
		for (int i = 0; i < items - 1; i++)
		{
			if (*e < *e->next)
			{
				Swap(e, e->next);
				sorted = false;
			}
			else
				e = e->next;
		}
	} while (!sorted);
}

/***************************************************************************/
// For debugging, returns false if the list is corrupted
/***************************************************************************/
template <class T>
inline bool DoublyLinkedList<T>::Debug(void)
{
	// Basics
	if (head == NULL && tail == NULL)
		return(true);

	if (head == NULL && tail != NULL)
		return(false);

	if (tail == NULL && head != NULL)
		return(false);

	if (head->prev != NULL)
		return(false);

	if (tail->next != NULL)
		return(false);

	T *t = head;
	while (t)
	{
		if (t->next == t)
			return(false);

		if (t->prev == t)
			return(false);

		if (t->prev == NULL && head != t)
			return(false);

		if (t->next == NULL && tail != t)
			return(false);

		t = t->next;
	}

	// verify item numbers
	int count = 0;
  	t = head;
	while (t)
	{
		++count;
		t = t->next;
	}
	if (count != items)
		return(false);

	t = tail;
	while (t)
	{
		--count;
		t = t->prev;
	}
	if (count != 0)
		return(false);

	// okay
	return(true);
}

