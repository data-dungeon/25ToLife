/********************************************************************
	created:	2005/02/03
	created:	3:2:2005   14:37
	file base:	pqueue
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Implement a priority queue abstract base class.
*********************************************************************/

#if !defined( _PQUEUE_H )
#define _PQUEUE_H

#include "container/Darray.h"
#include "container/pool.h"

/*! A priority-sorted binary heap.
Elements in this heap are sorted according to a priority rating, given when the element is inserted
into the queue.  PriorityQueue is a virtual base-class.  Specific implementations will need to
implement the ComparePriorities method to enable use of the class.  The ComparePriorities method
takes two priorites and compares them.  This allows different types of priority queues - some with
maximum priority first, others with minimum priority first.  Insertion and deletion from the queue
are approximately O(LogN) operations.

The Value class must have a vaild '==' operator.
The Value class needs a sane copy constructor.
The Priority class must have vaild '<', '>', '=', and '==' operators.
The Priority class needs a sane copy constructor.
*/

template< class Priority, class Value >
class PriorityQueue
{
public:

	/*! @name Constructor and Destructor. */
	//@{
	/*! Construct a PriorityQueue.
	@param iInitialSize The initial amount of space to allocate for elements of the queue.
	@param iGrowSize The amount of space to grow the queue when the current space is not sufficient.
	@param bAllowDuplicateKeys Flags the queue whether duplicate keys are allowed.  If not, when
	a duplicate is inserted, an ASSERTION occurs.  Otherwise, the elements will be inserted normally,
	with the duplicate moving as little as possible.
	*/
	PriorityQueue( unsigned int iInitialSize, unsigned int iGrowSize, bool bAllowDuplicePriorities = true );

	virtual					~PriorityQueue( void );
	//@}

	/*! @name Operators */
	//@{
	/*! Get to a value by index.
	@return A pointer to the actual value stored in the queue.  This memory is owned by the queue.
	*/
	Value	*operator[]( unsigned int index ); // 0-based index of the desired item (for iteration)
	//@}

	/*! @name Methods */
	//@{

	/*! The number of elements currently in the queue.
	@return The number of elements currently in the queue.
	*/
	virtual unsigned int	Count( void );	// number of items in the queue (for iteration)

	/*! Add an element to the queue.
	@param priority [in] The priority to associate with the element.
	@param value [in] The value to associate with the element.
	*/
	virtual void			Enqueue( const Priority &priority, const Value &value );

	/*! Remove the element with the greatest priority.
	@param value [out] A copy of the value associated with the greatest priority element is stored here.
	@return <b>true</b> if a value was successfully removed from the queue.
	*/
	virtual bool			Dequeue( Value &value );

	/*! Remove the element associated with the given priority.
	@param value [in] The value to be removed from the queue.
	@param priority [out] The priority the value was associated with when inserted into the queue.  This parameter will
	only be changed if the value indicated is detected within the queue.
	@return <b>true</b> if a value was found and copied, <b>false</b> otherwise.
	*/
	virtual bool			RemoveByValue( const Value &value, Priority &priority );

	/*! Remove the element at the given index, returning the value associated with that element.
	@param index [in] Index of the element to remove.
	@param value [out] Place to hold a copy of the value removed from the queue.
	@return <b>true</b> if an element was removed, and a value copied, <b>false</b> otherwise.
	*/
	virtual bool			RemoveValueAt( unsigned int index, Value &value );

	/*! Remove the element at the given index.
	@param index [in] Index of the element to remove.
	@return <b>true</b> if an element was removed, <b>false</b> otherwise.
	*/
	virtual bool			RemoveAt( unsigned int index );

	/*! Remove all the elements.
	This method keeps all the memory allocated, but calls a destructor as appropriate.  The purpose of the method
	is to reset the data structure for reuse - without requiring more memory allocation.
	*/
	virtual void			RemoveAll( void );

	/*! Find the element with the given value.
	@param value [in] Value to search the queue for.
	@param priority [in] Priority to search for.
	@param index [out] Index of the element associated with the given priority.
	@param iStartIndex [in] Index at which to search.
	@return <b>true</b> if an element was associated with the given priority, <b>false</b> otherwise.
	*/
	virtual bool			FindValue( const Value &value, unsigned int &index );

	/*! Find the element with the given value, using the priority associated with the value when the element was Enqueued.
	@param priority [in] The priority associated with the value when it was Enqueued.
	@param value [in] The value to search for.
	@param index [out] The index at which the value was discovered - if it was in the queue.
	@param iStartIndex [in] The index at which to start the search.
	@return <b>true</b> if the value was found, <b>false</b> otherwise.
	*/
	virtual bool			FindValueWithPriority( const Priority &priority, const Value &value, unsigned int &index, unsigned int iStartIndex = 0 );

	/*! Check if the queue is empty.
	@return <b>true</b> if the queue is empty, <b>false</b> otherwise.
	*/
	virtual bool			IsEmpty( void );

	//@}

protected:

	/*!  Compare two keys for relative priority.
	Similar semantics to strcmp; if a has greater priority that b, result is 1; if b has greater priority,
	result is -1; if they have equal priority, result is 0.
	@param a [in] One of the priorities to compare.
	@param b [in] One of the priorities to compare.
	@return ( ( a > b ) ? 1 : ( ( a < b ) ? -1 : 0 ) ); comparing PRIORITY, not numerical values.
	*/
	virtual int				ComparePriorities( const Priority &a, const Priority &b ) = 0;

	/*! Recursively move highest priority child into the current element until a leaf node is found.
	Once a leaf node is reached, move the last element to fill the hole, if necessary, and return
	the index of the last element moved - at which point, the SortUp() method should be called to place
	that element in the correct index.
	@param index [in] The index at which to begin/continue the Fill operation.
	@reutrn The index at which the operation stopped.
	*/

	virtual unsigned int	FillDown( unsigned int index );
	/*! Recursively sort the given element up in the tree.  Compare the given element to it's parent,
	switching them if this element is higher priority than the parent.
	@param index [in] The index at which to begin/continue the Sort operation.
	@return The index at which the operation stopped.
	*/
	virtual unsigned int	SortUp( unsigned int index );

	typedef struct sPriorityRecord
	{
		Priority	m_Priority;
		Value*	m_pValue;
	};

	DynamicArray< sPriorityRecord* >	m_BinaryHeap;
	Pool< sPriorityRecord >				m_PriorityRecords;
	Pool< Value >							m_ValueRecords;
	bool										m_bAllowDuplicePriorities;
};

/*! Minimum priority queue.
The elements of this queue are sorted with minimum priority first.
*/
template< class Priority, class Value >
class MinQueue : public PriorityQueue< Priority, Value >
{
public:
	MinQueue();
	virtual ~MinQueue();

protected:
	virtual int				ComparePriorities( const Priority &a, const Priority &b );
};


/*! Maximum priority queue.
The elements of this queue are sorted with maximum priority first.
*/
template< class Priority, class Value >
class MaxQueue : public PriorityQueue< Priority, Value >
{
public:
	MaxQueue();
	virtual ~MaxQueue();

protected:
	virtual int				ComparePriorities( const Priority &a, const Priority &b );
};

#include "container/pqueue.hpp"

#endif