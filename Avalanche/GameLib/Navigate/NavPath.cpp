/********************************************************************
	created:	2003/12/18
	created:	18:12:2003   11:17
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavPath.cpp
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavPath
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement Path interface
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "Navigate/NavPath.h"
#include "Navigate/NavMesh.h"
#include "Navigate/NavIterators.h"

#include "Math/Vector.h"
#include "collide/tritest.h"

#include "Navigate/NavManager.h"

#if defined( _DEBUG )
#	include "cmdcon/cmdcon.h"
#endif

#define USE_PATH_SMOOTHING ( 1 )

static bool s_GetSharedFaceMidPoint( Navigate::Mesh *pMesh, unsigned int iFromNode, unsigned int iToNode, Vector3 &vPoint )
{
	// first, find which face these two share
	bool bFoundSharedFace = false;
	unsigned int iSharedFace;
	for( iSharedFace = 0; iSharedFace < 3; iSharedFace++ )
	{
		if ( pMesh->nodes[ iFromNode ].neighbors[ iSharedFace ] == iToNode )
		{
			bFoundSharedFace = true;
			break;
		}
	}

	if ( !bFoundSharedFace )
	{
		return( false );
	}

	Vector3 a, b;
	a = pMesh->points[ pMesh->nodes[ iFromNode ].points[ iSharedFace ] ];
	b = pMesh->points[ pMesh->nodes[ iFromNode ].points[ ( iSharedFace < 2 ? iSharedFace + 1 : 0 ) ] ];

	// get the mid-point of the shared face
	vPoint = ( ( ( b - a ) * 0.5f ) + a );
	return( true );
}

namespace Navigate
{
	//////////////////////////////////////////////////////////////////////////
	// PATHCONNECTION IMPLEMENTATION
	//////////////////////////////////////////////////////////////////////////

	PathConnection::~PathConnection( void )
	{
	}

	//////////////////////////////////////////////////////////////////////////
	// PATHSEGMENT IMPLEMENTATION
	//////////////////////////////////////////////////////////////////////////

	unsigned int PathSegment::PathSearch( unsigned int iSteps )
	{
		unsigned int iStepsUsed = gbl_NavManager.AStar_Node( m_pMesh, iSteps, data.searching.pAgent );

		if ( iStepsUsed < iSteps )
		{	// move on to the next phase

			// counting the work to transition to a new phase as a 'work unit'
			++iStepsUsed;

			Vector3	vBegin = m_Waypoints[ 0 ];
			Vector3	vEnd = m_Waypoints[ 1 ];

			// reset the waypoint list for real use
			m_Waypoints.Clear();

			// our waypoints will begin w/ the begin point
			m_Waypoints.Add( vBegin );

			// now, build the m_Waypoint list by using the node id's in meshNodes
			if ( data.searching.pAgent->nodesInPath.Count() > 2 )
			{
				bool	bFoundSharedFace = false;
				Vector3	vPoint;
				for( unsigned int iCurrNode = 0; iCurrNode < ( unsigned int )( data.searching.pAgent->nodesInPath.Count() - 1 ); iCurrNode++ )
				{	// don't iterate the last node - it should contain our endpoint!
					bFoundSharedFace = s_GetSharedFaceMidPoint( m_pMesh, data.searching.pAgent->nodesInPath[ iCurrNode ], data.searching.pAgent->nodesInPath[ iCurrNode + 1 ], vPoint );
					if ( bFoundSharedFace )
					{
						m_Waypoints.Add( vPoint );
					}
					else
					{	// d'oh!  no shared face?!
						ASSERT( bFoundSharedFace );
						return( false );
					}
				}
			}

			// our waypoints will end w/ the end point
			m_Waypoints.Add( vEnd );

			if ( data.searching.pAgent != NULL )
			{
				//delete data.searching.pAgent;
				data.searching.pAgent->Reset();
				data.searching.pAgent = NULL;
			}

			// begin the refining process
			if ( m_Waypoints.Count() > 2 )
			{
				data.simplifying.iFromIndex = 0;
				data.simplifying.pQuery = NULL;
				m_Phase = PathSegment::SIMPLIFYING;
			}
			else
			{
				// no need to smooth or refine a straight path
				data.analyzing.iCurrWaypoint = 0;
				m_Phase = PathSegment::ANALYZING;
			}
		}

		return( iStepsUsed );
	}

	unsigned int PathSegment::Simplify( unsigned int iSteps )
	{
		// Precondition: m_ShortWaypoints[ 0 ] must contain the first waypoint

		unsigned int iStepsUsed = 0;

#if USE_PATH_SMOOTHING

		Navigate::LineEdgeQuery	*pHolder = NULL;

		while ( ( iStepsUsed < iSteps ) && ( m_Phase == PathSegment::SIMPLIFYING ) )
		{
			if (
				( m_Waypoints.Count() <= 2 ) ||
				( ( data.simplifying.iFromIndex + 2 ) >= ( unsigned int )m_Waypoints.Count() )
				)
			{	// BASE CASE: this phase is done

				// clean up our query
				delete pHolder;
				pHolder = NULL;

				delete data.simplifying.pQuery;
				data.simplifying.pQuery = NULL;

				// move to the next phase
				data.smoothing.iPadding = 0;
				m_Phase = PathSegment::SMOOTHING;
			}
			else if ( data.simplifying.pQuery == NULL )
			{	// time to start the smoothing query
				if ( pHolder != NULL )
				{
					data.simplifying.pQuery = pHolder;
					pHolder = NULL;
				}
				else
				{
					data.simplifying.pQuery = new Navigate::LineEdgeQuery();
					pHolder = NULL;
				}

				bool bConfigured = data.simplifying.pQuery->Configure( m_pMesh, m_Waypoints[ data.simplifying.iFromIndex ], m_Waypoints[ data.simplifying.iFromIndex + 2 ] );
				ASSERT( bConfigured );
			}
			else if ( !data.simplifying.pQuery->HasWorkToDo() )
			{	// use any results, then start the next query
				if ( !data.simplifying.pQuery->FoundAHardEdgeIntersection() )
				{
					m_Waypoints.Delete( data.simplifying.iFromIndex + 1 );
				}
				else
				{
					++data.simplifying.iFromIndex;
				}

				pHolder = data.simplifying.pQuery;
				data.simplifying.pQuery = NULL;
			}
			else
			{	// let the query continue to work
				iStepsUsed += data.simplifying.pQuery->DoWork( iSteps );
			}
		}

		//while ( ( m_Waypoints.Count() > 2 ) && ( ( data.simplifying.iFromIndex + 2 ) < ( unsigned int )m_Waypoints.Count() ) )
		//{
		//	if ( iStepsUsed >= iSteps )
		//	{
		//		return( iStepsUsed );
		//	}

		//	LineEdgeQuery leq( m_pMesh, m_Waypoints[ data.simplifying.iFromIndex ], m_Waypoints[ data.simplifying.iFromIndex + 2 ] );
		//	++iStepsUsed;

		//	if ( ( bool )leq )
		//	{	// we had an intersection, so we can't reduce the array
		//		++data.simplifying.iFromIndex;
		//	}
		//	else
		//	{	// no intersection, go ahead and reduce the array
		//		m_Waypoints.Delete( data.simplifying.iFromIndex + 1 );
		//	}
		//}
#else

		// update our phase
		m_Phase = PathSegment::SMOOTHING;
		data.smoothing.iPadding = 0;

#endif

		return( iStepsUsed );
	}

	unsigned int PathSegment::Smooth( unsigned int iSteps )
	{
		data.analyzing.iCurrWaypoint = 0;
		m_Phase = PathSegment::ANALYZING;
		return( 0 );
	}

	unsigned int PathSegment::Analyze( unsigned int iSteps )
	{
		unsigned int iStepsUsed = 0;

#if USE_PATH_SMOOTHING

		// calculate segment lengths
		for ( unsigned int iCurrWaypoint = data.analyzing.iCurrWaypoint; ( iCurrWaypoint + 1 ) < ( unsigned int )m_Waypoints.Count(); ++iCurrWaypoint )
		{
			float length = ( m_Waypoints[ iCurrWaypoint + 1 ] - m_Waypoints[ iCurrWaypoint ] ).Length();
			m_SegmentLengths.Add( length );

			data.analyzing.iCurrWaypoint++;

			if ( ++iStepsUsed >= iSteps )
			{
				return( iStepsUsed );
			}
		}

#endif

		data.analyzing.iCurrWaypoint = 0;
		m_Phase = PathSegment::COMPLETE;

		return( iStepsUsed );
	}

	void PathSegment::Reverse( void )
	{
		unsigned int dwCount = ( unsigned int )m_Waypoints.Count();
		unsigned int dwBackward = dwCount - 1;
		unsigned int dwStop = dwBackward / 2;
		unsigned int dwForward;
		for ( dwForward = 0; dwForward <= dwStop; ++dwForward )
		{
			Vector3 vTemp = m_Waypoints[ dwBackward ];
			m_Waypoints[ dwBackward ] = m_Waypoints[ dwForward ];
			m_Waypoints[ dwForward ] = vTemp;
			--dwBackward;
		}

		dwCount -= 1;
		dwBackward = dwCount - 1;
		dwStop = dwBackward / 2;
		for ( dwForward = 0; dwForward <= dwStop; ++dwForward )
		{
			float fLength = m_SegmentLengths[ dwBackward ];
			m_SegmentLengths[ dwBackward ] = m_SegmentLengths[ dwForward ];
			m_SegmentLengths[ dwForward ] = fLength;
		}
	}

	PathSegment::PathSegment( void ) :
	m_pMesh( NULL ),
	m_pPath( NULL ),
	m_Waypoints(),
	next( NULL ),
	prev( NULL ),
	m_Phase( PathSegment::UNINITIALIZED )
	{
		m_pAgent = new AStarNodeAgent();
	}

	PathSegment::PathSegment( Mesh *pMesh, Path *pPath ) :
	m_pMesh( pMesh ),
	m_pPath( pPath ),
	m_Waypoints(),
	next( NULL ),
	prev( NULL ),
	m_Phase( PathSegment::UNINITIALIZED )
	{
		m_pAgent = new AStarNodeAgent();
	}

	PathSegment::~PathSegment( void )
	{
		Reset();
		if ( m_pAgent != NULL )
		{
			delete m_pAgent;
			m_pAgent = NULL;
		}
	}

	void PathSegment::Reset( void )
	{
		if ( m_Phase == PathSegment::SEARCHING )
		{
			if ( data.searching.pAgent != NULL )
			{
				//delete data.searching.pAgent;
				data.searching.pAgent = NULL;
			}
		}
		else if ( m_Phase == PathSegment::SIMPLIFYING )
		{
			if ( data.simplifying.pQuery != NULL )
			{
				delete data.simplifying.pQuery;
				data.simplifying.pQuery = NULL;
			}
		}

#if defined( _DEBUG )
		if ( ( m_Phase != PathSegment::UNINITIALIZED ) && ( m_Phase != PathSegment::COMPLETE ) )
		{
			// output the fact that we're killing a path segment that hasn't completed
			g_console.Debug( "Navigate::PathSegment  WARNING!! Destroying a PathSegment object that hasn't completed the path search" );
		}
#endif

		// keeps memory around
		m_pAgent->Reset();

		// keeps memory around
		m_Waypoints.Clear();

		// keeps memory around
		m_SegmentLengths.Clear();

		m_iBeginNodeID = 0xFFFF;
		m_iEndNodeID = 0xFFFF;

		m_Phase = PathSegment::UNINITIALIZED;
	}

	void PathSegment::Reset( Mesh *pMesh )
	{
		Reset();
		m_pMesh = pMesh;
	}

	void PathSegment::Reset( Mesh *pMesh, Path *pPath )
	{
		Reset( pMesh );
		m_pPath = pPath;
	}

	bool PathSegment::SetPoints( const Vector3 &vBegin, const Vector3 &vEnd )
	{
		ASSERT( m_Phase == PathSegment::UNINITIALIZED );

		// associate the start/end points with our mesh
		Navigate::ClosestPointNodeSearch	skFrom;
		Navigate::ClosestPointNodeSearch	skTo;

		//// update the y-value of our points to be w/in the bounds of the node's y-plane (average the y-values)
		m_iBeginNodeID = skFrom.SearchForPoint( m_pMesh, vBegin );
		m_iEndNodeID = skTo.SearchForPoint( m_pMesh, vEnd );

		if ( ( m_iBeginNodeID == 0xFFFF ) || ( m_iEndNodeID == 0xFFFF ) )
		{	// either one of the given points are outside of our search mesh
			// @todo Gracefully handle the condition when a path waypoint is outside of the search mesh
			return( false );
		}

		// record our begin/end points
		m_Waypoints.Add( vBegin );
		m_Waypoints.Add( vEnd );

		m_Phase = PathSegment::READY;

		return( true );
	}

	bool PathSegment::SetPoints( NodeIndexType iBegin, NodeIndexType iEnd )
	{
		ASSERT( m_Phase == PathSegment::UNINITIALIZED );

		if (
			( ( iBegin == 0xFFFF ) || ( iEnd == 0xFFFF ) ) ||
			( ( iBegin >= m_pMesh->nodeCount ) || ( iEnd >= m_pMesh->nodeCount ) )
			)
		{	// invalid node index detected
			return( false );
		}

		m_iBeginNodeID = iBegin;
		m_iEndNodeID = iEnd;

		// vBegin
		Vector3 v0 = m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 0 ] ];
		Vector3 v1 = m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 1 ] ];
		Vector3 v2 = m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 2 ] ];
		Vector3 vPoint = TriangleTest::Centroid( v0, v1, v2 );
		m_Waypoints.Add( vPoint );

		// vEnd
		v0 = m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 0 ] ];
		v1 = m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 1 ] ];
		v2 = m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 2 ] ];
		vPoint = TriangleTest::Centroid( v0, v1, v2 );
		m_Waypoints.Add( vPoint );

		m_Phase = PathSegment::READY;

		return( true );
	}

	bool PathSegment::Initialize( void )
	{
		ASSERT( m_Phase == PathSegment::READY );

		// update the y-value of our points to be w/in the bounds of the node's y-plane (average the y-values)
		m_Waypoints[ 0 ].y(
			(
			m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 0 ] ].y() +
			m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 1 ] ].y() +
			m_pMesh->points[ m_pMesh->nodes[ m_iBeginNodeID ].points[ 2 ] ].y() ) / 3.0f
		);
		m_Waypoints[ 1 ].y(
			(
			m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 0 ] ].y() +
			m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 1 ] ].y() +
			m_pMesh->points[ m_pMesh->nodes[ m_iEndNodeID ].points[ 2 ] ].y() ) / 3.0f
			);

		// setup for the first phase
		m_Phase = PathSegment::SEARCHING;

		data.searching.pAgent			= m_pAgent;//new AStarNodeAgent();
		data.searching.pAgent->iFrom	= m_iBeginNodeID;
		data.searching.pAgent->iTo		= m_iEndNodeID;
		data.searching.pAgent->bStarted	= false;

		return( true );
	}

	unsigned int PathSegment::DoWork( unsigned int iSteps )
	{
		unsigned int iStepsTaken = 0;

PathSegment_RefineContine:

		switch( m_Phase )
		{
		case UNINITIALIZED:
			ASSERT( 0 );
			break;
		case SEARCHING:
			{
				if ( iStepsTaken < iSteps )
				{
					iStepsTaken += PathSearch( iSteps - iStepsTaken );

					if ( iStepsTaken >= iSteps )
					{
						break;
					}
					else
					{
						goto PathSegment_RefineContine;
					}
				}
				else
				{
					break;
				}
			}
		case SIMPLIFYING:
			{
				if ( iStepsTaken < iSteps )
				{
					iStepsTaken += Simplify( iSteps - iStepsTaken );

					if ( iStepsTaken >= iSteps )
					{
						break;
					}
					else
					{
						goto PathSegment_RefineContine;
					}
				}
				else
				{
					break;
				}
			}
		case SMOOTHING:
			{
				if ( iStepsTaken < iSteps )
				{
					iStepsTaken += Smooth( iSteps - iStepsTaken );

					if ( iStepsTaken >= iSteps )
					{
						break;
					}
					else
					{
						goto PathSegment_RefineContine;
					}
				}
				else
				{
					break;
				}
			}
		case ANALYZING:
			{
				if ( iStepsTaken < iSteps )
				{
					iStepsTaken += Analyze( iSteps - iStepsTaken );

					if ( iStepsTaken >= iSteps )
					{
						break;
					}
					else
					{
						goto PathSegment_RefineContine;
					}
				}
				else
				{
					break;
				}
			}
		case COMPLETE:
			break;
		default:
			ASSERT( 0 );
			break;
		}

		return( iStepsTaken );
	}

	bool PathSegment::HasWorkToDo( void )
	{
		return( m_Phase != PathSegment::COMPLETE );
	}

	float PathSegment::Length()
	{
		float fLength = 0.0f;
		if (HasWorkToDo() == false)	// only valid if path segment is complete
		{
			for (int i = 0; i < m_SegmentLengths.Count(); i++)
				fLength += m_SegmentLengths[i];
		}
		return(fLength);
	}

	//////////////////////////////////////////////////////////////////////////
	//	PATH IMPLEMENTATION
	//////////////////////////////////////////////////////////////////////////

	Path::Path( Manager *pManager, Mesh *pMesh ) :
	m_pManager( pManager ),
	m_pMesh( pMesh )
	{
		ASSERT( m_pManager != NULL );
		ASSERT( m_pMesh != NULL );
	}

	Path::~Path( void )
	{
	}

	float Path::Length()
	{
		float fLength = 0.0f;
		if (HasWorkToDo() == false)	// only valid if path is complete
		{
			for (uint i = 0; i < GetPathSegmentCount(); i++)
				fLength += GetPathSegment(i)->Length();
		}
		return(fLength);
	}

	PathSegment* Path::AquireSegment( void )
	{
		ASSERT( m_pManager != NULL );
		if ( m_pManager != NULL )
		{
			PathSegment *pSegment = m_pManager->AquireSegment();
			if ( pSegment != NULL )
			{
				pSegment->Reset( m_pMesh, this );
				return( pSegment );
			}
		}

		return( NULL );
	}

	void Path::ReleaseSegment( PathSegment *pSegment )
	{
		ASSERT( m_pManager != NULL );
		if ( m_pManager != NULL )
		{
			m_pManager->ReleaseSegment( pSegment );
		}
	}
}
