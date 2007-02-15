#include <Assert.h>
#include <string.h>

template < class T > CArray<T>::CArray( unsigned int _StartSize, unsigned int _ExpandBy ) :
	p_Array( 0 ),
	m_Allocated( 0 ),
	m_Items( 0 ),
	m_ExpandBy( _ExpandBy )
{
	if( _StartSize )
		expand( _StartSize );
}

template < class T > CArray<T>::~CArray( void )
{
	if( p_Array )
		delete [] p_Array;
}

////////////////////////////////////////////////////////////////////////////

template <class T> unsigned int CArray<T>::add( T & item )
{
   if( (m_Items + 1) > m_Allocated )
		expand( (m_ExpandBy == 0) ? 1 : m_ExpandBy );

	p_Array[m_Items] = item;

	return m_Items++;
}

////////////////////////////////////////////////////////////////////////////

template <class T> void CArray<T>::remove( unsigned int index )
{
	assert( index < m_Items );

	for( unsigned int i = index; i < (m_Items - 1); i++ )
		p_Array[i] = p_Array[i + 1];

	m_Items--;
}

template <class T> void CArray<T>::remove( T & item )
{
	for( unsigned int i = 0; i < m_Items; i++ )
		if( p_Array[i] == item )
			remove( i );
}

////////////////////////////////////////////////////////////////////////////

template <class T> T & CArray<T>::get( unsigned int index ) const
{
	assert( index < m_Items );

	return p_Array[index];
}

////////////////////////////////////////////////////////////////////////////

template <class T> T & CArray<T>::operator[]( unsigned int index ) const
{
    assert( index < m_Items );

    return p_Array[index];
}

////////////////////////////////////////////////////////////////////////////

template <class T> void CArray<T>::expand( unsigned int addCount )
{
	assert( addCount );

	unsigned int m_NewSize = m_Allocated + addCount;

	T * p_NewArray = new T[m_NewSize];
	assert( p_NewArray );
   
	if( p_Array )
	{
		for( unsigned int i = 0; i < m_Items; i++ )
			p_NewArray[i] = p_Array[i];

		delete [] p_Array;
	}

	m_Allocated = m_NewSize;
	p_Array = p_NewArray;
}

