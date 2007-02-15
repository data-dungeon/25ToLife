/********************************************************************
	created:	2003/12/18
	created:	18:12:2003   10:18
	filename: 	C:\dev\avalanche\gamelib\Navigate\NavManager.h
	file path:	C:\dev\avalanche\gamelib\Navigate
	file base:	NavManager
	file ext:	h
	author:		Shon C. Love

	purpose:	Manage loaded navigation meshs, create paths
*********************************************************************/
#if !defined( NAV_MANAGER_H )
#define NAV_MANAGER_H

#include "Navigate/NavTree.h"

/* forward declarations */

struct ts_DBChunkHdr_tag;		// in Game/Database/DBFile.h

/*!	@brief The Navigation Library.

	This library is used for dynamic path generation.  There is also support for searching for cover.
*/
namespace Navigate
{
	class Path;
	class Mesh;
	class Node;
	class AStarAgent;
	class AStarNodeAgent;
	class CoverIterator;
	class PointSearch;

	class PQSearchNode
	{
	public:
		PQSearchNode( void );
		virtual ~PQSearchNode( void );

		PQSearchNode	*pParent;
		float				fCostFromStart;
		float				fEstCostToGoal;
		NodeIndexType	iNode;
		bool				bOnOpenList;
	};

	class Manager
	{
	friend class Path;
	public:
							Manager( void );
		virtual			~Manager( void );

		//! Register a mesh with the navigation manager.
		static void		RegisterMesh( Navigate::Mesh *pMesh );

		//! Callback for loading dbl chunk-- matches t_ChunkLoadCallback typedef
		static bool		LoadChunk(void *pData, ts_DBChunkHdr_tag *pChunkHdr, void **ppRetData, u32 *pu32DataSize);

		//! searches the world dbl for any Navigation Mesh chunks, initializes them, and records them
		bool				Initialize( unsigned int iStepsPerUpdate );
		int				GetWorkUnitsPerFrame()										{ return(m_iMaxStepsCount); }				// mainly for debugging, so we can hook up to watch
		void				SetWorkUnitsPerFrame(unsigned int unitsPerFrame)	{ m_iMaxStepsCount = unitsPerFrame; }	// mainly for debugging, so we can hook up to watch

		//! cleans up the paths.  should be called whenever between world loads, etc.
		bool				ShutDown( void );

		/*!@name Path Creation Methods */
		/*! These path creation routines take a number of way-points (n), followed by (n) const Vector3*'s, which represent the way-points. */
		//@{
		Navigate::Path	*CreateOneWayPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... );
		Navigate::Path	*CreateOneWayPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints );
		Navigate::Path	*CreateBackAndForthPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... );
		Navigate::Path	*CreateBackAndForthPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints );
		Navigate::Path	*CreateCircuitPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, ... );
		Navigate::Path	*CreateCircuitPath( Navigate::Mesh *pMesh, unsigned int iWayPointCount, const Vector3 **pPoints );
		//@}

		//! Destroy the given path.
		void				DestroyPath( Path *pPath );

		//! Create a CoverIterator.
		Navigate::CoverIterator	*CreateCoverIterator( Navigate::Mesh *pMesh, const Vector3 &vTarget, const Vector3 &vCenter, const float fSize );
		Navigate::CoverIterator	*CreateCoverIterator( Navigate::Mesh *pMesh, const Vector3 &vTarget, const AABB &vBox );

		//! Destroy the given iterator.
		void				DestroyCoverIterator( Navigate::CoverIterator *pIterator );

		//! Determine the best mesh to associate with the given point.
		Navigate::Mesh	*FindBestMesh( const Vector3 &vPoint, bool bIgnoreObstructions = false );
		Navigate::Mesh	*FindBestMeshXZ( const Vector3 &vPoint, bool bIgnoreObstructions = false );
		int				MeshCount()	{ return(m_MeshCollection.Count()); }

		//! Find a random point in the given mesh.
		Vector3			FindRandomPosition( Navigate::Mesh *pMesh );

		//! Find the closest point in a mesh, within the given volume, to the given point.
		PointSearch*	CreateClosestPointSearch( Navigate::Mesh *pMesh, const Vector3 &vPoint, float fRadius );
		PointSearch*	CreateClosestPointSearch( Navigate::Mesh *pMesh, const AABB &aabb );

		//! Destroy the given point search.
		void				DestroyPointSearch( Navigate::PointSearch *pPointSearch );

		//! Test if the given point is 'in' a navigation mesh.
		bool				TestPointInMesh( Navigate::Mesh *pMesh, const Vector3 &vPoint, int &iNodeID, bool bIgnoreObstructions = false );

		//! Test if the given point is 'in' a navigation mesh.
		bool				TestPointInMesh( Navigate::Mesh *pMesh, const Vector3 &vPoint, Vector3 &vPointOnMesh, float fDistance = 1.0f, bool bIgnoreObstructions = false );

		//! Reset node collision test flag.
		//!@param pMesh The mesh whose nodes will be reset.
		void				ResetNodeCollisions( Navigate::Mesh *pMesh );

		//! Records an obstruction - returns a handle to the obstruction record
		//! The handles used in these calls are recycled.
		unsigned int	Obstruct( Navigate::Mesh *pMesh, const AABB &blockage );
		//! Clears an obstruction - takes the handle returned by Obstruct() as its parameter
		void				RemoveObstruction( Navigate::Mesh *pMesh, unsigned int iHandle );

		//! @param iSteps The max number of steps to take on this iteration.
		//! @param pAgent Contains the state of the search.
		//! @return Returns the number of steps taken, not to exceed iSteps.
		unsigned int	AStar_Node( Navigate::Mesh *pMesh, unsigned int iSteps, AStarNodeAgent *pAgent );

		//! Update all pending refinement operations
		void				Update( float fDeltaSec );

#ifndef CDROM
		/* stats */

		void IncPathsCreated()		{ m_totalPathsCreated++; }

		/* visualization */

		void ToggleDrawNavMesh()	{ m_drawNavMesh ^= 1; }
		bool m_drawNavMesh;
		void ToggleDrawNavMeshOcclusion() { m_drawNavMeshOcclusions ^= 1; }
		bool m_drawNavMeshOcclusions;
#endif //CDROM

		//! A record of interesting 
		struct ObstructionRecord
		{
			Navigate::Mesh	*m_pMesh;
			AABB				m_Obstruction;
		};

	protected:
		//! Remove a deferred worker from the collection
		bool				RemoveDeferredWorker( Navigate::DeferredWorker *pWorker );

		//! Has the manager been properly initialized?
		bool				m_bInitialized;

		//! The number of work steps to be taken per call to Update
		unsigned int	m_iMaxStepsCount;

		//! The collection of currently active path objects
		DynamicArray< Navigate::Path* >				m_PathCollection;

		//! The collection of currently active CoverIterator objects.
		DynamicArray< Navigate::CoverIterator* >	m_CoverIteratorCollection;

		//! The collection of currently active DeferredWorker objects
		DynamicArray< Navigate::DeferredWorker* >	m_DeferredWorkerCollection;

		//! A list of current obstructions.
		DynamicArray< ObstructionRecord >	m_Obstructions;

		////! This may need to become a linked list of mesh objects
		//Navigate::Mesh	*m_pMesh;
		//! The collection of mesh objects
		static DynamicArray< Navigate::Mesh* >		m_MeshCollection;

		//!@name PathSegment collection members.
		//@{
		PathSegment		*AquireSegment( void );
		void				ReleaseSegment( PathSegment *pSegment );
		DoublyLinkedList< PathSegment >		m_AvailablePathSegments;
		DoublyLinkedList< PathSegment >		m_InUsePathSegments;
		//@}

#ifndef CDROM
		/* stats */

		float m_totalWorkSeconds;
		int m_totalPathsCreated;
		float m_averageSecondsPerPath;
		float m_statsPrintTime;
#endif //CDROM
	};
}

DefineSingleton(Navigate::Manager)
#define gbl_NavManager GetSingleton(Navigate::Manager)
//extern Navigate::Manager gbl_NavManager;

#endif