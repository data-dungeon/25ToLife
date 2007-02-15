/********************************************************************
	created:	2003/12/15
	created:	15:12:2003   10:35
	filename: 	c:\dev\avalanche\Tools\TupperFilters\MeshPath\TreeFactory.cpp
	file path:	c:\dev\avalanche\Tools\TupperFilters\MeshPath
	file base:	TreeFactory
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement the TreeNode factory
*********************************************************************/

#include "MeshPathPch.h"
#include "TreeFactory.h"
#include <tupperware/keeper.h>
#include "collide/tritest.h"

static std::map< Navigate::TreeNode*, Octant > s_extentsRecord;

void TreeNodeFactory::Reset( void )
{
	s_extentsRecord.clear();
}

void TreeNodeFactory::DestroyRoot( Navigate::TreeNode *root )
{
	// delete all memory allocated with root - this method assumes that root and it's children own ALL of their pointed-to data (no block allocations)

	if ( root == NULL )
	{
		return;
	}

	if ( root->flags & Navigate::TreeNode::LEAF )
	{
		root->data.leaf.nodeCount = 0;
		if ( root->data.leaf.nodes != NULL )
		{
			delete [] root->data.leaf.nodes;
			root->data.leaf.nodes = NULL;
		}

		root->data.leaf.softEdgeCount = 0;
		root->data.leaf.hardEdgeCount = 0;
		root->data.leaf.extraCoverCount = 0;
		if ( root->data.leaf.edgeNodes != NULL )
		{
			delete [] root->data.leaf.edgeNodes;
			root->data.leaf.edgeNodes = NULL;
		}
		if ( root->data.leaf.edgeData != NULL )
		{
			delete [] root->data.leaf.edgeData;
			root->data.leaf.edgeData = NULL;
		}
	}
	else
	{
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				DestroyRoot( root->data.branch.children[ iCurrChild ] );
				root->data.branch.children[ iCurrChild ] = NULL;
			}
		}
	}

	delete root;
}

static bool GetNodesIndexByNodeID( Navigate::NodeIndexType *nodes, unsigned int iNodeCount, unsigned int iNodeID, unsigned char &ucIndex )
{
	for( unsigned int i = 0; i < iNodeCount; ++i )
	{
		if ( nodes[ i ] == iNodeID )
		{
			ucIndex = i;
			return( true );
		}
	}
	return( false );
}

struct EdgeRecord
{
	unsigned int	iNodeID;
	unsigned int	iEdgeID;
	bool operator==( const EdgeRecord &rhs ) { return( ( iNodeID == rhs.iNodeID ) && ( iEdgeID == rhs.iEdgeID ) ); }
};

Navigate::TreeNode *TreeNodeFactory::BuildChildren( std::vector< unsigned int > &vNodeCoverFlags, Navigate::Mesh *partialMesh, Octant &extents, unsigned int currDepth, unsigned int maxDepth, unsigned char maxNodeCount )
{
	Navigate::TreeNode *thisNode = new Navigate::TreeNode();
	memset(thisNode,0,sizeof(Navigate::TreeNode)); // clear memory (constructor apparently does not do it!)
	s_extentsRecord[ thisNode ] = extents;

	// go through partialMesh and build an octtree representing it's nodes

	// build the list of nodes we will be working with
	Keeper< unsigned int > containedNodes;
	for( unsigned int iNodeIndex = 0; iNodeIndex < partialMesh->nodeCount; iNodeIndex++ )
	{
		Vector3 a, b, c, n;
		a = partialMesh->points[ partialMesh->nodes[ iNodeIndex ].points[ 0 ] ];
		b = partialMesh->points[ partialMesh->nodes[ iNodeIndex ].points[ 1 ] ];
		c = partialMesh->points[ partialMesh->nodes[ iNodeIndex ].points[ 2 ] ];
		n = ( b - a ).Cross( c - a ).Normalized();

		if ( extents.Touches( n, a, b, c, 0.01f ) || extents.Contains( a, b, c, 0.01f ) )
		{
			containedNodes.Add( iNodeIndex );
		}
	}

	if ( containedNodes.GetSize() <= 0 )
	{	// no nodes, no child box here
		s_extentsRecord.erase( thisNode );
		delete thisNode;
		thisNode = NULL;
	}
	else
	{
		// determine if we are to be a leaf node, or if we need to be sub-divided further
		bool bSubDivide = false;
		if ( containedNodes.GetSize() > ( int )maxNodeCount )
		{
			// HEURISTIC #1 = Max node count per treenode
			// we already know we have to sub-divide
			bSubDivide = true;
		}
		else
		{
			// HEURISTIC #2 = No contained nodes may intersect in the XZ plane
			// check 
			if ( CheckNodeIntersectionsXZ( partialMesh, containedNodes ) )
			{
				bSubDivide = true;
			}
			else
			{
				// HEURISTIC #3 = All contained nodes must be connected
				if ( CheckNodeConnections( partialMesh, containedNodes ) )
				{
					bSubDivide = true;
				}
			}
		}

		if ( bSubDivide )
		{	// We're not a leaf node - subdivide

			if( ( currDepth + 1 ) >= maxDepth )
			{	// sanity checking
				Vector3 center = extents.Center();
				float halfSize = extents.HalfSize();
				throw( PluginException( ERROR_FATAL, "MeshPath - Export\r\n  %s; Center:(%f,%f,%f); halfSize:%f\r\n",
					"Attempted to subdivide beyond max recursion depth",
					center.x(), center.y(), center.z(), halfSize ) );
			}

			thisNode->flags &= ~( Navigate::TreeNode::LEAF );

			for( unsigned int iChild = 0; iChild < 8; iChild++ )
			{
				Octant childExtents = extents.Child( ( Octant::OCTANT )iChild );
				thisNode->data.branch.children[ iChild ] = BuildChildren( vNodeCoverFlags, partialMesh, childExtents, currDepth+1, maxDepth, maxNodeCount );
			}
		}
		else
		{	// We're a leaf node

			// build any edge data for this octnode
			Keeper< EdgeRecord > containedHardEdgeIDs;
			for( unsigned int iNodeIndex = 0; iNodeIndex < ( unsigned int )containedNodes.GetSize(); ++iNodeIndex )
			{	// for each node we're working with, discover any hard edges.
				unsigned int iNodeID = containedNodes[ iNodeIndex ];
				for( unsigned int iEdgeID = 0; iEdgeID < 3; ++iEdgeID )
				{
					if ( partialMesh->nodes[ iNodeID ].neighbors[ iEdgeID ] == ( Navigate::NodeIndexType )-1 )
					{	// this is a hard edge
						EdgeRecord record;
						record.iNodeID = iNodeID;
						record.iEdgeID = iEdgeID;
						containedHardEdgeIDs.Add( record );
					}
				}
			}

			thisNode->flags |= Navigate::TreeNode::LEAF;

			// record the node list
			if ( containedNodes.GetSize() > 0 )
			{
				thisNode->data.leaf.nodeCount	= containedNodes.GetSize();
				thisNode->data.leaf.nodes		= new Navigate::NodeIndexType[ thisNode->data.leaf.nodeCount ];

				for( int i = 0; i < containedNodes.GetSize(); i++ )
				{
					thisNode->data.leaf.nodes[ i ] = containedNodes[ i ];
				}
			}

			// record the edge data
			if ( containedHardEdgeIDs.GetSize() > 0 )
			{
				thisNode->data.leaf.softEdgeCount	= 0;						// check the associated node ID's w/ the list of hard edges flagged as 'no cover'
				thisNode->data.leaf.hardEdgeCount	= containedHardEdgeIDs.GetSize();
				thisNode->data.leaf.extraCoverCount	= 0;						// check the node ID's w/ the list of soft edges flagged as other than 'no cover'

				unsigned int iCount = thisNode->data.leaf.softEdgeCount + thisNode->data.leaf.hardEdgeCount + thisNode->data.leaf.extraCoverCount;
				thisNode->data.leaf.edgeNodes		= new unsigned char[ iCount ];
				thisNode->data.leaf.edgeData		= new Navigate::TreeNode::EdgeData[ iCount ];

				unsigned int iOffset = 0;
				// hard edges flagged as 'no cover'
				for( iCount = 0; iCount < thisNode->data.leaf.softEdgeCount; ++iCount )
				{
					thisNode->data.leaf.edgeNodes[ iOffset + iCount ] = 0;			// replace w/ node ID of hard edge that was flagged as 'no cover'
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeID = 0;	// replace w/ correct edge ID for this edge
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeCoverType = 0;	// replace w/ cover flags
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeMiscFlags = 0;
				}

				// unflagged hard edges (default to full cover) and flagged hard edges that and are still cover 
				iOffset = thisNode->data.leaf.softEdgeCount;
				for( iCount = 0; iCount < thisNode->data.leaf.hardEdgeCount; ++iCount )
				{
					unsigned char ucIndex;
					bool bFound = GetNodesIndexByNodeID( thisNode->data.leaf.nodes, thisNode->data.leaf.nodeCount, containedHardEdgeIDs[ iCount ].iNodeID, ucIndex );
					ASSERT( bFound );
					thisNode->data.leaf.edgeNodes[ iOffset + iCount ] = ucIndex;
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeID = containedHardEdgeIDs[ iCount ].iEdgeID;

					unsigned int uValue = 0;
					if ( vNodeCoverFlags.size() > 0 )
					{
						uValue = vNodeCoverFlags[ containedHardEdgeIDs[ iCount ].iNodeID ];
					}
					else
					{
						uValue = Navigate::TreeNode::FULLCOVER_EDGE;
					}

					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeCoverType = uValue;
					switch ( containedHardEdgeIDs[ iCount ].iEdgeID )
					{
					case 0:
						partialMesh->nodes[ ucIndex ].edgeFlags_0 = uValue;
						break;
					case 1:
						partialMesh->nodes[ ucIndex ].edgeFlags_1 = uValue;
						break;
					case 2:
						partialMesh->nodes[ ucIndex ].edgeFlags_2 = uValue;
						break;
					default:
						break;
					}

					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeMiscFlags = 0;
				}

				// soft edges flagged as some form of cover
				iOffset = thisNode->data.leaf.hardEdgeCount;
				for( iCount = 0; iCount < thisNode->data.leaf.extraCoverCount; ++iCount )
				{
					thisNode->data.leaf.edgeNodes[ iOffset + iCount ] = 0;			// replace w/ node ID of soft edge that was flagged as some sort of cover
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeID = 0;	// replace w/ correct edge ID for this edge
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeCoverType = 0;	// replace w/ cover flags
					thisNode->data.leaf.edgeData[ iOffset + iCount ].edgeMiscFlags = 0;
				}

			}
		}
	}

	return( thisNode );
}

static bool LineIntersect( float &t, const Vector3 &o1, const Vector3 &d1, const Vector3 &o2, const Vector3 &d2 )
{
	Vector3 d1n = d1.Normalized();
	Vector3 d2n = d2.Normalized();
	Vector3 den = d1n.Cross( d2n );
	float denominator = den.Dot( den );

	if ( Math::Equal( denominator, 0.0f ) )
	{	// no intersection
		return( false );
	}

	Matrix3x3 m;
	m.SetRow0( o2 - o1 );
	m.SetRow1( d1n );
	m.SetRow2( d1n.Cross( d2n ) );

	t = m.Det() / denominator;

	bool bResult = ( ( t >= 0.0f ) && ( ( t * t ) <= ( d1 - o1 ).LengthSquared() ) );

	float length = ( d1 - o1 ).Length();
	if ( length != 0.0f )
	{
		t = t / length;
	}

	return( bResult );
}

static bool TriangleCenterPoint( Vector3 &center, const Vector3 &a, const Vector3 &b, const Vector3 &c )
{
	Vector3 o1, d1, o2, d2;
	o1 = ( b - a ) * 0.5f;
	o2 = ( c - a ) * 0.5f;
	d1 = ( c - a );
	d2 = ( b - a );

	float t = 0.0f;

	bool bResult = LineIntersect( t, o1, d1, o2, d2 );

	center = ( ( t * ( d1 - o1 ) ) + o1 );

	return( bResult );
}

static void BuildNeighborList( Navigate::Mesh *partialMesh, Navigate::TreeNode *root, unsigned int nodeIndex, std::map< Navigate::TreeNode*, unsigned int > &neighbors )
{
	if ( root->flags & Navigate::TreeNode::LEAF )
	{	// we have nodes
		for( unsigned int iCurrNode = 0; iCurrNode < root->data.leaf.nodeCount; iCurrNode++ )
		{
			if ( root->data.leaf.nodes[ iCurrNode ] == nodeIndex )
			{
				neighbors[ root ] = nodeIndex;
			}
		}
	}
	else
	{	// no nodes, we must have children
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				BuildNeighborList( partialMesh, root->data.branch.children[ iCurrChild ], nodeIndex, neighbors );
			}
		}
	}
}

void TreeNodeFactory::Count( Navigate::TreeNode *root, unsigned int &nodeCount, unsigned int &childCount, unsigned int &edgeCount )
{
	childCount++;
	if ( root->flags & Navigate::TreeNode::LEAF )
	{
		nodeCount += root->data.leaf.nodeCount;
		edgeCount += root->data.leaf.softEdgeCount + root->data.leaf.hardEdgeCount + root->data.leaf.extraCoverCount;
	}
	else
	{
		for( unsigned int iChild = 0; iChild < 8; iChild++ )
		{
			if ( root->data.branch.children[ iChild ] != NULL )
			{
				Count( root->data.branch.children[ iChild ], nodeCount, childCount, edgeCount );
			}
		}
	}
}

static void CopyTreeNodes(
	Navigate::TreeNode *root,
	void *buffer,
	unsigned int &offset,
	unsigned int startingOffset,
	std::map< unsigned int, unsigned int > &fileRecords,
	std::map< unsigned int, unsigned int > &relocationRecords )
{
	Navigate::TreeNode *dest = ( Navigate::TreeNode* )( ( unsigned int )buffer + offset );

	// byte-wise copy
	*dest = *root;

	// record the position as a file offset
	relocationRecords[ ( unsigned int )root ] = ( unsigned int )dest;
	fileRecords[ ( unsigned int )dest ] = ( offset + startingOffset );

	if ( !( root->flags & Navigate::TreeNode::LEAF ) )
	{
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				// move to the next position
				offset += sizeof( Navigate::TreeNode );

				CopyTreeNodes( root->data.branch.children[ iCurrChild ], buffer, offset, startingOffset, fileRecords, relocationRecords );
			}
		}
	}
}

static void CopyNodeLists(
	Navigate::TreeNode *root,
	void *buffer,
	unsigned int &offset,
	unsigned int startingOffset,
	std::map< unsigned int, unsigned int > &fileRecords,
	std::map< unsigned int, unsigned int > &relocationRecords )
{
	if ( root->flags & Navigate::TreeNode::LEAF )
	{
		Navigate::NodeIndexType *dest = ( Navigate::NodeIndexType* )( ( unsigned int )buffer + offset );

		// copy the indecies
		for( unsigned int i = 0; i < root->data.leaf.nodeCount; i++ )
		{
			dest[ i ] = root->data.leaf.nodes[ i ];
		}

		// record the position as a file offset
		relocationRecords[ ( unsigned int )( root->data.leaf.nodes ) ] = ( unsigned int )dest;
		fileRecords[ ( unsigned int )dest ] = ( offset + startingOffset );

		// move to the next position
		offset += ( sizeof( Navigate::NodeIndexType ) * root->data.leaf.nodeCount );
	}
	else
	{
		// update the children
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				CopyNodeLists( root->data.branch.children[ iCurrChild ], buffer, offset, startingOffset, fileRecords, relocationRecords );
			}
		}
	}
}


static void CopyEdgeNodes(
						 Navigate::TreeNode *root,
						 void *buffer,
						 unsigned int &offset,
						 unsigned int startingOffset,
						 std::map< unsigned int, unsigned int > &fileRecords,
						 std::map< unsigned int, unsigned int > &relocationRecords )
{
	if ( root->flags & Navigate::TreeNode::LEAF )
	{
		// copy our edge data sets
		unsigned int recordCount = root->data.leaf.softEdgeCount + root->data.leaf.hardEdgeCount + root->data.leaf.extraCoverCount;

		void *dest = ( void* )( ( unsigned int )buffer + offset );
		memcpy( dest, ( const void * )( root->data.leaf.edgeNodes ), sizeof( unsigned char ) * recordCount );
		relocationRecords[ ( unsigned int )( root->data.leaf.edgeNodes ) ] = ( unsigned int )dest;
		fileRecords[ ( unsigned int )dest ] = ( offset + startingOffset );

		offset += ( sizeof( unsigned char ) * recordCount );
	}
	else
	{
		// update the children
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				CopyEdgeNodes( root->data.branch.children[ iCurrChild ], buffer, offset, startingOffset, fileRecords, relocationRecords );
			}
		}
	}
}

static void CopyEdgeData(
						 Navigate::TreeNode *root,
						 void *buffer,
						 unsigned int &offset,
						 unsigned int startingOffset,
						 std::map< unsigned int, unsigned int > &fileRecords,
						 std::map< unsigned int, unsigned int > &relocationRecords )
{
	if ( root->flags & Navigate::TreeNode::LEAF )
	{
		// copy our edge data sets
		unsigned int recordCount = root->data.leaf.softEdgeCount + root->data.leaf.hardEdgeCount + root->data.leaf.extraCoverCount;
		void *dest = ( void* )( ( unsigned int )buffer + offset );
		memcpy( dest, ( const void * )( root->data.leaf.edgeData ), sizeof( Navigate::TreeNode::EdgeData ) * recordCount );
		relocationRecords[ ( unsigned int )( root->data.leaf.edgeData ) ] = ( unsigned int )dest;
		fileRecords[ ( unsigned int )dest ] = ( offset + startingOffset );

		// make sure our next data is 4-byte aligned by adding the byteAlign value
		offset += ( sizeof( Navigate::TreeNode::EdgeData ) * recordCount );
	}
	else
	{
		// update the children
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( root->data.branch.children[ iCurrChild ] != NULL )
			{
				CopyEdgeData( root->data.branch.children[ iCurrChild ], buffer, offset, startingOffset, fileRecords, relocationRecords );
			}
		}
	}
}

static void PatchTreeNodes(
	Navigate::TreeNode *array,
	unsigned int childCount,
	std::map< unsigned int, unsigned int > &fileRecords,
	std::map< unsigned int, unsigned int > &relocationRecords )
{
	unsigned int value;

	// First go through all the nodes and update the pointers to point to the block buffer
	unsigned int iCurrNode;
	for( iCurrNode = 0; iCurrNode < childCount; iCurrNode++ )
	{
		if ( array[ iCurrNode ].flags & Navigate::TreeNode::LEAF )
		{
			value = relocationRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.edgeNodes ) ];
			ASSERT( value != 0 );
			array[ iCurrNode ].data.leaf.edgeNodes = ( unsigned char * )value;

			value = relocationRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.edgeData ) ];
			ASSERT( value != 0 );
			array[ iCurrNode ].data.leaf.edgeData = ( Navigate::TreeNode::EdgeData * )( value );

			value = relocationRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.nodes ) ];
			ASSERT( value != 0 );
			array[ iCurrNode ].data.leaf.nodes = ( Navigate::NodeIndexType* )value;
		}
		else
		{
			// patch the children node pointers
			for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
			{
				if ( array[ iCurrNode ].data.branch.children[ iCurrChild ] != NULL )
				{
					value = relocationRecords[ ( unsigned int )( array[ iCurrNode ].data.branch.children[ iCurrChild ] ) ];
					ASSERT( value != 0 );
					array[ iCurrNode ].data.branch.children[ iCurrChild ] = ( Navigate::TreeNode* )value;
				}
			}
		}
	}

	// Second go through all the nodes and change the pointers to file offsets
	for( iCurrNode = 0; iCurrNode < childCount; iCurrNode++ )
	{
		if ( array[ iCurrNode ].flags & Navigate::TreeNode::LEAF )
		{
			// patch the node list pointer
			if ( array[ iCurrNode ].data.leaf.nodeCount > 0 )
			{
				value = ( fileRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.nodes ) ] );
				ASSERT( value != 0 );
				array[ iCurrNode ].data.leaf.nodes = ( Navigate::NodeIndexType* )value;
			}
			else
			{
				array[ iCurrNode ].data.leaf.nodes = ( Navigate::NodeIndexType* )0xffffffff;
			}

			unsigned int iRecordCount = array[ iCurrNode ].data.leaf.softEdgeCount + array[ iCurrNode ].data.leaf.hardEdgeCount + array[ iCurrNode ].data.leaf.extraCoverCount;
			if ( iRecordCount > 0 )
			{
				value = ( fileRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.edgeNodes ) ] );
				ASSERT( value != 0 );
				array[ iCurrNode ].data.leaf.edgeNodes = ( unsigned char * )value;

				value = ( fileRecords[ ( unsigned int )( array[ iCurrNode ].data.leaf.edgeData ) ] );
				ASSERT( value != 0 );
				array[ iCurrNode ].data.leaf.edgeData = ( Navigate::TreeNode::EdgeData * )value;
			}
			else
			{
				array[ iCurrNode ].data.leaf.edgeNodes = ( unsigned char* )0xffffffff;
				array[ iCurrNode ].data.leaf.edgeData = ( Navigate::TreeNode::EdgeData * )0xffffffff;
			}
		}
		else
		{
			// patch the children node pointers
			for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
			{
				if ( array[ iCurrNode ].data.branch.children[ iCurrChild ] != NULL )
				{
					value = fileRecords[ ( unsigned int )( array[ iCurrNode ].data.branch.children[ iCurrChild ] ) ];
					ASSERT( value != 0 );
					array[ iCurrNode ].data.branch.children[ iCurrChild ] = ( Navigate::TreeNode* )value;
				}
				else
				{
					array[ iCurrNode ].data.branch.children[ iCurrChild ] = ( Navigate::TreeNode* )0xffffffff;
				}
			}
		}
	}
}

Navigate::TreeNode	*TreeNodeFactory::MakeSingleFileBlock( Navigate::TreeNode *root, unsigned int startingOffset, unsigned int &dataSize, bool bDeleteRootWhenDone )
{
	unsigned int childCount = 0;
	unsigned int nodeCount = 0;
	unsigned int edgeCount = 0;

	// walk the tree, counting all the data we need to have allocated
	Count( root, nodeCount, childCount, edgeCount );

	unsigned int nodeOffset = ( sizeof( Navigate::TreeNode ) * childCount );
	unsigned int edgeNodesOffset	= nodeOffset + ( sizeof( Navigate::NodeIndexType ) * nodeCount );
	unsigned int edgeDataOffset		= edgeNodesOffset + ( sizeof( unsigned char ) * edgeCount );
	dataSize = ( edgeDataOffset + ( sizeof( unsigned char ) * edgeCount ) );

	unsigned int currChildOffset = 0;
	unsigned int currNodeOffset = nodeOffset;
	unsigned int currEdgeNodeOffset = edgeNodesOffset;
	unsigned int currEdgeDataOffset = edgeDataOffset;

	Navigate::TreeNode *newRoot = ( Navigate::TreeNode* )( new unsigned char[ dataSize ] );

	// map for live pointer->file offset records
	std::map< unsigned int, unsigned int >	fileRecords;
	std::map< unsigned int, unsigned int >	relocationRecords;

	// start copying data into the block allocation
	CopyTreeNodes( root, ( void* )newRoot, currChildOffset, startingOffset, fileRecords, relocationRecords );
	CopyNodeLists( root, ( void* )newRoot, currNodeOffset, startingOffset, fileRecords, relocationRecords );
	CopyEdgeNodes( root, ( void* )newRoot, currEdgeNodeOffset, startingOffset, fileRecords, relocationRecords );
	CopyEdgeData( root, ( void* )newRoot, currEdgeDataOffset, startingOffset, fileRecords, relocationRecords );
	PatchTreeNodes( newRoot, childCount, fileRecords, relocationRecords );

	// clean up when we're done
	if ( bDeleteRootWhenDone )
	{
		DestroyRoot( root );
	}

	return( newRoot );
}

bool TreeNodeFactory::CheckNodeIntersectionsXZ( Navigate::Mesh *partialMesh, Keeper< unsigned int > &containedNodes )
{
	unsigned int iNodeCount = containedNodes.GetSize();

	if ( iNodeCount <= 0 )
	{
		return( false );
	}

	for( unsigned int iBaseNodeIndex = 0; iBaseNodeIndex < ( iNodeCount - 1 ); iBaseNodeIndex++ )
	{	// for each node,
		for( unsigned int iNextNodeIndex = iBaseNodeIndex + 1; iNextNodeIndex < iNodeCount; iNextNodeIndex++ )
		{	// check each other unchecked node

			// reject tri's that share verts
			if (
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 0 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 0 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 0 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 1 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 0 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 2 ] ) ||

				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 1 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 0 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 1 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 1 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 1 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 2 ] ) ||

				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 2 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 0 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 2 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 1 ] ) ||
				( partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 2 ] == partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 2 ] )
				)
			{
				continue;
			}

			Vector3 p[ 3 ];
			Vector3 q[ 3 ];

			p[ 0 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 0 ] ];
			p[ 1 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 1 ] ];
			p[ 2 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iBaseNodeIndex ] ].points[ 2 ] ];

			q[ 0 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 0 ] ];
			q[ 1 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 1 ] ];
			q[ 2 ] = partialMesh->points[ partialMesh->nodes[ containedNodes[ iNextNodeIndex ] ].points[ 2 ] ];

			// drop it all into the XZ plane - ASSUMES tri's are MOSTLY facing along the y-axis
			p[ 0 ].y( 0.0f ); p[ 1 ].y( 0.0f ); p[ 2 ].y( 0.0f );
			q[ 0 ].y( 0.0f ); q[ 1 ].y( 0.0f ); q[ 2 ].y( 0.0f );

			// test each point
			bool bInside = (
				TriangleTest::PointInsideXZ( q[ 0 ].x(), q[ 0 ].z(), p[ 0 ], p[ 1 ], p[ 2 ] ) ||
				TriangleTest::PointInsideXZ( q[ 1 ].x(), q[ 1 ].z(), p[ 0 ], p[ 1 ], p[ 2 ] ) ||
				TriangleTest::PointInsideXZ( q[ 2 ].x(), q[ 2 ].z(), p[ 0 ], p[ 1 ], p[ 2 ] )
			);

			if ( bInside )
			{	// at least 1 point of q is inside triangle p
				return( true );
			}
		}
	}

	return( false );
}

// stupid linear search for the given index
static bool Contains( unsigned int index, Keeper< unsigned int > &containedNodes, unsigned int *where = NULL )
{
	int i = containedNodes.GetIndex( index );
	if ( i >= 0 )
	{
		if ( where != NULL )
		{
			*where = ( unsigned int )i;
		}
		return( true );
	}
	else
	{
		if ( where != NULL )
		{
			*where = 0xffffffff;
		}
		return( false );
	}
}

static void BuildTestList( Navigate::Mesh *partialMesh, unsigned int containedNodeIndex, Keeper< unsigned int > &tested, Keeper< unsigned int > &containedNodes )
{
	tested.Add( containedNodes[ containedNodeIndex ] );
	for( unsigned int iCurrNeighbor = 0; iCurrNeighbor < 3; iCurrNeighbor++ )
	{
		unsigned int neighbor = partialMesh->nodes[ containedNodes[ containedNodeIndex ] ].neighbors[ iCurrNeighbor ];
		if ( neighbor != ( Navigate::NodeIndexType )-1 )
		{
			unsigned int idx;
			if ( ( Contains( neighbor, containedNodes, &idx ) ) && ( !Contains( neighbor, tested ) ) )
			{
				BuildTestList( partialMesh, idx, tested, containedNodes );
			}
		}
	}
}

bool TreeNodeFactory::CheckNodeConnections( Navigate::Mesh *partialMesh, Keeper< unsigned int > &containedNodes )
{
	Keeper< unsigned int > tested;

	unsigned int iNodeCount = ( unsigned int )containedNodes.GetSize();
	if ( iNodeCount <= 0 )
	{
		return( false );
	}

	// build a connection list starting w/ index 0
	BuildTestList( partialMesh, 0, tested, containedNodes );

	// find any nodes that didn't have a connection from the node at index 0 - if so, then we don't have a path between all the nodes in this octant and we need to subdivide further
	for( unsigned int iCurrNode = 0; iCurrNode < iNodeCount; iCurrNode++ )
	{
		if ( !Contains( containedNodes[ iCurrNode ], tested ) )
		{	// we don't have a connection path between some of the nodes in this octant, subdivide
			return( true );
		}
	}

	// all the nodes in the given list are connected, don't subdivide
	return( false );
}
