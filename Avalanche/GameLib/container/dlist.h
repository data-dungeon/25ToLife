/********************************************************************
	created:	2004/10/19
	created:	19:10:2004   16:36
	filename: 	C:\dev\Avalanche\GameLib\container\dlist.h
	file path:	C:\dev\Avalanche\GameLib\container
	file base:	dlist
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Template for a doubly-linked list container.

	note: If you are using a (T) instead of (T*) container, a
	meaningful copy constructor should be implemented for (T).
*********************************************************************/

#if !defined( _DLIST_H )
#define _DLIST_H

#include "container/pool.h"

template< class T >
class DoubleList
{
public:
	DoubleList();
	virtual ~DoubleList();

	/*! Clear the list of all elements.
	*/
	virtual void Clear( void );

	//!@name Perl-style list modification methods.
	//@{

	/*! List element count.
	This method returns the current number of elements in the list.
	@return The number of elements in the list.
	*/
	virtual unsigned int Count( void );

	/*! Place a value at the head of the list.
	This method places a copy of the <b>value</b> parameter at the head
	of the list.
	@param value The value to place at the head of the list.
	@return The number of elements in the list following this operation.
	*/
	virtual unsigned int Unshift( const T& value );

	/*! Remove a value from the head of the list.
	This method removes the value at the head of the list and returns
	a copy of it in the <b>value</b> parameter.  Please note that the
	return value will be <b>0</b> even if the list was already empty
	before the call to this method.  In that case, the <b>value</b>
	parameter will be unchanged.  Please call the Count() method
	before Shifting to check if you have an empty list.
	Be aware that calling this method will render any iterator handles
	associated with the popped value invalid.
	@param value Place holder for the value at the head of the list.
	@return The number of elements in the list following this operation.
	*/
	virtual unsigned int Shift( T& value );

	/*! Place a value at the tail of the list.
	This method places a copy of the <b>value</b> parameter at the tail
	of the list.
	@param value The value to place at the tail of the list.
	@return The number of elements in the list following this operation.
	*/
	virtual unsigned int Push( const T& value );

	/*! Remove a value from the tail of the list.
	This method removes the value at the tail of the list and returns
	a copy of it in the <b>value</b> parameter.  Please note that the
	return value will be <b>0</b> even if the list was already empty
	before the call to this method.  In that case, the <b>value</b>
	parameter will be unchanged.  Please call the Count() method
	before Popping to check if you have an empty list.
	Be aware that calling this method will render any iterator handles
	associated with the popped value invalid.
	@param value Place holder for the value at the tail of the list.
	@return The number of elements in the list following this operation.
	*/
	virtual unsigned int Pop( T& value );

	//@}

	//!@name Iteration API
	//@{
	/*! Get a handle to the head of the list.
	@return The handle for the head element in the list.  This handle is
	associated with the first element, not in front of the first element.
	So, comparing an iterator given from Head() to the Tail() result will
	tell you when you are currently interacting with the last element, not
	when you have gone past the last element in the list.
	Will return NULL when called on an empty list.
	*/
	virtual void* Head( void );

	/*! Get a handle to the tail of the list.
	@return The handle for the tail element in the list.  This handle is
	associated with the last element, not behind the last element.  So,
	comparing an iterator given from Head() to the Tail() result will tell
	you when you are currently interacting with the last element, not when
	you have gone past the last element in the list.
	Will return NULL when called on an empty list.
	*/
	virtual void* Tail( void );

	/*! Iterate the handle forward to the next value.
	@param hIterator The iterator handle retrieved from Head() or Tail().
	@return <b>true</b> if the handle is valid when the method is called,
	<b>false</b> otherwise.
	*/
	virtual bool IterateForward( void*& hIterator );

	/*! Iterate the handle backward to the next value.
	@param hIterator The iterator handle retrieved from Head() or Tail().
	@return <b>true</b> if the handle is valid when the method is called,
	<b>false</b> otherwise.
	*/
	virtual bool IterateBack( void*& hIterator );

	/*! Get a copy of the value associated with the given iterator handle.
	@param hIterator The iterator handle retrieved from Head() or Tail().
	@param value A place to copy the associated value.
	@return <b>true</b> if the handle is associated with a valid value,
	<b>false</b> otherwise.
	*/
	virtual bool GetValueAt( void* hIterator, T& value );

	/*! Remove the value associated with the given iterator handle.
	Removing the value will render the iterator handle invalid.
	@param hIterator The iterator handle retrieved from Head() or Tail().
	@return <b>true</b> if a value was removed from the list, <b>false</b>
	otherwise.
	*/
	virtual bool RemoveValueAt( void* hIterator );
	//@}

protected:

	/*! A bucket class for the list.
	*/
	struct DoubleList_Bucket
	{
		T	m_Value;
		DoubleList_Bucket	*m_pNext;
		DoubleList_Bucket	*m_pPrev;
	};

	/*! The number of elements in the list.
	*/
	unsigned int		m_iCount;

	/*! The head of the list.
	*/
	DoubleList_Bucket	*m_pHead;

	/*! The tail of the list.
	*/
	DoubleList_Bucket	*m_pTail;

	/*! A pool of buckets.
	*/
	Pool< DoubleList_Bucket >	m_BucketPool;
};

#include "container/dlist.hpp"

#endif	// _DLIST_H