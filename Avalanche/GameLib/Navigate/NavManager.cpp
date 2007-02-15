/********************************************************************
	created:	2003/12/18
	created:	18:12:2003   10:36
	filename: 	C:\dev\avalanche\gamelib\Navigate\NavManager.cpp
	file path:	C:\dev\avalanche\gamelib\Navigate
	file base:	NavManager
	file ext:	cpp
	author:		Shon C. Love
	
	purpose:	Implement the navigation manager class
*********************************************************************/

#include "Navigate/NavigatePCH.h"
#include "Navigate/NavManager.h"
#include "Navigate/NavPath.h"
#include "Navigate/OneWayPath.h"
#include "Navigate/NavIterators.h"
#include "GameHelper/loadsom.h"

#include "container/skiplist.h"
#include "container/pqueue.h"

#if defined( NAVIGATION_DEBUGGING )
#	include "EngineHelper/timer.h"
#	include "Math/fastrand.h"
#	include "CmdCon/CmdCon.h"
#	include "Math/Mat4x4.h"
#	include "linearAlgebra/cquat.h"
#endif

#ifdef DIRECTX_PC
#include "Layers/Timing.h"
#endif

#include <stdarg.h>

DeclareSingleton( Navigate::Manager );

DynamicArray< Navigate::Mesh* > Navigate::Manager::m_MeshCollection;

#if defined( NAVIGATION_DEBUGGING )
static void s_DrawOctant( Octant &oBox, Navigate::TreeNode *pNode )
{
	// draw a box for oBox
	Vector3 a, b;
	float fHalfSize = oBox.HalfSize();
	a.Set( fHalfSize, fHalfSize, fHalfSize );
	a += oBox.Center();
	fHalfSize *= -1.0f;
	b.Set( fHalfSize, fHalfSize, fHalfSize );
	b += oBox.Center();
	DrawUtility::Box( a, b, NULL, NULL, DrawUtility::BLUE );

	// process all the children
	if ( !( pNode->flags & Navigate::TreeNode::LEAF ) )
	{
		for( unsigned int iCurrChild = 0; iCurrChild < 8; iCurrChild++ )
		{
			if ( pNode->data.branch.children[ iCurrChild ] != NULL )
			{
				Octant child = oBox.Child( ( Octant::OCTANT )iCurrChild );
				s_DrawOctant( child, pNode->data.branch.children[ iCurrChild ] );
			}
		}
	}
}
#endif

namespace Navigate
{
	PQSearchNode::PQSearchNode( void ) :
	pParent( NULL ),
	fCostFromStart( MAXFLOAT ),
	fEstCostToGoal( MAXFLOAT ),
	iNode( 0xFFFF ),
	bOnOpenList( false )
	{
	}

	PQSearchNode::~PQSearchNode( void )
	{
		pParent = NULL;
		fCostFromStart = MAXFLOAT;
		fEstCostToGoal = MAXFLOAT;
		iNode = 0xFFFF;
		bOnOpenList = false;
	}

	Manager::Manager( void ) :
		m_iMaxStepsCount( 0 ),
		m_bInitialized( false )
	{
		/* initial state of debug drawing */

		#ifndef CDROM
			#ifdef NAVIGATION_DEBUGGING
				m_drawNavMesh = true;
				m_drawNavMeshOcclusions = true;
			#else // NAVIGATION_DEBUGGING
				m_drawNavMesh = false;
				m_drawNavMeshOcclusions = false;
			#endif // NAVIGATION_DEBUGGING
		#endif //CDROM
	}

	Manager::~Manager( void )
	{
		// make sure ShutDown was called before the object is destroyed.
		bool bSuccess = ShutDown();
		ASSERT( bSuccess );
	}

	void Manager::RegisterMesh( Navigate::Mesh *pMesh )
	{
		m_MeshCollection.Add( pMesh );
	}

	/* callback to process a data chunk in a .dbl file */
	/*** static function ***/

	bool Manager::LoadChunk(void *pData, ts_DBChunkHdr_tag *pChunkHdr, void **ppRetData, u32 *pu32DataSize)
	{
		Mesh::DiskFixup( pData );
		Manager::RegisterMesh( ( Navigate::Mesh* )pData );
		return(true);
	}

	void Manager::DestroyPath( Path *pPath )
	{
		ASSERT( pPath != NULL );

		bool bFoundPathCollection = false;
		bool bFoundDeferredWorker = false;

		// remove the path from the path collection
		for( unsigned int iCurrPath = 0; iCurrPath < ( unsigned int )m_PathCollection.Count(); ++iCurrPath )
		{
			if ( m_PathCollection[ iCurrPath ] == ( Navigate::Path* )pPath )
			{
				m_PathCollection.Delete( iCurrPath );
				bFoundPathCollection = true;
				break;
			}
		}

		// find the referenced iterator, if it's still in our worker collection
		bFoundDeferredWorker = RemoveDeferredWorker( ( Navigate::DeferredWorker* )pPath );

		// destroy the path
		if ( ( bFoundPathCollection ) || ( bFoundDeferredWorker ) )
		{
			delete pPath;
		}
	}

	void Manager::DestroyCoverIterator( Navigate::CoverIterator *pIterator )
	{
		ASSERT( pIterator != NULL );

		bool bFoundCoverCollection = false;
		bool bFoundDeferredWorker = false;

		// find the referenced iterator, if it's still in our CoverIterator collection
		for( unsigned int iCurrIterator = 0; iCurrIterator < ( unsigned int )m_CoverIteratorCollection.Count(); ++iCurrIterator )
		{
			if ( m_CoverIteratorCollection[ iCurrIterator ] == ( Navigate::CoverIterator* )pIterator )
			{
				m_CoverIteratorCollection.Delete( iCurrIterator );
				bFoundCoverCollection = true;
				break;
			}
		}

		// find the referenced iterator, if it's still in our worker collection
		bFoundDeferredWorker = RemoveDeferredWorker( (Navigate::DeferredWorker*) pIterator );

		// delete the iterator, regardless whether we found it in our collection.
		if ( ( bFoundCoverCollection ) || ( bFoundDeferredWorker ) )
		{
			delete pIterator;
		}
	}

	void Manager::DestroyPointSearch( Navigate::PointSearch *pPointSearch )
	{
		ASSERT( pPointSearch != NULL );

		bool bFound = RemoveDeferredWorker( ( Navigate::DeferredWorker* )pPointSearch );

		delete pPointSearch;
	}

	bool Manager::RemoveDeferredWorker( Navigate::DeferredWorker *pWorker )
	{
		bool bFound = false;
		for ( unsigned int iCurrWorker = 0; iCurrWorker < ( unsigned int )m_DeferredWorkerCollection.Count(); ++iCurrWorker )
		{
			if ( m_DeferredWorkerCollection[ iCurrWorker ] == pWorker )
			{
				m_DeferredWorkerCollection.Delete( iCurrWorker );
				bFound = true;
				break;
			}
		}
		return( bFound );
	}

	void Manager::Update( float fDeltaSec )
	{
		if ( !m_bInitialized )
			return;

		Navigate::DeferredWorker *pWorker = NULL;

		// iterate the worker collection
		unsigned int iCount = ( unsigned int )m_DeferredWorkerCollection.Count();

		// retire workers that have no more work to do so we have an accurate count for distributing available work units
		if (iCount > 0)
		{
			unsigned int iCurrWorker = 0;
			while ( iCurrWorker < ( unsigned int )m_DeferredWorkerCollection.Count() )
			{
				pWorker = m_DeferredWorkerCollection[ iCurrWorker ];
				if ( pWorker->HasWorkToDo() )
					++iCurrWorker;
				else
				{
					/* worker has no more work to do, remove from list */

					m_DeferredWorkerCollection.Delete( iCurrWorker );
					iCount--;	// update count of workers with work to do
				}
			}
		}

		// share our work among all active workers
		if ( iCount > 0 )
		{
#ifndef CDROM
			/* stats */

			u32 startMilliseconds = EngineTimer__Time();
#endif //CDROM

			unsigned int iStepsPerRefine = ( m_iMaxStepsCount / iCount );
			if ( iStepsPerRefine <= 0 )
				iStepsPerRefine = 1;	// guarantee at least one work unit per worker

			// allow each worker to do some work. each worker gets his share of work units, plus whatever's left over from previous workers
			unsigned int iStepsAvailable = 0;
			unsigned int iCurrWorker = 0;
			while ( iCurrWorker < ( unsigned int )m_DeferredWorkerCollection.Count() )
			{
				pWorker = m_DeferredWorkerCollection[ iCurrWorker ];
				ASSERT( pWorker->HasWorkToDo() );							// since we retired inactive workers above
				iStepsAvailable += iStepsPerRefine;								// add in this worker's share of work units
				iStepsAvailable -= pWorker->DoWork( iStepsAvailable );	// DoWork returns number of units it used-- it's possible that it's more than available
				++iCurrWorker;
			}

#ifndef CDROM
			/* stats. milliseconds may not be granular enough to get accurate results here */

			m_totalWorkSeconds += (float) (EngineTimer__Time() - startMilliseconds) / 1000.0f;
			if (m_totalPathsCreated > 0)
				m_averageSecondsPerPath = m_totalWorkSeconds / (float) m_totalPathsCreated;
//			dbgPrint("Nav manager (frame %d): %d/%d work units used\n", g_timer.GetFrameCount(), m_iMaxStepsCount - iStepsAvailable, m_iMaxStepsCount);
#if 0	// print out some stats every so often
			if (g_timer.GetStartSec() > m_statsPrintTime)
			{
				dbgPrint("Nav manager: %.2f total seconds, %d paths, %.5f seconds/path\n", m_totalWorkSeconds, m_totalPathsCreated, m_averageSecondsPerPath);
				m_statsPrintTime = g_timer.GetStartSec() + 3.0f;
			}
#endif //0/1
#endif //CDROM

		}

#if defined( NAVIGATION_DEBUGGING )
		for ( int iCurrMesh = 0; iCurrMesh < m_MeshCollection.Count(); ++iCurrMesh )
		{
			Navigate::Mesh *pMesh = m_MeshCollection[ iCurrMesh ];
			if ( ( pMesh != NULL ) && ( fDeltaSec > 0.0f ) )
			{
				//static bool bPathNotDone = false;
				//static bool bSetup = false;
				//static Navigate::Path *pPath = NULL;
				//static Navigate::PathConnection	*pPathConnection = NULL;
				//static Vector3 vStart, vSecond, vEnd;
				//static CoverIterator *pItCover = NULL;

				//if ( !bPathNotDone )
				//{
				//	bPathNotDone = true;
				//	bSetup = false;
				//	if ( pItCover != NULL )
				//	{
				//		DestroyCoverIterator( pItCover );
				//		pItCover = NULL;
				//	}
				//	if ( pPath != NULL )
				//	{
				//		pPath->ReleaseConnection( pPathConnection );
				//		pPathConnection = NULL;
				//		delete pPath;
				//		pPath = NULL;
				//	}
				//}

				//if ( !bSetup )
				//{
				//	// random start position
				//	do
				//	{	// try making paths until we get a valid one
				//		if ( pPath != NULL )
				//		{
				//			delete pPath;
				//			pPath = NULL;
				//		}

				//		// random start position
				//		vStart = pMesh->extents.Center();
				//		vStart.x( vStart.x() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vStart.z( vStart.z() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vStart.y( 0.0f );

				//		// random second position
				//		vSecond = pMesh->extents.Center();
				//		vSecond.x( vSecond.x() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vSecond.z( vSecond.z() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vSecond.y( 0.0f );

				//		// random end position
				//		vEnd = pMesh->extents.Center();
				//		vEnd.x( vEnd.x() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vEnd.z( vEnd.z() + ( pMesh->extents.HalfSize() * Random::NegPos() ) );
				//		vEnd.y( 0.0f );

				//		pPath = CreateCircuitPath( 3, &vStart, &vSecond, &vEnd );
				//		pPathConnection = pPath->GetConnection();

				//		pItCover = CreateCoverIterator( vEnd, vStart, 35.0f );

				//	} while( !pPath->Initialize() );

				//	bSetup = true;
				//}

				// draw the octree
				// s_DrawOctant( pMesh->extents, pMesh->root );

				// draw any active paths
				unsigned int dwPathCount = ( unsigned int )m_PathCollection.Count();
				ts_fRGB cStart, cStop;
				cStart.R = 1.0f; cStart.G = 0.5f; cStart.B = 0.0f;
				cStop.R = 1.0f; cStart.G = 0.5f; cStart.B = 0.5f;
				for ( unsigned int dwCurrPath = 0; dwCurrPath < dwPathCount; ++dwCurrPath )
				{
					Path *pPath = m_PathCollection[ dwCurrPath ];
					ASSERT( pPath != NULL );
					if ( !pPath->HasWorkToDo() )
					{
						pPath->DrawPath( cStart, cStop );
						cStart.R += Math::Clamp( cStart.R + 0.12f );
						cStart.G += Math::Clamp( cStart.G + 0.12f );
						cStart.B += Math::Clamp( cStart.B + 0.12f );
						cStop.R += Math::Clamp( cStop.R + 0.12f );
						cStop.G += Math::Clamp( cStop.G + 0.12f );
						cStop.B += Math::Clamp( cStop.B + 0.12f );
					}
				}

				//if ( ( pPathConnection != NULL ) && ( pPathConnection->GetStatus() == PathConnection::PATHCONNECTION_VALID ) )
				//{
				//	// draw the pathsegments
				//	unsigned int iPathSegments = ( unsigned int )pPath->GetPathSegmentCount();
				//	for( unsigned int iCurrSegment = 0; iCurrSegment < iPathSegments; iCurrSegment++ )
				//	{
				//		Navigate::PathSegment *pCurrSegment = pPath->GetPathSegment( iCurrSegment );
				//		unsigned int iLines = ( unsigned int )( pCurrSegment->m_Waypoints.Count() - 1 );
				//		// draw the rough (A*) path
				//		for( unsigned int iCurrFrom = 0; iCurrFrom < iLines; ++iCurrFrom )
				//		{
				//			DrawUtility::Line( pCurrSegment->m_Waypoints[ iCurrFrom ], pCurrSegment->m_Waypoints[ iCurrFrom + 1 ], ( ( iCurrFrom & 0x01 ) ? DrawUtility::GREEN : DrawUtility::CYAN ) );
				//		}
				//	}

				//	// draw our connected box
				//	Quat qRot;
				//	Cartesian4x4 mRot;
				//	Vector3 vPos = pPathConnection->GetPosition();
				//	Vector3 vA = vPos - 0.5f;
				//	Vector3 vB = vA + 1.0f;
				//	DrawUtility::Box( vA, vB );
				//	float fDistance = 4.0f * fDeltaSec;
				//	float fDistanceMoved = 0.0f;

				//	// reset the path when we get to the end of the current one
				//	bPathNotDone = pPathConnection->MoveAlongPath( fDistance, fDistanceMoved );

				//	// draw the cover
				//	if ( ( pItCover != NULL ) && ( !pItCover->HasWorkToDo() ) )
				//	{	//
				//		Vector3 vA, vB;
				//		vA = ( vEnd - 0.25f );
				//		vA.y( vA.y() + 0.75f );
				//		vB = ( vEnd + 0.25f );
				//		vB.y( vB.y() + 0.75f );
				//		DrawUtility::Box( vA, vB, NULL, NULL, DrawUtility::WHITE );

				//		unsigned int iCoverCount = pItCover->Count();
				//		for( unsigned int iCurrCover = 0; iCurrCover < iCoverCount; ++iCurrCover )
				//		{
				//			Cover cover;
				//			cover = ( *pItCover )[ iCurrCover ];
				//			vA = *( cover.m_pBegin );
				//			vA.y( vA.y() + 0.5f );
				//			vB = *( cover.m_pEnd );
				//			vB.y( vB.y() + 0.5f );

				//			ts_fRGB clr;
				//			clr.R = 0.0f; clr.G = 1.0f; clr.B = 0.0f;
				//			switch ( cover.m_iCoverType )
				//			{
				//			case Navigate::TreeNode::FULLCOVER_EDGE:
				//				clr.R = 1.0f; clr.G = 1.0f; clr.B = 0.0f;
				//				break;
				//			case Navigate::TreeNode::STANDCOVER_EDGE:
				//				clr.R = 1.0f; clr.G = 0.5f; clr.B = 0.0f;
				//				break;
				//			case Navigate::TreeNode::CROUCHCOVER_EDGE:
				//				clr.R = 1.0f; clr.G = 0.0f; clr.B = 0.0f;
				//				break;
				//			case Navigate::TreeNode::NOCOVER_EDGE:
				//				clr.R = 1.0f; clr.G = 1.0f; clr.B = 1.0f;
				//				break;
				//			default:
				//				ASSERT( 0 );
				//				break;
				//			}

				//			DrawUtility::Line( vA, vB, clr );
				//		}
				//	}
				//}
			}
		}
#endif
	}

#define MAX_AVAILABLE_PATHSEGMENT_OBJECTS ( 6 )

	bool Manager::Initialize( unsigned int iStepsPerUpdate )
	{
		if ( m_bInitialized )
		{
			ShutDown();
		}

		// the m_MeshCollection is initialized during level load.

		m_iMaxStepsCount = iStepsPerUpdate;

		m_bInitialized = ( m_MeshCollection.Count() > 0 );

#ifndef CDROM
		/* stats */

		m_totalWorkSeconds = 0.0f;
		m_totalPathsCreated = 0;
		m_averageSecondsPerPath = 0.0f;
		m_statsPrintTime = 0.0f;
#endif //CDROM

		// allocate some PathSegment objects for use in the system
		for ( int i = 0; i < MAX_AVAILABLE_PATHSEGMENT_OBJECTS; ++i )
		{
			PathSegment *pSegment = new PathSegment();
			m_AvailablePathSegments.AddHead( pSegment );
		}

		return( m_bInitialized );
	}

	bool Manager::ShutDown( void )
	{
		// destroy all the Path objects.
		if ( m_PathCollection.Count() > 0 )
		{
			for( int iCurrPath = ( m_PathCollection.Count() - 1 ); iCurrPath >= 0; --iCurrPath )
			{
				if ( m_PathCollection[ iCurrPath ] != NULL )
				{
					delete m_PathCollection[ iCurrPath ];
				}
			}
		}
		m_PathCollection.Clear();
		m_PathCollection.Reset();

		// destroy all the CoverIterator objects.
		if ( m_CoverIteratorCollection.Count() > 0 )
		{
			for( int iCurrIterator = ( ( unsigned int )m_CoverIteratorCollection.Count() - 1 ); iCurrIterator >= 0; --iCurrIterator )
			{
				if ( m_CoverIteratorCollection[ iCurrIterator ] != NULL )
				{
					delete m_CoverIteratorCollection[ iCurrIterator ];
				}
			}
		}
		m_CoverIteratorCollection.Clear();
		m_CoverIteratorCollection.Reset();

		// everyone in this collection should already have been deleted by the previous calls.
		m_DeferredWorkerCollection.Clear();
		m_DeferredWorkerCollection.Reset();

		// release all the PathSegment objects still around
		ASSERT( m_InUsePathSegments.Items() <= 0 );
		m_AvailablePathSegments.CallDestructors( true );
		while ( m_AvailablePathSegments.Items() > 0 )
		{
			m_AvailablePathSegments.Remove( m_AvailablePathSegments.Head() );
		}

		m_MeshCollection.Clear();
		m_iMaxStepsCount = 0;
		m_bInitialized = false;

		return( !m_bInitialized );
	}

	Navigate::Path *Manager::CreateOneWayPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		va_list points;
		va_start( points, iWayPointCount );
		OneWayPath	*pPath = new OneWayPath( this, pMesh, iWayPointCount, ( const Vector3 ** )points );
		va_end( points );

		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::Path *Manager::CreateOneWayPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		OneWayPath	*pPath = new OneWayPath( this, pMesh, iWayPointCount, pPoints );
		
		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::Path* Navigate::Manager::CreateBackAndForthPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		va_list points;
		va_start( points, iWayPointCount );
		BackAndForthPath	*pPath = new BackAndForthPath( this, pMesh, iWayPointCount, ( const Vector3 ** )points );
		va_end( points );

		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::Path* Navigate::Manager::CreateBackAndForthPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		BackAndForthPath	*pPath = new BackAndForthPath( this, pMesh, iWayPointCount, pPoints );

		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::Path* Navigate::Manager::CreateCircuitPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		va_list points;
		va_start( points, iWayPointCount );
		CircuitPath	*pPath = new CircuitPath( this, pMesh, iWayPointCount, ( const Vector3 ** )points );
		va_end( points );

		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::Path* Navigate::Manager::CreateCircuitPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		CircuitPath	*pPath = new CircuitPath( this, pMesh, iWayPointCount, pPoints );

		// keep track of the newly created path
		m_PathCollection.Add( pPath );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		m_DeferredWorkerCollection.Add( pPath );

		return( pPath );
	}

	Navigate::CoverIterator *Manager::CreateCoverIterator( Navigate::Mesh *pMesh, const Vector3 &vTarget, const AABB &vBox )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		Navigate::CoverIterator	*pIterator = new Navigate::CoverIterator( pMesh, vTarget, vBox );

		// record this CoverIterator
		m_CoverIteratorCollection.Add( pIterator );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		Navigate::DeferredWorker* worker = (Navigate::DeferredWorker*) pIterator;
		m_DeferredWorkerCollection.Add( worker );

		return( pIterator );
	}

	Navigate::CoverIterator *Manager::CreateCoverIterator( Navigate::Mesh *pMesh, const Vector3 &vTarget, const Vector3 &vCenter, const float fSize )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return NULL;

		Navigate::CoverIterator	*pIterator = new Navigate::CoverIterator( pMesh, vTarget, vCenter, fSize );

		// record this CoverIterator.
		m_CoverIteratorCollection.Add( pIterator );

		// make sure we add the Path to the DeferredWorker collection, so it's work can get done.
		Navigate::DeferredWorker* worker = (Navigate::DeferredWorker*) pIterator;
		m_DeferredWorkerCollection.Add( worker );

		return( pIterator );
	}

	Navigate::PointSearch* Manager::CreateClosestPointSearch( Navigate::Mesh *pMesh, const Vector3 &vPoint, float fRadius )
	{
		AABB aabb;
		aabb.Set( ( vPoint - fRadius ), ( vPoint + fRadius ) );
		return( CreateClosestPointSearch( pMesh, aabb ) );
	}

	Navigate::PointSearch* Manager::CreateClosestPointSearch( Navigate::Mesh *pMesh, const AABB &aabb )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
		{
			return( NULL );
		}

		Navigate::PointSearch *pSearch = new Navigate::PointSearch( pMesh, aabb );

		Navigate::DeferredWorker *pWorker = ( Navigate::DeferredWorker* )pSearch;
		m_DeferredWorkerCollection.Add( pWorker );

		return( pSearch );
	}

	static void s_ObstructMesh( const Navigate::Manager::ObstructionRecord &aObstructionRecord, Navigate::TreeNode *pTreeNode, const Octant &oBox, int iObstructionValue )
	{
		if ( ( pTreeNode->flags & Navigate::TreeNode::LEAF ) == Navigate::TreeNode::LEAF )
		{	// this is a leaf node, check all the tri's for an interaction with the obstruction
			for( unsigned int iCurrNode = 0; iCurrNode < pTreeNode->data.leaf.nodeCount; iCurrNode++ )
			{
				unsigned int iNode = pTreeNode->data.leaf.nodes[ iCurrNode ];
				if ( aObstructionRecord.m_pMesh->nodes[ iNode ].collisionTest == 1 )
				{	// we've checked this node already
					continue;
				}

				Navigate::Node *pNode = &( aObstructionRecord.m_pMesh->nodes[ iNode ] );

				Vector3 v0 = aObstructionRecord.m_pMesh->points[ pNode->points[ 0 ] ];
				Vector3 v1 = aObstructionRecord.m_pMesh->points[ pNode->points[ 1 ] ];
				Vector3 v2 = aObstructionRecord.m_pMesh->points[ pNode->points[ 2 ] ];
				//Vector3 vN = ( v1 - v0 ).Cross( v2 - v0 ).Normalized();

				if (
					aObstructionRecord.m_Obstruction.PointIn( v0 ) ||
					aObstructionRecord.m_Obstruction.PointIn( v1 ) ||
					aObstructionRecord.m_Obstruction.PointIn( v2 ) ||
					aObstructionRecord.m_Obstruction.TriangleTouches( ( v1 - v0 ).Cross( v2 - v0 ).Normalized(), v0, v1, v2 )
					)
				{	// this node interacts with the volume
					pNode->collisionTest = 1;

					// sanity check
					ASSERT( 
						( ( pNode->occlusionCount + iObstructionValue ) < 32 ) &&	// 5 bits available
						( ( pNode->occlusionCount + iObstructionValue ) >= 0 )
					);

					if ( iObstructionValue > 0 )
					{	// obstructing - add a positive value
						if( ( pNode->occlusionCount + iObstructionValue ) < 32 )	// 5 bits available
						{
							pNode->occlusionCount += iObstructionValue;
						}
					}
					else
					{	// clearing - add a negative value
						if( ( pNode->occlusionCount + iObstructionValue ) >= 0 )
						{
							pNode->occlusionCount += iObstructionValue;
						}
					}
				}
			}
		}
		else
		{
			for ( unsigned int iCurrChild = 0; iCurrChild < 8; ++iCurrChild )
			{
				Octant oExtents = oBox.Child( ( Octant::OCTANT )iCurrChild );
				Navigate::TreeNode *pCurrChild = pTreeNode->data.branch.children[ iCurrChild ];
				if ( ( pCurrChild != NULL ) && ( oExtents.Touches( aObstructionRecord.m_Obstruction ) || oExtents.Contains( aObstructionRecord.m_Obstruction ) ) )
				{
					s_ObstructMesh( aObstructionRecord, pCurrChild, oExtents, iObstructionValue );
				}
			}
		}
	}

	unsigned int Manager::Obstruct( Navigate::Mesh *pMesh, const AABB &blockage )
	{
		unsigned int uHandle = 0xffffffff;

		if ( pMesh == NULL )
		{
			ASSERT( false );//, "Attempting Obstruct with a NULL mesh pointer."	);
			return( uHandle );
		}

		// search for a handle to recycle
		for ( int i = 0; i < m_Obstructions.Count(); ++i )
		{
			if ( m_Obstructions[ i ].m_pMesh == NULL )
			{	// this is an unused (recycled) slot
				uHandle = i;
				m_Obstructions[ i ].m_pMesh = pMesh;
				m_Obstructions[ i ].m_Obstruction = blockage;
				break;
			}
		}

		if ( uHandle == 0xffffffff )
		{	// no handle to recycle
			Manager::ObstructionRecord	oRecord;
			oRecord.m_pMesh = pMesh;
			oRecord.m_Obstruction = blockage;
			// grab the count now as it will be the last index after we
			// insert a copy of this new record.
			uHandle = m_Obstructions.Count();
			// now insert the copy.
			m_Obstructions.Add( oRecord );
		}

		// now recursively record the obstruction
		s_ObstructMesh( m_Obstructions[ uHandle ], pMesh->root, pMesh->extents, 1 );

		// clear traversal data
		ResetNodeCollisions( pMesh );

		return( uHandle );
	}

	void Manager::RemoveObstruction( Navigate::Mesh *pMesh, unsigned int iHandle )
	{
		if ( pMesh == NULL )
		{
			ASSERT( false );//, "Attempting RemoveObstruction with a NULL mesh pointer."	);
			return;
		}

		// check the handle
		ASSERT( iHandle <= ( unsigned int )m_Obstructions.Count() );
		if ( iHandle > ( unsigned int )m_Obstructions.Count() )
		{
			ASSERT( false );//, "Attempting to remove an obstruction with an out-of-bounds handle" );
			return;
		}

		if ( m_Obstructions[ iHandle ].m_pMesh == NULL )
		{	// attempting to use a recycled handle
			ASSERT( false );//, "Attempting to remove a previously recycled handle" );
			return;
		}

		// now recursively record the obstruction
		s_ObstructMesh( m_Obstructions[ iHandle ], pMesh->root, pMesh->extents, -1 );

		// clear traversal data
		ResetNodeCollisions( pMesh );

		// recycle the record
		m_Obstructions[ iHandle ].m_pMesh = NULL;
		m_Obstructions[ iHandle ].m_Obstruction.Clear();
	}

	unsigned int Manager::AStar_Node( Navigate::Mesh *pMesh, unsigned int iSteps, AStarNodeAgent *pAgent )
	{
		// each step through the search is fairly cheap, so we'll allow several search steps per official 'work unit'
		const unsigned int	iSubStepsPerStep = 15;
		unsigned int		iStepsUsed = 0;
		unsigned int		iSubStepsUsed = 0;

		if ( !pAgent->bStarted )
		{	// search initialization-- put start node on open list
			PQSearchNode *pStartNode	= pAgent->nodePool.New();
			pStartNode->bOnOpenList		= true;
			pStartNode->fCostFromStart	= 0.0f;
			pStartNode->fEstCostToGoal	= pAgent->PathCostEstimate( pMesh, pAgent->iFrom, pAgent->iTo );
			++iSubStepsUsed;
			pStartNode->pParent			= NULL;
			pStartNode->iNode				= pAgent->iFrom;

			pAgent->masterList.Insert( pStartNode->iNode, pStartNode );
			pAgent->openList.Enqueue( ( pStartNode->fCostFromStart + pStartNode->fEstCostToGoal ), pStartNode );

			pAgent->bStarted = true;
		}

		// search steps
		while ( ( iStepsUsed < iSteps ) && ( !pAgent->openList.IsEmpty() ) )
		{
			PQSearchNode *pCurrNode;

			/* here's a rough overview of how this process works:

				we have start and end triangles in the nav mesh, the goal is to connect them. the code deals with "nodes," each node refers to a
				triangle in the mesh

				each time this code executes, it takes the node from the top of the "open list" (more on that later), and looks to extend the path
				to the (up to) 3 nodes that neighbor the current node. for each new node, a new path length is calculated, and those
				nodes are put onto the open list if they represent an advancement of the path

				the open list is a list of the ends of the all active paths. it is sorted, best to worst, so that the code is always working
				on advancing the most promising path. *** this is the key to the algorithm ***

				once the path reaches the end node, that's the best path, and we follow the chain back to the start node to get the path

				when a node is taken from the head of the open list, and the path search is expanded to its neighbors, it is not put back
				on the open list because it no longer is the end of a possible path

				the open list is sorted on the distance traversed so far plus the straight line distance from the node to the end point. this
				is a reasonable test for keeping the open list sorted, so that the most promising path stays at the top of the list

				as well as the open list, there is a master list of all nodes that have been tested, and the result for each. this is necessary
				for clean up, of course, but it is also used to determine which of two paths that go through the same node is better

			*/

			// remove the next node to process from the open list
			if ( !pAgent->openList.Dequeue( pCurrNode ) )
			{	// we really must get a node at this point
				ASSERT( false );
			}
			pCurrNode->bOnOpenList = false;

			if ( pCurrNode->iNode == pAgent->iTo )
			{	// construct the path back from this node to the start node

				// count how many steps we'll be taking
				unsigned int iStepCount = 0;
				for( PQSearchNode *pCurr = pCurrNode; pCurr != NULL; pCurr = pCurr->pParent )
				{
					iStepCount++;
					pAgent->nodesInPath.Add( 0 );
				}

				// construct the list of steps
				unsigned int iCurrIndex = ( iStepCount - 1 );
				while ( pCurrNode != NULL )
				{
					pAgent->nodesInPath[ iCurrIndex-- ] = pCurrNode->iNode;
					pCurrNode = pCurrNode->pParent;
				}

				// success!
				return( true );
			}
			else
			{
				// for each successor <iCurrNeighbor> of pCurrNode->iNode
				for( unsigned int iCurrNeighbor = 0; iCurrNeighbor < 3; iCurrNeighbor++ )
				{
					Navigate::NodeIndexType iNeighborNode = pMesh->nodes[ pCurrNode->iNode ].neighbors[ iCurrNeighbor ];
					if ( iNeighborNode == ( Navigate::NodeIndexType )-1 )
					{	// make sure we only work with real neighbors
						continue;
					}

					if ( pMesh->nodes[ iNeighborNode ].occlusionCount > 0 )
					{	// skip all occluded neighbors
						continue;
					}

					float fNewCost = 0.0f;
					if ( pCurrNode->pParent != NULL )
					{
						fNewCost = pCurrNode->fCostFromStart + pAgent->TraverseCost( pMesh, pCurrNode->pParent->iNode, pCurrNode->iNode, iNeighborNode );
						++iSubStepsUsed;
					}

					PQSearchNode *pNewNode = NULL;
					bool bFound = pAgent->masterList.Find( iNeighborNode, pNewNode );
					if ( ( bFound ) && ( pNewNode == pCurrNode ) )
					{
						ASSERT( 0 );
					}

					/* if node is found in master list, reject it if path doubles back on itself, or if the existing path that goes through */
					/* the node is shorter than the path being newly extended into that node */

					if ( bFound && ( ( pNewNode == pCurrNode->pParent ) || ( pNewNode->fCostFromStart <= fNewCost ) ) )
						continue;

					// new or improved information

					if ( !bFound )
					{	// new information
						pNewNode = pAgent->nodePool.New();
						pAgent->masterList.Insert( iNeighborNode, pNewNode );
					}

					if ( ( bFound ) && ( pNewNode->bOnOpenList ) )
					{	// NewNode is on the openList, remove it so it can be reinserted after the update
						float fPriority = ( pNewNode->fCostFromStart + pNewNode->fEstCostToGoal );
						bool bRemoveSuccess = pAgent->openList.RemoveByValue( pNewNode, fPriority );
						ASSERT( bRemoveSuccess );
						pNewNode->bOnOpenList = false;
					}

					// update the data for this search node
					pNewNode->pParent				= pCurrNode;
					pNewNode->iNode				= iNeighborNode;
					pNewNode->fCostFromStart	= fNewCost;
					pNewNode->fEstCostToGoal	= pAgent->PathCostEstimate( pMesh, iNeighborNode, pAgent->iTo );
					++iSubStepsUsed;
					pNewNode->bOnOpenList		= true;
					
					pAgent->openList.Enqueue( ( pNewNode->fCostFromStart + pNewNode->fEstCostToGoal ), pNewNode );
				}
			}

			// we're done with this node, for now (node is no longer on the open list)
			pCurrNode->bOnOpenList = false;

			// update our step count
			if ( iSubStepsUsed++ >= iSubStepsPerStep )
			{
				++iStepsUsed;
				iSubStepsUsed = 0;
			}
		}

		return( iStepsUsed );
	}

	Mesh* Manager::FindBestMeshXZ( const Vector3 &vPoint, bool bIgnoreObstructions /*= false*/ )
	{
		for ( int i = 0; i < m_MeshCollection.Count(); ++i )
		{
			int iNodeID = -1;
			if ( TestPointInMesh( m_MeshCollection[ i ], vPoint, iNodeID, bIgnoreObstructions ) )
			{
				return( m_MeshCollection[ i ] );
			}
		}
		return( NULL );
	}

	struct __BestMesh
	{
		float				fDistanceSqrd;
		Navigate::Mesh	*pMesh;
		Vector3			vPointOnMesh;
	};

	Mesh* Manager::FindBestMesh( const Vector3 &vPoint, bool bIgnoreObstructions /*= false*/ )
	{
		DynamicArray< __BestMesh >	searchResult;

		// add support for multiple meshes if we ever have them.
		for ( int i = 0; i < m_MeshCollection.Count(); ++i )
		{
			__BestMesh	mesh;
			mesh.pMesh = m_MeshCollection[ i ];
			if ( TestPointInMesh( mesh.pMesh, vPoint, mesh.vPointOnMesh, 1.0f, bIgnoreObstructions ) )
			{
				mesh.fDistanceSqrd = ( mesh.vPointOnMesh - vPoint ).LengthSquared();
				ASSERT( mesh.fDistanceSqrd >= 0.0f );
				searchResult.Add( mesh );
			}
		}

		if ( searchResult.Count() > 0 )
		{
			float fBestDist = MAXFLOAT;
			int iBestIndex = -1;
			for ( int j = 0; j < searchResult.Count(); ++j )
			{
				if ( searchResult[ j ].fDistanceSqrd < fBestDist )
				{
					iBestIndex = j;
					fBestDist = searchResult[ j ].fDistanceSqrd;
				}
			}

			if ( iBestIndex >= 0 )
			{
				return( searchResult[ iBestIndex ].pMesh );
			}
		}

		// d'oh!
		//ASSERT( false ); this is ok because some guys repel (etc) onto the mesh...brian
		return( NULL );
	}

	Vector3 Manager::FindRandomPosition( Navigate::Mesh *pMesh )
	{
		RandomPointSearch ptSearch( pMesh );
		return( ptSearch.m_vPoint );
	}

	bool Manager::TestPointInMesh( Navigate::Mesh *pMesh, const Vector3 &vPoint, int &iNodeID, bool bIgnoreObstructions )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return false;

		ClosestPointNodeSearch skPoint;

#ifndef CDROM
		/* stats */

		u32 startMilliseconds = EngineTimer__Time();
#endif //CDROM

		//PointNodeIterator itPoint( pMesh, vPoint );
		iNodeID = ( int )skPoint.SearchForPoint( pMesh, vPoint );

#ifndef CDROM
		/* stats. milliseconds may not be granular enough to get accurate results here */

		m_totalWorkSeconds += (float) (EngineTimer__Time() - startMilliseconds) / 1000.0f;
#endif //CDROM

		//iNodeID = -1;
		//if ( itPoint.Count() > 0 )
		//{
		//	iNodeID = ( int )( itPoint[ 0 ] );
		//}

		if ( ( !bIgnoreObstructions ) && ( iNodeID >= 0 ) )
		{	// check for obstructions
			return( pMesh->nodes[ iNodeID ].occlusionCount == 0 );
		}

		return( iNodeID != -1 );
	}

	bool Manager::TestPointInMesh( Navigate::Mesh *pMesh, const Vector3 &vPoint, Vector3 &vPointOnMesh, float fDistance, bool bIgnoreObstructions )
	{
		ASSERT_PTR( pMesh );
		if ( !pMesh )
			return false;

#ifndef CDROM
		/* stats */

		u32 startMilliseconds = EngineTimer__Time();
#endif //CDROM

		SemiSegment sSegment;
		Vector3 vStart, vEnd;
		vStart = vPoint;
		vStart.y( vStart.y() + fDistance );
		vEnd = vPoint;
		vEnd.y( vEnd.y() - fDistance );
		sSegment.SetEndPoints( vStart, vEnd );

		SegmentIntersectIterator it( pMesh, sSegment, bIgnoreObstructions );

		// search for the closest intersection
		int iClosestPointIndex = -1;
		int iMaxIndex = ( int )it.Count();
		float fShortestDistanceSqr = MAXFLOAT;
		Segment seg;
		seg.start = vPoint;
		for ( int i = 0; i < iMaxIndex; ++i )
		{
			seg.end = it[ i ];
			float fLengthSqr = seg.LengthSquared();
			if ( fLengthSqr < fShortestDistanceSqr )
			{
				fShortestDistanceSqr = fLengthSqr;
				iClosestPointIndex = i;
			}
		}

#ifndef CDROM
		/* stats. milliseconds may not be granular enough to get accurate results here */

		m_totalWorkSeconds += (float) (EngineTimer__Time() - startMilliseconds) / 1000.0f;
#endif //CDROM

		if ( iClosestPointIndex >= 0 )
		{	// we found at least one intersection
			vPointOnMesh = it[ iClosestPointIndex ];
			return( true );
		}
		else
		{
			return( false );
		}
	}
}

void Navigate::Manager::ResetNodeCollisions( Navigate::Mesh *pMesh )
{
	ASSERT_PTR( pMesh );
	if ( !pMesh )
		return;

	for ( NodeIndexType i = 0; i < pMesh->nodeCount; ++i )
	{
		pMesh->nodes[ i ].collisionTest = 0;
	}
}

Navigate::PathSegment* Navigate::Manager::AquireSegment( void )
{
	PathSegment *pSegment = NULL;
	if ( m_AvailablePathSegments.Items() > 0 )
	{	/* use an old segment */
		pSegment = m_AvailablePathSegments.Head();
		m_AvailablePathSegments.Unlink( pSegment );

		// try to keep the number of available segments to a reasonable number
		if ( m_AvailablePathSegments.Items() > MAX_AVAILABLE_PATHSEGMENT_OBJECTS )
		{
			PathSegment *pDead = m_AvailablePathSegments.Tail();
			m_AvailablePathSegments.Unlink( pDead );
			ASSERT( !m_InUsePathSegments.Contains( pDead ) );
			delete pDead;
		}
	}
	else
	{	/* create a new segment */
		pSegment = new PathSegment();
	}

	/* now record that this segment is in use */
	ASSERT( pSegment != NULL );
	ASSERT( !m_InUsePathSegments.Contains( pSegment ) );
	m_InUsePathSegments.AddHead( pSegment );

	/* done */
	return( pSegment );
}

void Navigate::Manager::ReleaseSegment( PathSegment *pSegment )
{
	ASSERT( pSegment != NULL );
	if ( m_InUsePathSegments.Contains( pSegment ) )
	{
		m_InUsePathSegments.Unlink( pSegment );
	}
	ASSERT( !m_AvailablePathSegments.Contains( pSegment ) );
	m_AvailablePathSegments.AddHead( pSegment );
}
