/*
**
**  File:   StackArray.h
**  Date:   February 17, 2003
**  By:     Bryant Collard
**  Desc:   A template for an expandable array.
**
**  Copyright (c) 2003, Avalanche Software Inc.
**  All Rights Reserved.
**
**   $Archive: /Avalanche/GameLibNew/collide/StackArray.h $
**  $Revision: 9 $
**      $Date: 7/23/03 2:28p $
**    $Author: Bcollard $
**
*/

#ifndef STACK_ARRAY_H
#define STACK_ARRAY_H

// GameLib includes //
#include "collide/Stack.h"

template <class T> class CCollisionStackArray : public CCollisionStackItem
{
  public:
	// Allocate/deallocate a new object off/on the global stack.
	static CCollisionStackArray* New(CCollisionStack* i_stack);

	// Constructors for objects that use dynamic memory.
	CCollisionStackArray();
	CCollisionStackArray(uint32 i_growBy);

	// Destructor.
	~CCollisionStackArray();

	// Build the array.
	void InitArray(void);
	T* ExpandArray(int i_size = 1);
	bool ContractArray(int i_size = 1);
	bool AllocateArray(int i_size);

	// Access array.
	int NumEntries(void) {return(d_numEntries);}
	T* EntryPtr(int i_index);
	T& EntryRef(int i_index);
	T& operator[](int i_index);

#ifdef DEBUG_STACK
	virtual const char* GetName(void) {return("Array");}
	virtual void AppendDescription(char* io_msg);
#endif

  private:
	// Constructors for objects that use the stack.
	CCollisionStackArray(CCollisionStack &i_stack);

	// Expand the array.
	bool ExpandStack(int i_size);
	bool ExpandDynamic(int i_size);

	uint32 d_growBy;
	int d_numEntries;
	int d_allocatedEntries;
	T* d_array;
};

template <class T> class CCollisionStackEndArray
{
  public:
	// Constructor
	CCollisionStackEndArray();
	~CCollisionStackEndArray();

	// Build the array.
	void OpenArray(void);
	void CloseArray(void);
	void InitArray(CCollisionStack* i_stack);
	T* ExpandArray(int i_size = 1);
	bool ContractArray(int i_size = 1);

	// Access array.
	int NumEntries(void) {return(d_numEntries);}
	T* EntryPtr(int i_index);
	T& EntryRef(int i_index);
	T& operator[](int i_index);

  private:
	CCollisionStack* d_stack;
	int d_numEntries;
	T* d_array;
	uint32 d_key;
};

#define STACK_ARRAY_HPP
#include "collide/StackArray.hpp"

#endif // STACK_ARRAY_H
