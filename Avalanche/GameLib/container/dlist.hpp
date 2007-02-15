template <class T>
inline DoubleList<T>::DoubleList() :
m_iCount( 0 ),
m_pHead( NULL ),
m_pTail( NULL ),
m_BucketPool()
{
}

template < class T >
inline DoubleList< T >::~DoubleList()
{
	Clear();
}

template < class T >
inline unsigned int DoubleList< T >::Count( void )
{
	return( m_iCount );
}

template < class T >
inline void DoubleList< T >::Clear( void )
{
	DoubleList< T >::DoubleList_Bucket *pCurr = m_pHead;
	while ( pCurr != NULL )
	{
		DoubleList< T >::DoubleList_Bucket *pNext = pCurr->m_pNext;
		m_BucketPool.Delete( pCurr );
		pCurr = pNext;
	}
	m_iCount = 0;
	m_BucketPool.Clear();
}

template < class T >
inline unsigned int DoubleList< T >::Unshift( const T& value )
{
	DoubleList< T >::DoubleList_Bucket *pBucket = m_BucketPool.New();
	pBucket->m_Value = value;
	pBucket->m_pNext = m_pHead;
	pBucket->m_pPrev = NULL;
	if ( m_pHead != NULL )
	{
		m_pHead->m_pPrev = pBucket;
	}
	m_pHead = pBucket;
	if ( m_pTail == NULL )
	{
		m_pTail = m_pHead;
	}

	return( ++m_iCount );
}

template < class T >
inline unsigned int DoubleList< T >::Shift( T& value )
{
	DoubleList< T >::DoubleList_Bucket *pBucket = m_pHead;
	if ( m_iCount == 0 )
	{	// no elements - nothing to shift off the list
		return( m_iCount );
	}
	else if ( m_iCount == 1 )
	{	// only one element, head/tail are the same
		m_pHead = m_pTail = NULL;
	}
	else
	{	// two or more elements - head/tail differ
		m_pHead = m_pHead->m_pNext;
		m_pHead->m_pPrev = NULL;
	}

	// copy out the value
	value = pBucket->m_Value;

	pBucket->m_pNext = NULL;
	pBucket->m_pPrev = NULL;
	m_BucketPool.Delete( pBucket );

	return( --m_iCount );
}

template < class T >
inline unsigned int DoubleList< T >::Push( const T& value )
{
	DoubleList< T >::DoubleList_Bucket *pBucket = m_BucketPool.New();
	pBucket->m_Value = value;
	pBucket->m_pPrev = m_pTail;
	pBucket->m_pNext = NULL;
	if ( m_pTail != NULL )
	{
		m_pTail->m_pNext = pBucket;
	}
	m_pTail = pBucket;
	if ( m_pHead == NULL )
	{
		m_pHead = m_pTail;
	}

	return( ++m_iCount );
}

template < class T >
inline unsigned int DoubleList< T >::Pop( T& value )
{
	DoubleList< T >::DoubleList_Bucket *pBucket = m_pTail;
	if ( m_iCount == 0 )
	{	// no elements - nothing to shift off the list
		return( m_iCount );
	}
	else if ( m_iCount == 1 )
	{	// only one element, head/tail are the same
		m_pHead = m_pTail = NULL;
	}
	else
	{	// two or more elements - head/tail differ
		m_pTail = m_pTail->m_pPrev;
		m_pTail->m_pNext = NULL;
	}

	// copy out the value
	value = pBucket->m_Value;

	pBucket->m_pNext = NULL;
	pBucket->m_pPrev = NULL;
	m_BucketPool.Delete( pBucket );

	return( --m_iCount );
}

template < class T >
inline void* DoubleList< T >::Head( void )
{
	return( ( void* )m_pHead );
}

template < class T >
inline void* DoubleList< T >::Tail( void )
{
	return( ( void* )m_pTail );
}

template < class T >
inline bool DoubleList< T >::IterateForward( void*& hIterator )
{
	if ( hIterator != NULL )
	{
		( DoubleList< T >::DoubleList_Bucket*& )hIterator = ( ( DoubleList< T >::DoubleList_Bucket*& )hIterator )->m_pNext;
		return( hIterator == NULL );
	}
	else
	{
		return( false );
	}
}

template < class T >
inline bool DoubleList< T >::IterateBack( void*& hIterator )
{
	if ( hIterator != NULL )
	{
		( DoubleList< T >::DoubleList_Bucket*& )hIterator = ( ( DoubleList< T >::DoubleList_Bucket*& )hIterator )->m_pPrev;
		return( hIterator == NULL );
	}
	else
	{
		return( false );
	}
}

template < class T >
inline bool DoubleList< T >::GetValueAt( void* hIterator, T& value )
{
	if ( hIterator != NULL )
	{
		value = ( ( DoubleList< T >::DoubleList_Bucket* )hIterator )->m_Value;
		return( true );
	}
	else
	{
		return( false );
	}
}

template < class T >
inline bool DoubleList< T >::RemoveValueAt( void* hIterator )
{
	DoubleList< T >::DoubleList_Bucket *pBucket = ( DoubleList< T >::DoubleList_Bucket* )hIterator;
	if ( pBucket != NULL )
	{	// we have a bucket of some sort

		// fix previous pointers
		if ( pBucket->m_pPrev != NULL )
		{	// not head
			pBucket->m_pPrev->m_pNext = pBucket->m_pNext;
		}
		else
		{	// head
			m_pHead = pBucket->m_pNext;
			if ( m_pHead != NULL )
			{
				m_pHead->m_pPrev = NULL;
			}
		}

		// fix next pointers
		if ( pBucket->m_pNext != NULL )
		{	// not tail
			pBucket->m_pNext->m_pPrev = pBucket->m_pPrev;
		}
		else
		{	// tail
			m_pTail = pBucket->m_pPrev;
			if ( m_pTail != NULL )
			{
				m_pTail->m_pNext = NULL;
			}
		}

		// finish extraction
		pBucket->m_pNext = NULL;
		pBucket->m_pPrev = NULL;
		m_BucketPool.Delete( pBucket );
		--m_iCount;
		return( true );
	}
	else
	{
		return( false );
	}
}
