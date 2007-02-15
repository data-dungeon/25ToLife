/********************************************************************
	created:	2003/12/23
	created:	23:12:2003   10:33
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavAgents.cpp
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavAgents
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement some A* Agents.
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "Navigate/NavAgents.h"
#include "Navigate/NavManager.h"

#include "Math/Vector.h"
#include "collide/tritest.h"

static bool s_ComparePQSearchNodes( const Navigate::PQSearchNode *&lhs, const Navigate::PQSearchNode *&rhs )
{
	// we're using Greater-than here because we want paths w/ LEAST cost to have the highest priority
	return( ( lhs->fCostFromStart + lhs->fEstCostToGoal ) > ( rhs->fCostFromStart + rhs->fEstCostToGoal ) );
}

Navigate::AStarNodeAgent::AStarNodeAgent( void ) :
nodePool( 64, 32 ),
nodesInPath( 32, 16 ),
openList(),
masterList(),
bStarted( false ),
iFrom( 0xFFFF ),
iTo( 0xFFFF )
{
}

Navigate::AStarNodeAgent::~AStarNodeAgent( void )
{
}

float Navigate::AStarNodeAgent::PathCostEstimate( Mesh *mesh, NodeIndexType iFrom, NodeIndexType iTo )
{
	// Find the Centroid of the From node
	Vector3 vFromCenter = TriangleTest::Centroid(
		mesh->points[ mesh->nodes[ iFrom ].points[ 0 ] ],
		mesh->points[ mesh->nodes[ iFrom ].points[ 1 ] ],
		mesh->points[ mesh->nodes[ iFrom ].points[ 2 ] ]
		);

	// Find the Centroid of the To node
	Vector3 vToCenter = TriangleTest::Centroid(
		mesh->points[ mesh->nodes[ iTo ].points[ 0 ] ],
		mesh->points[ mesh->nodes[ iTo ].points[ 1 ] ],
		mesh->points[ mesh->nodes[ iTo ].points[ 2 ] ]
		);

	// Return the euclidian distance between them (squared)
	return( ( vToCenter - vFromCenter ).Length() );
}

float Navigate::AStarNodeAgent::TraverseCost( Mesh* mesh, NodeIndexType iParent, NodeIndexType iFrom, NodeIndexType iTo )
{
	Vector3 v0, v1, v2;
	v0 = mesh->points[ mesh->nodes[ iFrom ].points[ 0 ] ];
	v1 = mesh->points[ mesh->nodes[ iFrom ].points[ 1 ] ];
	v2 = mesh->points[ mesh->nodes[ iFrom ].points[ 2 ] ];

	Segment line;
	line.start = ( ( ( v1 - v0 ) + ( v2 - v0 ) ) * 0.3334f );
	line.end = ( ( v1 - v0 ) * 0.5f );
	float fRadius = line.Length();

	return( fRadius * 2.0f );
}

void Navigate::AStarNodeAgent::Reset( void )
{
	iFrom		= 0xFFFF;
	iTo		= 0xFFFF;
	bStarted	= false;

	// reset our data structures
	NodeIndexType iKey = 0;
	PQSearchNode *pCurr = NULL;
	masterList.IterateInit();
	while ( masterList.Iterate( iKey, pCurr ) )
	{
		masterList.Delete( iKey );
		nodePool.Delete( pCurr );
		masterList.IterateInit();
	}
	masterList.Clear();
	nodesInPath.Clear();
	openList.RemoveAll();
}
