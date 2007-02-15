/********************************************************************
	created:	2003/12/11
	created:	11:12:2003   14:46
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavMesh.cpp
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavMesh
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement the navigation mesh
*********************************************************************/

#include "Navigate\NavigatePCH.h"

namespace Navigate
{
	Mesh *Mesh::DiskFixup( void *base )
	{
		if ( base == NULL )
		{
			return( NULL );
		}

		Navigate::Mesh	*mesh = ( Mesh* )base;

#ifdef GCN
		ByteSwap( mesh->nodes);
		ByteSwap( mesh->points);
		ByteSwap( mesh->root);
		ByteSwap( mesh->nodeCount);
		ByteSwap( mesh->pointCount);
		mesh->extents.EndianSwap( );
#endif //GCN

		mesh->nodes		= ( Node* )( ( unsigned int )mesh->nodes + ( unsigned int )base );
		mesh->points	= ( Vector3Packed* )( ( unsigned int )mesh->points + ( unsigned int )base );
		mesh->root		= ( TreeNode* )( ( unsigned int )mesh->root + ( unsigned int )base );

#ifdef GCN
		int i;
		for( i = 0; i < mesh->nodeCount; i++)
		{
			mesh->nodes[i].EndianSwap();
		}
		for( i = 0; i < mesh->pointCount; i++)
		{
			mesh->points[i].EndianSwap();
		}
#endif //GCN

		TreeNode::DiskFixup( mesh->root, ( unsigned int )base );

		return( mesh );
	}
}
