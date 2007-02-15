#ifndef __FVF_VB_BUILDER_H
#define __FVF_VB_BUILDER_H

#include "FVFVertex.h"
#include "VertexDescriptor.h"

#include <vector>
#include <set>

typedef std::vector<FVFVertex>	FVFVertexArray;

class FVFVertexRef
{
	const FVFVertexArray	*	p_array;
	unsigned int				d_idx;

	const FVFVertexRef & operator= (const FVFVertexRef & Other);
//	{
//		d_vtx = Other.d_vtx;
//		d_idx = Other.d_idx;
//	}
public:
	FVFVertexRef
	( 
		const FVFVertexArray	&		_array, 
		unsigned int 					_idx
	) :
		p_array( &_array),
		d_idx( _idx)
	{		
	}

	FVFVertexRef
	( 
		const FVFVertexRef & Other
	) :
		p_array( Other.p_array ),
		d_idx( Other.d_idx)
	{
	}

	unsigned int 	idx( void ) const 			{ return d_idx; }
	void				idx( unsigned int _idx)		{ d_idx = _idx; }

	bool operator< ( const FVFVertexRef & Other) const
	{
		assert( p_array );
		assert( d_idx < p_array->size( ) );
		assert( Other.p_array );
		assert( Other.d_idx < Other.p_array->size( ) );

		assert( (*p_array)[d_idx].bytes( ) == (*Other.p_array)[Other.d_idx].bytes( ) );

		return (*p_array)[d_idx] < (*Other.p_array)[Other.d_idx];
	}

	void reparent( const FVFVertexArray & parent )
	{
		p_array = & parent;
	}


} ;

typedef std::set<FVFVertexRef> 	FVFVertexRefSet;

class FVFVBBuilder
{
	bool							b_Closed;

	unsigned int				d_VBBytes;
	
	unsigned int				d_MaxVertices;

	FVFVertexRefSet			d_RefSet;

	FVFVBBuilder( );

public:
	VertexDescriptor			d_Desc;
	FVFVertexArray				d_VB;

	FVFVBBuilder( 
		const VertexDescriptor &	_Desc,
		unsigned int					_MaxVertices
	) :
		d_Desc( _Desc),
		d_VBBytes( 0 ),
		d_MaxVertices( _MaxVertices )
	{
		b_Closed = false;	
	}

	FVFVBBuilder( 
		const FVFVBBuilder &			Other
	) :
		d_Desc( Other.d_Desc ),
		d_RefSet( Other.d_RefSet ),
		d_VB( Other.d_VB ),
		d_VBBytes( Other.d_VBBytes ),
		d_MaxVertices( Other.d_MaxVertices ),
		b_Closed( Other.b_Closed )
	{
		std::set< FVFVertexRef >::iterator	it;

		for( it = d_RefSet.begin( ); it != d_RefSet.end( ); ++it )
		{
			(*it).reparent( d_VB );
		}

	}

	FVFVBBuilder &			operator= ( const FVFVBBuilder &Other)
	{
		d_Desc = Other.d_Desc;

		d_RefSet = Other.d_RefSet;
		d_VB = Other.d_VB;
		
		d_VBBytes = Other.d_VBBytes;
		d_MaxVertices = Other.d_MaxVertices;
		b_Closed = Other.b_Closed;

		std::set< FVFVertexRef >::iterator	it;

		for( it = d_RefSet.begin( ); it != d_RefSet.end( ); ++it )
		{
			(*it).reparent( d_VB );
		}

		return *this;
	}

	bool						full( void ) const	{ return (d_VB.size( ) == d_MaxVertices)  ||  b_Closed; }

	bool						operator< ( const FVFVBBuilder &Other ) const
	{
		if( d_Desc < Other.d_Desc )
			return true;

		return d_VB < Other.d_VB;
	}

	bool						operator== ( const FVFVBBuilder &Other ) const
	{
		if( d_Desc != Other.d_Desc )
			return false;

		return d_VB == Other.d_VB;
	}

	const FVFVertex &		operator[] ( unsigned int idx ) const
	{
		assert( idx < d_VB.size( ) );

		return d_VB[idx];
	}

	void						changeDescriptor( const VertexDescriptor & newDesc );
	void						close( void )					{ b_Closed = true; }

	unsigned int			vertices( void ) const		{ return d_VB.size( ); }
	unsigned int			verticesMax( void ) const	{ return d_MaxVertices; }
	unsigned int			verticesAvailable( void ) const	{ return d_MaxVertices - d_VB.size( ); }

	unsigned int			bytes( void ) const			{ return d_VBBytes; }

	int 						addVertex( const FVFVertex & vtx );

} ;


#endif
