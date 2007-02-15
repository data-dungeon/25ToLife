/*
**
**  File:   Stack.hpp
**  Date:   January 29, 2003
**  By:     Bryant Collard
**  Desc:   A simple stack for transient objects used in collision testing.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/StackArray.hpp $
**  $Revision: 10 $
**      $Date: 7/23/03 2:28p $
**    $Author: Bcollard $
**
*/

#ifdef STACK_ARRAY_HPP
#undef STACK_ARRAY_HPP

#define DEFAULT_GROWBY   5

//////////////////////////// CCollisionStackArray ///////////////////////////

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackArray<T>* CCollisionStackArray<T>::New(
		CCollisionStack* i_stack)
{
	if (i_stack != NULL)
	{
		CCollisionStackArray<T>* list =
				(CCollisionStackArray<T>*)i_stack->Allocate(
				sizeof(CCollisionStackArray<T>));

		if (list)
			new(list) CCollisionStackArray<T>(*i_stack);

		return(list);
	}

	return(new CCollisionStackArray<T>);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackArray<T>::CCollisionStackArray()
{
	d_growBy = DEFAULT_GROWBY;
	d_allocatedEntries = 0;
	d_numEntries = 0;
	d_array = NULL;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackArray<T>::CCollisionStackArray(uint32 i_growBy)
{
	if (i_growBy > 0)
		d_growBy = i_growBy;
	else
		d_growBy = DEFAULT_GROWBY;
	d_allocatedEntries = 0;
	d_numEntries = 0;
	d_array = NULL;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackArray<T>::CCollisionStackArray(CCollisionStack &i_stack) :
	CCollisionStackItem(i_stack)
{
	d_allocatedEntries = 0;
	d_numEntries = 0;
	d_array = (T*)(d_stack->Top());
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackArray<T>::~CCollisionStackArray()
{
	if (d_stack == NULL)
		delete[] d_array;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void CCollisionStackArray<T>::InitArray(void)
{
	d_numEntries = 0;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline bool CCollisionStackArray<T>::ExpandStack(int i_size)
{
	if (d_stack->Expand(i_size * sizeof(T), this) == NULL)
		return(false);

	d_allocatedEntries += i_size;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline bool CCollisionStackArray<T>::ExpandDynamic(int i_size)
{
	int growBy = d_growBy;
	while (growBy < i_size)
		growBy += d_growBy;

	int newAllocatedEntries = d_allocatedEntries + growBy;
	T* newArray = new T[newAllocatedEntries];

	if (newArray == NULL)
		return(false);

	for (int i = 0; i < d_allocatedEntries; i++)
		newArray[i] = d_array[i];
	delete[] d_array;
	d_array = newArray;

	d_allocatedEntries = newAllocatedEntries;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T* CCollisionStackArray<T>::ExpandArray(int i_size)
{
	ASSERT(i_size > 0);
	int allocateSize = i_size + d_numEntries - d_allocatedEntries;
	if (allocateSize > 0)
	{
		if (d_stack == NULL)
		{
			if (!ExpandDynamic(allocateSize))
				return(NULL);
		}
		else if (!ExpandStack(allocateSize))
			return(NULL);
	}

	T* ret = &d_array[d_numEntries];
	d_numEntries += i_size;
	return(ret);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline bool CCollisionStackArray<T>::ContractArray(int i_size)
{
	if (d_stack != NULL)
	{
		if (!d_stack->Contract(i_size * sizeof(T), this))
			return(false);
		d_allocatedEntries -= i_size;
	}

	d_numEntries -= i_size;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline bool CCollisionStackArray<T>::AllocateArray(int i_size)
{
	ASSERT(i_size > 0);
	int allocateSize = i_size - d_allocatedEntries;
	if (allocateSize > 0)
	{
		if (d_stack == NULL)
		{
			if (!ExpandDynamic(allocateSize))
				return(false);
		}
		else if (!ExpandStack(allocateSize))
			return(false);
	}

	return(true);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T* CCollisionStackArray<T>::EntryPtr(int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries));
	return(&(d_array[i_index]));
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T& CCollisionStackArray<T>::EntryRef(int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries));
	return(d_array[i_index]);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T& CCollisionStackArray<T>::operator[](int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries));
	return(d_array[i_index]);
}

#ifdef DEBUG_STACK
/***************************************************************************/
/***************************************************************************/
#include <string.h>
template <class T>
inline void CCollisionStackArray<T>::AppendDescription(char* io_msg)
{
	char msg[256];
	sprintf(msg, " of %d %d byte entries (%d allocated)", d_numEntries,
			sizeof(T), d_allocatedEntries);
	strcat(io_msg, msg);
}
#endif

////////////////////////// CCollisionStackEndArray //////////////////////////

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackEndArray<T>::CCollisionStackEndArray()
{
	d_stack = NULL;
	d_key = 0;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline CCollisionStackEndArray<T>::~CCollisionStackEndArray()
{
	CloseArray();
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void CCollisionStackEndArray<T>::OpenArray(void)
{
	ASSERT(d_stack != NULL);
	if (!d_stack->IsBottomKey(d_key))
		d_key = d_stack->LockBottom();
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void CCollisionStackEndArray<T>::CloseArray(void)
{
	if (d_stack != NULL)
		d_stack->UnlockBottom(d_key);
	d_key = 0;
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline void CCollisionStackEndArray<T>::InitArray(CCollisionStack* i_stack)
{
	ASSERT(i_stack != NULL);
	CloseArray();
	d_stack = i_stack;
	OpenArray();
	d_numEntries = 0;
	d_stack->ResetBottom(d_key);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T* CCollisionStackEndArray<T>::ExpandArray(int i_size)
{
	ASSERT((i_size > 0) && (d_stack != NULL) && d_stack->IsBottomKey(d_key));
	void* bottom = d_stack->ExpandBottom(i_size * sizeof(T), d_key);
	if (bottom == NULL)
		return(NULL);

	d_array = (T*)bottom;
	d_numEntries += i_size;
	return(&(d_array[i_size - 1]));
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline bool CCollisionStackEndArray<T>::ContractArray(int i_size)
{
	ASSERT((i_size > 0) && (d_stack != NULL) && d_stack->IsBottomKey(d_key));
	void* bottom = d_stack->ContractBottom(i_size * sizeof(T), d_key);
	if (bottom == NULL)
		return(false);

	d_array = (T*)bottom;
	d_numEntries -= i_size;
	return(true);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T* CCollisionStackEndArray<T>::EntryPtr(int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries) &&
			(d_stack != NULL) && d_stack->IsBottomKey(d_key));
	return(&(d_array[d_numEntries - i_index - 1]));
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T& CCollisionStackEndArray<T>::EntryRef(int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries) &&
			(d_stack != NULL) && d_stack->IsBottomKey(d_key));
	return(d_array[d_numEntries - i_index - 1]);
}

/***************************************************************************/
/***************************************************************************/
template <class T>
inline T& CCollisionStackEndArray<T>::operator[](int i_index)
{
	ASSERT((i_index >= 0) && (i_index < d_numEntries) &&
			(d_stack != NULL) && d_stack->IsBottomKey(d_key));
	return(d_array[d_numEntries - i_index - 1]);
}

#endif // STACK_ARRAY_HPP
