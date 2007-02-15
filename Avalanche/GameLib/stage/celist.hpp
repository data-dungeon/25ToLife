/***************************************************************************/
// Add an item to the start of the list
/***************************************************************************/
inline void ClusterElementList::AddHead(
ClusterElement *newItem)
{
	if (!newItem)
		return;

	// Insert it into the list
	if (IsEmpty())
	{
		head = tail = newItem;
		newItem->processingNext = NULL;
		newItem->processingPrev = NULL;
	}
	else
	{
		newItem->processingNext = head;
		newItem->processingPrev = NULL;
		head->processingPrev = newItem;
		head = newItem;
	}
}

/***************************************************************************/
/***************************************************************************/
inline ClusterElementList::ClusterElementList(
ClusterElement *ce)
{
	// Init the pointers
	head = NULL;
	tail = NULL;
	AddHead(ce);
}

/***************************************************************************/
// Remove this item from the list
/***************************************************************************/
inline void ClusterElementList::Remove(
ClusterElement *item)
{
	if (item)
	{
		ASSERT(head == item || item->processingPrev != NULL);
		ASSERT(tail == item || item->processingNext != NULL);

		// Removing the head?
		if (head == item)
			head = item->processingNext;
		else
			item->processingPrev->processingNext = item->processingNext;

		// Removing the tail?
		if (tail == item)
			tail = item->processingPrev;
		else
			item->processingNext->processingPrev = item->processingPrev;

		// Not in the list any more
		item->processingNext = item->processingPrev = NULL;
	}
}

/***************************************************************************/
// Remove all the items
/***************************************************************************/
inline void ClusterElementList::RemoveAll(void)
{
	while (Head())
		Remove(Head());
}

/***************************************************************************/
// Destroy everything
/***************************************************************************/
inline ClusterElementList::~ClusterElementList(void)
{
	RemoveAll();
}

/***************************************************************************/
// Add an item to the end of the list
/***************************************************************************/
inline void ClusterElementList::AddTail(
ClusterElement *newItem)
{
	if (!newItem)
		return;

	// Insert it into the list
	if (IsEmpty())
	{
		head = tail = newItem;
		newItem->processingNext = NULL;
		newItem->processingPrev = NULL;
	}
	else
	{
		newItem->processingNext = NULL;
		newItem->processingPrev = tail;
		tail->processingNext = newItem;
		tail = newItem;
	}
}

/***************************************************************************/
// Get at the head
/***************************************************************************/
inline ClusterElement *ClusterElementList::Head(void)
{
	return(head);
}

/***************************************************************************/
// Get the head and remove it from the list
/***************************************************************************/
inline ClusterElement *ClusterElementList::PopHead(void)
{
	ClusterElement *ce = Head();
	Remove(Head());
	return(ce);
}

/***************************************************************************/
// Get at the tail
/***************************************************************************/
inline ClusterElement *ClusterElementList::Tail(void)
{
	return(tail);
}

/***************************************************************************/
// Does the list have a single element?
/***************************************************************************/
inline bool ClusterElementList::OnlyOneElement(void)
{
	return(head != NULL && head == tail);
}

/***************************************************************************/
// Get the next element
/***************************************************************************/
inline ClusterElement *ClusterElementList::GetNextElement(
ClusterElement *currentElement)
{
	if (currentElement)
		return(currentElement->processingNext);
	return(head);
}

/***************************************************************************/
// Get the previous element
/***************************************************************************/
inline ClusterElement *ClusterElementList::GetPreviousElement(
ClusterElement *currentElement)
{
	if (currentElement)
		return(currentElement->processingPrev);
	return(tail);
}
/***************************************************************************/
// Is the list empty?
/***************************************************************************/
inline int ClusterElementList::IsEmpty(void)
{
	return(head == NULL);
}

