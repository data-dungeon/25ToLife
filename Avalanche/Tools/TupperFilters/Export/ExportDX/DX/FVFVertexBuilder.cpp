#include "ExportPch.h"
#include "FVFVertexBuilder.h"

void FVFVertexBuilder::startDataStreams( unsigned int streamLength )
{
	d_streamLength = streamLength;

	d_numStreams = 0;
	d_totalStreamWidth = 0;

	streamArray.clear( );

	if( p_VertexData)
	{
		delete [] p_VertexData;
		p_VertexData = 0;
	}

}

void FVFVertexBuilder::pushDataStream( FVFVertexStream * pStream )
{
	streamArray.push_back( pStream );

	d_numStreams++;
	d_totalStreamWidth += pStream->bytes( );

	if( p_VertexData)
		delete [] p_VertexData;

	p_VertexData = new char[d_totalStreamWidth];		

}

FVFVertex FVFVertexBuilder::operator [] ( unsigned int idx )
{
	char * pFVFOffset = p_VertexData;

	for( unsigned int i = 0; i < streamArray.size( ); i++)
	{
		FVFVertexStream & currStream = * streamArray[i];

		memcpy( pFVFOffset, currStream[idx], currStream.bytes( ) );

		pFVFOffset += currStream.bytes( );
	}

	return FVFVertex( d_totalStreamWidth, p_VertexData);
}

