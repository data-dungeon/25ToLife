#include "ExportPch.h"

#include "FVFVBBuilder.h"

void FVFVBBuilder::changeDescriptor( const VertexDescriptor & newDesc )
{
	assert( stride( d_Desc) == stride( newDesc ) );

	if( d_Desc != newDesc )
	{
		d_Desc = newDesc;
		d_RefSet.clear( );
	}
}

int FVFVBBuilder::addVertex( const FVFVertex & vtx )
{

	d_VB.push_back( vtx );

	unsigned int idx = d_VB.size( ) - 1;

	FVFVertexRef 	tempRef( d_VB, idx );

	FVFVertexRefSet::const_iterator	pos = d_RefSet.find( tempRef );

	if( pos != d_RefSet.end( ) )
	{
		d_VB.pop_back( );
		return (*pos).idx( );
	}
	else if( d_VB.size( ) > d_MaxVertices )
	{
		d_VB.pop_back( );
		return -1;
	}
	else
	{
		d_VBBytes += vtx.bytes( );
		d_RefSet.insert( tempRef );

		return idx;
	}
}

