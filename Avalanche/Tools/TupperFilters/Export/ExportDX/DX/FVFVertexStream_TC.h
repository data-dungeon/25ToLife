#ifndef __FVF_VERTEX_STREAM_TC_H
#define __FVF_VERTEX_STREAM_TC_H

#include <Math/Vector.h>

#include "FVFVertexStream.h"

template< class _TC >
class FVFVertexStream_DummyTC : public FVFVertexStream
{
	_TC					d_Dummy;

public:
	FVFVertexStream_DummyTC(
		const _TC &		_DummyTC
		) :
	FVFVertexStream( sizeof( _TC) ),
	d_Dummy( _DummyTC )
	{
	
	}

	char *					operator[]( unsigned int idx) const
	{
		return (char *) &d_Dummy;
	}

} ;

typedef FVFVertexStream_DummyTC< Vector2 >	FVFVertexStream_PadTC2;
typedef FVFVertexStream_DummyTC< Vector3 >	FVFVertexStream_PadTC3;

class FVFVertexStream_TC : public FVFVertexStream
{
	Vector3 *		p_UVVertsChannel;
	unsigned int	d_UVVertsChannelLen;

	int *				p_UVIndicesChannel;
	unsigned int	d_UVIndicesChannelLen;

	UVTransform *	p_UVTransform;

public:
	FVFVertexStream_TC(
		Vector3 *		_UVVertsChannel,
		unsigned int	_UVVertsChannelLen,

		int *				_UVIndicesChannel,
		unsigned int 	_UVIndicesChannelLen, 

		unsigned int	_bytes,
		UVTransform *	_Transform = 0
		) :
	FVFVertexStream( _bytes )
	{
		assert( _UVVertsChannel );
		assert( _UVVertsChannelLen );
		assert( _UVIndicesChannel );
		assert( _UVIndicesChannelLen );

		p_UVVertsChannel = _UVVertsChannel;
		d_UVVertsChannelLen = _UVVertsChannelLen;

		p_UVIndicesChannel = _UVIndicesChannel;
		d_UVIndicesChannelLen = _UVIndicesChannelLen;

		p_UVTransform = _Transform;
	} ;

	char *						operator[]( unsigned int idx) const
	{
	static Vector3 		tempUV;

		assert( idx < d_UVIndicesChannelLen );
		assert( p_UVIndicesChannel[idx] < (int) d_UVVertsChannelLen );

		tempUV = p_UVVertsChannel[ p_UVIndicesChannel[idx] ];

		if( p_UVTransform)
			ApplyUVTransform::Apply( *p_UVTransform, tempUV );

		return (char *) &tempUV;
	}
} ;


#endif