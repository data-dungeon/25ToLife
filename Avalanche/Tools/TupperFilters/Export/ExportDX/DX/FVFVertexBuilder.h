#ifndef __FVF_VERTEX_BUILDER_H
#define __FVF_VERTEX_BUILDER_H

#include <vector>

#include "FVFVertex.h"
#include "FVFVertexStream.h"

class FVFVertexBuilder
{
	unsigned int 	d_numStreams;
	unsigned int	d_streamLength;
	unsigned int	d_totalStreamWidth;
	char *			p_VertexData;

	std::vector<FVFVertexStream *> streamArray;

public:
	FVFVertexBuilder( )
	{
		d_numStreams = 0;
		d_totalStreamWidth = 0;

		p_VertexData = 0;

		streamArray.clear( );
	} ;

	~FVFVertexBuilder( )
	{
		if( p_VertexData)
			delete [] p_VertexData;
	}

	unsigned int	totalStreamWidth( void ) const	{ return d_totalStreamWidth; }
	unsigned int	numStreams( void ) const			{ return d_numStreams; }

	void				startDataStreams( unsigned int streamLength );

	void				pushDataStream( FVFVertexStream * stream );

	FVFVertex	 	operator []( unsigned int );

} ;

#endif


		














																	