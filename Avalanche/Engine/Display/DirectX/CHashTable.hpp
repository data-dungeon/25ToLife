
#include "limits.h"
#include "CHashTable.h"

template < class T > CHashEntry<T>::CHashEntry( const T & _item, unsigned int _key ) :
item( _item ),
key( _key )
{

}

template < class T > CHashTable<T>::CHashTable( unsigned int _tableEntries ) :
	m_TableEntries( _tableEntries )
{
	p_Table = new CSLinkList<const CHashEntry<T> *>[m_TableEntries];

}

template < class T > CHashTable<T>::~CHashTable( void )
{

	for( unsigned int i = 0; i < m_TableEntries; i++ )
	{
		for( unsigned int j = 0; j < p_Table[i].count( ); j++ )
		{
			const CHashEntry<T> * pEntry = p_Table[i].get( j );

			delete pEntry;
		}
	}

	delete [] p_Table;
}

template < class T > unsigned int CHashTable<T>::index( unsigned int key )
{
	float	fVal = (float) key / (float) UINT_MAX;

	return (unsigned int) (fVal * m_TableEntries);
}

template < class T > void CHashTable<T>::insert( const T & item, unsigned int key )
{
	unsigned int idx = index( key );
	assert( idx < m_TableEntries );
	
	CHashEntry<T> * pEntry = new CHashEntry<T>( item, key );

	p_Table[idx].add( pEntry );
}

template < class T > bool CHashTable<T>::remove( const T & item, unsigned int key )
{
	unsigned int idx = index( key );
	assert( idx < m_TableEntries );

	for( unsigned int i = 0; i < p_Table[idx].count( ); i++ )
	{
		const CHashEntry<T> * pEntry = p_Table[idx].get( i );
		assert( pEntry );

		if( pEntry->item == item )
		{
			p_Table[idx].remove( pEntry );

			delete pEntry;

			return true;
		}
	}

	return false;
}

template < class T > const T * CHashTable<T>::find( const T & item, unsigned int key )
{
	unsigned int idx = index( key );
	assert( idx < m_TableEntries );

	for( unsigned int i = 0; i < p_Table[idx].count( ); i++ )
	{
		const CHashEntry<T> * pEntry = p_Table[idx].get( i );

		if( pEntry->item == item )
			return & pEntry->item;
	}

	return 0;
}


template < class T > void CHashTable<T>::forEach( CHashCallback pCallback )
{
	assert( pCallback );

	for( unsigned int i = 0; i < m_TableEntries; i++ )
	{
		SLink<const CHashEntry *> * pLink = p_Table[i].head( );

		while( pLink )
		{
			SLink<const CHashEntry *> * pNext = pLink->next;

			pCallback( * (pLink->item) );

			pLink = pNext;
		}
	}
}

#define mix(a,b,c) \
{ \
	a = a - b;		a = a - c;		a = a ^ (c>>13); \
	b = b - c;		b = b - a;		b = b ^ (a<<8);  \
	c = c - a;		c = c - b;		c = c ^ (b>>13); \
	a = a - b;		a = a - c;		a = a ^ (c>>12); \
	b = b - c;		b = b - a;		b = b ^ (a<<16); \
	c = c - a;		c = c - b;		c = c ^ (b>>5);  \
	a = a - b;		a = a - c;		a = a ^ (c>>3);  \
	b = b - c;		b = b - a;		b = b ^ (a<<10); \
	c = c - a;		c = c - b;		c = c ^ (b>>15); \
}

template < class T > unsigned int CHashTable<T>::hashPtr( const void * pPtr )
{

      const unsigned char *   d = ( const unsigned char *) & pPtr;
      unsigned int            a, b, c;  /* the internal state */

      a = b = 0x9e3779b9;     /* the golden ratio; an arbitrary value */
      c = 0;                  /* variable initialization of internal state */

      a = a + ( (unsigned int) d[3] << 24 );
      a = a + ( (unsigned int) d[2] << 16 );
      a = a + ( (unsigned int) d[1] <<  8 );
      a = a + ( (unsigned int) d[0] );

      mix( a, b, c );

      return c;
   }

template < class T > unsigned int CHashTable<T>::hashBuffer( const void * pBuffer, unsigned int length, unsigned int initVal )
{
   const unsigned char *      d = (const unsigned char *) pBuffer;
   unsigned int               a, b, c;  /* the internal state */

   /* Set up the internal state */
   a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
   c = initVal;         /* variable initialization of internal state */

   /*---------------------------------------- handle most of the key */
   while ( length >= 12 )
   {
      a = a + ( d[0] + ((unsigned int) d[ 1] << 8) + ((unsigned int) d[ 2] << 16) + ((unsigned int) d[ 3] << 24) );
      b = b + ( d[4] + ((unsigned int) d[ 5] << 8) + ((unsigned int) d[ 6] << 16) + ((unsigned int) d[ 7] << 24) );
      c = c + ( d[8] + ((unsigned int) d[ 9] << 8) + ((unsigned int) d[10] << 16) + ((unsigned int) d[11] << 24) );

      mix( a, b, c );

      d = d + 12; length = length - 12;
   }

   /*------------------------------------- handle the last 11 bytes */
   c = c + length;

   switch( length )              /* all the case statements fall through */
   {
   case 11: c = c + ((unsigned int) d[10] << 24);
   case 10: c = c + ((unsigned int) d[ 9] << 16);
   case  9: c = c + ((unsigned int) d[ 8] <<  8);
      /* the first byte of c is reserved for the length */
   case  8: b = b + ((unsigned int) d[ 7] << 24);
   case  7: b = b + ((unsigned int) d[ 6] << 16);
   case  6: b = b + ((unsigned int) d[ 5] <<  8);
   case  5: b = b +  d[4];
   case  4: a = a + ((unsigned int) d[ 3] << 24);
   case  3: a = a + ((unsigned int) d[ 2] << 16);
   case  2: a = a + ((unsigned int) d[ 1] <<  8);
   case  1: a = a +  d[0];
     /* case 0: nothing left to add */
   }

   mix( a, b, c );

   return c;
}


			

