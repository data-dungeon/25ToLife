#include "ExportPCH.h"
#include "FVFBuilder.h"

unsigned int FVFBuilder::addVertex( const FVFVertex & vtx)
{
	assert( vtx.FVF( ) == d_FVF);

	FVFVertexRef 	tempRef( vtx, 0);

	FVFVertexRefSet::const_iterator	pos = d_RefSet.find( tempRef);

	if( pos != d_RefSet.end( ))
		return (*pos).idx( );

	d_VB.push_back( vtx);
	
	unsigned int idx = d_VB.end( ) - 1;

	d_RefSet.insert( FVFVertexRef( d_VB[idx], idx) );

	return idx;
}

