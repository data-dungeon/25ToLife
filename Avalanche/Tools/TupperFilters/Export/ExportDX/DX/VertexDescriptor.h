#ifndef __VERTEX_DESCRIPTOR_H
#define __VERTEX_DESCRIPTOR_H

#include <d3d9.h>
#include <vector>

#include "StreamFlags.h"

unsigned int	bytes( _D3DDECLTYPE type );

bool				operator < ( const D3DVERTEXELEMENT9 & e1, const D3DVERTEXELEMENT9 & e2 );
bool				operator == ( const D3DVERTEXELEMENT9 & e1, const D3DVERTEXELEMENT9 & e2 );

typedef std::vector< D3DVERTEXELEMENT9 > 	VertexDescriptor;

unsigned int	stride( const VertexDescriptor & desc );


D3DVERTEXELEMENT9 createD3DVERTEXELEMENT9( unsigned short stream, unsigned short offset, unsigned char type, unsigned char method, unsigned char usage, unsigned char index);

class FVFVertexStream;

typedef struct
{
	enum StreamType
	{
		STREAM_POSITION = 1,
		STREAM_POSITION_W,
		STREAM_NORMAL,
		STREAM_BINORMAL,
		STREAM_DIFFUSE,
		STREAM_SPECULAR,
		STREAM_TEXCOORD,

		STREAM_MASK				= 0x00ff,
		STREAM_FLAG_PAD		= 0x0100,

		STREAM_FORCE_DWORD	= 0xffffffff
	} ;

	unsigned int				type;
	FVFVertexStream *			ptr;

} StreamElement;

inline bool operator < ( const StreamElement & strL, const StreamElement & strR )
{
	if( strL.type < strR.type )
		return true;

	return false;
}

inline bool operator == ( const StreamElement & strL, const StreamElement & strR )
{
	if( strL.type == strR.type )
		return true;

	return false;
}
StreamElement createStreamElement( unsigned int _type, FVFVertexStream * _ptr );

typedef std::vector< StreamElement > 		StreamDescriptor;


void				convertDescriptors( const VertexDescriptor & vtxDesc, const StreamDescriptor & strDesc, unsigned int & streamFlags, unsigned int & FVF );

#endif 