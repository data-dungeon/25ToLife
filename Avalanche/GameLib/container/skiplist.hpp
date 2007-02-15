/********************************************************************
	created:	2003/12/22
	created:	22:12:2003   15:28
	filename: 	c:\dev\avalanche\gamelib\container\skiplist.hpp
	file path:	c:\dev\avalanche\gamelib\container
	file base:	skiplist
	file ext:	hpp
	author:		Shon C. Love
	
	purpose:	Implementation of a templated skip list.

	additional:
				http://www.cs.umd.edu/~pugh/skiplist.pdf or
				http://epaperpress.com/sortsearch/download/skiplist.pdf

*********************************************************************/

#include "new.h"

// sanity check - this number may be increased as needed, but
// this value should be adequate for most cases.
static const unsigned int s_iSkipListMaxSaneLevel = 24;

template< class Key, class Value, unsigned int DefaultMaxLevel >
SkipList< Key, Value, DefaultMaxLevel >::SkipList( void ) :
m_iCurrMaxLevel( 0 ),
m_iMaxListLevel( DefaultMaxLevel - 1 ),		// index variable
m_iNodeCount( 0 ),
m_pCurrIterationNode( NULL )
{
	ASSERT( DefaultMaxLevel <= s_iSkipListMaxSaneLevel );
	for ( unsigned int i = 0; i < DefaultMaxLevel; ++i )
	{
		m_pNodePools[ i ] = new Pool< SkipList< Key, Value, DefaultMaxLevel >::SkipListNode >( 16, 16, ( sizeof( SkipList< Key, Value, DefaultMaxLevel >::SkipListNode ) + ( sizeof( SkipListNode* ) * ( i + 1 ) ) ) );
	}
	m_pHead = CreateNode( DefaultMaxLevel );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
SkipList< Key, Value, DefaultMaxLevel >::~SkipList( void )
{
	Clear();

	if ( m_pHead != NULL )
	{
		m_pNodePools[ m_pHead->m_iLevel ]->Delete( m_pHead );

		m_pHead = NULL;
	}

	m_pCurrIterationNode = NULL;

	for ( unsigned int i = 0; i < DefaultMaxLevel; ++i )
	{
		delete m_pNodePools[ i ];
		m_pNodePools[ i ] = NULL;
	}
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
bool SkipList< Key, Value, DefaultMaxLevel >::Iterate( Key &key, Value &item )
{
	if ( m_pCurrIterationNode == NULL )
	{
		return( false );
	}

	key = m_pCurrIterationNode->m_Key;
	item = m_pCurrIterationNode->m_Value;
	m_pCurrIterationNode = m_pCurrIterationNode->m_pChildren[ 0 ];
	return( true );
}


template< class Key, class Value, unsigned int DefaultMaxLevel >
bool SkipList< Key, Value, DefaultMaxLevel >::Iterate( Value &item )
{
	if ( m_pCurrIterationNode == NULL )
	{
		return( false );
	}

	item = m_pCurrIterationNode->m_Value;
	m_pCurrIterationNode = m_pCurrIterationNode->m_pChildren[ 0 ];
	return( true );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
void SkipList< Key, Value, DefaultMaxLevel >::IterateInit( void )
{
	m_pCurrIterationNode = m_pHead->m_pChildren[ 0 ];
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
unsigned int SkipList< Key, Value, DefaultMaxLevel >::Count( void ) const
{
	return( m_iNodeCount );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
inline bool SkipList< Key, Value, DefaultMaxLevel >::Insert( const Key &key, const Value &value )
{
	SkipListNode	*update[ s_iSkipListMaxSaneLevel + 1 ];
	SkipListNode	*pCurrNode = NULL;

	pCurrNode = m_pHead;
	int iCurrLevel;
	for( iCurrLevel = m_iMaxListLevel; iCurrLevel >= 0; iCurrLevel-- )
	{
		while (
			( pCurrNode->m_pChildren[ iCurrLevel ] != NULL ) &&
			( pCurrNode->m_pChildren[ iCurrLevel ]->m_Key < key )
			)
		{
			pCurrNode = pCurrNode->m_pChildren[ iCurrLevel ];
		}

		update[ iCurrLevel ] = pCurrNode;
	}

	pCurrNode = pCurrNode->m_pChildren[ 0 ];
	if (
		( pCurrNode != NULL ) &&
		( pCurrNode->m_Key == key )
		)
	{
		// duplicate key insertion
		// action is to replace current value with new value
		pCurrNode->m_Value = value;
		return( true );
	}

	/* determine level */
	unsigned int iNewLevel = rand() % DefaultMaxLevel;
	if (
		( m_iCurrMaxLevel < m_iMaxListLevel ) &&
		( iNewLevel > m_iCurrMaxLevel )
		)
	{
		iNewLevel = m_iCurrMaxLevel++;
	}

	/* make new node */
	pCurrNode = CreateNode( iNewLevel + 1 );//new SkipListNode( iNewLevel + 1 );
	pCurrNode->m_Key = key;
	pCurrNode->m_Value = value;
	m_iNodeCount++;

	/* update forward links */
	for( iCurrLevel = 0; ( unsigned int )iCurrLevel <= iNewLevel; iCurrLevel++ )
	{
		pCurrNode->m_pChildren[ iCurrLevel ] = update[ iCurrLevel ]->m_pChildren[ iCurrLevel ];
		update[ iCurrLevel ]->m_pChildren[ iCurrLevel ] = pCurrNode;
	}

	return( true );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
inline bool SkipList< Key, Value, DefaultMaxLevel >::Delete( const Key &key )
{
	SkipListNode *update[ s_iSkipListMaxSaneLevel + 1 ], *pCurrNode;

	// find where data belongs
	pCurrNode = m_pHead;
	int i;
	for( i = m_iCurrMaxLevel; i >= 0; i-- )
	{
		while (
			( pCurrNode->m_pChildren[ i ] != NULL ) &&
			( pCurrNode->m_pChildren[ i ]->m_Key < key )
			)
		{
			pCurrNode = pCurrNode->m_pChildren[ i ];
		}

		update[ i ] = pCurrNode;
	}

	pCurrNode = pCurrNode->m_pChildren[ 0 ];
	if (
		( pCurrNode == NULL ) ||
		( !( pCurrNode->m_Key == key ) )
		)
	{
		return( false );
	}

	// adjust forward pointers
	for( i = 0; i <= (signed) m_iCurrMaxLevel; i++ )
	{
		if ( update[ i ]->m_pChildren[ i ] != pCurrNode )
		{
			break;
		}
		update[ i ]->m_pChildren[ i ] = pCurrNode->m_pChildren[ i ];
	}

	m_pNodePools[ pCurrNode->m_iLevel ]->Delete( pCurrNode );

	pCurrNode = NULL;
	m_iNodeCount--;

	// adjust header level
	while (
		( m_iCurrMaxLevel > 0 ) &&
		( m_pHead->m_pChildren[ m_iCurrMaxLevel ] == NULL )
		)
	{
		m_iCurrMaxLevel--;
	}

	return( true );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
inline bool SkipList< Key, Value, DefaultMaxLevel >::Find( const Key &key, Value &value )
{
	SkipListNode *pCurrNode = m_pHead;

	for( int i = m_iCurrMaxLevel; i >= 0; i-- )
	{
		while (
			( pCurrNode->m_pChildren[ i ] != NULL ) &&
			( pCurrNode->m_pChildren[ i ]->m_Key < key )
			)
		{
			pCurrNode = pCurrNode->m_pChildren[ i ];
		}
	}

	pCurrNode = pCurrNode->m_pChildren[ 0 ];
	if (
		( pCurrNode != NULL ) &&
		( pCurrNode->m_Key == key )
		)
	{
		value = pCurrNode->m_Value;
		return( true );
	}

	return( false );
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
inline void SkipList< Key, Value, DefaultMaxLevel >::Clear( void )
{
	ASSERT( m_pHead->m_pChildren != NULL );
	SkipListNode *pCurr = m_pHead->m_pChildren[ 0 ];
	SkipListNode *pNext = NULL;

	// delete all the storage buckets
	while ( pCurr != NULL )
	{
		pNext = pCurr->m_pChildren[ 0 ];
		m_pNodePools[ pCurr->m_iLevel ]->Delete( pCurr );
		pCurr = pNext;
	}

	// reset the head node
	for( unsigned int i = 0; i < m_iMaxListLevel; i++ )
	{
		m_pHead->m_pChildren[ i ] = NULL;
	}

	// reset our counters
	m_iCurrMaxLevel = 0;
	m_iNodeCount = 0;
	m_pCurrIterationNode = NULL;
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
inline typename SkipList< Key, Value, DefaultMaxLevel >::SkipListNode * SkipList< Key, Value, DefaultMaxLevel >::CreateNode( unsigned int iLevel )
{
	ASSERT( iLevel >= 1 );
	unsigned int iObjectSize = sizeof( SkipList< Key, Value, DefaultMaxLevel >::SkipListNode );
	unsigned int iAllocSize = iObjectSize + ( sizeof( SkipListNode* ) * iLevel );
	ASSERT( m_pNodePools[ iLevel - 1 ] != NULL );
	unsigned char *buff = ( unsigned char * )m_pNodePools[ iLevel - 1 ]->NewPtr();// new unsigned char[ iAllocSize ];
	SkipListNode *pReturn = new ( buff ) SkipListNode( iLevel );
	return( pReturn );
}

//////////////////////////////////////////////////////////////////////////

template< class Key, class Value, unsigned int DefaultMaxLevel >
SkipList< Key, Value, DefaultMaxLevel >::SkipListNode::SkipListNode( unsigned int iLevel ) :
m_pChildren( NULL ),
m_iLevel( 0xFFFFFFFF )
{
	ASSERT( iLevel >= 1 );
	m_iLevel = iLevel - 1;
	m_pChildren = ( SkipListNode** )( ( unsigned int )this + ( sizeof( SkipList< Key, Value, DefaultMaxLevel >::SkipListNode ) ) );
	for( unsigned int i = 0; i <= m_iLevel; ++i )
	{
		m_pChildren[ i ] = NULL;
	}
}

template< class Key, class Value, unsigned int DefaultMaxLevel >
SkipList< Key, Value, DefaultMaxLevel >::SkipListNode::~SkipListNode( void )
{
	for( unsigned int i = 0; i <= m_iLevel; ++i )
	{
		m_pChildren[ i ] = NULL;
	}
	m_pChildren = NULL;
	m_iLevel = 0xFFFFFFFF;
}

