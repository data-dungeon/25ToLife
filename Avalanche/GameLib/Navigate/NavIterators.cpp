/********************************************************************
	created:	2003/12/11
	created:	11:12:2003   16:33
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavIterators.cpp
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavIterators
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implementation of the Navigate::Mesh Iterator classes
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "container/pqueue.h"
#include "container/skiplist.h"
#include "collide/tritest.h"
#include "linearAlgebra/semiseg.h"
#include "linearAlgebra/rayseg.h"
#include "Math/fastrand.h"

#if defined( NAVIGATION_DEBUGGING )
#include "EngineHelper/DrawUtil.h"
#endif

//static Octant::OCTANT s_OctantPairsXZ[ Octant::OCTANTS ][ 2 ] =
//{
//	// XYZ = 0,			// 000
//	{
//		Octant::XYZ,
//		Octant::X_YZ
//	},
//	// _XYZ = 1,    	// 001
//	{
//		Octant::_XYZ,
//		Octant::_X_YZ,
//	},
//	// X_YZ = 2,    	// 010
//	{
//		Octant::X_YZ,
//		Octant::XYZ,
//	},
//	// _X_YZ = 3,    	// 011
//	{
//		Octant::_X_YZ,
//		Octant::_XYZ,
//	},
//	// XY_Z = 4,  	  	// 100
//	{
//		Octant::XY_Z,
//		Octant::X_Y_Z,
//	},
//	// _XY_Z = 5,    	// 101
//	{
//		Octant::_XY_Z,
//		Octant::_X_Y_Z,
//	},
//	// X_Y_Z = 6,    	// 110
//	{
//		Octant::X_Y_Z,
//		Octant::XY_Z,
//	},
//	// _X_Y_Z = 7,    // 111
//	{
//		Octant::_X_Y_Z,
//		Octant::_XY_Z,
//	},
//};

/* Helper structure for PointNodeIterator class. */
struct _PointNodeIteratorCandidate
{
	unsigned int	iNode;
	float			fDistance;

	bool			operator==( const _PointNodeIteratorCandidate &rhs ){ return( ( iNode == rhs.iNode ) && ( fDistance == fDistance ) ); }
	bool			operator<( const _PointNodeIteratorCandidate &rhs ){ return( fDistance < rhs.fDistance ); }
};

static bool s_PointNodeIterator_CompareLT( const _PointNodeIteratorCandidate &lhs, const _PointNodeIteratorCandidate &rhs )
{
	// return GREATER-THAN here because we want the SMALLEST distance to have the HIGHEST priority
	return( lhs.fDistance > rhs.fDistance );
}

// find all nodes in the box that
static void s_ClosestPointNodeSearch_ClosestPointXZ( Navigate::Mesh *pMesh, Navigate::TreeNode *pTreeNode, const Octant &oBox, const Vector3 &vPoint, _PointNodeIteratorCandidate &aNode )
{
	if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
	{	// this is a leaf node, check the tri's for one that includes vPoint in XZ
		for( unsigned int iCurrNode = 0; iCurrNode < pTreeNode->data.leaf.nodeCount; iCurrNode++ )
		{
			unsigned int iNode = pTreeNode->data.leaf.nodes[ iCurrNode ];
			bool bFound = ( aNode.iNode == iNode );

			if ( !bFound )
			{	// we didn't find the given iNode in our lFoundNode look-up
				Vector3 v0 = pMesh->points[ pMesh->nodes[ iNode ].points[ 0 ] ];
				Vector3 v1 = pMesh->points[ pMesh->nodes[ iNode ].points[ 1 ] ];
				Vector3 v2 = pMesh->points[ pMesh->nodes[ iNode ].points[ 2 ] ];

				if ( TriangleTest::PointInsideXZ( vPoint, v0, v1, v2 ) )
				{	// add this node to the list of nodes
					Segment line;
					line.start = TriangleTest::Centroid( v0, v1, v2 );
					line.end = vPoint;
					float fDistance = line.LengthSquared();

					if ( fDistance < aNode.fDistance )
					{
						aNode.fDistance = fDistance;
						aNode.iNode = iNode;
					}
					break;
				}
			}
		}
	}
	else
	{	// recursively check the appropriate children
		Vector3 vLow, vHigh;
		vLow = vPoint; vLow.y( oBox.Min().y() );
		vHigh = vPoint; vHigh.y( oBox.Max().y() );
		SemiSegment seg;
		seg.SetEndPoints( vLow, vHigh );

		for ( unsigned int iCurrChild = 0; iCurrChild < 8; ++iCurrChild )
		{
			Octant oExtents = oBox.Child( ( Octant::OCTANT )iCurrChild );
			Navigate::TreeNode *pCurrChild = pTreeNode->data.branch.children[ iCurrChild ];
			if ( ( pCurrChild != NULL ) && ( oExtents.Touches( seg ) || oExtents.Contains( seg ) ) )
			{
				s_ClosestPointNodeSearch_ClosestPointXZ( pMesh, pCurrChild, oExtents, vPoint, aNode );
			}
		}
	}
}

// find all nodes in the box that
static void s_PointNodeIterator_CollectPointsXZ( Navigate::Mesh *pMesh, Navigate::TreeNode *pTreeNode, const Octant &oBox, const Vector3 &vPoint, PriorityQueue< float, _PointNodeIteratorCandidate > &aNodes )
{
	if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
	{	// this is a leaf node, check the tri's for one that includes vPoint in XZ
		for( unsigned int iCurrNode = 0; iCurrNode < pTreeNode->data.leaf.nodeCount; iCurrNode++ )
		{
			unsigned int iNode = pTreeNode->data.leaf.nodes[ iCurrNode ];
			bool bFound = false;

			// linear search to see if we've already found this node (shouldn't be too many nodes found, after all)
			for( unsigned int i = 0; ( i < ( unsigned int )aNodes.Count() ) && ( !bFound ); i++ )
			{
				bFound = ( aNodes[ i ]->iNode == iNode );
			}

			if ( !bFound )
			{	// we didn't find the given iNode in our lFoundNode look-up
				Vector3 v0 = pMesh->points[ pMesh->nodes[ iNode ].points[ 0 ] ];
				Vector3 v1 = pMesh->points[ pMesh->nodes[ iNode ].points[ 1 ] ];
				Vector3 v2 = pMesh->points[ pMesh->nodes[ iNode ].points[ 2 ] ];

				if ( TriangleTest::PointInsideXZ( vPoint, v0, v1, v2 ) )
				{	// add this node to the list of nodes
					Segment line;
					_PointNodeIteratorCandidate candidate;
					candidate.iNode = iNode;
					line.start = TriangleTest::Centroid( v0, v1, v2 );
					line.end = vPoint;
					candidate.fDistance = line.LengthSquared();
					aNodes.Enqueue( candidate.fDistance, candidate );
					break;
				}
			}
		}
	}
	else
	{	// recursively check the appropriate children
		Vector3 vLow, vHigh;
		vLow = vPoint; vLow.y( oBox.Min().y() );
		vHigh = vPoint; vHigh.y( oBox.Max().y() );
		SemiSegment seg;
		seg.SetEndPoints( vLow, vHigh );

		for ( unsigned int iCurrChild = 0; iCurrChild < 8; ++iCurrChild )
		{
			Octant oExtents = oBox.Child( ( Octant::OCTANT )iCurrChild );
			Navigate::TreeNode *pCurrChild = pTreeNode->data.branch.children[ iCurrChild ];
			if ( ( pCurrChild != NULL ) && ( oExtents.Touches( seg ) || oExtents.Contains( seg ) ) )
			{
				s_PointNodeIterator_CollectPointsXZ( pMesh, pCurrChild, oExtents, vPoint, aNodes );
			}
		}

		//Navigate::TreeNode *pNode;
		//Octant::OCTANT oChildSet = oBox.BestChild( vPoint );

		//Octant child = oBox.Child( s_OctantPairsXZ[ oChildSet ][ 0 ] );
		//pNode = pTreeNode->data.branch.children[ s_OctantPairsXZ[ oChildSet ][ 0 ] ];
		//if ( pNode != NULL )
		//{
		//	s_PointNodeIterator_CollectPointsXZ( pMesh, pNode, child, vPoint, aNodes );
		//}

		//child = oBox.Child( s_OctantPairsXZ[ oChildSet ][ 1 ] );
		//pNode = pTreeNode->data.branch.children[ s_OctantPairsXZ[ oChildSet ][ 1 ] ];
		//if ( pNode != NULL )
		//{
		//	s_PointNodeIterator_CollectPointsXZ( pMesh, pNode, child, vPoint, aNodes );
		//}
	}
}

Navigate::PointNodeIterator::PointNodeIterator( Mesh *pMesh, const Vector3 &vPoint ) :
Navigate::Iterator< unsigned int >(),
m_iCurrIndex( 0 )
{
	// record all nodes that vPoint lies in w/in the XZ plane.  Sort them in distance order.
	MinQueue< float, _PointNodeIteratorCandidate > aNodes;
	s_PointNodeIterator_CollectPointsXZ( pMesh, pMesh->root, pMesh->extents, vPoint, aNodes );

	// add the candidate nodes in order
	_PointNodeIteratorCandidate candidate;
	while ( aNodes.Dequeue( candidate ) )
	{
		m_Nodes.Add( candidate.iNode );
	}
}

Navigate::PointNodeIterator::~PointNodeIterator( void )
{
	m_Nodes.Clear();
	m_Nodes.Reset();
	m_iCurrIndex = 0;
}

unsigned int Navigate::PointNodeIterator::operator []( unsigned int index )
{
	ASSERT( index < ( unsigned int )m_Nodes.Count() );
	return( m_Nodes[ index ] );
}

unsigned int Navigate::PointNodeIterator::Count( void )
{
	return( ( unsigned int )m_Nodes.Count() );
}

bool Navigate::PointNodeIterator::IterateInit( void )
{
	m_iCurrIndex = 0;
	return( m_Nodes.Count() > 0 );
}

bool Navigate::PointNodeIterator::Iterate( unsigned int &item )
{
	if ( m_iCurrIndex < ( unsigned int )m_Nodes.Count() )
	{
		item = m_Nodes[ m_iCurrIndex++ ];
		return( m_iCurrIndex < ( unsigned int )m_Nodes.Count() );
	}
	else
	{
		return( false );
	}
}

//////////////////////////////////////////////////////////////////////////
// ClosestPointNodeSearch
//////////////////////////////////////////////////////////////////////////

Navigate::ClosestPointNodeSearch::ClosestPointNodeSearch( void )
{
}

Navigate::ClosestPointNodeSearch::~ClosestPointNodeSearch( void )
{
}

unsigned int Navigate::ClosestPointNodeSearch::SearchForPoint( Mesh *pMesh, const Vector3 &vPoint )
{
	// record all nodes that vPoint lies in w/in the XZ plane.  Sort them in distance order.
	_PointNodeIteratorCandidate aNode;
	aNode.fDistance = MAXFLOAT;
	aNode.iNode = ( unsigned int )-1;
	s_ClosestPointNodeSearch_ClosestPointXZ( pMesh, pMesh->root, pMesh->extents, vPoint, aNode );

	return( aNode.iNode );
}

//////////////////////////////////////////////////////////////////////////
// PointOctantIterator
//////////////////////////////////////////////////////////////////////////

static Navigate::TreeNode *s_FindPointOctant( Navigate::TreeNode *node, Octant &oBox, const Vector3 &vPoint )
{
	if ( ( node->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
	{	// leaf node, this must be it
		return( node );
	}
	else
	{	// keep searching in the best child
		Octant::OCTANT oBestChild = oBox.BestChild( vPoint );
		if ( node->data.branch.children[ ( unsigned int )oBestChild ] == NULL )
		{
			return( NULL );
		}
		else
		{
			Octant oboxChild = oBox.Child( oBestChild );
			return( s_FindPointOctant( node->data.branch.children[ ( unsigned int )oBestChild ], oboxChild, vPoint ) );
		}
	}
}

Navigate::PointOctantIterator::PointOctantIterator( Mesh *pMesh, const Vector3 &vPoint ) :
Navigate::Iterator< Navigate::TreeNode* >()
{
	m_pTreeNode = s_FindPointOctant( pMesh->root, pMesh->extents, vPoint );
	ASSERT( m_pTreeNode != NULL );
}

Navigate::PointOctantIterator::~PointOctantIterator( void )
{
	m_pTreeNode = NULL;
}

Navigate::TreeNode *Navigate::PointOctantIterator::operator []( unsigned int index )
{
	ASSERT( index == 0 );
	return( m_pTreeNode );
}

unsigned int Navigate::PointOctantIterator::Count( void )
{
	return( m_pTreeNode != NULL ? 1 : 0 );
}

bool Navigate::PointOctantIterator::IterateInit( void )
{
	return( m_pTreeNode != NULL );
}

bool Navigate::PointOctantIterator::Iterate( Navigate::TreeNode *&item )
{
	item = m_pTreeNode;
	return( false );
}

//////////////////////////////////////////////////////////////////////////
// SegmentIntersectIterator
//////////////////////////////////////////////////////////////////////////
/*!
Given an AABB, iterates the Navigate::TreeNodes that contain it.
*/
static void s_FindAABBOctant( Navigate::TreeNode *node, Octant &oBox, MinQueue< float, Navigate::TreeNode* > &aTreeNodes, const AABB &iBox )
{
	if ( node == NULL )
	{
		return;
	}
	else if ( ( node->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
	{	// leaf node, this must be it
		float fFirstGuessDistanceSQ = ( oBox.Center() - iBox.Center() ).LengthSquared();
		aTreeNodes.Enqueue( fFirstGuessDistanceSQ, node );
		return;
	}
	else
	{	// keep searching if this octant interacts with the AABB
		if ( oBox.Touches( iBox ) || oBox.Contains( iBox ) )
		{
			for ( unsigned int i = 0; i < 8; ++i )
			{
				Octant oChild = oBox.Child( ( Octant::OCTANT )i );
				bool bInteracts = ( oChild.Touches( iBox ) || oChild.Contains( iBox ) );
				if ( bInteracts )
				{	// only search children that interact with the AABB
					s_FindAABBOctant( node->data.branch.children[ i ], oChild, aTreeNodes, iBox );
				}
			}
		}
	}
}

Navigate::AABBOctantIterator::AABBOctantIterator( Mesh *pMesh, const Vector3 &vPoint, float fRadius ) :
m_TreeNodes()
{
	AABB Box;
	Box.Set( ( vPoint - fRadius ), ( vPoint + fRadius ) );
	s_FindAABBOctant( pMesh->root, pMesh->extents, m_TreeNodes, Box );
}

Navigate::AABBOctantIterator::AABBOctantIterator( Mesh *pMesh, const AABB &Box ) :
m_TreeNodes()
{
	ASSERT_PTR( pMesh );
	s_FindAABBOctant( pMesh->root, pMesh->extents, m_TreeNodes, Box );
}

Navigate::AABBOctantIterator::~AABBOctantIterator( void )
{
}

Navigate::TreeNode* Navigate::AABBOctantIterator::operator []( unsigned int index )
{
	// this will not give an in-order traversal of the queue, but it will hit every element.
	return( *( m_TreeNodes[ index ] ) );
}

unsigned int Navigate::AABBOctantIterator::Count( void )
{
	return( ( unsigned int )m_TreeNodes.Count() );
}

bool Navigate::AABBOctantIterator::IterateInit( void )
{
	return( m_TreeNodes.Count() > 0 );
}

bool Navigate::AABBOctantIterator::Iterate( TreeNode *&item )
{
	if ( ( unsigned int )m_TreeNodes.Count() == 0 )
	{
		return( false );
	}
	else
	{
		bool bFound = m_TreeNodes.Dequeue( item );
		return( bFound );
	}
}

//////////////////////////////////////////////////////////////////////////
// SegmentIntersectIterator
//////////////////////////////////////////////////////////////////////////

static void s_SegmentIntersectIterator_Recurse( Navigate::Mesh *pMesh, const SemiSegment &sSegment, const Octant &oExtents, Navigate::TreeNode *pTreeNode, DynamicArray< Vector3 > &aIntersections, bool bIgnoreObstructions = true )
{
	if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
	{
		//if ( ( oExtents.Touches( sSegment ) || oExtents.Contains( sSegment ) ) )
		//{	// if the segment interacts with the extents, test the segment against all the faces
			float fT, fU, fV, fLength;
			Vector3 vNormalDirection = ( sSegment.End() - sSegment.Start() );
			fLength = vNormalDirection.Length();
			vNormalDirection.Normalize();
			Vector3 v0, v1, v2;
			Navigate::TreeNode::Leaf *pLeaf = &( pTreeNode->data.leaf );
			for ( int i = 0; i < pLeaf->nodeCount; ++i )
			{
				/* don't retest nodes */

				if ( pMesh->nodes[ pLeaf->nodes[ i ] ].collisionTest != 0 )
					continue;

				/* if considering obstructions, bail if node is occluded */

				if ( ( bIgnoreObstructions == false ) && ( pMesh->nodes[ pLeaf->nodes[ i ] ].occlusionCount > 0 ) )
					continue;

				/* test this node */

				Navigate::Node *pNode = &( pMesh->nodes[ pLeaf->nodes[ i ] ] );
				v0 = pMesh->points[ pNode->points[ 0 ] ];
				v1 = pMesh->points[ pNode->points[ 1 ] ];
				v2 = pMesh->points[ pNode->points[ 2 ] ];
				fT = fU = fV = 0.0f;
				if ( ( TriangleTest::RayIntersection( sSegment.Start(), vNormalDirection, v0, v1, v2, fT, fU, fV ) ) && ( fT <= fLength ) )
				{
					aIntersections.Add( ( vNormalDirection * fT ) + sSegment.Start() );
				}
				// record that we've checked this node
				pMesh->nodes[ pLeaf->nodes[ i ] ].collisionTest = 1;
			}
		//}
		//else
		//{
		//	return;
		//}
	}
	else
	{	// test each child node for interaction with the given segment
		for ( unsigned int iCurrChild = 0; iCurrChild < 8; ++iCurrChild )
		{
			Octant oChild = oExtents.Child( ( Octant::OCTANT )iCurrChild );
			Navigate::TreeNode *pCurrChild = pTreeNode->data.branch.children[ iCurrChild ];
			if ( ( pCurrChild != NULL ) && ( oChild.Touches( sSegment ) || oChild.Contains( sSegment ) ) )
			{	// recurse!
				s_SegmentIntersectIterator_Recurse( pMesh, sSegment, oChild, pCurrChild, aIntersections );
			}
		}
	}
}

Navigate::SegmentIntersectIterator::SegmentIntersectIterator( Mesh *pMesh, const SemiSegment &sSegment, bool bIgnoreObstructions ) :
m_iCurrIntersection( 0 )
{
	gbl_NavManager.ResetNodeCollisions( pMesh );	// clear previous collision test flags
	m_Intersections.Clear();
	s_SegmentIntersectIterator_Recurse( pMesh, sSegment, pMesh->extents, pMesh->root, m_Intersections, bIgnoreObstructions );
}

Navigate::SegmentIntersectIterator::~SegmentIntersectIterator( void )
{
}

Vector3 Navigate::SegmentIntersectIterator::operator []( unsigned int index )
{
	return( m_Intersections[ index ] );
}

unsigned int Navigate::SegmentIntersectIterator::Count( void )
{
	return( m_Intersections.Count() );
}

bool Navigate::SegmentIntersectIterator::IterateInit( void )
{
	m_iCurrIntersection = 0;
	return( m_Intersections.Count() > 0 );
}

bool Navigate::SegmentIntersectIterator::Iterate( Vector3 &item )
{
	if ( m_iCurrIntersection < m_Intersections.Count() )
	{
		item = m_Intersections[ m_iCurrIntersection++ ];
		return( true );
	}
	else
	{
		return( false );
	}
}

//////////////////////////////////////////////////////////////////////////
// LineEdgeQuery
//////////////////////////////////////////////////////////////////////////

Navigate::LineEdgeQuery::LineEdgeQuery() :
m_bResult( false ),
m_bHasWorkToDo( true ),
m_pMesh( NULL ),
m_iRecursionLevel( 0 )
{
	m_vBegin.Clear();
	m_vEnd.Clear();
}

Navigate::LineEdgeQuery::~LineEdgeQuery( void )
{
	m_pMesh = NULL;
	m_vBegin.Clear();
	m_vEnd.Clear();
	m_bHasWorkToDo = false;
	m_iRecursionLevel = 0;
}

bool Navigate::LineEdgeQuery::Configure( Mesh *pMesh, const Vector3 &vBegin, const Vector3 &vEnd )
{
	m_pMesh			= pMesh;
	m_vBegin			= vBegin;
	m_vEnd			= vEnd;
	m_bHasWorkToDo	= true;
	m_iRecursionLevel	= 0;

	m_RecursionFrames[ m_iRecursionLevel ].m_pTreeNode = m_pMesh->root;
	m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fX = m_pMesh->extents.Center().x();
	m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fY = m_pMesh->extents.Center().y();
	m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fZ = m_pMesh->extents.Center().z();
	m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fHalfSize = m_pMesh->extents.HalfSize();
	m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr = 0;

	return( true );
}

bool Navigate::LineEdgeQuery::HasWorkToDo( void )
{
	return( m_bHasWorkToDo );
}

unsigned int Navigate::LineEdgeQuery::DoWork( unsigned int iSteps )
{
	unsigned int iStepsUsed = 0;

	while ( ( m_bHasWorkToDo ) && ( iStepsUsed < iSteps ) )
	{
		Navigate::TreeNode	*pTreeNode	= m_RecursionFrames[ m_iRecursionLevel ].m_pTreeNode;

		if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
		{	// test this leaf node for intersection
			unsigned int iCurrNode = m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrNode;

			while ( iCurrNode < pTreeNode->data.leaf.nodeCount )
			{
				unsigned int iCurrEdge = m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrEdge;
				while ( iCurrEdge < 3 )
				{
					Navigate::Node *pNode = &( m_pMesh->nodes[ pTreeNode->data.leaf.nodes[ iCurrNode ] ] );
					if (
						( pNode->neighbors[ iCurrEdge ] == ( Navigate::NodeIndexType )-1 ) ||
						( m_pMesh->nodes[ pNode->neighbors[ iCurrEdge ] ].occlusionCount > 0 )
						)
					{	// this is a hard edge, or the edge of an obstructed node
						Vector3 vEdgeBegin	= m_pMesh->points[ pNode->points[ iCurrEdge ] ];
						Vector3 vEdgeEnd	= m_pMesh->points[ pNode->points[ ( iCurrEdge + 1 ) % 3 ] ];

						++iStepsUsed;

						if ( TriangleTest::LinesIntersectXZ( vEdgeBegin, vEdgeEnd, m_vBegin, m_vEnd ) )
						{
							// we are *done*
							m_bResult = true;
							m_bHasWorkToDo = false;
							m_iRecursionLevel = 0;
							return( iStepsUsed );
						}
					}

					++( m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrEdge );
					iCurrEdge = m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrEdge;

					if ( iStepsUsed >= iSteps )
					{
						return( iStepsUsed );
					}
				}

				// reset the current edge
				m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrEdge = 0;

				++( m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrNode );
				iCurrNode = m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrNode;
			}

			if ( m_iRecursionLevel == 0 )
			{	// this leaf node was the only node
				m_bHasWorkToDo = false;
				m_iRecursionLevel = 0;
				return( iStepsUsed );
			}
			else
			{	// continue to process parent node
				--m_iRecursionLevel;
			}

			// don't check work units here - there was no 'work' performed outside of the inner loop

		}
		else
		{	// check the children for (possible) interaction with the segment
			Vector3					vCenter(
				m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fX,
				m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fY,
				m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fZ );
			Octant					oBox( vCenter, m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fHalfSize );
			unsigned char			octCurr = m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr;

			while ( octCurr < 8 )
			{
				if ( pTreeNode->data.branch.children[ octCurr ] != NULL )
				{
					Octant oChild = oBox.Child( ( Octant::OCTANT )octCurr );
					SemiSegment seg;
					seg.SetEndPoints( m_vBegin, m_vEnd );
					++iStepsUsed;
					if ( ( oChild.Contains( seg ) ) || ( oChild.Touches( seg, 0.01f ) ) )
					{	// only recurse if the line touches this child

						// record which child we will be working on if/when we come back
						m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr = ( octCurr + 1 );

						++m_iRecursionLevel;
						ASSERT( m_iRecursionLevel < LINEEDGEQUERY_MAX_RECURSION_DEPTH );

						// setup for the recursive step
						m_RecursionFrames[ m_iRecursionLevel ].m_pTreeNode = pTreeNode->data.branch.children[ octCurr ];
						if ( ( m_RecursionFrames[ m_iRecursionLevel ].m_pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
						{
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrNode = 0;
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.leaf.m_iCurrEdge = 0;
						}
						else
						{
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fX = oChild.Center().x();
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fY = oChild.Center().y();
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fZ = oChild.Center().z();
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_fHalfSize = oChild.HalfSize();
							m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr = 0;
						}

						// break out of the child loop
						break;
					}

					if ( iStepsUsed >= iSteps )
					{
						break;
					}
				}

				// increment to the next octant child
				++( m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr );
				octCurr = m_RecursionFrames[ m_iRecursionLevel ].m_Data.branch.m_octCurr;

				if ( iStepsUsed >= iSteps )
				{
					break;
				}
			}

			if ( m_iRecursionLevel == 0 )
			{	// done searching - unsuccessful search
				m_bHasWorkToDo = false;
				return( iStepsUsed );
			}
			else if ( octCurr >= 8 )
			{	// done with this child node
				--m_iRecursionLevel;
			}
		}
	}

	return( iStepsUsed );
}

//// utility function to find the TreeNode for vBegin, chop the line to w/in the TreeNode, returning if and intersections happen, and the point of the new line 
//bool Navigate::LineEdgeQuery::TestLine( const Navigate::Mesh *pMesh, const Navigate::TreeNode *pTreeNode, const Octant &oBox, const SemiSegment &seg ) const
//{
//	if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
//	{	// test this leaf node for intersection
//		for( unsigned int iCurrNode = 0; iCurrNode < pTreeNode->data.leaf.nodeCount; ++iCurrNode )
//		{	// for each node in this Octant
//			for( unsigned int iCurrEdge = 0; iCurrEdge < 3; ++iCurrEdge )
//			{	// for each edge of this face
//				Navigate::Node *pNode = &( pMesh->nodes[ pTreeNode->data.leaf.nodes[ iCurrNode ] ] );
//				if (
//					( pNode->neighbors[ iCurrEdge ] == ( Navigate::NodeIndexType )-1 ) ||
//					( pMesh->nodes[ pNode->neighbors[ iCurrEdge ] ].occlusionCount > 0 )
//				)
//				{	// this is a hard edge, or the edge of an obstructed node
//					Vector3 vEdgeBegin	= pMesh->points[ pNode->points[ iCurrEdge ] ];
//					Vector3 vEdgeEnd	= pMesh->points[ pNode->points[ ( iCurrEdge + 1 ) % 3 ] ];
//
//					if ( TriangleTest::LinesIntersectXZ( vEdgeBegin, vEdgeEnd, seg.Start(), seg.End() ) )
//					{
//						return( true );
//					}
//				}
//			}
//		}
//	}
//	else
//	{	// check the children for (possible) interaction with the segment
//		for( unsigned int oct = 0; oct < 8; ++oct )
//		{
//			if ( pTreeNode->data.branch.children[ oct ] != NULL )
//			{
//				Octant oChild = oBox.Child( ( Octant::OCTANT )oct );
//				if ( ( oChild.Contains( seg ) ) || ( oChild.Touches( seg, 0.01f ) ) )
//				{	// only recurse if the line touches this child
//					bool bResult = TestLine( pMesh, pTreeNode->data.branch.children[ oct ], oChild, seg );
//					if ( bResult )
//					{
//						return( bResult );
//					}
//				}
//			}
//		}
//	}
//
//	return( false );
//}
//////////////////////////////////////////////////////////////////////////
// COVER IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

Navigate::Cover::Cover( void ) :
	m_pBegin( NULL ),
	m_pEnd( NULL ),
	m_pNext( NULL ),
	m_pPrev( NULL ),
	m_fDot( 0.0f ),
	m_iNodeID( 0 ),
	m_iEdgeID( 0 ),
	m_iCoverType( 0 ),
	m_iMiscFlags( 0 )
{
}

//////////////////////////////////////////////////////////////////////////
// COVEREDGEITERATOR IMPLEMENTATION
//////////////////////////////////////////////////////////////////////////

static float s_CalcCoverPoint( const Vector3Packed &Test, const Vector3Packed &Begin, const Vector3Packed &End, Vector3Packed &Result )
{
	float   fT;
	Vector3 Dir, Temp;

	// Calculate the direction of the line
	Dir = ( End - Begin );

	// Get the direction between the test point and the start of the line.
	Temp = ( Test - Begin );

	// Get the dot product between the 2 directions. This will tell
	// you how far up the line the new point needs to be
	fT = Temp.Dot( Dir ) / Dir.Dot( Dir );

	if ( fT < 0.0f )
	{
		fT = 0.0f;
	}
	else if ( fT > 1.0f )
	{
		fT = 1.0f;
	}

	Result = ( Begin + ( Dir * fT ) );

	return( fT );
}

static Vector3	s_vSortTarget;

static int s_CompareCover( const void *pA, const void *pB )
{
	Navigate::Cover *pLHS = ( Navigate::Cover* )pA;
	Navigate::Cover *pRHS = ( Navigate::Cover* )pB;

	float fLHS_Distance = ( s_vSortTarget - pLHS->m_vCover ).LengthSquared();
	float fRHS_Distance = ( s_vSortTarget - pRHS->m_vCover ).LengthSquared();

	if ( fLHS_Distance < fRHS_Distance )
	{
		return( -1 );
	}
	else if ( Math::Equal( fLHS_Distance, fRHS_Distance ) )
	{
		return( 0 );
	}
	else
	{
		return( 1 );
	}
}

Navigate::CoverIterator::CoverIterator( Mesh *pMesh, const Vector3 &vTargetPoint, const Vector3 &vOrigin, const float fRadius, const float fDotThreshold ) :
m_SearchFrames( 32, 0 ),
m_bHasWorkToDo( true )
{
	m_pMesh = pMesh;
	m_vTargetPoint = vTargetPoint;
	m_AABB.Set( ( vOrigin - fRadius ), ( vOrigin + fRadius ) );

	_CoverSearchFrame frame;
	frame.m_pCurrent			= m_pMesh->root;
	frame.m_pParent			= NULL;
	frame.m_iCurrIndex		= 0;
	frame.m_Octant				= m_pMesh->extents;
	frame.m_fDotThreshold	= fDotThreshold;
	m_SearchFrames.Add( frame );
}

Navigate::CoverIterator::CoverIterator( Mesh *pMesh, const Vector3 &vTargetPoint, const AABB &box, const float fDotThreshold ) :
m_SearchFrames( 32, 0 ),
m_bHasWorkToDo( true )
{
	m_pMesh = pMesh;
	m_vTargetPoint = vTargetPoint;
	m_AABB = box;

	_CoverSearchFrame frame;
	frame.m_pCurrent			= m_pMesh->root;
	frame.m_pParent			= NULL;
	frame.m_iCurrIndex		= 0;
	frame.m_Octant				= m_pMesh->extents;
	frame.m_fDotThreshold	= fDotThreshold;
	m_SearchFrames.Add( frame );
}

Navigate::CoverIterator::~CoverIterator( void )
{
	m_Cover.Clear();
	m_Cover.Reset();
	m_SearchFrames.Clear();
	m_SearchFrames.Reset();
}

unsigned int Navigate::CoverIterator::FindCover( unsigned int iSteps )
{
	unsigned int iStepsTaken = 0;

	if ( iStepsTaken < iSteps )
	{
		_CoverSearchFrame *pCurrFrame = m_SearchFrames.Get( -1 );
		ASSERT( pCurrFrame != NULL );
		ASSERT( ( pCurrFrame->m_pCurrent->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF );

		// test if the check volume interacts w/ the current node's volume
		if ( pCurrFrame->m_Octant.Touches( m_AABB ) )
		{
			Cover	cover;
			Vector3	vBegin, vEnd, vCenter, vTargetToEdge;
			const Vector3 vYAxis( 0.0f, 1.0f, 0.0f );
			Navigate::TreeNode *pCurrTreeNode = pCurrFrame->m_pCurrent;

			unsigned int iCoverCount = pCurrTreeNode->data.leaf.hardEdgeCount + pCurrTreeNode->data.leaf.extraCoverCount;
			for( unsigned int iCurrCover = pCurrFrame->m_iCurrCoverIndex; iCurrCover < iCoverCount; ++iCurrCover )
			{
				// keep the restart variable up-to-date
				pCurrFrame->m_iCurrCoverIndex = iCurrCover;

				// bail if we're out of 'work units'
				if ( iStepsTaken >= iSteps )
				{
					return( iStepsTaken );
				}

				// check for cover
				unsigned int iNodeID = pCurrTreeNode->data.leaf.nodes[ pCurrTreeNode->data.leaf.edgeNodes[ iCurrCover ] ];
				unsigned int iEdgeID = pCurrTreeNode->data.leaf.edgeData[ iCurrCover ].edgeID;

				// skip previously recorded cover nodes
				bool bAlreadyCover = false;
				for( unsigned int i = 0; i < ( unsigned int )m_Cover.Count(); ++i )
				{
					if ( ( m_Cover[ i ].m_iNodeID == iNodeID ) && ( m_Cover[ i ].m_iEdgeID == iEdgeID ) )
					{
						bAlreadyCover = true;
						break;
					}
				}
				if ( bAlreadyCover )
				{
					continue;
				}

				// make sure this edge is really cover
				cover.m_iCoverType	= pCurrTreeNode->data.leaf.edgeData[ iCurrCover ].edgeCoverType;
				if (
					( cover.m_iCoverType == Navigate::TreeNode::FULLCOVER_EDGE )		||
					( cover.m_iCoverType == Navigate::TreeNode::STANDCOVER_EDGE )		||
					( cover.m_iCoverType == Navigate::TreeNode::CROUCHCOVER_EDGE )
				)
				{

					// check for cover edge
					ASSERT( m_pMesh->nodes[ iNodeID ].neighbors[ iEdgeID ] == ( Navigate::NodeIndexType )-1 );
					cover.m_pBegin = &( m_pMesh->points[ m_pMesh->nodes[ iNodeID ].points[ iEdgeID ] ] );
					cover.m_pEnd = &( m_pMesh->points[ m_pMesh->nodes[ iNodeID ].points[ ( iEdgeID < 2 ? ( iEdgeID + 1 ) : 0 ) ] ] );
					vCenter = ( *cover.m_pEnd - *cover.m_pBegin ) * 0.5f;
					vTargetToEdge = ( ( vCenter + *cover.m_pBegin ) - m_vTargetPoint );

					// Here's where the a major portion of our work comes in, so count this as the work step
					vTargetToEdge.Normalize();
					vCenter.Normalize();
					++iStepsTaken;

					cover.m_fDot			= vTargetToEdge.Cross( vYAxis ).Dot( vCenter );
					if ( cover.m_fDot > pCurrFrame->m_fDotThreshold )
					{	// potential cover
						cover.m_iNodeID		= iNodeID;
						cover.m_iEdgeID		= iEdgeID;
						cover.m_iMiscFlags	= pCurrTreeNode->data.leaf.edgeData[ iCurrCover ].edgeMiscFlags;
						s_CalcCoverPoint( m_AABB.Center(), *( cover.m_pBegin ), *( cover.m_pEnd ), cover.m_vCover );
						m_Cover.Add( cover );
					}
				}
			}

			pCurrFrame->m_iCurrCoverIndex = 0;
		}
	}

	return( iStepsTaken );
}

bool Navigate::CoverIterator::HasWorkToDo( void )
{
	return( m_bHasWorkToDo );
}

unsigned int Navigate::CoverIterator::DoWork( unsigned int iSteps )
{
	ASSERT( m_bHasWorkToDo );

	unsigned int iStepsTaken = 0;

	if ( m_SearchFrames.Count() <= 0 )
	{	
		// no more searching to do
		// flag this search as complete
		m_bHasWorkToDo = false;
		return( iStepsTaken );
	}

	while (
		( m_SearchFrames.Count() > 0 ) &&	// The search is incomplete.
		( iStepsTaken < iSteps )			// We still have time to work.
		)
	{
		_CoverSearchFrame	*pCurrFrame = m_SearchFrames.Get( -1 );
		ASSERT( pCurrFrame != NULL );

		if ( ( pCurrFrame->m_pCurrent->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
		{	// leaf node, perform the search
			iStepsTaken += FindCover( iSteps - iStepsTaken );
			if ( iStepsTaken < iSteps )
			{	// we're done searching in the current octant, pop that frame from the stack.
				// otherwise, we need to continue processing that frame on the next DoWork call.
				m_SearchFrames.Delete( -1 );
			}
		}
		else if ( pCurrFrame->m_iCurrIndex >= 8 )
		{	// we're done with this frame, go back up a step.
			m_SearchFrames.Delete( -1 );
		}
		else
		{	// check the children
			while ( pCurrFrame->m_iCurrIndex < 8 )
			{
				if ( iStepsTaken >= iSteps )
				{
					break;
				}
				else
				{	// recurse
					if ( pCurrFrame->m_pCurrent->data.branch.children[ pCurrFrame->m_iCurrIndex ] != NULL )
					{	// don't recurse where there is no interesting data.
						_CoverSearchFrame	frame;
						frame.m_iCurrIndex	= 0;
						frame.m_pParent		= pCurrFrame->m_pCurrent;
						frame.m_pCurrent	= frame.m_pParent->data.branch.children[ pCurrFrame->m_iCurrIndex ];
						frame.m_Octant		= pCurrFrame->m_Octant.Child( ( Octant::OCTANT )pCurrFrame->m_iCurrIndex );
						frame.m_iCurrCoverIndex	= 0;
						m_SearchFrames.Add( frame );
						++pCurrFrame->m_iCurrIndex;
						break;
					}
				}
				++pCurrFrame->m_iCurrIndex;
			}
		}
	}

	if ( m_SearchFrames.Count() <= 0 )
	{
		if ( iStepsTaken < iSteps )
		{	// sort the Cover in ascending order of distance(^2) from the target volume's center
			s_vSortTarget = m_AABB.Center();
			qsort( m_Cover.GetPtr(), m_Cover.Count(), sizeof( Cover ), s_CompareCover );

			// record the sort as one 'work unit'
			++iStepsTaken;
		}

		if ( iStepsTaken < iSteps )
		{	// connect the cover edges (prev,next)
			unsigned int iCoverCount = m_Cover.Count();
			for( int i = 0; i < ( int )( iCoverCount - 1 ); ++i )
			{
				for( unsigned int j = ( unsigned int )( i + 1 ); j < iCoverCount; ++j )
				{
					ASSERT( !( ( m_Cover[ i ].m_iNodeID == m_Cover[ j ].m_iNodeID ) && ( m_Cover[ i ].m_iEdgeID == m_Cover[ j ].m_iEdgeID ) ) );

					if ( m_Cover[ i ].m_pBegin == m_Cover[ j ].m_pEnd )
					{	// these two are connected
						m_Cover[ i ].m_pPrev = &m_Cover[ j ];
						m_Cover[ j ].m_pNext = &m_Cover[ i ];
					}

					if ( m_Cover[ i ].m_pEnd == m_Cover[ j ].m_pBegin )
					{	// these two are connected the other way
						m_Cover[ i ].m_pNext = &m_Cover[ j ];
						m_Cover[ j ].m_pPrev = &m_Cover[ i ];
					}
				}
			}

			// record the connecting as one 'work unit'
			++iStepsTaken;

			// flag this search as complete
			m_bHasWorkToDo = false;
		}
	}

	return( iStepsTaken );
}

Navigate::Cover Navigate::CoverIterator::operator []( unsigned int index )
{
	ASSERT( index < ( unsigned int )m_Cover.Count() );
	return( m_Cover[ index ] );
}

unsigned int Navigate::CoverIterator::Count( void )
{
	return( ( unsigned int )m_Cover.Count() );
}

bool Navigate::CoverIterator::IterateInit( void )
{
	m_iCurrIndex = 0;
	return( m_Cover.Count() > 0 );
}

bool Navigate::CoverIterator::Iterate( Cover &item )
{
	unsigned int iCoverCount = ( unsigned int )m_Cover.Count();

	ASSERT( iCoverCount > 0 );

	if ( iCoverCount == 0 )
	{
		return( false );
	}

	if ( m_iCurrIndex < iCoverCount )
	{
		item = m_Cover[ m_iCurrIndex ];
		if ( ( m_iCurrIndex + 1 ) < iCoverCount )
		{
			++m_iCurrIndex;
			return( true );
		}
	}

	return( false );
}

Navigate::RandomPointSearch::RandomPointSearch( Navigate::Mesh *pMesh ) :
m_bFoundValidPoint( false ),
m_vPoint( 0.0f, 0.0f, 0.0f )
{
	// search for a random leaf node.
	bool bFoundLeaf = false;
	Navigate::TreeNode *pCurrNode = pMesh->root;
	Octant extents = pMesh->extents;
	while ( !bFoundLeaf )
	{
		if ( ( pCurrNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
		{
			bFoundLeaf = true;
			break;
		}
		else
		{	// pick a random NON-NULL child to traverse
			unsigned int dwChild = g_random.UpTo( 7 ); //0..7
			unsigned int dwStarted = dwChild;
			while ( pCurrNode->data.branch.children[ dwChild ] == NULL )
			{
				dwChild = ( dwChild + 1 ) % 8;
				ASSERT( dwChild != dwStarted );
				if ( dwChild == dwStarted )
				{
					m_bFoundValidPoint = false;
					return;
				}
			}
			extents = extents.StandardChild( ( Octant::OCTANT )dwChild );
			pCurrNode = pCurrNode->data.branch.children[ dwChild ];
		}
	}

	// randomly pick a node in this leaf
	unsigned int dwNodeID = g_random.UpTo( pCurrNode->data.leaf.nodeCount );

	// now, pick a point in the triangle using barycentric coordinates
	Vector3 v0, v1, v2;
	v0 = pMesh->points[ pMesh->nodes[ pCurrNode->data.leaf.nodes[ dwNodeID ] ].points[ 0 ] ];
	v1 = pMesh->points[ pMesh->nodes[ pCurrNode->data.leaf.nodes[ dwNodeID ] ].points[ 1 ] ];
	v2 = pMesh->points[ pMesh->nodes[ pCurrNode->data.leaf.nodes[ dwNodeID ] ].points[ 2 ] ];

	unsigned short dwU, dwV, dwW;
	dwU = ( unsigned short )( ( unsigned int )g_random.Int() & 0x0000ffff );
	dwV = ( unsigned short )( ( unsigned int )g_random.Int() & 0x0000ffff );
	dwW = ( unsigned short )( ( unsigned int )g_random.Int() & 0x0000ffff );

	float u, v, w;
	u = ( ( float )dwU / ( float )( dwU + dwV + dwW ) );
	v = ( ( float )dwV / ( float )( dwU + dwV + dwW ) );
	w = 1.0f - ( float )u - ( float )v;

	ASSERT( Math::Equal( ( u + v + w ), 1.0f ) );

	m_vPoint = (
		( w * v0 ) +
		( u * v1 ) +
		( v * v2 )
	);
	m_bFoundValidPoint = true;
}

Navigate::RandomPointSearch::~RandomPointSearch( void )
{
}

//////////////////////////////////////////////////////////////////////////
// Search for the closest point in the mesh
//////////////////////////////////////////////////////////////////////////

Navigate::PointSearch::PointSearch( Navigate::Mesh *pMesh, const AABB &aabb ) :
m_OctantIterator( pMesh, aabb )	/* this finds all tree nodes that interact w/ the given aabb */
{
	ASSERT_PTR( pMesh );
	m_pMesh = pMesh;
	m_pCurrentTreeNode = NULL;
	m_iCurrentFaceIndex = 0;
	m_iCurrentEdgeIndex = 0;
	m_bHasWorkToDo = true;
	m_bCandidateFound = false;
	m_vTarget = aabb.Center();
	m_vCandidate.Clear();
	m_fBestCandidateDistSQ = MAXFLOAT;
}

Navigate::PointSearch::~PointSearch( void )
{
}

bool Navigate::PointSearch::HasWorkToDo( void )
{
	return( m_bHasWorkToDo );
}

unsigned int Navigate::PointSearch::DoWork( unsigned int iSteps )
{
	unsigned int iCurrentSteps = 0;

	while ( iCurrentSteps < iSteps )
	{
		if ( m_pCurrentTreeNode == NULL )
		{
			if ( !m_OctantIterator.Iterate( m_pCurrentTreeNode ) )
			{	// no more nodes to process
				m_pCurrentTreeNode = NULL;
				m_bHasWorkToDo = false;
				break;
			}

			m_iCurrentFaceIndex = 0;
		}

		ASSERT( ( m_pCurrentTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF );

		while ( ( iCurrentSteps < iSteps ) && ( m_iCurrentFaceIndex < m_pCurrentTreeNode->data.leaf.nodeCount ) )
		{
			Navigate::NodeIndexType iNode = m_pCurrentTreeNode->data.leaf.nodes[ m_iCurrentFaceIndex ];

			if ( m_iCurrentEdgeIndex >= 3 )
			{
				m_iCurrentEdgeIndex = 0;
			}

			while ( ( iCurrentSteps < iSteps ) && ( m_iCurrentEdgeIndex < 3 ) )
			{
				//
				// only check *hard* edges
				if ( m_pMesh->nodes[ iNode ].neighbors[ m_iCurrentEdgeIndex ] == ( Navigate::NodeIndexType )-1 )
				{
					Segment seg;
					seg.Set( m_pMesh->points[ m_pMesh->nodes[ iNode ].points[ m_iCurrentEdgeIndex ] ], m_pMesh->points[ m_pMesh->nodes[ iNode ].points[ ( ( m_iCurrentEdgeIndex + 1 ) % 3 ) ] ] );

					Vector3	vPoint;
					float		fDistSQ;

					vPoint = seg.TrueClosestPoint( m_vTarget );
					fDistSQ = ( m_vTarget - vPoint ).LengthSquared();
					if ( fDistSQ < m_fBestCandidateDistSQ )
					{
						m_vCandidate = vPoint;
						m_fBestCandidateDistSQ = fDistSQ;
						m_bCandidateFound = true;
					}

					// one work step per edge checked
					++iCurrentSteps;
				}

				++m_iCurrentEdgeIndex;
			}

			++m_iCurrentFaceIndex;
		}

		if ( m_iCurrentFaceIndex >= m_pCurrentTreeNode->data.leaf.nodeCount )
		{	// move on to the next tree node full of potential faces to check
			m_iCurrentFaceIndex = 0;
			m_pCurrentTreeNode = NULL;
		}
	}

	return( iCurrentSteps );
}

bool Navigate::PointSearch::IsSearchSuccessful( void )
{
	return( !HasWorkToDo() && m_bCandidateFound );
}

Vector3 Navigate::PointSearch::GetPoint( void )
{
	ASSERT( IsSearchSuccessful() );
	return( m_vCandidate );
}
