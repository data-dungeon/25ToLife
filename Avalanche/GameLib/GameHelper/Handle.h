/*
**
**  File:   Handle.h
**  Date:   February 3, 2005
**  By:     Bryant Collard
**  Desc:   A template class that supports handles for things that are
**          dynamically allocated and deallocated.
**
**  Copyright (c) 2005, Avalanche Software Inc.
**  All Rights Reserved.
**
**
*/

#pragma once

#ifndef HANDLE_H
#define HANDLE_H

#include "platform/BaseType.h"

#define INVALID_OBJECT_HANDLE  ((uint32)0)

/////////////////////////////////// CHandle ////////////////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class TypePtr>
class CHandle
{
  public:
	// Constructors
	CHandle(uint32 i_value = INVALID_OBJECT_HANDLE) : d_value(i_value) {}
	CHandle(void* i_value) : d_value((uint32)i_value) {}
	CHandle(TypePtr i_value) : d_value(INVALID_OBJECT_HANDLE) {ASSERT(false);}

	// Get/set the value.
	uint32 GetValue(void) const {return(d_value);}
	void SetValue(uint32 i_value) {d_value = i_value;}

	// Assignment
	CHandle &operator=(const CHandle &i_handle)
		{d_value = i_handle.d_value; return(*this);}
	CHandle &operator=(uint32 i_value)
		{d_value = i_value; return(*this);}
	CHandle &operator=(void* i_value)
		{d_value = (uint32)i_value; return(*this);}
	CHandle &operator=(TypePtr i_value)
		{ASSERT(false); d_value = INVALID_OBJECT_HANDLE; return(*this);}

	// Conversion
	operator bool() const {return(d_value != INVALID_OBJECT_HANDLE);}
	operator uint32() const {return(d_value);}
	operator void*() const {return((void*)d_value);}
	operator TypePtr() const {ASSERT(false); return(INVALID_OBJECT_HANDLE);}

	// Comparison
	bool operator==(const CHandle &i_handle) const
		{return(d_value == i_handle.d_value);}
	bool operator!=(const CHandle &i_handle) const
		{return(d_value != i_handle.d_value);}
	bool operator==(uint32 i_value) const {return(d_value == i_value);}
	bool operator!=(uint32 i_value) const {return(d_value != i_value);}
	bool operator==(void* i_value) const {return(d_value == (uint32)i_value);}
	bool operator!=(void* i_value) const {return(d_value != (uint32)i_value);}

	// Validity
	void MakeInvalid(void) {d_value = INVALID_OBJECT_HANDLE;}
	bool Valid(void) const {return(d_value != INVALID_OBJECT_HANDLE);}

	// Masking
	uint32 operator&(uint32 i_mask) const {return(d_value & i_mask);}
	
  private:
	uint32 d_value;
};

/******************************************************************************/
/******************************************************************************/
template<class TypePtr>
inline bool operator==(uint32 i_value, const CHandle<TypePtr> &i_handle)
{
	return(i_handle == i_value);
}

/******************************************************************************/
/******************************************************************************/
template<class TypePtr>
inline bool operator!=(uint32 i_value, const CHandle<TypePtr> &i_handle)
{
	return(i_handle != i_value);
}

/******************************************************************************/
/******************************************************************************/
template<class TypePtr>
inline bool operator==(void* i_value, const CHandle<TypePtr> &i_handle)
{
	return(i_handle == i_value);
}

/******************************************************************************/
/******************************************************************************/
template<class TypePtr>
inline bool operator!=(void* i_value, const CHandle<TypePtr> &i_handle)
{
	return(i_handle != i_value);
}

/******************************************************************************/
/******************************************************************************/
#define handledef(Type)                                                        \
	class Type;                                                                 \
	typedef CHandle<Type*>

//////////////////////////////// CHandleCurator ////////////////////////////////

/******************************************************************************/
/******************************************************************************/
template<class Type, uint32 WIDTH>
class CHandleCurator
{
  public:
	// Construct/Destruct
	CHandleCurator(Type* i_target);
	~CHandleCurator();

	// Retrieve a pointer to the target, if any, given a handle. Returns NULL
	// if target does not exist.
	static Type* Retrieve(CHandle<Type*> i_handle);

	// Get the handle.
	CHandle<Type*> Get(void) const {return(d_handle);}

	// Change the location of the target.
	void Relocate(Type* i_newTarget);

	// Debugging.

	// Get the total number of targets currently managed.
	static uint32 Total(void);

	// Find the largest number of targets in a single bucket.
	static uint32 MaxBucketSize(void);

	// Get the total number of buckets.
	static uint32 NumBuckets(void) {return(s_mask + 1);}

	// Get the number of buckets containing the given number of targets.
	static uint32 BucketCount(uint32 i_numTargets);
	
  private:
	// Protect against default construction.
	CHandleCurator();

	// The next handle to be assigned.
	static uint32 s_nextHandle;

	// Using the mask with a handle yields the index into the table.
	static const uint32 s_mask;

	// An array of linked lists of handles.
	static CHandleCurator* s_bucket[~(((uint32)~0x0) << WIDTH) + 1];

	// A pointer to the target.
	Type* d_target;

	// The assigned handle.
	CHandle<Type*> d_handle;

	// The previous and next members of this handle's linked list.
	CHandleCurator* d_prev;
	CHandleCurator* d_next;
};

/******************************************************************************/
// Define a macro that declares the static member variables.
/******************************************************************************/
#define DeclareHandleCurator(T, W)                                             \
	uint32 CHandleCurator<T, W>::s_nextHandle = INVALID_OBJECT_HANDLE + 1;      \
	const uint32 CHandleCurator<T, W>::s_mask = ~(((uint32)~0x0) << W);         \
	CHandleCurator<T, W>*                                                       \
			CHandleCurator<T, W>::s_bucket[~(((uint32)~0x0) << W) + 1] = {0}

/******************************************************************************/
// Construct
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline CHandleCurator<Type, WIDTH>::CHandleCurator(Type* i_target)
{
	d_target = i_target;
	d_handle = s_nextHandle++;
	if (s_nextHandle == INVALID_OBJECT_HANDLE)
		s_nextHandle++;

	uint32 index = d_handle & s_mask;
	d_prev = NULL;
	d_next = s_bucket[index];
	s_bucket[index] = this;

	if (d_next != NULL)
		d_next->d_prev = this;
}

/******************************************************************************/
// Destruct
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline CHandleCurator<Type, WIDTH>::~CHandleCurator()
{
	if (d_prev != NULL)
		d_prev->d_next = d_next;
	else
		s_bucket[d_handle & s_mask] = d_next;

	if (d_next != NULL)
		d_next->d_prev = d_prev;
}

/******************************************************************************/
// Retrieve a pointer to the target, if any, given a handle. Returns NULL if
// target does not exist.
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline Type* CHandleCurator<Type, WIDTH>::Retrieve(CHandle<Type*> i_handle)
{
	CHandleCurator* curr = s_bucket[i_handle & s_mask];
	while (curr != NULL)
	{
		if (curr->d_handle == i_handle)
			return(curr->d_target);
		curr = curr->d_next;
	}
	return(NULL);
}

/******************************************************************************/
// Change the location of the target.
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline void CHandleCurator<Type, WIDTH>::Relocate(Type* i_newTarget)
{
	d_target = i_newTarget;

	if (d_prev != NULL)
		d_prev->d_next = this;
	else
		s_bucket[d_handle & s_mask] = this;

	if (d_next != NULL)
		d_next->d_prev = this;
}

/******************************************************************************/
// Get the total number of targets currently managed.
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline uint32 CHandleCurator<Type, WIDTH>::Total(void)
{
	uint32 result = 0;
	for (uint32 i = 0; i <= s_mask; i++)
	{
		CHandleCurator* curr = s_bucket[i];
		while (curr != NULL)
		{
			result++;
			curr = curr->d_next;
		}
	}
	return(result);
}

/******************************************************************************/
// Find the largest number of targets in a single bucket.
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline uint32 CHandleCurator<Type, WIDTH>::MaxBucketSize(void)
{
	uint32 result = 0;
	for (uint32 i = 0; i <= s_mask; i++)
	{
		uint32 targets = 0;
		CHandleCurator* curr = s_bucket[i];
		while (curr != NULL)
		{
			targets++;
			curr = curr->d_next;
		}
		if (targets > result)
			result = targets;
	}
	return(result);
}

/******************************************************************************/
// Get the number of buckets containing the given number of targets.
/******************************************************************************/
template<class Type, uint32 WIDTH>
inline uint32 CHandleCurator<Type, WIDTH>::BucketCount(uint32 i_numTargets)
{
	uint32 result = 0;
	for (uint32 i = 0; i <= s_mask; i++)
	{
		uint32 targets = 0;
		CHandleCurator* curr = s_bucket[i];
		while (curr != NULL)
		{
			targets++;
			curr = curr->d_next;
		}
		if (targets == i_numTargets)
			result++;
	}
	return(result);
}

#endif // HANDLE_H
