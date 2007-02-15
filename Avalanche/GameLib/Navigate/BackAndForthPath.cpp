/********************************************************************
	created:	2004/02/18
	created:	18:2:2004   13:52
	filename: 	C:\dev\avalanche\gamelib\Navigate\BackAndForthPath.cpp
	file path:	C:\dev\avalanche\gamelib\Navigate
	file base:	BackAndForthPath
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implementation of a back-and-forth path object
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "Navigate/BackAndForthPath.h"

namespace Navigate
{
	BackAndForthPath::BackAndForthPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints ) :
	OneWayPath( pManager, pMesh, iWaypointCount, pPoints )
	{
	}

	BackAndForthPath::~BackAndForthPath( void )
	{
	}

	PathConnection* BackAndForthPath::CreateNewConnection( void )
	{
		return( ( PathConnection* )( new BackAndForthPathConnection( this ) ) );
	}




	BackAndForthPathConnection::BackAndForthPathConnection( void ) :
		OneWayPathConnection(),
		m_bReversed( false )
	{
	}

	BackAndForthPathConnection::BackAndForthPathConnection( PathConnection &copy ) :
		OneWayPathConnection( copy ),
		m_bReversed( false )
	{
	}

	BackAndForthPathConnection::BackAndForthPathConnection( BackAndForthPath *pPath ) :
		OneWayPathConnection( pPath ),
		m_bReversed( false )
	{
	}

	bool BackAndForthPathConnection::OnPathEndReached( void )
	{
		// reorder all the waypoints
		this->m_pPath->Reverse();

		m_bAtEndOfPath = m_bReversed;
		m_bPositionDirty = true;
		m_bDirectionDirty = true;
		m_bDirectionNormalDirty = true;

		m_iCurrentSegment = 0;
		m_iCurrentWaypoint = 0;
		m_fCurrentT = 0.0f;

		m_bReversed = !m_bReversed;

		return( m_bReversed );
	}
}
