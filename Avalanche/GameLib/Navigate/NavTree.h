/********************************************************************
	created:	2003/12/11
	created:	11:12:2003   14:38
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavTree.h
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavTree
	file ext:	h
	author:		Shon C. Love
	
	purpose:	An octtree for subdividing the navigation mesh
*********************************************************************/

#ifndef NAVTREE_H
#define NAVTREE_H

#include "Navigate/NavIterators.h"
#include "collide/cube.h"

namespace Navigate
{

	// subdivision heuristics:
	//	* At most (n) Nodes
	//	* No Nodes may intersect on the XZ plane
	//		This allows us to reduce the 'line of sight' refinements to a
	//		2D problem
	//	* All Nodes must be connected
	//		This ensures that there are valid paths between all TreeNode
	//		faces that have Node intersections - for coarse-path navigation

#pragma pack( push, navigate_align, 1 )
	class TreeNode
	{
	public:

		enum FLAGS
		{
			// TreeNode Flags
			LEAF					= 0x80000000,

			// EdgeData Flags
			NOCOVER_EDGE		= 0,	// the edge is not cover, even though it's a mesh hard edge (remove the edge from the cover search)
			FULLCOVER_EDGE		= 1,	// can't see over the cover, need to look around the edge
			STANDCOVER_EDGE	= 2,	// can see over if standing, otherwise, look around the edge
			CROUCHCOVER_EDGE	= 3,	// can see over if crouching, otherwise, look around the edge
		};

		// determine if this node is a leaf or a branch node - poor man's RTTI
		unsigned int	flags;

		struct EdgeData
		{
			unsigned char	edgeID			: 2;	// triangle edge numbers 0..2
			unsigned char	edgeCoverType	: 2;	// edge flags (NOCOVER_EDGE, FULLCOVER_EDGE, STANDCOVER_EDGE, or CROUCHCOVER_EDGE)
			unsigned char	edgeMiscFlags	: 4;	// room to grow
		};

		struct Leaf
		{
			// interesting counters

			unsigned short	nodeCount;			// the number of Mesh::Node id's associated with this octant

			unsigned short	softEdgeCount;		// the number of edges that should not be counted in cover queries, but used in path simplification queries
			unsigned short	hardEdgeCount;		// the number of edges that should be counted in cover queries and used in path simplification queries
			unsigned short	extraCoverCount;	// the number of faces that aren't edges, but should be considered in cover queries

			// interesting pointers

			Navigate::NodeIndexType	*nodes;	// the nodeCount data

			// each edge will have the same index in each of the following arrays
			// there are (softEdgeCount + hardEdgeCount + extraCoverCount) elements in the following arrays;
			//   the softEdgeCount edges are first in the arrays, followed by all the hardEdgeCount edges,
			//   and finally the extraCoverCount edges are last in these arrays.
			// this arrangement lets us roll through both edge detection and cover detection quickly.
			unsigned char	*edgeNodes;	// the index into the 'nodes' array to get the edge's node id
			EdgeData		*edgeData;	// the id of the edge, and any flags associated with the edge
		};

		struct Branch
		{
			TreeNode	*children[ 8 ];	// array of pointers to this TreeNode's children
		};

		// data for the different node types
		union
		{
			Leaf	leaf;
			Branch	branch;
		} data;

		// Fixup from disk
		static TreeNode *DiskFixup( void *root, unsigned int baseOffset );
	};
#pragma pack( pop, navigate_align )

}

#endif