/****************************************************************************/
/****************************************************************************/

/****************************************************************************/
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
HashList<T, powerOf2Size>::HashList(void) :
list(1 << powerOf2Size, 128, false),
pool(1 << powerOf2Size, 12+8)
{
	// clear the table
	unsigned long size = 1 << powerOf2Size;
	for (unsigned long i = 0; i < size; i++)
		hashTable[i] = NULL;

	// figure out the mask
	hashMask = size - 1;
}

/****************************************************************************/
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
HashList<T, powerOf2Size>::~HashList(void)
{
	// Hehe, the pools will clean up the bunk
}

/****************************************************************************/
// Add an item, returning it's index in the flat list
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
inline unsigned long HashList<T, powerOf2Size>::Add(T &t)
{
	// already in the list
	unsigned long index;
	if (Find(t, index))
		return(index);

	// Hash it
	unsigned long hash = (HashKey(t) & hashMask);

	// allocate a new element
	Element *e = NewElement();
	ASSERT(e);

	// set the value
	e->index = list.Count();
	list.Add(t, e->index);

	// insert it into the table
	Element *next = hashTable[hash];
	hashTable[hash] = e;
	e->next = next;
	return(e->index);
}

/****************************************************************************/
// Find an item
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
inline bool HashList<T, powerOf2Size>::Find(
T &t,
unsigned long &index)
{
	// Hash it
	unsigned long hash = (HashKey(t) & hashMask);

	// find it
	Element *e = hashTable[hash];
	while (e)
	{
		if (list[e->index] == t)
		{
			index = e->index;
			return(true);
		}
		e = e->next;
	}

	// No luck
	return(false);
}

/****************************************************************************/
// Load 0-100%.  The closer this is to 100, the
// better your key function is
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
inline unsigned long HashList<T, powerOf2Size>::Load(void)
{
	unsigned long used = 0;

	unsigned long size = 1 << powerOf2Size;
	for (unsigned long i = 0; i < size; i++)
		if (hashTable[i] != NULL)
			++used;

	return((used * 100) / size);
}

/****************************************************************************/
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
inline typename HashList<T, powerOf2Size>::Element *HashList<T, powerOf2Size>::NewElement(void)
{
	Element *e = pool.New();
	return(e);
}

/****************************************************************************/
/****************************************************************************/
template <class T, unsigned long powerOf2Size>
inline void HashList<T, powerOf2Size>::DeleteElement(
Element *e)
{
	if (e)
		pool.Delete(e);
}

