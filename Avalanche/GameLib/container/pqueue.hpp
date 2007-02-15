/********************************************************************
created:	2003/12/31
created:	31:12:2003   15:02
file base:	pqueue
file ext:	hpp
author:		Shon C. Love

purpose:	Implementation of PriorityQueue.
*********************************************************************/

template< class Priority, class Value >
PriorityQueue< Priority, Value >::PriorityQueue< Priority, Value >( unsigned int iInitialSize, unsigned int iGrowSize, bool bAllowDuplicePriorities ) :
m_BinaryHeap( iInitialSize + 1, iGrowSize ),
m_PriorityRecords( iInitialSize, iGrowSize ),
m_ValueRecords( iInitialSize, iGrowSize ),
m_bAllowDuplicePriorities( bAllowDuplicePriorities )
{
}

template< class Priority, class Value >
PriorityQueue< Priority, Value >::~PriorityQueue< Priority, Value >( void )
{
	// call all the destructor methods
	for ( int i = 1; i < m_BinaryHeap.Count(); ++i )
	{
		if ( m_BinaryHeap[ i ] != NULL )
		{
			if ( m_BinaryHeap[ i ]->m_pValue != NULL )
			{
				m_ValueRecords.Delete( m_BinaryHeap[ i ]->m_pValue );
			}
			m_PriorityRecords.Delete( m_BinaryHeap[ i ] );
		}
	}

	// clear out all the structures
	m_BinaryHeap.Clear();
	m_BinaryHeap.Reset();
	m_PriorityRecords.Clear();
	m_ValueRecords.Clear();
}

template< class Priority, class Value >
Value* PriorityQueue< Priority, Value >::operator[]( unsigned int index )
{
	ASSERT( ( index + 1 ) < ( unsigned int )m_BinaryHeap.Count() );
	return( m_BinaryHeap[ index + 1 ]->m_pValue );
}

template< class Priority, class Value >
unsigned int PriorityQueue< Priority, Value >::Count( void )
{
	if ( m_BinaryHeap.Count() <= 1 )
	{
		return( 0 );
	}
	else
	{
		return( ( unsigned int )( m_BinaryHeap.Count() - 1 ) );
	}
}

template< class Priority, class Value >
void PriorityQueue< Priority, Value >::Enqueue( const Priority &priority, const Value &value )
{
	unsigned int index = 0xffffffff;
	if ( ( !m_bAllowDuplicePriorities ) && ( FindValueWithPriority( priority, value, index ) ) )
	{
		ASSERTS( false, "Attempt to insert duplicate priority value in a uniqe-priority queue." );
	}

	if ( m_BinaryHeap.Count() == 0 )
	{	// NULL-out the first element of the binary heap - so the index math works out for the tree
		m_BinaryHeap.Add( NULL );
	}

	sPriorityRecord *pRecord = m_PriorityRecords.New();
	Value *pValue = m_ValueRecords.New();
	*pValue = value;
	pRecord->m_Priority = priority;
	pRecord->m_pValue = pValue;
	m_BinaryHeap.Add( pRecord );

	/* -1 for the empty [0] slot in the heap, and -1 because Count() is a count, not an index */
	index = ( unsigned int )( m_BinaryHeap.Count() - 2 );
	index = SortUp( index );
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::Dequeue( Value &value )
{
	return( RemoveValueAt( 0, value ) );
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::RemoveByValue( const Value &value, Priority &priority )
{
	unsigned int index = 0;
	if ( FindValue( value, index ) )
	{
		// copy the priority associated with the value
		priority = m_BinaryHeap[ index + 1 ]->m_Priority;
		// remove the element from the queue
		return( RemoveAt( index ) );
	}
	else
	{
		return( false );
	}
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::RemoveValueAt( unsigned int index, Value &value )
{
	unsigned int uMaxIndex = ( unsigned int )m_BinaryHeap.Count();

	if ( uMaxIndex <= 1 )
	{	// empty queue
		return( false );
	}
	else
	{
		uMaxIndex -= 1;
	}

	// we're using m_BinaryHeap array as a 1-based array (ignore the '0' element).
	index += 1;

	if ( index > uMaxIndex )
	{	// invalid index
		return( false );
	}

	// save a copy of the value to return
	value = *( m_BinaryHeap[ index ]->m_pValue );

	// save this data for later removal
	sPriorityRecord *pPriorityRecord = m_BinaryHeap[ index ];

	/* index was 1-based before here */
	unsigned int uZeroIndex = ( index - 1 );
	uZeroIndex = FillDown( uZeroIndex );
	uZeroIndex = SortUp( uZeroIndex );

	// now clean up the data associated with the value that was in the hole
	m_ValueRecords.Delete( pPriorityRecord->m_pValue );
	m_PriorityRecords.Delete( pPriorityRecord );

	return( true );
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::RemoveAt( unsigned int index )
{
	unsigned int iMaxIndex = ( unsigned int )m_BinaryHeap.Count();

	if ( iMaxIndex <= 1 )
	{	// empty queue
		return( false );
	}

	// we're using m_BinaryHeap array as a 1-based array (ignore the '0' element).
	index += 1;
	iMaxIndex -= 1;

	if ( index > iMaxIndex )
	{	// invalid index
		return( false );
	}

	// save this data for later removal
	sPriorityRecord *pPriorityRecord = m_BinaryHeap[ index ];

	/* index was 1-based before here */
	unsigned int uZeroIndex = ( index - 1 );
	uZeroIndex = FillDown( uZeroIndex );
	uZeroIndex = SortUp( uZeroIndex );

	// now clean up the data associated with the value that was in the hole
	m_ValueRecords.Delete( pPriorityRecord->m_pValue );
	m_PriorityRecords.Delete( pPriorityRecord );

	return( true );
}

template< class Priority, class Value >
void PriorityQueue< Priority, Value >::RemoveAll( void )
{
	// attempt to keep all memory allocated, just clear the structure for future use.

	for( int i = 0; i < m_BinaryHeap.Count(); ++i )
	{
		if ( m_BinaryHeap[ i ] != NULL )
		{
			if ( m_BinaryHeap[ i ]->m_pValue != NULL )
			{
				m_ValueRecords.Delete( m_BinaryHeap[ i ]->m_pValue );
				m_BinaryHeap[ i ]->m_pValue = NULL;
			}
			m_PriorityRecords.Delete( m_BinaryHeap[ i ] );
			m_BinaryHeap[ i ] = NULL;
		}
	}

	m_BinaryHeap.Clear();
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::FindValue( const Value &value, unsigned int &index )
{
	if ( m_BinaryHeap.Count() <= 1 )
	{	// SANITY CHECK
		return( false );
	}

	unsigned int uMaxIndex = ( unsigned int )( m_BinaryHeap.Count() - 1 );

	// we're stuck with a linear search in this method, as we know nothing about the
	// priority associated with the value upon insertion
	for ( unsigned int uIndex = 1; uIndex <= uMaxIndex; ++uIndex )
	{
		if ( *( m_BinaryHeap[ uIndex ]->m_pValue ) == value )
		{
			/* return a 0-based index */
			index = ( uIndex - 1 );
			return( true );
		}
	}

	return( false );
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::FindValueWithPriority( const Priority &priority, const Value &value, unsigned int &index, unsigned int iStartIndex )
{
	//!@todo IMPLEMENT THIS FOR REAL!!
	return( FindValue( value, index ) );

	// since we know the priority associated with the value upon insertion to the queue, we can use that to search the queue more efficiently.
}

template< class Priority, class Value >
bool PriorityQueue< Priority, Value >::IsEmpty( void )
{
	return( ( bool )( m_BinaryHeap.Count() <= 1 ) );
}

template< class Priority, class Value >
unsigned int PriorityQueue< Priority, Value >::FillDown( unsigned int index )
{
	// move the highest priority child into this index until a 'leaf' node is reached

	unsigned int uMaxIndex = ( unsigned int )( m_BinaryHeap.Count() );
	if ( uMaxIndex <= 1 )
	{	// no elements, return 0-based index
		return( 0 );
	}
	else
	{	// adjust this value to the last valid 0-based *index* in the binary heap
		uMaxIndex -= 1;
	}

	// adjust index to our BinaryHeap-style 1-based index
	index += 1;

	unsigned int iLeftIndex = ( index << 1 );
	unsigned int iRightIndex = iLeftIndex + 1;

	if ( iLeftIndex > uMaxIndex )
	{	// no children

		// attempt to yank the last element into this slot
		if ( index != uMaxIndex )
		{	// this element is not the last one in the queue, copy the last one to this slot
			m_BinaryHeap[ index ] = m_BinaryHeap[ uMaxIndex ];
		}

		// remove the last element
		m_BinaryHeap[ uMaxIndex ] = NULL;
		m_BinaryHeap.Delete( uMaxIndex );
		ASSERT( m_BinaryHeap.Count() >= 1 );

		// NOTE we don't actually free the memory associated with the last slot - it should've been
		// copied elsewhere - this method *only* copies elements in the heap to fill a hole

		return( index - 1 );	/* adjust index to be 0-based */
	}
	else if ( iRightIndex > uMaxIndex )
	{	// in the case of one child - that child *is* the last element in the heap - move it here
		m_BinaryHeap[ index ] = m_BinaryHeap[ iLeftIndex ];
		m_BinaryHeap[ iLeftIndex ] = NULL;
		m_BinaryHeap.Delete( -1 );
		ASSERT( m_BinaryHeap.Count() >= 1 );
		return( index - 1 );	/* adjust index to be 0-based */
	}
	else
	{	// two children - move the highest priority, and recurse
		int iHighestPriorityChild = ( ComparePriorities( m_BinaryHeap[ iLeftIndex ]->m_Priority, m_BinaryHeap[ iRightIndex ]->m_Priority ) ) > 0 ? iLeftIndex : iRightIndex ;

		m_BinaryHeap[ index ] = m_BinaryHeap[ iHighestPriorityChild ];
		return( FillDown( iHighestPriorityChild - 1 ) );	/* adjust index to be 0-based again */
	}
}

template< class Priority, class Value >
unsigned int PriorityQueue< Priority, Value >::SortUp( unsigned int index )
{
	unsigned int uMaxIndex = ( unsigned int )( m_BinaryHeap.Count() );
	if ( uMaxIndex <= 1 )
	{
		return( 0 );
	}
	else
	{
		// -1 because Count() isn't an index.
		uMaxIndex -= 1;
	}

	// work with index as a 1-based index
	index += 1;

	if ( ( index >= 1 ) && ( index <= uMaxIndex ) )
	{	// valid index to work with

		// compare vs. the parent
		unsigned int uParentIndex = ( index >> 1 );

		if ( uParentIndex < 1 )
		{	// parent is < index, so we've passed the head of the binary heap, so we're done
			return( index - 1 );
		}

		int iCompare = ComparePriorities( m_BinaryHeap[ index ]->m_Priority, m_BinaryHeap[ uParentIndex ]->m_Priority );
		if ( iCompare > 0 )
		{	// index > iParentIndex
			sPriorityRecord *pTemp = m_BinaryHeap[ index ];
			m_BinaryHeap[ index ] = m_BinaryHeap[ uParentIndex ];
			m_BinaryHeap[ uParentIndex ] = pTemp;
			return( SortUp( uParentIndex - 1 ) );
		}
	}

	return( index - 1 );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template< class Priority, class Value >
MinQueue< Priority, Value >::MinQueue() :
PriorityQueue< Priority, Value >( 256, 256, true )
{
}

template< class Priority, class Value >
MinQueue< Priority, Value >::~MinQueue< Priority, Value >()
{
}

template< class Priority, class Value >
int MinQueue< Priority, Value >::ComparePriorities( const Priority &a, const Priority &b )
{
	if ( a > b )
	{
		return( -1 );
	}
	else if ( b > a )
	{
		return( 1 );
	}
	else
	{
		return( 0 );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

template< class Priority, class Value >
MaxQueue< Priority, Value >::MaxQueue() :
PriorityQueue< Priority, Value >( 256, 256, true )
{
}

template< class Priority, class Value >
MaxQueue< Priority, Value >::~MaxQueue< Priority, Value >()
{
}

template< class Priority, class Value >
int MaxQueue< Priority, Value >::ComparePriorities( const Priority &a, const Priority &b )
{
	if ( a > b )
	{
		return( 1 );
	}
	else if ( b > a )
	{
		return( -1 );
	}
	else
	{
		return( 0 );
	}
}
