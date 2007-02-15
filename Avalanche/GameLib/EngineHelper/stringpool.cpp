/***************************************************************************/
// A fixed size string pool
/***************************************************************************/
#include "EngineHelper/EngineHelperPCH.h"
#include "EngineHelper/stringpool.h"

/***************************************************************************/
StringPool::StringPool(
int startStrings,
int stringsGrowBy,
int startHeapSize,
int heapGrowBy) :
m_string(startStrings, stringsGrowBy)
{
	m_heapGrowBy = heapGrowBy;
	m_heap = NULL;
	GrowHeap(startHeapSize);
}

/***************************************************************************/
StringPool::~StringPool()
{
	while (m_heap)
	{
		HeapEntry *next = m_heap->next;
		memFree(m_heap);
		m_heap = next;
	}
}

/***************************************************************************/
// Find a string returns NULL if not there
/***************************************************************************/
const char *StringPool::Find(
const char *lookFor)
{
	int count = m_string.Count();
	for (int s = 0; s < count; s++)
	{
		const char *string = m_string[s].string;
		if (strcmp(string, lookFor) == 0)
			return string;
	}

	return NULL;
}

/***************************************************************************/
// Find a string returns NULL if not there
/***************************************************************************/
const char *StringPool::FindI(
const char *lookFor)
{
	int count = m_string.Count();
	for (int s = 0; s < count; s++)
	{
		const char *string = m_string[s].string;
		if (strcmpi(string, lookFor) == 0)
			return string;
	}

	return NULL;
}

/***************************************************************************/
// Add a string (if it doesn't exist)
/***************************************************************************/
const char *StringPool::Add(
const char *add,
AddType type)
{
	// Already here?
	const char *found = (type == SPADD_IGNORECASE) ? FindI(add) : Find(add);
	if (found)
		return found;

	// What are we adding?
	int size = strlen(add) + 1;

	// Find an open slot
	HeapEntry *entry = FindSpace(size);
	if (!entry)
	{
		int growBy = size > m_heapGrowBy ? size : m_heapGrowBy;
		entry = GrowHeap(growBy);
	}
	if (!entry)
		return NULL;

	// Will it fit?
	int freeSpace = entry->size - entry->used;
	ASSERT(size <= freeSpace);

	// Place it in
	StringEntry stringEntry;
	stringEntry.string = HeapData(entry) + entry->used;
	strncpy(stringEntry.string, add, size);
	if (type == SPADD_FORCELOWER)
		strlwr(stringEntry.string);
	entry->used += size;

	// Add the string
	bool ret = m_string.Add(stringEntry);
	if (!ret)
		return NULL;

	// copy it in
	return stringEntry.string;
}

/***************************************************************************/
/***************************************************************************/
void StringPool::Clear()
{
	// done with this
	m_string.Clear();

	// Free down to the first block
	while (m_heap && m_heap->next)
	{
		HeapEntry *next = m_heap->next;
		memFree(m_heap);
		m_heap = next;
	}
}

/***************************************************************************/
// Show stats
/***************************************************************************/
void StringPool::DebugDump(
const char *name)
{
	int totalHeap = 0;
	int heapUsed = 0;
	int heapEntries = 0;

	HeapEntry *entry = m_heap;
	while (entry)
	{
		totalHeap += entry->size;
		heapUsed += entry->used;
		++heapEntries;

		entry = entry->next;
	}

	// Count up stuff
	TRACE("String Pool %s has %d strings, %d total heap (%d used, %d unused) in %d heap chunks.\n",
				name,
				m_string.Count(),
				totalHeap,
				heapUsed,
				totalHeap - heapUsed,
				heapEntries);

#ifdef _DEBUG
	int count = m_string.Count();
	for (int s = 0; s < count; s++)
	{
		TRACE("\tString %d: %s\n", s, m_string[s].string);
	}
#endif
}

/***************************************************************************/
/***************************************************************************/
StringPool::HeapEntry *StringPool::GrowHeap(
int size)
{
	// Anything to do?
	if (size <= 0)
		return NULL;

	// Make the heap
	HeapEntry *newHeap = (HeapEntry *)memAlloc(sizeof(HeapEntry) + size);
	ASSERT(newHeap);
	if (!newHeap)
		return NULL;

	// Fill out the info
	newHeap->used = 0;
	newHeap->size = size;

	// Put it in the list
	newHeap->next = m_heap;
	m_heap = newHeap;

	return newHeap;
}

/***************************************************************************/
/***************************************************************************/
StringPool::HeapEntry *StringPool::FindSpace(int size)
{
	HeapEntry *entry = m_heap;
	while (entry)
	{
		int freeSpace = entry->size - entry->used;
		if (size <= freeSpace)
			return entry;

		entry = entry->next;
	}

	// Can't find anything
	return NULL;
}








