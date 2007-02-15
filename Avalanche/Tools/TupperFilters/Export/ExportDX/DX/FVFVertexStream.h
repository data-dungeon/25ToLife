#ifndef __FVF_VERTEX_STREAM_H
#define __FVF_VERTEX_STREAM_H

#include "FVFMatrixCache.h"

class FVFVertexStream
{
protected:

	unsigned int 	d_width;

public:

	FVFVertexStream( 
		unsigned int 	_bytes 
		) 	
	{ 
			d_width = _bytes; 
	}

	unsigned int				bytes( void ) const 	{ return d_width; }

	virtual char *			  	operator[]( unsigned int idx) const = 0;
} ;


template <class InT> 
class FVFVertexStream_Simple : public FVFVertexStream
{
	InT *				p_stream;
	unsigned int	d_streamLen;

public:
	FVFVertexStream_Simple( 
		InT * 			_stream, 
		unsigned int 	_streamLen, 
		unsigned int	_stride
		) :
	FVFVertexStream( sizeof( InT ) )
	{ 
		assert( _stream );
		assert( _streamLen );
		assert( _stride );

		p_stream = _stream; 
		d_streamLen = _streamLen;
	}

	char *						operator[]( unsigned int idx) const
	{
		assert( idx < d_streamLen );

		return (char *) (p_stream + idx);
	}

} ;

extern unsigned int	dbgIdx, dbgSlot;
extern unsigned int  dbgStripIndex, dbgMLIndex;

template< class InT >
class FVFVertexStream_Indexed : public FVFVertexStream
{
	InT *					p_stream;
	unsigned int		d_streamLen;

	int *					p_indices;
	unsigned int		d_indicesLen;

public:
	FVFVertexStream_Indexed(
		InT *				_stream,
		unsigned int	_streamLen,
		int *				_indices,
		unsigned int	_indicesLen
		) :
	FVFVertexStream( sizeof( InT ) )
	{
		assert( _stream );
		assert( _streamLen );

		assert( _indices );
		assert( _indicesLen );

		p_stream = _stream;
		d_streamLen = _streamLen;

		p_indices = _indices;
		d_indicesLen = _indicesLen;
	}

	char *						operator[] (unsigned int idx) const
	{
		assert( idx < d_indicesLen );
		assert( p_indices[idx] < (int) d_streamLen );

		return (char *) (p_stream + p_indices[idx]);
	}
} ;

typedef FVFVertexStream_Indexed<Vector3>	FVFVertexStream_Position;
typedef FVFVertexStream_Indexed<Vector3>	FVFVertexStream_Normal;

class FVFVertexStream_Vector3Packed16 : public FVFVertexStream
{
	Vector3 *			p_stream;
	unsigned int		d_streamLen;

	int *					p_indices;
	unsigned int		d_indicesLen;

public:
	FVFVertexStream_Vector3Packed16(
		Vector3 *		_stream,
		unsigned int	_streamLen,
		int *				_indices,
		unsigned int	_indicesLen
		) :
	FVFVertexStream( 3 * sizeof( short ) )
	{
		assert( _stream );
		assert( _streamLen );

		assert( _indices );
		assert( _indicesLen );

		p_stream = _stream;
		d_streamLen = _streamLen;

		p_indices = _indices;
		d_indicesLen = _indicesLen;
	}

	char *						operator[] (unsigned int idx) const
	{
		assert( idx < d_indicesLen );
		assert( p_indices[idx] < (int) d_streamLen );

		static short			output[3] = { 0 };

		output[0] = (short) (p_stream[p_indices[idx]].x( ) * 32767.0f );
		output[1] = (short) (p_stream[p_indices[idx]].y( ) * 32767.0f );
		output[2] = (short) (p_stream[p_indices[idx]].z( ) * 32767.0f );

		return (char *) output;
	}
} ;

class FVFVertexStream_PositionW : public FVFVertexStream
{
	Vector3 *			p_PositionStream;
	unsigned int		d_PositionStreamLen;

	int *					p_PositionIndices;
	unsigned int		d_PositionIndicesLen;

	int *					p_BoneIndices;

	FVFMatrixCache &	m_MatrixCache;

public:
	FVFVertexStream_PositionW(
		Vector3 *		_PositionStream,
		unsigned int	_PositionStreamLen,

		int *				_PositionIndices,
		unsigned int	_PositionIndicesLen,

		int *				_BoneIndices,
		FVFMatrixCache & _MatrixCache
		) :
	FVFVertexStream( sizeof( Vector4) ),
	m_MatrixCache( _MatrixCache )
	{ 
	
		p_PositionStream = _PositionStream;
		d_PositionStreamLen = _PositionStreamLen;

		p_PositionIndices = _PositionIndices;
		d_PositionIndicesLen = _PositionIndicesLen;

		p_BoneIndices = _BoneIndices;
	}

	char *						operator[] (unsigned int idx) const
	{
		assert( idx < d_PositionIndicesLen );
		assert( p_PositionIndices[idx] < (int) d_PositionStreamLen );

		static Vector4		PositionW;

		PositionW = p_PositionStream[ p_PositionIndices[idx] ];

		unsigned int idxMatrix = m_MatrixCache.addIndex( p_BoneIndices[ p_PositionIndices[idx] ] );
		assert( idxMatrix != - 1);

		dbgIdx = p_BoneIndices[ p_PositionIndices[idx] ];
		dbgSlot = idxMatrix;

		float fBoneId = (float) idxMatrix;

		PositionW.w( fBoneId);

		return (char *) &PositionW;
	}
} ;

typedef struct _argbDWORD
{
	u8					B, G, R, A;
} argbDWORD;

class FVFVertexStream_RGBA : public FVFVertexStream
{
	Vector3 *			p_RGBStream;
	unsigned int		d_RGBStreamLen;

	int *					p_RGBIndices;
	unsigned int		d_RGBIndicesLen;

	Vector3 *			p_AlphaStream;
	unsigned int		d_AlphaStreamLen;

	int *					p_AlphaIndices;
	unsigned int		d_AlphaIndicesLen;

public:
	FVFVertexStream_RGBA(
		Vector3 *			_RGBStream,
		unsigned int		_RGBStreamLen,

		int *					_RGBIndices,
		unsigned int		_RGBIndicesLen,

		Vector3 *			_AlphaStream,
		unsigned int		_AlphaStreamLen,

		int *					_AlphaIndices,
		unsigned int		_AlphaIndicesLen
		) :
	FVFVertexStream( 4 )
	{		
		p_RGBStream = _RGBStream;

		if( p_RGBStream )
		{
			assert( _RGBStreamLen );
			assert( _RGBIndices );
			assert( _RGBIndicesLen );	

			d_RGBStreamLen = _RGBStreamLen;

			p_RGBIndices = _RGBIndices;
			d_RGBIndicesLen = _RGBIndicesLen;
		}

		p_AlphaStream = _AlphaStream;

		if( p_AlphaStream )
		{	
			assert( _AlphaStreamLen );
			assert( _AlphaIndices );
			assert( _AlphaIndicesLen );

			d_AlphaStreamLen = _AlphaStreamLen;

			p_AlphaIndices = _AlphaIndices;
			d_AlphaIndicesLen = _AlphaIndicesLen;			
		}

	}

	char *					operator[] (unsigned int idx ) const
	{
		static argbDWORD			destColor32 = { 255, 255, 255, 255 };

		if( p_RGBStream )
		{
			assert( idx < d_RGBIndicesLen );
			assert( p_RGBIndices[idx] < (int) d_RGBStreamLen );

			Vector3 &			sourceColor = p_RGBStream[p_RGBIndices[idx]];

			destColor32.R = (u8) (sourceColor.x( ) * 255.0f);
			destColor32.G = (u8) (sourceColor.y( ) * 255.0f);
			destColor32.B = (u8) (sourceColor.z( ) * 255.0f);
		}
		else
		{
			destColor32.R = 255;
			destColor32.G = 255;
			destColor32.B = 255;
		}


		if( p_AlphaStream )
		{
			assert( idx < d_AlphaIndicesLen );
			assert( p_AlphaIndices[idx] < (int) d_AlphaStreamLen );

			Vector3 &			sourceAlpha = p_AlphaStream[p_AlphaIndices[idx]];

			destColor32.A = (u8) (sourceAlpha.x( ) * 255.0f);
		}
		else
			destColor32.A = 255;

		return (char *) &destColor32;
	}
} ;						

class FVFVertexStream_PadRGBA : public FVFVertexStream
{
	argbDWORD					color;

public:
	FVFVertexStream_PadRGBA( 
		unsigned char			_R, 
		unsigned char			_G,
		unsigned char			_B,
		unsigned char			_A
		) :
		FVFVertexStream( 4 )
	{
	
		color.R = _R;
		color.G = _G;
		color.B = _B;
		color.A = _A;
	}

	char *					operator[] (unsigned int idx ) const
	{
		return (char *) &color;
	}

} ;

#endif
