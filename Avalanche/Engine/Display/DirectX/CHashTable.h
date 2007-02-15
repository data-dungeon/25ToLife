#ifndef C_HASH_TABLE_H
#define C_HASH_TABLE_H

#include "CSLinkList.h"

template < class T >
class CHashEntry
{
public:
	T							item;
	unsigned int			key;

	CHashEntry( 
		const T &				_item,
		unsigned int			_key
		) ;
} ;

template < class T >
class CHashTable
{
	typedef void (* CHashCallback)( T & item );

	unsigned int					m_TableEntries;
	CSLinkList<const CHashEntry<T> *> *	p_Table;

	unsigned int			index( unsigned int key );
public:
	CHashTable( unsigned int _tableEntries );
	~CHashTable( );

	static unsigned int	hashPtr( const void * pPtr );
	static unsigned int	hashBuffer( const void * pBuffer, unsigned int length, unsigned int initVal = 0 );

	void						insert( const T & item, unsigned int key );
	bool						remove( const T & item, unsigned int key );

	const T *				find( const T & item, unsigned int key );

	void						forEach( CHashCallback pCallBack );

} ;

#include "CHashTable.hpp"

#endif
