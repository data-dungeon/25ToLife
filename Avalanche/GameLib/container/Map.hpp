/*
**
**  File:   Map.hpp
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
**      $File: //depot/Avalanche/GameLib/container/Map.hpp $
**  $Revision: #2 $
**  $DateTime: 2005/06/24 20:40:40 $
**    $Author: Bryant_Collard $
**
*/

#ifdef MAP_HPP
#undef MAP_HPP

#ifdef AVALANCHE_ENGINE

#include "Layers/memory.h"

#define MEMORY_ALLOC memAlloc
#define MEMORY_RESIZE memResize
#define MEMORY_FREE memFree

#else

#include <stdlib.h>
#include <malloc.h>

#define MEMORY_ALLOC malloc
#define MEMORY_RESIZE realloc
#define MEMORY_FREE free
#define ASSERT_PTR(p)

#endif

#include <new.h>

#define AT_END  (unsigned int)(~0)
#define AT_START  (AT_END - 1)

/******************************************************************************/
// Construct. Result will be invalid.
/******************************************************************************/
template<class KEY, class VALUE>
inline CMap<KEY, VALUE>::CIterator::CIterator()
{
	d_map = NULL;
	d_index = AT_START;
}

/******************************************************************************/
// Copy constructor.
/******************************************************************************/
template<class KEY, class VALUE>
inline CMap<KEY, VALUE>::CIterator::CIterator(const CIterator &i_iterator)
{
	d_map = i_iterator.d_map;
	d_key = i_iterator.d_key;
	d_index = i_iterator.d_index;
	d_transactionId = i_iterator.d_transactionId;
}

/******************************************************************************/
// Assignment operator.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator&
		CMap<KEY, VALUE>::CIterator::operator=(const CIterator &i_iterator)
{
	d_map = i_iterator.d_map;
	d_key = i_iterator.d_key;
	d_index = i_iterator.d_index;
	d_transactionId = i_iterator.d_transactionId;
	return(*this);
}

/******************************************************************************/
// Address the entry succeeding the current key. This works even if the iterator
// is currently invalid.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator&
		CMap<KEY, VALUE>::CIterator::operator++()
{
	if ((d_map != NULL) && (d_index != AT_END))
	{
		if (d_index == AT_START)
			d_index = 0;
		else if ((d_transactionId == d_map->d_transactionId) ||
				d_map->Search(d_key, d_index, d_index))
			d_index++;

		if (d_index < d_map->d_numEntries)
		{
			d_key = d_map->d_array[d_index].d_key;
			d_transactionId = d_map->d_transactionId;
		}
		else
			d_index = AT_END;
	}
	return(*this);
}

/******************************************************************************/
// Address the entry preceeding the current key. This works even if the iterator
// is currently invalid.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator&
		CMap<KEY, VALUE>::CIterator::operator--()
{
	if ((d_map != NULL) && (d_index != AT_START))
	{
		if (d_index == AT_END)
			d_index = d_map->d_numEntries;
		else if (d_transactionId != d_map->d_transactionId)
			d_map->Search(d_key, d_index, d_index);

		if (d_index > 0)
		{
			d_index--;
			d_key = d_map->d_array[d_index].d_key;
			d_transactionId = d_map->d_transactionId;
		}
		else
			d_index = AT_START;
	}
	return(*this);
}

/******************************************************************************/
// Indicate if the iterator addresses an entry preceeding the first entry. Will
// always return true if there are no entries.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::CIterator::Start(void)
{
	if ((d_map == NULL) || (d_map->d_numEntries == 0) || (d_index == AT_START))
		return(true);

	if ((d_index != AT_END) && (d_transactionId != d_map->d_transactionId))
	{
		// Try to find the associated entry.
		unsigned int index;
		if (!d_map->Search(d_key, d_index, index))
		{
			// If there is not an associated entry, the index found will be the
			// succeeding entry. If that is 0, the iterator is at the start. If not
			// at the start, don't update the transaction so other operations will
			// no be fouled up.
			if (index == 0)
			{
				d_index = AT_START;
				return(true);
			}
		}

		// If there is an associated entry, update the transaction id while we
		// are here.
		else
			d_transactionId = d_map->d_transactionId;
	}

	return(false);
}

/******************************************************************************/
// Indicate if the iterator addresses an entry succeeding the last entry. Will
// always return true if there are no entries.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::CIterator::End(void)
{
	if ((d_map == NULL) || (d_map->d_numEntries == 0) || (d_index == AT_END))
		return(true);

	if ((d_index != AT_START) && (d_transactionId != d_map->d_transactionId))
	{
		// Try to find the associated entry.
		unsigned int index;
		if (!d_map->Search(d_key, d_index, index))
		{
			// If there is not an associated entry, the index found will be the
			// succeeding entry. If that is equal to the number of entries, the
			// iterator is at the end. If not at the end, don't update the
			// transaction so other operations will no be fouled up.
			if (index == d_map->d_numEntries)
			{
				d_index = AT_START;
				return(true);
			}
		}

		// If there is an associated entry, update the transaction id while we
		// are here.
		else
			d_transactionId = d_map->d_transactionId;
	}

	return(false);
}

/******************************************************************************/
// Indicate if the iterator addresses an entry preceeding the first entry or
// succeeding the last entry.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::CIterator::Out(void)
{
	if ((d_map == NULL) || (d_map->d_numEntries == 0) || (d_index == AT_START) ||
			(d_index == AT_END))
		return(true);

	if (d_transactionId != d_map->d_transactionId)
	{
		// Try to find the associated entry.
		unsigned int index;
		if (!d_map->Search(d_key, d_index, index))
		{
			if (index == 0)
			{
				d_index = AT_START;
				return(true);
			}
			if (index == d_map->d_numEntries)
			{
				d_index = AT_START;
				return(true);
			}
		}

		// If there is an associated entry, update the transaction id while we
		// are here.
		else
			d_transactionId = d_map->d_transactionId;
	}

	return(false);
}

/******************************************************************************/
// Determine if the iterator is valid. An iterator will be invalid if it hasn't
// been assigned or there is no longer an entry with the associated key. Note
// that an iterator can be invalidated only when an entry is added or removed.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::CIterator::Valid(void) const
{
	if (d_map == NULL)
		return(false);

	if ((d_index == AT_START) || (d_index == AT_END) ||
			(d_transactionId == d_map->d_transactionId))
		return(true);

	return(d_map->Query(d_key, d_index));
}

/******************************************************************************/
// Return a pointer to the key of the entry addressed. Returns NULL if the
// iterator is invalid or if it addresses an entry preceeding the first entry or
// succeeding the last entry.
/******************************************************************************/
template<class KEY, class VALUE>
inline const KEY* CMap<KEY, VALUE>::CIterator::Key(void) const
{
	if ((d_map != NULL) && (d_index != AT_START) && (d_index != AT_END) &&
			((d_transactionId == d_map->d_transactionId) ||
			d_map->Query(d_key, d_index)))
		return(&d_map->d_array[d_index].d_key);
	return(NULL);
}

/******************************************************************************/
// Return a pointer to the value of the entry addressed. Returns NULL if
// the iterator is invalid or if it addresses an entry preceeding the
// first entry or succeeding the last entry.
/******************************************************************************/
template<class KEY, class VALUE>
inline VALUE* CMap<KEY, VALUE>::CIterator::Value(void) const
{
	if ((d_map != NULL) && (d_index != AT_START) && (d_index != AT_END) &&
			((d_transactionId == d_map->d_transactionId) ||
			d_map->Query(d_key, d_index)))
		return(&d_map->d_array[d_index].d_value);
	return(NULL);
}

/******************************************************************************/
/******************************************************************************/
template<class KEY, class VALUE>
inline CMap<KEY, VALUE>::CMap(unsigned int i_growBy,
		unsigned int i_shrinkThreshold)
{
	d_growBy = (i_growBy > 0) ? i_growBy : 1;
	d_shrinkThreshold = i_shrinkThreshold;
	d_maxEntries = 0;
	d_numEntries = 0;
	d_transactionId = 0;
	d_array = NULL;
}

/******************************************************************************/
/******************************************************************************/
template<class KEY, class VALUE>
inline CMap<KEY, VALUE>::~CMap()
{
	if (d_array != NULL)
	{
		for (unsigned int i = 0; i < d_numEntries; i++)
		{
			d_array[i].d_key.~KEY();
			d_array[i].d_value.~VALUE();
		}
		MEMORY_FREE(d_array);
	}
}

/******************************************************************************/
// Return a reference to the value associated with the key. If there was no
// entry for the key, a new entry is created.
/******************************************************************************/
template<class KEY, class VALUE>
inline VALUE &CMap<KEY, VALUE>::operator[](const KEY &i_key)
{
	unsigned int index;
	if (!Search(i_key, index))
		Add(i_key, index);
	return(d_array[index].d_value);
}

/******************************************************************************/
// Return a reference to the value associated with the key. If there was no
// entry for the key, a new entry is created.
/******************************************************************************/
template<class KEY, class VALUE>
inline VALUE &CMap<KEY, VALUE>::Get(const KEY &i_key, bool* o_newEntry)
{
	unsigned int index;
	bool newEntry = !Search(i_key, index);
	if (o_newEntry != NULL)
		*o_newEntry = newEntry;
	if (newEntry)
		Add(i_key, index);
	return(d_array[index].d_value);
}

/******************************************************************************/
// Return a pointer to the value associated with the key. NULL is returned if
// there is no entry for the key.
/******************************************************************************/
template<class KEY, class VALUE>
inline VALUE* CMap<KEY, VALUE>::Find(const KEY &i_key)
{
	unsigned int index;
	if (Search(i_key, index))
		return(&d_array[index].d_value);
	return(NULL);
}

/******************************************************************************/
// Return a pointer to the value in the entry inserted that is associated with
// the key. NULL is returned if an entry already exists.
/******************************************************************************/
template<class KEY, class VALUE>
inline VALUE* CMap<KEY, VALUE>::Insert(const KEY &i_key)
{
	unsigned int index;
	if (Search(i_key, index))
		return(NULL);

	Add(i_key, index);
	return(&d_array[index].d_value);
}

/******************************************************************************/
// Add an entry at the specified index.
/******************************************************************************/
template<class KEY, class VALUE>
inline void CMap<KEY, VALUE>::Add(const KEY &i_key, unsigned int i_index)
{
	// Increase the max array size if necessary.
	if (d_array == NULL)
	{
		d_numEntries = 0;
		d_maxEntries = d_growBy;
		d_array = (SEntry*)MEMORY_ALLOC(d_maxEntries * sizeof(SEntry));
		ASSERT_PTR(d_array);
	}
	else if (d_numEntries >= d_maxEntries)
	{
		d_maxEntries += d_growBy;
		d_array = (SEntry*)MEMORY_RESIZE(d_array, d_maxEntries * sizeof(SEntry));
		ASSERT_PTR(d_array);
	}

	// Open up space for the entry.
	for (unsigned int i = d_numEntries; i > i_index; i--)
		memcpy(&d_array[i], &d_array[i - 1], sizeof(SEntry));
	d_numEntries++;

	// Call the entry member constructors.
	new(&d_array[i_index].d_key) KEY;
	new(&d_array[i_index].d_value) VALUE;

	// Assign the key.
	d_array[i_index].d_key = i_key;

	// Note the transaction.
	d_transactionId++;
}

/******************************************************************************/
// Try to remove the entry associated with the key. Returns true if entry was
// removed, false if there was no entry to remove.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::Remove(const KEY &i_key)
{
	// Try to find the entry.
	unsigned int index;
	if (!Search(i_key, index))
		return(false);

	// Call the entry member destructors.
	d_array[index].d_key.~KEY();
	d_array[index].d_value.~VALUE();

	// Fill in the space used by the entry.
	d_numEntries--;
	for (unsigned int i = index; i < d_numEntries; i++)
		memcpy(&d_array[i], &d_array[i + 1], sizeof(SEntry));

	// See if the number of free entries exceeds the shrink threshold. Note that
	// d_maxEntries >= d_numEntries.
	unsigned int excess = d_maxEntries - d_numEntries;
	if (excess >= d_shrinkThreshold)
	{
		// Find the number of "blocks" of memory above the amount that must be
		// maintained. This is computed in a two-step process because
		// d_shrinkThreshold could be larger than excess.
		excess = (excess - d_shrinkThreshold) / d_growBy;

		// Free extra blocks.
		if (excess > 0)
		{
			d_maxEntries -= excess * d_growBy;
			if (d_maxEntries > 0)
			{
				d_array = (SEntry*)MEMORY_RESIZE(d_array,
						d_maxEntries * sizeof(SEntry));
				ASSERT_PTR(d_array);
			}
			else
			{
				MEMORY_FREE(d_array);
				d_array = NULL;
			}
		}
	}

	// Note the transaction and return.
	d_transactionId++;
	return(true);
}

/******************************************************************************/
// Return an iterator addressing the entry preceeding the first entry.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::Start(void) const
{
	CIterator iterator;
	iterator.d_map = this;
	iterator.d_index = AT_START;
	return(iterator);
}

/******************************************************************************/
// Return an iterator addressing the first element in the map.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::First(void) const
{
	CIterator iterator;
	iterator.d_map = this;
	if (d_numEntries > 0)
	{
		iterator.d_index = 0;
		iterator.d_key = d_array[0].d_key;
		iterator.d_transactionId = d_transactionId;
	}
	else
		iterator.d_index = AT_START;
	return(iterator);
}

/******************************************************************************/
// Return an iterator addressing the last element in the map.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::Last(void) const
{
	CIterator iterator;
	iterator.d_map = this;
	if (d_numEntries > 0)
	{
		iterator.d_index = d_numEntries - 1;
		iterator.d_key = d_array[iterator.d_index].d_key;
		iterator.d_transactionId = d_transactionId;
	}
	else
		iterator.d_index = AT_END;
	return(iterator);
}

/******************************************************************************/
// Return an iterator addressing the entry succeeding the last entry.
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::End(void) const
{
	CIterator iterator;
	iterator.d_map = this;
	iterator.d_index = AT_END;
	return(iterator);
}

/******************************************************************************/
// Return an iterator addressing the entry with the key or the closest
// preceeding entry (may be before the first entry).
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::GotoBack(
		const KEY &i_key) const
{
	CIterator iterator;
	iterator.d_map = this;

	if (!Search(i_key, iterator.d_index))
		iterator.d_index = (iterator.d_index > 0) ? iterator.d_index-- : AT_START;
	if (iterator.d_index != AT_START)
	{
		iterator.d_key = d_array[iterator.d_index].d_key;
		iterator.d_transactionId = d_transactionId;
	}

	return(iterator);
}

/******************************************************************************/
// Return an iterator addressing the entry with the key or the closest
// succeeding entry (may be after the last entry).
/******************************************************************************/
template<class KEY, class VALUE>
inline typename CMap<KEY, VALUE>::CIterator CMap<KEY, VALUE>::GotoForward(
		const KEY &i_key) const
{
	CIterator iterator;
	iterator.d_map = this;

	Search(i_key, iterator.d_index);
	if (iterator.d_index < d_numEntries)
	{
		iterator.d_key = d_array[iterator.d_index].d_key;
		iterator.d_transactionId = d_transactionId;
	}
	else
		iterator.d_index = AT_END;

	return(iterator);
}

/******************************************************************************/
// Perform a binary search to find an entry with the key. If an entry exists,
// return true; otherwise, find what would be the succeeding one and return
// false.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::Search(const KEY &i_key,
		unsigned int &o_index) const
{
	// Can't find anything if the map has no entries.
	if (d_numEntries == 0)
	{
		o_index = 0;
		return(false);
	}

	// Each step of the search isolates a test region. If you consider virtual
	// entries at each end that reside at +/- infinity, the key is known to lie
	// between the entry immediately preceeding and immediately succeeding the
	// region. An entry in the region is selected as a test entry. The number of
	// entries preceeding the test entry is equal to or one less than the number
	// of entries succeeding the test entry.
	//
	// All entries are in the first test region.
	unsigned int below = (d_numEntries - 1) >> 1;
	unsigned int above = d_numEntries >> 1;
	o_index = below;
	while(below > 0)
	{
		// If the key preceeds the test entry, use the entries preceeding the test
		// entry as the new region.
		if (i_key < d_array[o_index].d_key)
		{
			above = below >> 1;
			below = (below - 1) >> 1;
			o_index -= (above + 1);
		}

		// If the key succeeds the test entry, use the entries succeeding the test
		// entry as the new region.
		else if (d_array[o_index].d_key < i_key)
		{
			below = (above - 1) >> 1;
			above = above >> 1;
			o_index += (below + 1);
		}

		// The key matches the test entry.
		else
			return(true);

		// Keep narrowing the test region until there is no region preceeding the
		// test entry to use as the next region. Note that there still may be a
		// one entry region succeeding the test entry.
	}

	// o_index refers to an entry that has yet to be tested. Since it is known
	// that the key succeeds the entry just before the test entry, the test entry
	// is the immediate successor of the key if the key is less than the test
	// entry.
	if (i_key < d_array[o_index].d_key)
		return(false);

	// If the test entry is less than the key, some following entry will be
	// returned; otherwise, the key matches the current test entry.
	if (d_array[o_index].d_key < i_key)
	{
		// Move on to the next entry.
		o_index++;

		// If there was no region above the last test entry or if the key preceeds
		// the new test entry, there is no match and the new test entry is the
		// key's immediate successor.
		if ((above == 0) || (i_key < d_array[o_index].d_key))
			return(false);

		// If the test entry preceeds the key, the next key (which is either the
		// virtual entry at the end or an entry that has been tested) is the key's
		// immediate successor; otherwise, the key matches the test entry.
		if (d_array[o_index].d_key < i_key)
		{
			o_index++;
			return(false);
		}
	}

	return(true);
}

/******************************************************************************/
// Perform a linear search starting from a given point to find an entry with the
// key. If an entry exists, return true; otherwise, find what would be the
// succeeding one and return false.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::Search(const KEY &i_key, unsigned int i_start,
		unsigned int &o_result) const
{
	// Can't find anything if the map has no entries.
	if (d_numEntries == 0)
	{
		o_result = 0;
		return(false);
	}

	// Make sure the starting point is an actual entry.
	o_result = (i_start < d_numEntries) ? i_start : d_numEntries - 1;

	// Find an entry that matches or preceeds the key.
	while(i_key < d_array[o_result].d_key)
	{
		// If the key preceeds the first entry, the first entry is the succeeding
		// entry and no entry is a match.
		if (o_result == 0)
			return(false);

		// Move to the preceeding entry.
		o_result--;
	}

	// Find the entry that matches or immediately succeeds the key.
	while(d_array[o_result].d_key < i_key)
	{
		// Move on to the succeeding entry.
		o_result++;

		// If last entry preceeds the key, the succeeding entry is the virtual
		// entry after the last entry and there is no matching entry.
		if (o_result == d_numEntries)
			return(false);
	}

	// If the key is less than the found entry, no match was found.
	return(!(i_key < d_array[o_result].d_key));
}

/******************************************************************************/
// Determine if an entry with the given key exists at the given index.
/******************************************************************************/
template<class KEY, class VALUE>
inline bool CMap<KEY, VALUE>::Query(const KEY &i_key,
		unsigned int i_index) const
{
	return(!((i_index >= d_numEntries) || (i_key < d_array[i_index].d_key) ||
			(d_array[i_index].d_key < i_key)));
}

#endif // MAP_HPP
