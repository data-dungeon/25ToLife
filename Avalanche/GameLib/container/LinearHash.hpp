/*
**
**  File:   LinearHash.hpp
**  Date:   February 20, 2004
**  By:     Bryant Collard
**  Desc:   An implementation of a linear hash table. A linear hash table is
**          a table of buckets where each bucket contains a chain of entries.
**          It is an expanding table with, in this case, the same number of
**          buckets as entries so, on average, there is one entry per bucket.
**          When an entry is added, a bucket is added to the end of the table
**          and some existing entries may be put into it. This process is
**          called splitting.
**
**          The table is divided into two sections, a front partition and an
**          expansion partition. The size of the front partition is always a
**          power of 2 and is the largest it can be for the size of the table,
**          for example, if the table has 7 entries, the front partition's size
**          is 4 and the expansion partition's size is 3.
**
**          A split index is maintained that is the size if the expansion
**          partition. To find the bucket index for an entry given a 32 bit
**          hash value, hash value mod front partition size is computed (since
**          the front partition size is a power of 2, this is just a mask of
**          some number of bottom bits). If the result is greater than or equal
**          to the split index, the result is used as the bucket index;
**          otherwise, hash mod twice the front partition size (again, just a
**          mask) is used. As a result, the index will be below the split index
**          or in the expansion partition.
**
**          When an entry is added, the entries in the bucket pointed at by the
**          split index are split between it and the new bucket at the end of
**          the expansion partition. Effectively what happens is that the
**          expansion partition grows by one so the spilt index is incremented.
**          Since the index of the bucket that was at the split index is now
**          below it, the entries in it have to be rehashed. Rehashing is simple
**          because each entries new bucket is determined by the next bit in
**          its hash value. When the size of the expansion partition is equal
**          to the front partition, the front partition is doubled to include
**          the expansion partition, the expansion partition size becomes 0 and
**          the split index is reset to 0. Removing an entry reverses the
**          process.
**
**          This implementation arranges memory in segments with the size of
**          each segment being a power of 2. The size of the segments and the
**          maximum number of segments are specified at construction time. Two
**          arrays of pointers are allocated at construction whose lengths are
**          the maximum number of segments. New segments are then allocated as
**          needed. So, even though this is an expanding table, its maximum size
**          must be specified; however, an absurdly huge table can be specified
**          with very little impact on memory.
**
**          A core structure is the entry. An entry contains the hash value,
**          information about the key the actual value stored in the table and
**          a pointer to another entry so buckets can be arranged in a linked
**          list. There are two basic arrays (arranged in segments). One is the
**          bucket array and each member points to the first entry in its
**          associated chain (if any). The other is an array of entries. When
**          allocated, these entries are added to a pool and when a value is
**          added to the table, an entry is removed from the pool, populated and
**          added to the appropriate bucket. The fact that the entry array is
**          an array is irrelevant. It is allocated this way because it takes
**          advantage of the fact that the number of buckets in the table is
**          always equal to the number of entries in it. So, there will always
**          be exactly enough entries available for values the table can hold.
**
**          As entries are removed, segments in the bucket array will become
**          unused. A threshold can be set at construction that controls when
**          memory is freed. It is freed when the difference between start of
**          last segment and the number of entries in the table is equal to the
**          threshold. If the threshold is 0, the memory is freed as soon as
**          possible. If it is larger than the maximum table size minus the
**          segment size, memory will never be freed (this is the default). The
**          threshold provides some hysteresis to avoid thrashing memory but
**          provides a way for memory to dynamically shrink as well as grow.
**          Note that the last bucket segment becomes unused as the number of
**          entries grows smaller but that the members of the entry array do not
**          become unused in an organized way. So, when a segment is freed, some
**          shuffling must occur to collect the entries in the last segment in
**          the free entry pool before that segment can be freed.
**
**  Copyright (c) 2004, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/container/LinearHash.hpp $
**  $Revision: #7 $
**  $DateTime: 2005/08/05 20:23:23 $
**    $Author: Bryant_Collard $
**
*/

#ifdef LINEAR_HASH_HPP
#undef LINEAR_HASH_HPP

// Macros
#define SEGMENT(i) ((i) >> d_segmentShift)
#define OFFSET(i) ((i) & d_segmentMask)

/***************************************************************************/
// Compute a hash value given a key.
/***************************************************************************/
#define MIX(a, b, c) \
{ \
	a = a - b;		a = a - c;		a = a ^ (c >> 13); \
	b = b - c;		b = b - a;		b = b ^ (a << 8);  \
	c = c - a;		c = c - b;		c = c ^ (b >> 13); \
	a = a - b;		a = a - c;		a = a ^ (c >> 12); \
	b = b - c;		b = b - a;		b = b ^ (a << 16); \
	c = c - a;		c = c - b;		c = c ^ (b >> 5);  \
	a = a - b;		a = a - c;		a = a ^ (c >> 3);  \
	b = b - c;		b = b - a;		b = b ^ (a << 10); \
	c = c - a;		c = c - b;		c = c ^ (b >> 15); \
}

inline uint32 ComputeLinearHash(const void* i_key, int i_keyLen)
{
	register const unsigned char* k = (const unsigned char*)i_key;
	register uint32 a, b, c;  /* the internal state */
	uint32 len;               /* how many key bytes still need mixing */

	/* Set up the internal state */
	len = i_keyLen;
	a = b = 0x9e3779b9; /* the golden ratio; an arbitrary value */
	c = 0;              /* always initialize this to 0 */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a = a + (k[0] + ((uint32)k[1] << 8) + ((uint32)k[2] << 16) +
				((uint32)k[3] << 24));
		b = b + (k[4] + ((uint32)k[5] << 8) + ((uint32)k[6] << 16) +
				((uint32)k[7] << 24));
		c = c + (k[8] + ((uint32)k[9] << 8) + ((uint32)k[10] << 16) +
				((uint32)k[11] << 24));
		MIX(a, b, c);
		k = k + 12;
		len = len - 12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c = c + i_keyLen;

	/* all the case statements fall through */
	switch(len)
	{
	 case 11:
		c = c + ((uint32)k[10] << 24);
	 case 10:
		c = c + ((uint32)k[9] << 16);
	 case 9:
		c = c + ((uint32)k[8] << 8);

	 /* the first byte of c is reserved for the length */
	 case 8:
		b = b + ((uint32)k[7] << 24);
	 case 7:
		b = b + ((uint32)k[6] << 16);
	 case 6:
		b = b + ((uint32)k[5] << 8);
	 case 5:
		b = b + k[4];

	 case 4:
		a = a + ((uint32)k[3] << 24);
	 case 3:
		a = a + ((uint32)k[2] << 16);
	 case 2:
		a = a + ((uint32)k[1] << 8);
	 case 1:
		 a = a + k[0];
	}
	MIX(a, b, c);

	/*-------------------------------------------- report the result */
	return c;
}

inline uint32 ComputeLinearHash(const char* i_key)
{
	return(ComputeLinearHash(i_key, strlen(i_key)));
}

/***************************************************************************/
// Constructor
/***************************************************************************/
template<class VALUE>
inline CLinearHash<VALUE>::CLinearHash(uint32 i_segmentSizeExp,
		uint32 i_maxSegments, uint32 i_removeThreshold)
{
/*
 *	Make sure arguments make sense.
 */
	ASSERTS((i_segmentSizeExp > 0) && (i_maxSegments > 0),
			"Zero size hash table specified\n");
/*
 *	Setup the segment size.
 */
	d_segmentShift = i_segmentSizeExp;
	d_segmentMask = 0x0;
	for (uint32 i = 0; i < d_segmentShift; i++)
		d_segmentMask |= 1 << i;
/*
 *	Make sure the maximum segments makes sense.
 */
	if (i_maxSegments > (~d_segmentMask >> d_segmentShift))
	{
		ASSERTS(false, "Requesting hash table larger than addressable memory\n");
		d_maxSegments = (~d_segmentMask >> d_segmentShift);
	}
	else
		d_maxSegments = i_maxSegments;
/*
 *	Save the remove threshold.
 */
	d_removeThreshold = i_removeThreshold;
/*
 *	Initialize segment array pointers.
 */
	d_bucketSegment = NULL;
	d_entrySegment = NULL;
/*
 *	Initialize table sizes.
 */
	d_numSegments = 0;
	d_numEntries = 0;
/*
 *	Initialize split control.
 */
	d_splitControl = (uint32)(-1);
	d_splitIndex = 0;
	d_frontMask = 0x0;
/*
 *	Initialize the entry pool as empty.
 */
	d_entryPool = NULL;
}

/***************************************************************************/
// Destructor
/***************************************************************************/
template<class VALUE>
inline CLinearHash<VALUE>::~CLinearHash()
{
	if (d_numSegments != 0)
	{
		for (uint32 i = 0; i < d_maxSegments; i++)
		{
			delete[] d_bucketSegment[i];
			d_bucketSegment[i] = NULL;
			delete[] d_entrySegment[i];
			d_entrySegment[i] = NULL;
		}
		delete[] d_bucketSegment;
		d_bucketSegment = NULL;
		delete[] d_entrySegment;
		d_entrySegment = NULL;
	}
}

/***************************************************************************/
// Compute the current index given a hash value.
/***************************************************************************/
template<class VALUE>
inline uint32 CLinearHash<VALUE>::Index(uint32 i_hash)
{
	uint32 index = i_hash & d_frontMask;
	if (index >= d_splitIndex)
		return(index);
	return(i_hash & ((d_frontMask << 1) | 0x1));
}

/***************************************************************************/
// Add a value. Only fails if the table is full. Duplicate entries are not
// checked for. Duplicates wiil result in inaccessable values. If set, the
// test key should point to a location that contains the source key value.
// It is used as a final verification when getting an entry just in case two
// keys hash to the same value (not the same bucket).
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::Add(const void* i_key, int i_keyLen,
		VALUE i_value, void* i_testKey)
{
/*
 *	Allocate the segment arrays if necessary.
 */
	if (d_numSegments == 0)
	{
		d_bucketSegment = new SEntry**[d_maxSegments];
		d_entrySegment = new SEntry*[d_maxSegments];
		for (uint32 i = 0; i < d_maxSegments; i++)
		{
			d_bucketSegment[i] = NULL;
			d_entrySegment[i] = NULL;
		}
	}
/*
 *	Allocate a new segment if necessary.
 */
	if ((SEGMENT(d_numEntries) + 1) > d_numSegments)
	{
		if (d_numSegments >= d_maxSegments)
		{
			ASSERTS(false, "Overran hash table.\n");
			return(NULL);
		}
		d_bucketSegment[d_numSegments] = new SEntry*[d_segmentMask + 1];
		d_entrySegment[d_numSegments] = new SEntry[d_segmentMask + 1];

		ASSERT(d_entryPool == NULL);
		d_entryPool = &d_entrySegment[d_numSegments][0];
		for (uint32 i = 0; i < d_segmentMask; i++)
		{
			d_entrySegment[d_numSegments][i].d_next =
					&d_entrySegment[d_numSegments][i + 1];
		}
		d_entrySegment[d_numSegments][d_segmentMask].d_next = NULL;

		d_numSegments++;
	}
/*
 *	Perform the split.
 *
 *	Get the expansion bucket and initialize it.
 */
	SEntry* &expansion =
			d_bucketSegment[SEGMENT(d_numEntries)][OFFSET(d_numEntries)];
	expansion = NULL;
/*
 *	Get the split bucket, save a pointer to its entry list and then empty it
 *	so it is ready to receive the sub-bucket.
 */
	SEntry* &split =
			d_bucketSegment[SEGMENT(d_splitIndex)][OFFSET(d_splitIndex)];
	SEntry* entry = split;
	split = NULL;
/*
 *	Split the entries, expanding the table by one bucket. Note that on the first
 *	entry there really isn't a split bucket and that what is used as the split
 *	bucket is really the expansion bucket; however, the expansion bucket is
 *	initialized before the split bucket's entry list is saved so the entry list
 *	to split will always appear empty and there will not be a split attempt.
 */
	while(entry)
	{
		SEntry* curr = entry;
		entry = entry->d_next;
		if ((curr->d_hash & (d_frontMask + 1)) == 0)
		{
			curr->d_next = split;
			split = curr;
		}
		else
		{
			curr->d_next = expansion;
			expansion = curr;
		}
	}
	d_numEntries++;
/*
 *	Increment the split index and reset the front partition if appropriate.
 */
	d_splitControl++;
	if (d_splitControl > d_frontMask)
	{
		d_splitControl = 0;
		d_frontMask = ((d_frontMask << 1) | 0x1);
	}
	d_splitIndex = d_splitControl;
/*
 *	Get an entry from the pool.
 */
	entry = d_entryPool;
	ASSERT_PTR(entry);
	d_entryPool = entry->d_next;
/*
 *	Fill in the entry.
 */
	entry->d_hash = ComputeLinearHash(i_key, i_keyLen);
	entry->d_testKey = i_testKey;
	entry->d_keyLen = i_keyLen;
	entry->d_value = i_value;
/*
 *	Add the entry.
 */
	uint32 index = Index(entry->d_hash);
	SEntry* &bucket = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
	entry->d_next = bucket;
	bucket = entry;
/*
 *	Check for duplicate hashes.
 */
#ifdef DEBUG
	uint32 hash = entry->d_hash;
	while ((entry = entry->d_next) != NULL)
	{
		ASSERT(hash != entry->d_hash);
	}
#endif

	return(&entry->d_value);
}

/***************************************************************************/
// Remove an entry. If the value is a pointer to some dynamic memory, it is
// the user's responsibility to free it.
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::Remove(const void* i_key, int i_keyLen)
{
/*
 *	Can only remove if an entry exists. (Index() will give bogus value unless
 *	this is checked.)
 */
	if (d_numEntries == 0)
		return(false);
/*
 *	Find the bucket.
 */
	uint32 hash = ComputeLinearHash(i_key, i_keyLen);
	uint32 index = Index(hash);
	SEntry* &bucket = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
/*
 *	Search the bucket for a matching value.
 */
	SEntry* entry = bucket;
	while(entry != NULL)
	{
		if ((hash == entry->d_hash) && (i_keyLen == entry->d_keyLen) &&
				((entry->d_testKey == NULL) ||
				!memcmp(i_key, entry->d_testKey, i_keyLen)))
		{
			break;
		}
		entry = entry->d_next;
	}
/*
 *	Return if no match was found.
 */
	if (entry == NULL)
		return(false);
/*
 *	Remove the entry from the bucket.
 */
	if (bucket == entry)
		bucket = entry->d_next;
	else
	{
		SEntry* curr = bucket;
		while(curr->d_next != entry)
		{
			curr = curr->d_next;
			ASSERT_PTR(curr);
		}
		curr->d_next = entry->d_next;
	}
/*
 *	Put the entry back in the pool.
 */
	entry->d_next = d_entryPool;
	d_entryPool = entry;
/*
 *	Adjust the split index and front partition.
 */
	if (d_splitControl == 0)
	{
		d_frontMask >>= 1;
		d_splitControl = d_frontMask;
	}
	else
		d_splitControl--;
	d_splitIndex = d_splitControl;
/*
 *	Decrement the number of entries. If the last entry is being removed, set up
 *	for adding a new first entry; otherwise, combine the tail entry.
 */
	d_numEntries--;
	if (d_numEntries == 0)
		d_splitControl = (uint32)(-1);
	else
	{
		SEntry* &combine =
				d_bucketSegment[SEGMENT(d_splitIndex)][OFFSET(d_splitIndex)];
		if (combine == NULL)
			combine = d_bucketSegment[SEGMENT(d_numEntries)][OFFSET(d_numEntries)];
		else
		{
			entry = combine;
			while(entry->d_next != NULL)
				entry = entry->d_next;
			entry->d_next =
					d_bucketSegment[SEGMENT(d_numEntries)][OFFSET(d_numEntries)];
		}
	}
/*
 *	Find the number of entries that would be available for expansion if the
 *	memory for the tail segment were freed. If it is greater than the threshold,
 *	free the memory.
 */
	uint32 tailSegment = d_numSegments - 1;
	int32 bucketBuffer = (tailSegment << d_segmentShift) - d_numEntries;
	if ((bucketBuffer >= 0) && ((uint32)bucketBuffer >= d_removeThreshold))
	{
/*
 *		Find the address range of the tail entry segment.
 */
		SEntry* start = &d_entrySegment[tailSegment][0];
		SEntry* end = &d_entrySegment[tailSegment][d_segmentMask];
/*
 *		Remove the entries in the pool that are in the tail segment and mark them
 *		as unused.
 */
		entry = d_entryPool;
		d_entryPool = NULL;
		while(entry != NULL)
		{
			SEntry* curr = entry;
			entry = entry->d_next;
			if ((curr >= start) && (curr <= end))
				curr->d_next = (SEntry*)~0x0;
			else
			{
				curr->d_next = d_entryPool;
				d_entryPool = curr;
			}
		}
/*
 *		Go through the entries in the tail segment and find the ones that are
 *		being used. Replace them with entries from the pool.
 */
		for (uint32 i = 0; i <= d_segmentMask; i++)
		{
			if ((int)d_entrySegment[tailSegment][i].d_next != ~0x0)
			{
				SEntry* retiring = &d_entrySegment[tailSegment][i];
				uint32 index = Index(retiring->d_hash);
				SEntry* &bucket = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
				ASSERT_PTR(bucket);
				entry = d_entryPool;
				ASSERT_PTR(entry);
				d_entryPool = entry->d_next;
				*entry = *retiring;
				if (bucket == retiring)
					bucket = entry;
				else
				{
					SEntry* curr = bucket;
					while(curr->d_next != retiring)
					{
						curr = curr->d_next;
						ASSERT_PTR(curr);
					}
					curr->d_next = entry;
				}
			}
		}
/*
 *		Delete the tail segments and set the pointers and counters.
 */
		delete[] d_bucketSegment[tailSegment];
		delete[] d_entrySegment[tailSegment];
		d_bucketSegment[tailSegment] = NULL;
		d_entrySegment[tailSegment] = NULL;
		d_numSegments = tailSegment;
	}
/*
 *	Free the segment arrays if no longer needed.
 */
	if (d_numSegments == 0)
	{
		delete[] d_bucketSegment;
		d_bucketSegment = NULL;
		delete[] d_entrySegment;
		d_entrySegment = NULL;
	}

	return(true);
}

/***************************************************************************/
// Get a pointer to a value. NULL indicates that no value was found.
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::Get(const void* i_key, int i_keyLen)
{
	if (d_numEntries == 0)
		return(NULL);

	uint32 hash = ComputeLinearHash(i_key, i_keyLen);
	uint32 index = Index(hash);

	SEntry* entry = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
	while(entry != NULL)
	{
		if ((hash == entry->d_hash) && (i_keyLen == entry->d_keyLen) &&
				((entry->d_testKey == NULL) ||
				!memcmp(i_key, entry->d_testKey, i_keyLen)))
		{
			return(&entry->d_value);
		}
		entry = entry->d_next;
	}
	return(NULL);
}

/***************************************************************************/
// Get a pointer to a value using a hash value. NULL indicates that no
// value was found.
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::Get(uint32 i_hash)
{
	if (d_numEntries == 0)
		return(NULL);

	uint32 index = Index(i_hash);

	SEntry* entry = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
	while(entry != NULL)
	{
		if (i_hash == entry->d_hash)
			return(&entry->d_value);
		entry = entry->d_next;
	}
	return(NULL);
}

/***************************************************************************/
// Set the value.
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::Set(const void* i_key, int i_keyLen,
		VALUE i_value, void* i_testKey)
{
	if (d_numEntries == 0)
		return(false);

	uint32 hash = ComputeLinearHash(i_key, i_keyLen);
	uint32 index = Index(hash);

	SEntry* entry = d_bucketSegment[SEGMENT(index)][OFFSET(index)];
	while(entry != NULL)
	{
		if ((hash == entry->d_hash) && (i_keyLen == entry->d_keyLen) &&
				((entry->d_testKey == NULL) ||
				!memcmp(i_key, entry->d_testKey, i_keyLen)))
		{
			entry->d_testKey = i_testKey;
			entry->d_value = i_value;
			return(true);
		}
		entry = entry->d_next;
	}
	return(false);
}

/***************************************************************************/
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::Add(const char* i_key, VALUE i_value,
		void* i_testKey)
{
	return(Add(i_key, strlen(i_key), i_value, i_testKey));
}

/***************************************************************************/
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::Remove(const char* i_key)
{
	return(Remove(i_key, strlen(i_key)));
}

/***************************************************************************/
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::Get(const char* i_key)
{
	return(Get(i_key, strlen(i_key)));
}

/***************************************************************************/
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::Set(const char* i_key, VALUE i_value,
		void* i_testKey)
{
	return(Set(i_key, strlen(i_key), i_value, i_testKey));
}

/***************************************************************************/
// Find the first entry
/***************************************************************************/
template<class VALUE>
inline void CLinearHash<VALUE>::CIterator::Init(CLinearHash<VALUE>* i_table)
{
/*
 *	Initialize and see if a valid table was passed in.
 */
	d_table = i_table;
	d_curr = NULL;

	if (i_table == NULL)
		return;
/*
 *	Following code assumes d_numEntries > 0 so check it here.
 */
	if (d_table->d_numEntries == 0)
		return;
/*
 *	Look through all of the full segments.
 */
	uint32 lastSegment = (d_table->d_numEntries - 1) >> d_table->d_segmentShift;
	for (uint32 segment = 0; segment < lastSegment; segment++)
	{
		for (uint32 offset = 0; offset <= d_table->d_segmentMask; offset++)
		{
			if (d_table->d_bucketSegment[segment][offset] != NULL)
			{
				d_curr = d_table->d_bucketSegment[segment][offset];
				return;
			}
		}
	}
/*
 *	Look through the last, partial segment.
 */
	uint32 maxOffset = (d_table->d_numEntries - 1) & d_table->d_segmentMask;
	for (uint32 offset = 0; offset <= maxOffset; offset++)
	{
		if (d_table->d_bucketSegment[lastSegment][offset] != NULL)
		{
			d_curr = d_table->d_bucketSegment[lastSegment][offset];
			return;
		}
	}
}

/***************************************************************************/
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::CIterator::End(void)
{
	return(d_curr == NULL);
}

/***************************************************************************/
// Increment
/***************************************************************************/
template<class VALUE>
inline void CLinearHash<VALUE>::CIterator::operator++(void)
{
/*
 *	Can't increment if at the end.
 */
	if (d_curr == NULL)
		return;
/*
 *	See if there is another entry in the current bucket.
 */
	if (d_curr->d_next != NULL)
	{
		d_curr = d_curr->d_next;
		return;
	}
/*
 *	Compute some indexes.
 */
	ASSERT_PTR(d_table);
	uint32 lastSegment = (d_table->d_numEntries - 1) >> d_table->d_segmentShift;
	uint32 index = d_table->Index(d_curr->d_hash);
	uint32 segment = (index >> d_table->d_segmentShift);
	uint32 offset = (index & d_table->d_segmentMask);
/*
 *	Find the last offset of the current segment.
 */
	uint32 lastOffset;
	if (segment < lastSegment)
		lastOffset = d_table->d_segmentMask;
	else
		lastOffset = (d_table->d_numEntries - 1) & d_table->d_segmentMask;
/*
 *	Search from the next bucket to the last offset of the current segment.
 */
	for (offset += 1; offset <= lastOffset; offset++)
	{
		if (d_table->d_bucketSegment[segment][offset] != NULL)
		{
			d_curr = d_table->d_bucketSegment[segment][offset];
			return;
		}
	}
/*
 *	Search the full segments. Note that if we started in or just before the last
 *	segment, this loop will not be entered.
 */
	for (segment += 1; segment < lastSegment; segment++)
	{
		for (offset = 0; offset <= d_table->d_segmentMask; offset++)
		{
			if (d_table->d_bucketSegment[segment][offset] != NULL)
			{
				d_curr = d_table->d_bucketSegment[segment][offset];
				return;
			}
		}
	}
/*
 *	Search the last partial segment. Note that if we started in the last segment,
 *	this block will not be entered.
 */
	if (segment == lastSegment)
	{
		lastOffset = (d_table->d_numEntries - 1) & d_table->d_segmentMask;
		for (offset = 0; offset <= lastOffset; offset++)
		{
			if (d_table->d_bucketSegment[segment][offset] != NULL)
			{
				d_curr = d_table->d_bucketSegment[segment][offset];
				return;
			}
		}
	}
/*
 *	The end has been reached.
 */
	d_curr = NULL;
	return;
}

/***************************************************************************/
// Get at the value
/***************************************************************************/
template<class VALUE>
inline VALUE* CLinearHash<VALUE>::CIterator::Get(void)
{
	if (d_curr != NULL)
		return(&d_curr->d_value);
	return(NULL);
}

/***************************************************************************/
// Set the value
/***************************************************************************/
template<class VALUE>
inline bool CLinearHash<VALUE>::CIterator::Set(VALUE i_value, void* i_testKey)
{
	if (d_curr == NULL)
		return(false);

	d_curr->d_testKey = i_testKey;
	d_curr->d_value = i_value;

	return(true);
}

#endif // LINEAR_HASH_HPP
