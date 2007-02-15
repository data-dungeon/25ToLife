#ifndef __TEMPLATE_LINK_H
#define __TEMPLATE_LINK_H

template <class T> class link
{
	link *					p_Next;
	T &						p_Link;	

public:
	link(
		T &					_Link,
		link *				_Next = 0
		) : p_Next( _Next), p_Link( _Link)
	{
		
	}
	
	void 					next( link * _Next) 				{ p_Next = _Next; }
	link *				next( void ) const 				{ return p_Next; }

	T &					get( void ) const					{ return p_Link; }
} ;

#endif
