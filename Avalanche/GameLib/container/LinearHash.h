/*
**
**  File:   LinearHash.h
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
**      $File: //depot/Avalanche/GameLib/container/LinearHash.h $
**  $Revision: #4 $
**  $DateTime: 2005/03/29 17:16:52 $
**    $Author: Rob $
**
*/

#pragma once

#ifndef LINEAR_HASH_H
#define LINEAR_HASH_H

#include "platform/BaseType.h"

inline uint32 ComputeLinearHash(const void* i_key, int i_keyLen);
inline uint32 ComputeLinearHash(const char* i_key);

template<class VALUE>
class CLinearHash
{
  public:
	// Construction/destruction
	inline CLinearHash(uint32 i_segmentSizeExp, uint32 i_maxSegments,
			uint32 i_removeThreshold = MAXUINT);
	inline ~CLinearHash();

	// Add a value. Only fails if the table is full. Duplicate entries are not
	// checked for. Duplicates wiil result in inaccessable values. If set, the
	// test key should point to a location that contains the source key value.
	// It is used as a final verification when getting an entry just in case two
	// keys hash to the same value (not the same bucket).
	inline VALUE* Add(const void* i_key, int i_keyLen, VALUE i_value,
			void* i_testKey = NULL);

	// Remove an entry. If the value is a pointer to some dynamic memory, it is
	// the user's responsibility to free it.
	inline bool Remove(const void* i_key, int i_keyLen);

	// Get a pointer to a value. NULL indicates that no value was found.
	inline VALUE* Get(const void* i_key, int i_keyLen);

	// Get a pointer to a value using a hash value. NULL indicates that no
	// value was found.
	inline VALUE* Get(uint32 i_hash);

	// Set the value.
	inline bool Set(const void* i_key, int i_keyLen, VALUE i_value,
			void* i_testKey = NULL);

	// Versions that use a string as a key just for convienience.
	inline VALUE* Add(const char* i_key, VALUE i_value, void* i_testKey = NULL);
	inline bool Remove(const char* i_key);
	inline VALUE* Get(const char* i_key);
	inline bool Set(const char* i_key, VALUE i_value, void* i_testKey = NULL);

  private:
	// No default constructor.
	CLinearHash();

	// Compute the current index given a hash value.
	inline uint32 Index(uint32 i_hash);

	// Core data structure. Used to store a value, related key info and the
	// next entry in a list.
	struct SEntry
	{
		SEntry* d_next;
		uint32 d_hash;
		void* d_testKey;
		int d_keyLen;
		VALUE d_value;
	};

	// Defining values set at construction.
	uint32 d_maxSegments;
	uint32 d_segmentShift;
	uint32 d_segmentMask;
	uint32 d_removeThreshold;

	// The segment arrays allocated at construction.
	SEntry*** d_bucketSegment;
	SEntry** d_entrySegment;

	// Current table sizes.
	uint32 d_numSegments;
	uint32 d_numEntries;

	// Values controlling bucket index computation, splitting and joining.
	uint32 d_splitControl;  // Used to add the first entry w/o special branches.
	uint32 d_splitIndex;
	uint32 d_frontMask;

	// The pool of available entries.
	SEntry* d_entryPool;

  // The iterator needs to be public but it also needs to know about SEntry.
  public:
	// Nested iterator class
	class CIterator
	{
	  public:
		// Construction
		inline CIterator() {d_table = NULL; d_curr = NULL;}

		//  the first entry
		inline void Init(CLinearHash<VALUE>* i_table);

		// Has the end been reached
		inline bool End(void);

		// Increment
		inline void operator++();

		// Get at the value.
		inline VALUE* Get(void);

		// Set the value.
		inline bool Set(VALUE i_value, void* i_testKey = NULL);

	  private:
		// Iteraration state.
		CLinearHash<VALUE>* d_table;
		SEntry* d_curr;

		friend class CLinearHash<VALUE>;
	};

	friend class CIterator;
};

// Implementation
#define LINEAR_HASH_HPP
#include "container/LinearHash.hpp"

#endif // LINEAR_HASH_H
