#ifndef __C_ARRAY_H
#define __C_ARRAY_H

template < class T > 
class CArray
{
	unsigned int			m_ExpandBy;
	unsigned int			m_Allocated;
	unsigned int			m_Items;
	T * 						p_Array;

	 void						expand( unsigned int expandBy );

public:
	CArray( unsigned int _StartSize, unsigned int _ExpandBy = 0 );
	~CArray( );

	unsigned int 			count( void ) const						{ return m_Items; }

	unsigned int			add( T & item );
	void 						remove( unsigned int index );
	void						remove( T & item );

	T &  						get( unsigned int index ) const;
	T & 						operator[]( unsigned int index ) const;
};

#include "CArray.hpp"

#endif 
