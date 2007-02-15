/********************************************************************
created:	2004/02/18
created:	18:2:2004   13:52
filename: 	C:\dev\avalanche\gamelib\Navigate\CircuitPath.cpp
file path:	C:\dev\avalanche\gamelib\Navigate
file base:	CircuitPath
file ext:	cpp
author:		Shon C. Love

purpose:	Implementation of a back-and-forth path object
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "Navigate/CircuitPath.h"

namespace Navigate
{
	CircuitPath::CircuitPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints ) :
	OneWayPath( pManager, pMesh, iWaypointCount, pPoints )
	{
		// create a segment from the end of the one-way path to the beginning of the one-way path to create a circuit path.
		unsigned int dwSegmentCount = ( unsigned int )m_Segments.Count();
		ASSERT( dwSegmentCount > 0 );
		unsigned int dwWaypointCount = ( unsigned int )( m_Segments[ dwSegmentCount - 1 ]->m_Waypoints.Count() );
		ASSERT( dwWaypointCount > 0 );

		PathSegment *pSegment = AquireSegment();//new PathSegment( m_pMesh, this, m_Segments[ dwSegmentCount - 1 ]->m_Waypoints[ dwWaypointCount - 1 ], m_Segments[ 0 ]->m_Waypoints[ 0 ] );
		//pSegment->Reset( pMesh, this );
		bool bSuccess = pSegment->SetPoints( m_Segments[ dwSegmentCount - 1 ]->m_Waypoints[ dwWaypointCount - 1 ], m_Segments[ 0 ]->m_Waypoints[ 0 ] );
		ASSERT( bSuccess );

		if ( bSuccess )
		{
			m_Segments.Add( pSegment );
		}
		else
		{
			ReleaseSegment( pSegment );
		}
	}

	CircuitPath::~CircuitPath( void )
	{
	}

	PathConnection* CircuitPath::CreateNewConnection( void )
	{
		return( ( PathConnection* )( new CircuitPathConnection( this ) ) );
	}



	CircuitPathConnection::CircuitPathConnection( void ) :
	OneWayPathConnection()
	{
	}

	CircuitPathConnection::CircuitPathConnection( PathConnection &copy ) :
	OneWayPathConnection( copy )
	{
	}

	CircuitPathConnection::CircuitPathConnection( CircuitPath *pPath ) :
	OneWayPathConnection( pPath )
	{
	}

	bool CircuitPathConnection::OnPathEndReached( void )
	{
		this->OneWayPathConnection::OnPathEndReached();
		return( true );
	}
}
