
template < class T > CSLinkList<T>::~CSLinkList( void )
{
	SLink<T> * pWalker = p_Head;

	while( pWalker )
	{
		SLink<T> * pRemove = pWalker;

		pWalker = pWalker->next;

		delete pRemove;
	}

	p_Head = 0;
	p_Current = 0;
	m_Items = 0;

}

template < class T > SLink<T> ** CSLinkList<T>::find( const T & item )
{
	SLink<T> ** ppLink = & p_Head;

	while( *ppLink )
	{
		if( (*ppLink)->item == item )
			return ppLink;

		ppLink = & (*ppLink)->next;
	}

	return 0;
}

template < class T > void CSLinkList<T>::add( const T & item )
{
	SLink<T> * pLink = new SLink<T>( p_Head, item );

	m_Items++;
	p_Head = pLink;

}

template < class T > void CSLinkList<T>::remove( const T & item )
{
	SLink<T> **	ppLink = find( item );

	if( ppLink  &&  *ppLink )
	{
		SLink<T> * pLink = *ppLink;
	
		*ppLink = pLink->next;

		m_Items--;
		delete pLink;

		p_Current = 0;
	}
}

template < class T > T & CSLinkList<T>::get( unsigned int index ) const
{
	assert( index < m_Items );

	SLink<T> * pWalker = p_Head;

	while( index-- )
	{
		pWalker = pWalker->next;

		assert( pWalker );
	}

	return pWalker->item;
}

template < class T > T & CSLinkList<T>::operator[]( unsigned int index ) const
{
	return get( index );
}

template < class T > T & CSLinkList<T>::first( void )
{
	assert( p_Head );

	p_Current = p_Head;

	return p_Current->item;
}

template < class T > T & CSLinkList<T>::next( const T & item )
{
	if( p_Current  &&  (p_Current->item == item) )
	{
		assert( p_Current->next );

		p_Current = p_Current->next;

	}
	else
	{
		SLink<T> ** ppLink = find( T );

		assert( ppLink );

		p_Current = *ppLink;

		assert( p_Current );
		assert( p_Current->next );

		p_Current = p_Current->next;
	}

	return p_Current->item;
}

		


