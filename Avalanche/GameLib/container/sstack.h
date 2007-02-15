/////////////////////////////////////////////////////////////////////////////
// Simple stack template
//
// These stacks never deal with constructors/destructors so BEWARE!
/////////////////////////////////////////////////////////////////////////////
#ifndef SSTACK_H
#define SSTACK_H

#include "container/Darray.h"

// Really simple stack that uses a preallocated array
// Note if you use T's (not T*'s) make sure the copy constructor is
// supported.
template <class T, int maxItems> class SimpleStack
{
public:
	inline SimpleStack();
	inline ~SimpleStack() {}
	// Push a new item onto the stack
	// Asserts if full
	inline void Push(T &newTop);

	// Pop an item off the stack, asserts if empty
	inline T Pop();

	// Peek at the top item (without removing it)
	// asserts if empty
	inline T &Peek();

	// Is the stack empty?
	inline bool Empty();

	// How many items are on the stack?
	inline int Items();

	// Clear the stack
	inline void Clear();

	// Remove an item from the tack
	inline void Remove(int index);

	// Get at the stack
	inline T &Peek(int index);

private:
	int		top;
	T			stack[maxItems];
};

template <class T> class DynamicStack
{
public:
	inline DynamicStack(int startItems = 8, int growBy = 8);
	inline ~DynamicStack() {}

	// Push a new item onto the stack
	// Asserts if full
	inline void Push(T &newTop);

	// Pop an item off the stack, asserts if empty
	inline T Pop();

	// Peek at the top item (without removing it)
	// asserts if empty
	inline T &Peek();

	// Is the stack empty?
	inline bool Empty();

	// How many items are on the stack?
	inline int Items();

	// Clear the stack
	inline void Clear();

	// Remove an item from the tack
	inline void Remove(int index);

	// Get at the stack
	inline T &Peek(int index);

private:
	DynamicArray<T>	stack;
};

// The all important implementation
#include "container/sstack.hpp"

#endif
