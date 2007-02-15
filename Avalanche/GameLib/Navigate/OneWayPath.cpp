#include "Navigate/NavigatePCH.h"
#include "Navigate/OneWayPath.h"

#if defined( NAVIGATION_DEBUGGING )
#include "EngineHelper/DrawUtil.h"
#endif

namespace Navigate
{
	//////////////////////////////////////////////////////////////////////////
	//	ONEWAYPATH IMPLEMENTATION
	//////////////////////////////////////////////////////////////////////////

	OneWayPath::OneWayPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints ) :
	Path( pManager, pMesh )
	{
		ASSERT( iWaypointCount >= 2 );

		// Build PathSegments between the given waypoints
		for( unsigned int iCurrFrom = 0; iCurrFrom < ( iWaypointCount - 1 ); iCurrFrom++ )
		{
			//PathSegment *pSegment = new PathSegment( m_pMesh, this, *pPoints[ iCurrFrom ], *pPoints[ iCurrFrom + 1 ] );
			PathSegment *pSegment = AquireSegment();
			//pSegment->Reset( pMesh, this );
			bool bSuccess = pSegment->SetPoints( *pPoints[ iCurrFrom ], *pPoints[ iCurrFrom + 1 ] );

			if ( bSuccess )
			{
				m_Segments.Add( pSegment );
			}
			else
			{
				ReleaseSegment( pSegment );
			}
		}
	}

	OneWayPath::~OneWayPath( void )
	{
		// delete any active PathSegment objects.
		if ( m_Segments.Count() > 0 )
		{
			for( int iCurrSegment = m_Segments.Count() - 1; iCurrSegment >= 0; --iCurrSegment )
			{
				if ( m_Segments[ iCurrSegment ] != NULL )
				{
					//m_Segments[ iCurrSegment ]->Reset();
					ReleaseSegment( m_Segments[ iCurrSegment ] );
					//delete m_Segments[ iCurrSegment ];
				}
			}
		}
		m_Segments.Clear();
		m_Segments.Reset();

		// delete any active PathConnection objects.
		if ( m_Connections.Count() > 0 )
		{
			for( int iCurrConnection = ( m_Connections.Count() - 1 ); iCurrConnection >= 0; --iCurrConnection )
			{
				if ( m_Connections[ iCurrConnection ] != NULL )
				{
					delete m_Connections[ iCurrConnection ];
				}
			}
		}
		m_Connections.Clear();
		m_Connections.Reset();

		m_pMesh = NULL;
	}

	bool OneWayPath::Initialize( void )
	{
		ASSERT( m_pMesh != NULL );

		bool bResult = false;
		for( unsigned int iCurrSegment = 0; iCurrSegment < ( unsigned int )m_Segments.Count(); ++iCurrSegment )
		{
			bResult = m_Segments[ iCurrSegment ]->Initialize();
			if ( !bResult )
			{
				return( bResult );
			}
		}

#ifndef CDROM
		gbl_NavManager.IncPathsCreated();
#endif // CDROM
		return( bResult );
	}

	unsigned int OneWayPath::DoWork( unsigned int iSteps )
	{
		unsigned int iStepsUsed = 0;
		unsigned int iSegmentCount = ( unsigned int )( m_Segments.Count() );

		for( unsigned int iCurrentSegment = 0; iCurrentSegment < iSegmentCount; ++iCurrentSegment )
		{	// for each segment, do work as necessary
			ASSERT( iSteps >= iStepsUsed );
			if ( m_Segments[ iCurrentSegment ]->HasWorkToDo() )
			{	// allow the segment some work, if it needs it
				iStepsUsed += m_Segments[ iCurrentSegment ]->DoWork( iSteps - iStepsUsed );

				if ( iStepsUsed >= iSteps )
				{	// we're out of time for refinement
					break;
				}
			}
		}

		return( iStepsUsed );
	}

	bool OneWayPath::HasWorkToDo( void )
	{
		bool bResult = false;
		unsigned int iSegmentCount = ( unsigned int )( m_Segments.Count() );

		for( unsigned int iCurrentSegment = 0; iCurrentSegment < iSegmentCount; ++iCurrentSegment )
		{
			if ( m_Segments[ iCurrentSegment ]->HasWorkToDo() )
			{
				bResult = true;
				break;
			}
		}

		return( bResult );
	}

	PathConnection* OneWayPath::GetConnection( void )
	{
		PathConnection	*pResult = CreateNewConnection();

		// record the connection in our collection.
		m_Connections.Add( pResult );

		return( pResult );
	}

	unsigned int OneWayPath::GetConnectionCount( void )
	{
		return( ( unsigned int )m_Connections.Count() );
	}

	void OneWayPath::ReleaseConnection( PathConnection *pConnection )
	{
		if ( pConnection )
		{
			// remove the connection from our collection.
			for( int iCurrConnection = ( m_Connections.Count() - 1 ); iCurrConnection >= 0; --iCurrConnection )
			{
				if ( m_Connections[ iCurrConnection ] == pConnection )
				{
					m_Connections.Delete( iCurrConnection );
					break;
				}
			}
			delete pConnection;
			pConnection = NULL;
		}
	}

	unsigned int OneWayPath::GetPathSegmentCount( void )
	{
		return( ( unsigned int )m_Segments.Count() );
	}

	PathSegment *OneWayPath::GetPathSegment( unsigned int iPathSegment )
	{
		return( m_Segments[ iPathSegment ] );
	}

	void OneWayPath::Reverse( void )
	{
		// switch the order of the segments
		unsigned int dwCount = ( unsigned int )m_Segments.Count();
		unsigned int dwStop = ( dwCount - 1 ) / 2;
		unsigned int dwBackward = dwCount - 1;
		for ( unsigned int dwForward = 0; dwForward <= dwStop; ++dwForward )
		{
			PathSegment *pTemp = m_Segments[ dwBackward ];
			m_Segments[ dwBackward-- ] = m_Segments[ dwForward ];
			m_Segments[ dwForward ] = pTemp;
		}

		// reverse the context of each connection
		for ( unsigned int dwCurrConnection = 0; dwCurrConnection < ( unsigned int )m_Connections.Count(); ++dwCurrConnection )
		{
			m_Connections[ dwCurrConnection ]->Reverse();
		}

		// reverse the order of the waypoints in each segment
		for ( unsigned int dwCurrSegment = 0; dwCurrSegment < dwCount; ++dwCurrSegment )
		{
			m_Segments[ dwCurrSegment ]->Reverse();
		}
	}

#if defined( NAVIGATION_DEBUGGING )
	void OneWayPath::DrawPath( const ts_fRGB &fStartColor, const ts_fRGB &fStopColor )
	{
		unsigned int dwLineCount = 0;
		unsigned int dwCurrPathSegment;
		for ( dwCurrPathSegment = 0; dwCurrPathSegment < ( unsigned int )m_Segments.Count(); ++dwCurrPathSegment )
		{
			PathSegment *pCurrSegment = m_Segments[ dwCurrPathSegment ];
			ASSERT( pCurrSegment != NULL );
			dwLineCount += ( unsigned int )pCurrSegment->m_SegmentLengths.Count();
		}
		float fLerpStep = 1.0f / ( 2.0f * dwLineCount );
		float fLerp = 1.0f;

		for ( dwCurrPathSegment = 0; dwCurrPathSegment < ( unsigned int )m_Segments.Count(); ++dwCurrPathSegment )
		{
			PathSegment *pCurrSegment = m_Segments[ dwCurrPathSegment ];
			ASSERT( pCurrSegment != NULL );
			for ( unsigned int dwCurrSegment = 0; dwCurrSegment < ( unsigned int )pCurrSegment->m_SegmentLengths.Count(); ++dwCurrSegment )
			{
				Vector3 vA, vB, vC;
				vA = pCurrSegment->m_Waypoints[ dwCurrSegment ];
				vB = pCurrSegment->m_Waypoints[ dwCurrSegment + 1 ];
				vC = ( ( ( vA - vB ) * 0.5f ) + vB );
				ts_fRGB cA, cB;

				cA.R = ( ( fStartColor.R * fLerp ) + ( fStopColor.R * ( 1.0f - fLerp ) ) );
				cA.G = ( ( fStartColor.G * fLerp ) + ( fStopColor.G * ( 1.0f - fLerp ) ) );
				cA.B = ( ( fStartColor.B * fLerp ) + ( fStopColor.B * ( 1.0f - fLerp ) ) );
				fLerp -= fLerpStep;

				cB.R = ( ( fStartColor.R * fLerp ) + ( fStopColor.R * ( 1.0f - fLerp ) ) );
				cB.G = ( ( fStartColor.G * fLerp ) + ( fStopColor.G * ( 1.0f - fLerp ) ) );
				cB.B = ( ( fStartColor.B * fLerp ) + ( fStopColor.B * ( 1.0f - fLerp ) ) );
				fLerp -= fLerpStep;

				DrawUtility::Line( vA, vC, cA );
				DrawUtility::Line( vC, vB, cB );
			}
		}
	}
#endif

	PathConnection* OneWayPath::CreateNewConnection( void )
	{
		return( ( PathConnection* )( new OneWayPathConnection( this ) ) );
	}

	//////////////////////////////////////////////////////////////////////////
	// ONEWAYPATHCONNECTION IMPLEMENTATION
	//////////////////////////////////////////////////////////////////////////

	PathConnection::Status OneWayPathConnection::GetStatus( void )
	{
		if ( m_pPath->HasWorkToDo() )
		{
			return( PathConnection::PATHCONNECTION_PENDING );
		}
		else
		{
			if ( m_bAtEndOfPath )
			{
				return( PathConnection::PATHCONNECTION_ENDOFPATH );
			}
			else
			{
				return( PathConnection::PATHCONNECTION_VALID );
			}
		}
	}

	bool OneWayPathConnection::MoveAlongPath( float fDistance, float &fDistanceMoved )
	{
		if ( m_bAtEndOfPath )
		{
			fDistanceMoved = 0.0f;
			return( false );
		}

		float fDistanceRemaining = fDistance;
		fDistanceMoved = 0.0f;
		unsigned int iWaypoints = 0;

		while ( ( fDistanceRemaining > 0.0f ) && ( m_iCurrentSegment < ( unsigned int )m_pPath->m_Segments.Count() ) )
		{
			while ( ( fDistanceRemaining > 0.0f ) && ( ( m_iCurrentWaypoint + iWaypoints + 1 ) < ( unsigned int )m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints.Count() ) )
			{	// update within our current segment as much as possible

				float fLength = m_pPath->m_Segments[ m_iCurrentSegment ]->m_SegmentLengths[ ( m_iCurrentWaypoint + iWaypoints ) ];
				float fRemainingLength = fLength * ( 1.0f - m_fCurrentT );

				if ( ( fDistanceMoved + fRemainingLength ) < fDistance )
				{	// only count the full length of the current waypoint if it won't take us beyond the end of the path
					fDistanceMoved += fRemainingLength;
					fDistanceRemaining -= fRemainingLength;
					m_fCurrentT = 0.0f;
					++iWaypoints;
				}
				else
				{	// current waypoint is long enough to accommodate the remainder of the requested fDistance
					m_iCurrentWaypoint += iWaypoints;
					m_fCurrentT += ( 1.0f - ( ( fLength - fDistanceRemaining ) / fLength ) );
					fDistanceMoved = fDistance;
					// flag all our stored positional data as dirty
					m_bAtEndOfPath = false;
					m_bPositionDirty = true;
					m_bDirectionDirty = true;
					m_bDirectionNormalDirty = true;
					return( true );
				}
			}

			iWaypoints = 0;
			m_fCurrentT = 0.0f;
			m_iCurrentWaypoint = 0;
			++m_iCurrentSegment;
		}

		// attempted to go beyond the end of the path

		return( OnPathEndReached() );
	}

	bool OneWayPathConnection::MoveToNextWaypoint( void )
	{
		m_bPositionDirty = true;
		m_bDirectionDirty = true;
		m_bDirectionNormalDirty = true;
		m_fCurrentT = 0.0f;
		m_iCurrentWaypoint++;
		if ( ( m_iCurrentWaypoint + 1 ) > ( unsigned int )m_pPath->GetPathSegment( m_iCurrentSegment )->m_SegmentLengths.Count() )
		{	// skip to the next segment
			m_iCurrentSegment++;
			m_iCurrentWaypoint = 0;
			if ( ( m_iCurrentSegment + 1 ) > m_pPath->GetPathSegmentCount() )
			{
				m_iCurrentSegment = m_pPath->GetPathSegmentCount() - 1;
				m_iCurrentWaypoint = ( unsigned int )( m_pPath->GetPathSegment( m_iCurrentSegment )->m_SegmentLengths.Count() ) - 1;
				OnPathEndReached();
				return( false );
			}
		}
		return( true );
	}

	bool OneWayPathConnection::OnPathEndReached( void )
	{
		// flag all our stored positional data as dirty
		m_bAtEndOfPath = true;
		m_bPositionDirty = true;
		m_bDirectionDirty = true;
		m_bDirectionNormalDirty = true;

		m_iCurrentSegment = ( unsigned int )( m_pPath->m_Segments.Count() - 1 );
		m_iCurrentWaypoint = ( unsigned int )( m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints.Count() - 1 );
		m_fCurrentT = 1.0f;

		return( false );
	}

	Vector3 OneWayPathConnection::GetPosition( void )
	{
		if ( m_bPositionDirty )
		{
			if ( m_bAtEndOfPath )
			{	// at the end of the path
				m_vPosition = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint ];
			}
			else
			{	// still w/in the path boundaries
				Vector3 vTo = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint + 1 ];
				Vector3 vFrom = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint ];
				m_vPosition = ( ( vTo - vFrom ) * m_fCurrentT ) + vFrom;
			}
			m_bPositionDirty = false;
		}

		return( m_vPosition );
	}

	Vector3 OneWayPathConnection::GetDirection( void )
	{
		if ( m_bDirectionDirty )
		{
			if ( ( m_bAtEndOfPath ) || ( m_fCurrentT >= 0.9f )  )
			{
				m_vDirection = GetPosition();
				m_vDirection -= m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint - 1 ];
				m_vDirection *= -1.0f;
				m_vDirection -= GetPosition();
			}
			else
			{
				m_vDirection = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint + 1 ];
				m_vDirection -= GetPosition();
			}
			m_bDirectionDirty = false;
		}

		return( m_vDirection );
	}

	Vector3 OneWayPathConnection::GetDirectionNormal( void )
	{
		if ( m_bDirectionNormalDirty )
		{
			m_vDirectionNormal = GetDirection();
			m_vDirectionNormal.Normalize();
			m_bDirectionNormalDirty = false;
		}

		return( m_vDirectionNormal );
	}

	Vector3 OneWayPathConnection::GetNextWaypoint( void )
	{
		Vector3 vResult;
        
		if ( m_bAtEndOfPath )
		{
			vResult = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint ];
		}
		else
		{
			vResult = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints[ m_iCurrentWaypoint + 1 ];
		}
		return( vResult );
	}

	struct _ClosestPointRecord
	{
		float				fLengthSquared;
		float				fT;
		unsigned int	dwWaypoint;
		unsigned int	dwSegment;
		Vector3			vPoint;

		bool			operator==( const _ClosestPointRecord &rhs )
		{
			return(
				( fLengthSquared == rhs.fLengthSquared ) &&
				( fT == rhs.fT ) &&
				( dwWaypoint == rhs.dwWaypoint ) &&
				( dwSegment == rhs.dwSegment ) &&
				( vPoint == vPoint )
				);
		};
	};

	Vector3	OneWayPathConnection::ConnectNearest( const Vector3 vPoint )
	{
		// calculate the closest point on our path to the given point - use precalc'd segment lengths.
		unsigned int dwRecordCount = 0;
		unsigned int dwCurrPathSegment;
		for ( dwCurrPathSegment = 0; dwCurrPathSegment < m_pPath->GetPathSegmentCount(); ++dwCurrPathSegment )
		{
			PathSegment *pSegment = m_pPath->GetPathSegment( dwCurrPathSegment );
			ASSERT( pSegment != NULL );
			dwRecordCount += ( unsigned int )pSegment->m_SegmentLengths.Count();
		}

		// build the PriorityQueue, and start populating it
		MinQueue< float, _ClosestPointRecord > pqRecords;

		for ( dwCurrPathSegment = 0; dwCurrPathSegment < m_pPath->GetPathSegmentCount(); ++dwCurrPathSegment )
		{
			PathSegment *pSegment = m_pPath->GetPathSegment( dwCurrPathSegment );
			ASSERT( pSegment != NULL );
			for ( unsigned int dwCurrSegment = 0; dwCurrSegment < ( unsigned int )pSegment->m_SegmentLengths.Count(); ++dwCurrSegment )
			{
				_ClosestPointRecord record;
				Segment seg;
				seg.Set( pSegment->m_Waypoints[ dwCurrSegment ], pSegment->m_Waypoints[ dwCurrSegment + 1 ] );
				record.dwSegment = dwCurrPathSegment;
				record.dwWaypoint = dwCurrSegment;
				record.vPoint = seg.TrueClosestPoint( vPoint, pSegment->m_SegmentLengths[ dwCurrSegment ], record.fT );
				record.fLengthSquared = ( vPoint - record.vPoint ).LengthSquared();
				pqRecords.Enqueue( record.fLengthSquared, record );
			}
		}

		_ClosestPointRecord record;
		bool bFound = pqRecords.Dequeue( record );
		ASSERT( bFound );

		m_iCurrentWaypoint = record.dwWaypoint;
		m_iCurrentSegment = record.dwSegment;
		m_fCurrentT = Math::Clamp( record.fT );
		m_vPosition = record.vPoint;
		m_bPositionDirty = false;

		// flag other values as dirty
		m_bDirectionDirty = true;
		m_bDirectionNormalDirty = true;

		if (
			( ( m_iCurrentSegment + 1 ) >= ( unsigned int )( m_pPath->GetPathSegmentCount() ) ) &&
			( ( m_iCurrentWaypoint + 1 ) >= ( unsigned int )( m_pPath->GetPathSegment( m_iCurrentSegment )->m_Waypoints.Count() ) ) &&
			( m_fCurrentT >= 1.0f )
		)
		{
			OnPathEndReached();
		}

		return( m_vPosition );
	}

	void OneWayPathConnection::Reverse( void )
	{
		m_fCurrentT = 1.0f - m_fCurrentT;
		m_iCurrentSegment = m_pPath->m_Segments.Count() - m_iCurrentSegment;
		m_iCurrentWaypoint = m_pPath->m_Segments[ m_iCurrentSegment ]->m_Waypoints.Count() - m_iCurrentWaypoint;
	}

	void OneWayPathConnection::Release( bool bReleaseUnusedPath )
	{
		Navigate::Path *pPath = m_pPath;
		pPath->ReleaseConnection( this );
		if ( ( bReleaseUnusedPath ) && ( pPath->GetConnectionCount() == 0 ) )
		{
			gbl_NavManager.DestroyPath( pPath );
		}
	}

	OneWayPathConnection::OneWayPathConnection( void ) :
		m_pPath( NULL ),
		m_iCurrentSegment( 0 ),
		m_iCurrentWaypoint( 0 ),
		m_fCurrentT( 0.0f ),
		m_bAtEndOfPath( false ),
		m_bPositionDirty( true ),
		m_bDirectionDirty( true ),
		m_bDirectionNormalDirty( true ),
		m_vPosition( 0.0f, 0.0f, 0.0f ),
		m_vDirection( 0.0f, 0.0f, 0.0f ),
		m_vDirectionNormal( 0.0f, 0.0f, 0.0f )
	{
	}

	OneWayPathConnection::OneWayPathConnection( PathConnection &copy )
	{
		ASSERT( 0 );
	}

	OneWayPathConnection::OneWayPathConnection( OneWayPath *pPath ) :
		m_pPath( pPath ),
		m_iCurrentSegment( 0 ),
		m_iCurrentWaypoint( 0 ),
		m_fCurrentT( 0.0f ),
		m_bAtEndOfPath( false ),
		m_bPositionDirty( true ),
		m_bDirectionDirty( true ),
		m_bDirectionNormalDirty( true ),
		m_vPosition( 0.0f, 0.0f, 0.0f ),
		m_vDirection( 0.0f, 0.0f, 0.0f ),
		m_vDirectionNormal( 0.0f, 0.0f, 0.0f )
	{
	}

}