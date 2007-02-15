#ifndef __STREAM_DESCRIPTORS_H
#define __STREAM_DESCRIPTORS_H

#include <vector>

#include "VertexDescriptor.h"

class SingleStreamDescriptor
{
public:
	VertexDescriptor				d_Desc;
	unsigned int					d_StreamDesc;
	unsigned int					d_FVF;
	unsigned int					d_VDIndex;
	unsigned int					d_MeshCount;

	SingleStreamDescriptor(
		const VertexDescriptor &	_VtxDesc,
		const StreamDescriptor &	_StrDesc,
		unsigned int					_VDIndex
	) :
		d_Desc( _VtxDesc),
		d_VDIndex( _VDIndex),
		d_MeshCount( 0 )
	{	

		convertDescriptors( _VtxDesc, _StrDesc, d_StreamDesc, d_FVF );
	
	}

	~SingleStreamDescriptor( )	
	{	}
	
	// sort operator
	bool operator < ( const SingleStreamDescriptor & strR ) const
	{
		if( d_Desc < strR.d_Desc )
			return true;

		if( d_StreamDesc < strR.d_StreamDesc )
			return true;

		if( d_FVF < strR.d_FVF )
			return true;

		return d_VDIndex < strR.d_VDIndex;
	}

	bool operator == ( const SingleStreamDescriptor & strR ) const 
	{
		if( d_Desc != strR.d_Desc )
			return false;

		if( d_StreamDesc != strR.d_StreamDesc )
			return false;

		if( d_FVF != strR.d_FVF )
			return false;

		return d_VDIndex == strR.d_VDIndex;
	}

} ;

typedef std::pair< unsigned int, unsigned int >	streamPair;

class MultiStreamDescriptor
{
public:
	VertexDescriptor				d_Desc;
	std::vector< streamPair > 	a_StreamIndexes;
	unsigned int					d_MeshCount;

	MultiStreamDescriptor(
		const VertexDescriptor &	_Desc
	) :
		d_Desc( _Desc),
		d_MeshCount( 0 )
	{	}

	~MultiStreamDescriptor( )	
	{	}
	
	unsigned int			numStreams( void ) const 		{ return a_StreamIndexes.size( ); }

	unsigned int			addStream( streamPair pair )	{ a_StreamIndexes.push_back( pair ); }

	// sort operator
	bool operator < ( const MultiStreamDescriptor & strR ) const
	{
		if( d_Desc < strR.d_Desc )
			return true;

		return a_StreamIndexes < strR.a_StreamIndexes;
	}

	bool operator == ( const MultiStreamDescriptor & strR ) const 
	{
		if( d_Desc != strR.d_Desc )
			return false;

		return a_StreamIndexes == strR.a_StreamIndexes;
	}

} ;


#endif