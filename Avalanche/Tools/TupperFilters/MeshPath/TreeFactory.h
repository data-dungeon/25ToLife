/********************************************************************
	created:	2003/12/15
	created:	15:12:2003   10:34
	filename: 	C:\dev\avalanche\Tools\TupperFilters\MeshPath\TreeFactory.h
	file path:	C:\dev\avalanche\Tools\TupperFilters\MeshPath
	file base:	TreeFactory
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Build a Navigate::TreeNode object, given a Navigate::Mesh
*********************************************************************/

#ifndef NAVIGATE_TREEFACTORY_H
#define NAVIGATE_TREEFACTORY_H

#include "Navigate/NavTree.h"
#include <map>
#include <vector>
class Octant;

class TreeNodeFactory
{
friend class Navigate::TreeNode;
public:
	static void					Reset( void );
	static void					DestroyRoot( Navigate::TreeNode *root );
	static Navigate::TreeNode	*BuildChildren( std::vector< unsigned int > &vNodeCoverFlags, Navigate::Mesh *partialMesh, Octant &extents, unsigned int currDepth, unsigned int maxDepth, unsigned char maxNodeCount );
	static Navigate::TreeNode	*MakeSingleFileBlock( Navigate::TreeNode *root, unsigned int startingOffset, unsigned int &dataSize, bool bDeleteRootWhenDone = true );

protected:
	static void Count( Navigate::TreeNode *root, unsigned int &nodeCount, unsigned int &childCount, unsigned int &edgeCount );
	static bool CheckNodeIntersectionsXZ( Navigate::Mesh *partialMesh, Keeper< unsigned int > &containedNodeIDs );
	static bool CheckNodeConnections( Navigate::Mesh *partialMesh, Keeper< unsigned int > &containedNodeIDs );
};

#endif