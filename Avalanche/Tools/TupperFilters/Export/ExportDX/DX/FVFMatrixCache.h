#ifndef __FVF_MATRIX_CACHE_H
#define __FVF_MATRIX_CACHE_H

#include <assert.h>

class FVFMatrixCache
{
	enum
	{
		ENT_EMPTY 		= 0x80000000,
		ENT_SEND			= 0x40000000,
		ENT_IN_USE		= 0x20000000,
		ENT_IDX_MASK	= ~(ENT_EMPTY | ENT_SEND | ENT_IN_USE)
	};

	unsigned int		m_NumIndices;
	unsigned int *		a_Indices;

public:
	FVFMatrixCache( unsigned int _Indices )
	{
		m_NumIndices = _Indices;

		a_Indices = new unsigned int [_Indices];

		for( unsigned int i = 0; i < _Indices; i++)
			a_Indices[i] = ENT_EMPTY;

	}

	FVFMatrixCache( const FVFMatrixCache &Other )
	{
		m_NumIndices = Other.m_NumIndices;
		a_Indices = new unsigned int [m_NumIndices];

		for( unsigned int i = 0; i < m_NumIndices; i++)
			a_Indices[i] = Other.a_Indices[i];
	}

	~FVFMatrixCache( )
	{
		if( a_Indices )
			delete [] a_Indices;
	}

	FVFMatrixCache & operator = ( const FVFMatrixCache &Other )
	{
		assert( m_NumIndices == Other.m_NumIndices);

		for( unsigned int i = 0; i < m_NumIndices; i++)
			a_Indices[i] = Other.a_Indices[i];

		return * this;
	}

	bool isEmpty( unsigned int idx )
	{
		assert( idx < m_NumIndices );

		return a_Indices[idx] & ENT_EMPTY ? true : false;
	}

	bool inUse( unsigned int idx )
	{
		assert( idx < m_NumIndices );
		
		return a_Indices[idx] & ENT_IN_USE ? true : false;
	}

	bool needsSend( unsigned int idx )
	{
		assert( idx < m_NumIndices );

		return a_Indices[idx] & ENT_SEND ? true : false;
	}

	void clearSends( void )
	{
		for( unsigned int i = 0; i < m_NumIndices; i++)
			a_Indices[i] &= ~(ENT_SEND | ENT_IN_USE);
	}

	void clearAll( void )
	{
		for( unsigned int i = 0; i < m_NumIndices; i++)
			a_Indices[i] = ENT_EMPTY;
	}

	int addIndex( unsigned int Index )
	{
		unsigned int			i;

		// find an slot currently in use
		for( i = 0; i < m_NumIndices; i++)
		{
			if( !(a_Indices[i] & ENT_EMPTY) )
			{
				unsigned int currIdx = a_Indices[i] & ENT_IDX_MASK;

				if( currIdx == Index)
				{
					a_Indices[i] |= ENT_IN_USE;
					return i;
				}
			}
		}
		
		// find an empty slot
		for( i = 0; i < m_NumIndices; i++)
			if( a_Indices[i] & ENT_EMPTY )
			{
				a_Indices[i] = ENT_SEND | ENT_IN_USE | Index;
				return i;
			}

		// can we place it
		for( i = 0; i < m_NumIndices; i++)
			if( !(a_Indices[i] & ENT_IN_USE) )
			{
				a_Indices[i] = ENT_SEND | ENT_IN_USE | Index;
				return i;
			}

		return -1;
	}

	void				optimizeAgainst( const FVFMatrixCache & Prev )
	{
		assert( m_NumIndices == Prev.m_NumIndices );

		for( unsigned int i = 0; i < m_NumIndices; i++ )
			if( (a_Indices[i] & ENT_IN_USE)  &&  (a_Indices[i] & ENT_IDX_MASK) != (Prev.a_Indices[i] & ENT_IDX_MASK) )
				a_Indices[i] |= ENT_SEND;

	}

	unsigned int	numSends( void ) const
	{
		unsigned int		countSend = 0;

		for( unsigned int i = 0; i < m_NumIndices; i++)
			if( a_Indices[i] & ENT_SEND )
				countSend++;

		return countSend;
	}

	unsigned int 	size( void ) const				{ return m_NumIndices; }

	unsigned int	operator [] ( unsigned int idx ) const
	{
		assert( idx < m_NumIndices );
	
		return a_Indices[idx] & ENT_IDX_MASK;
	}	

} ;

#endif