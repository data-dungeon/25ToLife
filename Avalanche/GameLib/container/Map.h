/*
**
**  File:   Map.h
**  Date:   June 6, 2005
**  By:     Bryant Collard
**  Desc:   A container similar to the STL map class. The container holds
**          a dynamically sized array of objects sorted by and accessible with
**          a key. The type of the key must have the < and the = operators
**          defined.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**      $File: //depot/Avalanche/GameLib/container/Map.h $
**  $Revision: #2 $
**  $DateTime: 2005/06/24 20:40:40 $
**    $Author: Bryant_Collard $
**
*/

#pragma once

#ifndef MAP_H
#define MAP_H

// This class is fully implemented in this file and the associated Map.hpp file.
// It is mostly self-contained except for the use of a memory manager. Comment
// out the following #define if this class is used outside the avalanche engine.
#define AVALANCHE_ENGINE

template<class KEY, class VALUE>
class CMap
{
  public:
	// Iterator.
	class CIterator
	{
	  public:
		// Construct. Result will be invalid.
		CIterator();

		// Copy constructor.
		CIterator(const CIterator &i_iterator);

		// Assignment operator.
		CIterator &operator=(const CIterator &i_iterator);

		// Address the entry succeeding the current key. This works even if
		// the iterator is currently invalid.
		CIterator &operator++();

		// Address the entry preceeding the current key. This works even if
		// the iterator is currently invalid.
		CIterator &operator--();

		// Indicate if the iterator addresses an entry preceeding the first entry.
		// Will always return true if there are no entries.
		bool Start(void);

		// Indicate if the iterator addresses an entry succeeding the last entry.
		// Will always return true if there are no entries.
		bool End(void);

		// Indicate if the iterator addresses an entry preceeding the first entry
		// or succeeding the last entry.
		bool Out(void);

		// Indicate if the iterator is valid. An iterator will be invalid if
		// it hasn't been assigned or there is no longer an entry with the
		// associated key. Note that an iterator can be invalidated only when
		// an entry is added or removed.
		bool Valid(void) const;

		// Return a pointer to the key of the entry addressed. Returns NULL if
		// the iterator is invalid or if it addresses an entry preceeding the
		// first entry or succeeding the last entry.
		const KEY* Key(void) const;

		// Return a pointer to the value of the entry addressed. Returns NULL if
		// the iterator is invalid or if it addresses an entry preceeding the
		// first entry or succeeding the last entry.
		VALUE* Value(void) const;

	  private:
		const CMap* d_map;
		KEY d_key;
		unsigned int d_index;
		unsigned int d_transactionId;

		friend class CMap<KEY, VALUE>;
	};

	CMap(unsigned int i_growBy = 1, unsigned int i_shrinkThreshold = ~0);
	~CMap();

	// Return a reference to the value associated with the key. If there was no
	// entry for the key, a new entry is created.
	VALUE &operator[](const KEY &i_key);

	// Return a reference to the value associated with the key. If there was no
	// entry for the key, a new entry is created.
	VALUE &Get(const KEY &i_key, bool* o_newEntry = NULL);

	// Return a pointer to the value associated with the key. NULL is returned
	// if there is no entry for the key.
	VALUE* Find(const KEY &i_key);

	// Return a pointer to the value in the entry inserted that is associated
	// with the key. NULL is returned if an entry already exists.
	VALUE* Insert(const KEY &i_key);

	// Try to remove the entry associated with the key. Returns true if entry
	// was removed, false if there was no entry to remove.
	bool Remove(const KEY &i_key);

	// Return an iterator addressing the entry preceeding the first entry.
	CIterator Start(void) const;

	// Return an iterator addressing the first entry.
	CIterator First(void) const;

	// Return an iterator addressing the last entry.
	CIterator Last(void) const;

	// Return an iterator addressing the entry succeeding the last entry.
	CIterator End(void) const;

	// Return an iterator addressing the entry with the key or the closest
	// preceeding entry (may be before the first entry).
	CIterator GotoBack(const KEY &i_key) const;

	// Return an iterator addressing the entry with the key or the closest
	// succeeding entry (may be after the last entry).
	CIterator GotoForward(const KEY &i_key) const;

  private:
	// Nested types.
	struct SEntry
	{
		KEY d_key;
		VALUE d_value;
	};

	// Prevent copy construction.
	CMap(const CMap &i_map);

	// Add an entry at the specified index.
	void Add(const KEY &i_key, unsigned int i_index);

	// Perform a binary search to find an entry with the key. If an entry exists,
	// return true; otherwise, find what would be the succeeding one and return
	// false.
	bool Search(const KEY &i_key, unsigned int &o_index) const;

	// Perform a linear search starting from a given point to find an entry with
	// the key. If an entry exists, return true; otherwise, find what would be
	// the succeeding one and return false.
	bool Search(const KEY &i_key, unsigned int i_start,
			unsigned int &o_result) const;

	// Determine if an entry with the given key exists at the given index.
	bool Query(const KEY &i_key, unsigned int i_index)const;

	unsigned int d_growBy;
	unsigned int d_shrinkThreshold;
	unsigned int d_maxEntries;
	unsigned int d_numEntries;
	unsigned int d_transactionId;
	SEntry* d_array;

	friend class CIterator;
};

#define MAP_HPP
#ifdef AVALANCHE_ENGINE
#include "container/Map.hpp"
#else
#include "Map.hpp"
#endif

#endif // MAP_H
