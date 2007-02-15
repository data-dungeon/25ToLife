/***************************************************************************/
// A fixed size string pool
/***************************************************************************/
#ifndef STRINGPOOL_H
#define STRINGPOOL_H

#include "container/darray.h"

class StringPool
{
public:
	StringPool(int startStrings, int stringsGrowBy, int startHeapSize, int heapGrowBy);
	~StringPool();

	// Find a string returns NULL if not there
	const char *Find(const char *lookFor);

	// Find a string (case insensitive) returns NULL if not there
	const char *FindI(const char *lookFor);

	// Add a string (if it doesn't exist)
	enum AddType
	{
		SPADD_NORMAL,
		SPADD_IGNORECASE,
		SPADD_FORCELOWER
	};
	const char *Add(const char *add, AddType type = SPADD_NORMAL);

	// Clear everything
	void Clear();

	// Show stats
	void DebugDump(const char *name);

private:
	struct HeapEntry
	{
		int			used;
 		int		  	size;
		HeapEntry	*next;
	};
	HeapEntry				*m_heap;
	int						m_heapGrowBy;
	struct StringEntry
	{
		char			*string;
	};
	DynamicArray<StringEntry>	m_string;

	HeapEntry *GrowHeap(int size);
	HeapEntry *FindSpace(int size);
	char *HeapData(HeapEntry *entry)
		{ ASSERT(entry); return (char *)(entry + 1); }
};

#endif
