#ifndef __C_S_LINK_LIST_H
#define __C_S_LINK_LIST_H

template < class S >
class SLink
{
public:
	SLink *				next;
	S						item;

	SLink( 
		SLink *			_next,
		S					_item
		) :
	next( _next ),
	item( _item )
	{

	}
} ;

template < class T >
class CSLinkList
{
	unsigned int			m_Items;

	SLink<T> *				p_Head;
	SLink<T> *				p_Current;

	SLink<T> **				find( const T & item );

public:
	CSLinkList( ) 
	{
		p_Head = 0;
		p_Current = 0;
		m_Items = 0;
	}

	~CSLinkList( );

	unsigned int			count( void ) const					{ return m_Items; }
	
	void						add( const T & item );
	void						remove( const T & item );

	SLink<T> *				head( void )							{ return p_List; }

	T &						first( void );
	T &						next( const T & item );

	T &						get( unsigned int index ) const;
	T &						operator[]( unsigned int index ) const;
} ;

#include <Assert.h>
#include "CSLinkList.hpp"

#endif