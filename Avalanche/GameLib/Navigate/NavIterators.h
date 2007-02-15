/********************************************************************
	created:	2003/12/11
	created:	11:12:2003   15:54
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavIterators.h
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavIterators
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Utility classes for querying the NavMesh
*********************************************************************/

#ifndef NAVITERATORS_H
#define NAVITERATORS_H

#include "Collide/AABB.h"
#include "linearalgebra/semiseg.h"
#include "Math/Vector.h"
#include "Navigate/NavMesh.h"
#include "container/Darray.h"
#include "container/pqueue.h"
#include "Navigate/NavPath.h"

namespace Navigate
{

	class DeferredWorker;

	/*! Mesh database iterator base class. */
	/*!
		Iterator template class for iterating over interesting data from the Mesh database.
	*/
	template< class IteratorType >
	class Iterator
	{
	public:
		/*!@name Iteration Methods */
		//@{
		/*!
			The [] operator - for direct indexing of a specific iteration member.
		*/
		virtual IteratorType	operator []( unsigned int index ) = 0;

		/*!
			Get the number of items in the iteration.
			@return Returns the total number of items in the iteration.
		*/
		virtual unsigned int	Count( void ) = 0;

		/*!
			Initialize the iteration.
			@return If there are no items to iterate, <b>false</b>, <b>true</b> otherwise.
		*/
		virtual bool			IterateInit( void ) = 0;

		/*!
			Iterate the next item in the iteration.
			@param item [<b>out</b>] The iteration item.
			@return If there is another item to iterate (after the one returned in the item variable), <b>true</b>, <b>false</b> otherwise.  Calling Iterate again after returning <b>false</b> will set item to the last item in the iteration.
		*/
		virtual bool			Iterate( IteratorType &item ) = 0;
		//@}
	};

	/*!
		Given a point, iterates over all Navigate::Node ID's in the given Navigate::Mesh.
	*/
	class PointNodeIterator : public Iterator< unsigned int >
	{
	public:
		/*!@name Constructors */
		//@{
		PointNodeIterator( Mesh *pMesh, const Vector3 &vPoint );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~PointNodeIterator( void );
		//@}

		/*!@name Iterator Methods */
		//@{
		virtual unsigned int	operator []( unsigned int index );
		virtual unsigned int	Count( void );
		virtual bool			IterateInit( void );
		virtual bool			Iterate( unsigned int &item );
		//@}

	protected:
		/*!@name Protected Data Members */
		//@{
		/*!
			The Node ID's that could be associated with the given point.  They are Stored in closest-farthest order.
		*/
		DynamicArray< unsigned int >	m_Nodes;

		/*!
			Current iteration variable.
		*/
		unsigned int					m_iCurrIndex;
		//@}
	};

	/*!
	Given a point, finds the closest node id in the mesh.
	*/
	class ClosestPointNodeSearch
	{
	public:
		/*!@name Constructors */
		//@{
		ClosestPointNodeSearch( void );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~ClosestPointNodeSearch( void );
		//@}

		/*! Search for the node id closest to the given point.
		@param pMesh The mesh to search.
		@param vPoint The point for which to search.
		@return The node id, if detected.  0xFFFFFFFF if no appropriate node was detected.
		*/
		virtual unsigned int SearchForPoint( Mesh *pMesh, const Vector3 &vPoint );
	};

	/*!
		Given a point, iterates the Navigate::TreeNode that contains it - Count() will only ever return 0, or 1 for this iterator.
	*/
	class PointOctantIterator : public Iterator< TreeNode* >
	{
	public:
		/*!@name Constructors */
		//@{
		PointOctantIterator( Mesh *pMesh, const Vector3 &vPoint );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~PointOctantIterator( void );
		//@}

		/*!@name Iterator Methods */
		//@{
		virtual TreeNode*		operator []( unsigned int index );
		virtual unsigned int	Count( void );
		virtual bool			IterateInit( void );
		virtual bool			Iterate( TreeNode *&item );
		//@}

	protected:
		/*!@name Protected Data Members */
		//@{
		/*!
			The Navigate::TreeNode associated with the given point.
		*/
		TreeNode				*m_pTreeNode;
		//@}
	};

	/*!
		Given an AABB, iterates the Navigate::TreeNodes that contain it.
	*/
	class AABBOctantIterator : public Iterator< Navigate::TreeNode* >
	{
	public:
		/*!@name Constructors */
		//@{
		AABBOctantIterator( Navigate::Mesh *pMesh, const Vector3 &vPoint, float fRadius );
		AABBOctantIterator( Navigate::Mesh *pMesh, const AABB &Box );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~AABBOctantIterator( void );
		//@}

		/*!@name Iterator Methods */
		//@{
		virtual Navigate::TreeNode*		operator []( unsigned int index );
		virtual unsigned int	Count( void );
		virtual bool			IterateInit( void );
		virtual bool			Iterate( Navigate::TreeNode *&item );
		//@}

	protected:
		/*!@name Protected Data Members */
		//@{
		/*!
		The Navigate::TreeNodes associated with the given AABB.
		*/
		MinQueue< float, Navigate::TreeNode* >	m_TreeNodes;
		//@}
	};

	/*!
		Iterates over the intersection points between the given NavMesh and the given Segment.
	*/
	class SegmentIntersectIterator : public Iterator< Vector3 >
	{
	public:
		/*!@name Constructors */
		//@{
		/*! Take the Mesh to search, and the SemiSegment to test. */
		SegmentIntersectIterator( Mesh *pMesh, const SemiSegment &sSegment, bool bIgnoreObstructions = false );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~SegmentIntersectIterator( void );
		//@}

		/*!@name Iterator Methods */
		//@{
		virtual Vector3			operator[]( unsigned int index );
		virtual unsigned int	Count( void );
		virtual bool			IterateInit( void );
		virtual bool			Iterate( Vector3 &item );
		//@}

	protected:
		int						m_iCurrIntersection;
		DynamicArray< Vector3 >	m_Intersections;

	};

	/*!
		Represents a cover position.
	*/
	class Cover
	{
	public:
		Cover();

		/*!@name Public Data Members */
		//@{

		/*!
			Record the closest cover point to the origin of the cover request.
		*/
		Vector3Packed	m_vCover;

		/*!
			Beginning point of the Cover edge.
		*/
		Vector3Packed	*m_pBegin;

		/*!
			End point of the Cover edge.
		*/
		Vector3Packed	*m_pEnd;

		/*!
			If this cover shares it's m_pEnd vertex with another one's m_pBegin vertex, in this result set, m_pNext points to that cover.
		*/
		Cover				*m_pNext;

		/*!
			If this cover shares it's m_pBegin vertex with another one's m_pEnd vertex, in this result set, m_pNext points to that cover.
		*/
		Cover				*m_pPrev;

		/*!
			Measure of how perpendicular this cover is to the target point.
			The closer this value is to 1.0f, the more perpendicular it is.
		*/
		float				m_fDot;

		/*!
		The id of the nav-mesh triangle this cover is associated with.
		*/
		unsigned int	m_iNodeID;

		/*!
			Determines which edge of the m_iNodeID node does this cover represent.
		*/
		unsigned char	m_iEdgeID		: 2;

		/*!
			Any cover flags associated with this cover.
			Valid values: Navigate::TreeNode::FLAGS( NOCOVER_EDGE, FULLCOVER_EDGE, STANDCOVER_EDGE, CROUCHCOVER_EDGE )
		*/
		unsigned char	m_iCoverType	: 2;

		/*!
			Any other Misc. flags.  Room to grow.
		*/
		unsigned char	m_iMiscFlags	: 4;
		//@}
	};

	/*! Search the given Mesh for all relevant Cover data */
	/*!	Given a radius and a target point, the CoverEdgeIterator will list all the possible Cover.
		The radius represents the halfWidth of an AABB used to look for Cover information.
	*/
	class CoverIterator : public Iterator< Cover >, DeferredWorker
	{
	public:
		/*!@name Constructors */
		//@{
		/*! Take a center point, and a size to create an AABB to search with. */
		CoverIterator( Mesh *pMesh, const Vector3 &vTargetPoint, const Vector3 &vOrigin, const float fRadius, const float fDotThreshold = 0.2f );
		/*! Take an AABB as the volume to search for Cover in relation to the given target point. */
		CoverIterator( Mesh *pMesh, const Vector3 &vTargetPoint, const AABB &box, const float fDotThreshold = 0.2f );
		//@}

		/*!@name Destructors */
		//@{
		virtual ~CoverIterator( void );
		//@}

		/*!@name Iterator Methods */
		//@{
		virtual Cover			operator []( unsigned int index );
		virtual unsigned int	Count( void );
		virtual bool			IterateInit( void );
		virtual bool			Iterate( Cover &item );
		//@}

		/*!@name DeferredWorker Methods */
		//@{
		virtual bool			HasWorkToDo( void );
		virtual unsigned int	DoWork( unsigned int iSteps );
		//@}

		/*!@name Search parameter Methods */
		//@{
		virtual AABB&			GetVolume( void ){ return( m_AABB ); }
		virtual Vector3CRef		GetTarget( void ){ return( m_vTargetPoint ); }
		//@}

	protected:
		/*!
			Search for cover in the current TreeNode of the search.
			@param iSteps The number of steps that may be taken during this method call.
			@return The number of steps actually used during this method call.
		*/
		unsigned int FindCover( unsigned int iSteps );

		/*!@name Protected Cover Search Data Members */
		//@{
		/*!
		Recursion frame structure for step-wise processing.
		*/
		struct _CoverSearchFrame
		{
			// variables for storing the DoWork state
			unsigned int			m_iCurrIndex;
			Navigate::TreeNode	*m_pParent;
			Navigate::TreeNode	*m_pCurrent;
			Octant					m_Octant;
			float						m_fDotThreshold;

			// variables for storing the FindCover state
			unsigned int			m_iCurrCoverIndex;
		};

		/*!
		Storage for our recursion frames.
		*/
		DynamicArray< _CoverSearchFrame >	m_SearchFrames;

		/*!
			The volume to search for cover points.
		*/
		AABB								m_AABB;

		/*!
			The Mesh we're searching.
		*/
		Navigate::Mesh						*m_pMesh;

		/*!
			The target point we're searching for cover against.
		*/
		Vector3								m_vTargetPoint;
		//@}

		/*!@name Protected Iteration Data Members */
		//@{
		/*!
		The list of Cover objects detected.
		*/
		DynamicArray< Cover >	m_Cover;

		/*!
		The current iteration counter.
		*/
		unsigned int			m_iCurrIndex;

		/*!
		Record our working status.
		*/
		bool					m_bHasWorkToDo;
		//@}
	};

	/*!
		Tests if the given line intersects with any hardEdges in the given Navigate::Mesh.
		The search stops as soon as an intersection is detected.
	*/
#define LINEEDGEQUERY_MAX_RECURSION_DEPTH	( 16 )
	class LineEdgeQuery : DeferredWorker
	{
	public:
		LineEdgeQuery( void );
		virtual ~LineEdgeQuery( void );

		/*!@name Methods */
		//@{
		virtual bool			Configure( Mesh *pMesh, const Vector3 &vBegin, const Vector3 &vEnd );
		virtual bool			FoundAHardEdgeIntersection( void ) { return( m_bResult ); };
		//@}

		/*!@name DeferredWorker Methods */
		//@{
		virtual bool			HasWorkToDo( void );
		virtual unsigned int	DoWork( unsigned int iSteps );
		//@}

	protected:
		//virtual bool	TestLine( const Navigate::Mesh *pMesh, const TreeNode *pNode, const Octant &oBox, const SemiSegment &seg ) const;

		Mesh				*m_pMesh;
		Vector3			m_vBegin;
		Vector3			m_vEnd;
		bool				m_bHasWorkToDo;

		bool				m_bResult;

		struct leq_Leaf
		{
			unsigned int	m_iCurrNode;
			unsigned char	m_iCurrEdge;
		};

		struct leq_Branch
		{
			float				m_fX;
			float				m_fY;
			float				m_fZ;
			float				m_fHalfSize;
			unsigned char	m_octCurr;
		};

		union leq_Data
		{
			leq_Leaf		leaf;
			leq_Branch	branch;
		};

		struct _LineEdgeQuery_Frame
		{
			Navigate::TreeNode	*m_pTreeNode;
			leq_Data					m_Data;
		};

		unsigned char	m_iRecursionLevel;
		_LineEdgeQuery_Frame	m_RecursionFrames[ LINEEDGEQUERY_MAX_RECURSION_DEPTH ];	/* this number must increase if we get really large meshes. */
	};

	/*!
		Searches for a valid random point in the given mesh
	*/
	class RandomPointSearch
	{
	public:
		RandomPointSearch( Mesh *pMesh );
		virtual ~RandomPointSearch( void );

		bool	m_bFoundValidPoint;
		Vector3	m_vPoint;
	};

	/*!
		Search for the closest point in the mesh to a given point.
	*/
	class PointSearch : public DeferredWorker
	{
	public:
		PointSearch( Navigate::Mesh *pMesh, const AABB &aabb );
		virtual ~PointSearch( void );

		//! Is the search complete?
		virtual bool			HasWorkToDo( void );

		//! Continue the search.
		virtual unsigned int	DoWork( unsigned int iSteps );

		//! Is the discovered point actually in the mesh?
		bool			IsSearchSuccessful( void );

		//! The best point discovered.
		Vector3		GetPoint( void );

	protected:
		bool									m_bHasWorkToDo;
		bool									m_bCandidateFound;
		Vector3								m_vTarget;
		Vector3								m_vCandidate;
		float									m_fBestCandidateDistSQ;

		Navigate::Mesh						*m_pMesh;
		Navigate::AABBOctantIterator	m_OctantIterator;
		Navigate::TreeNode				*m_pCurrentTreeNode;
		unsigned short						m_iCurrentFaceIndex;
		unsigned short						m_iCurrentEdgeIndex;
	};
}

#endif