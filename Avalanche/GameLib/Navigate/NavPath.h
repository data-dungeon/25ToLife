/********************************************************************
	created:	2003/12/18
	created:	18:12:2003   10:40
	filename: 	c:\dev\avalanche\gamelib\Navigate\NavPath.h
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	NavPath
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Interface for path classes
*********************************************************************/

#if !defined( NAV_PATH_H )
#define NAV_PATH_H

#include "Math/Vector.h"
#include "container/skiplist.h"

/* uncommenting this turns on stuff like path drawing */
//#define NAVIGATION_DEBUGGING

namespace Navigate
{
	class Mesh;
	class Manager;
	class Path;
	class PathSegment;
	class PathConnection;
	class OneWayPath;
	class OneWayPathConnection;
	class AStarNodeAgent;
	class LineEdgeQuery;

	/*! This class represents a client connection to a Path.

		This is an abstract base class for all Path client code to interface into the Path object.
		All the connection state data is stored children of this object.  
	*/
	class PathConnection
	{
	public:
		virtual ~PathConnection( void );

		enum Status
		{
			PATHCONNECTION_PENDING,	//!< The Path is still in the refinement stages, and is unusable.
			PATHCONNECTION_VALID,	//!< The connected Path is now usable.
			PATHCONNECTION_ENDOFPATH,	//!< The connection is at the end of the path.
		};

		/*! Get the status of this PathConnection.
			@return Information about the usability of this PathConnection.
		*/
		virtual Status			GetStatus( void ) = 0;

		/*! Moves the connection along the path a given distance.
			@param fDistance The distance to move along the connected path.
			@param fDistanceMoved The distance actually moved.  This will equal fDistance, unless 'false' is returned, then the actual amount moved is returned instead of fDistance.
			@return Returns true if the full distance was moved, false if the end of the path was encountered.
		*/
		virtual bool			MoveAlongPath( float fDistance, float &fDistanceMoved ) = 0;

		/*!	Moves the connection to the next waypoint.
			@return Returns false if there is not next waypoint (the current waypoint is the last one in the path).
		*/
		virtual bool			MoveToNextWaypoint( void ) = 0;

		/*! Discover the current position.
			@return The current position on the Path that this PathConnection is connected to.
		*/
		virtual Vector3		GetPosition( void ) = 0;

		/*! Get a direction vector for the current position.
			@return A vector pointing from the current position to the next waypoint.
		*/
		virtual Vector3		GetDirection( void ) = 0;

		/*! Get a normalized direction vector.
			@return A normalized vector pointing from the current position to the next waypoint.
		*/
		virtual Vector3		GetDirectionNormal( void ) = 0;

		/*! Get the next waypoint (immediate) position.
			@return The position of the next waypoint.  This is not necessarilly one of the waypoints that were used to create the connected Path, but is the next waypoint within the current PathSegment.
		*/
		virtual Vector3		GetNextWaypoint( void ) = 0;

		/*! Connect to the Path as close as possible to an arbitrary point.
			@param vPoint An arbitrary point that we'd like to associate to the Path, as close as possible.
			@return A point on the path that is as close as possible to the given point.
		*/
		virtual Vector3		ConnectNearest( const Vector3 vPoint ) = 0;

		/*! Reverse this connection.
		*/
		virtual void			Reverse( void ) = 0;

		/*! Release this connection, and possibly the associated path if this was the only connection.
		*/
		virtual void			Release( bool bReleaseUnusedPath = true ) = 0;
	};

	/*! Abstract base class for objects that want to do work-over-time.	*/
	class DeferredWorker
	{
	public:
		/*! Determine if the object has more work it needs to do.
			@return <b>true</b> if the object has more work to do, <b>false</b> otherwise.
		*/
		virtual bool			HasWorkToDo( void ) = 0;

		/*! Give the object some time to work.  The Navigate::Manager class has an Update method.  During a call to that method, all DeferredWorker object that it is aware of will get an equal share of 'work units', or steps that they may perform during that call to Navigate::Manager::Update.
			@param iSteps The number of 'work units' that is alloted for this object to perform during this update phase.
			@return The number of steps the object used during this update phase.  If the number returned is less than iSteps, the remaining steps may be used by another DeferredWorker object.
		*/
		virtual unsigned int	DoWork( unsigned int iSteps ) = 0;
	};

	/*!	Represents the path between two requested waypoints.

		The PathSegment contains all the path data between two consecutive waypoints in the owning Path object's set of waypoints.
		This is a DefferedWorker class.  The real work of path searching, simplifying, smoothing, and analyzing is performed at the PathSegment level.
	*/
	class PathSegment : public DeferredWorker
	{
	public:
		/*! @name Constructors */
		//@{
		/*! Construct a PathSegment from vBegin to vEnd.
			@param pMesh A pointer to the Mesh this PathSegment refers to.
			@param pPath A pointer to the Path object that owns this PathSegment.
			@param vBegin The point in 3D that the PathSegment is to begin.
			@param vEnd The point in 3D that the PathSegment is to end.
		*/
		PathSegment( void );
		PathSegment( Mesh *pMesh, Path *pPath );
		//@}

		/*! @name Destructors */
		//@{
		virtual ~PathSegment( void );
		//@}

		//! Reset the object for another search pass, using the original mesh and path.
		virtual void			Reset( void );
		virtual void			Reset( Mesh *pMesh );
		virtual void			Reset( Mesh *pMesh, Path *pPath );

		//! Set the working points for the segment.  Call this before Initialize().
		virtual bool			SetPoints( const Vector3 &vBegin, const Vector3 &vEnd );
		virtual bool			SetPoints( NodeIndexType iBegin, NodeIndexType iEnd );

		//! Initialize this PathSegment for processing by calls to DoWork.
		virtual bool			Initialize( void );

		//! Determine if this PathSegment needs further refining.
		virtual bool			HasWorkToDo( void );

		//! Perform some more work in refining this PathSegment.
		virtual unsigned int	DoWork( unsigned int iSteps = 10 );

		/*! The phases of refinement that a PathSegment can go through. */
		enum RefiningPhase
		{
			UNINITIALIZED,	//!< The PathSegment has not been initialzed.

			READY,			//!< The PathSegment has all the data it needs to begin searching.

			SEARCHING,		//!< The PathSegment is searching for a rough path.

			SIMPLIFYING,	//!< The PathSegment is pruning out extra waypoints.

			SMOOTHING,		//!< The PathSegment is doing smoothing operations.

			ANALYZING,		//!< The PathSegment is calculating segment lengths, etc.

			COMPLETE		//!< The PathSegment is done processing and is ready for use.
		};

		/*! Reverse the waypoints in this segment
		*/
		virtual void			Reverse( void );

		/*! Determine what refinement phase the PathSegment is in.
			@return The appropriate phase.
		*/
		RefiningPhase			GetPhase( void ){ return( m_Phase ); };

		/*! length of segment
		*/
		float						Length( void );

		//! @name Public Data Members
		//@{

		//! The node id associated with the given Begin point.
		NodeIndexType				m_iBeginNodeID;

		//! The node id associated with the given End point.
		NodeIndexType				m_iEndNodeID;

		DynamicArray< Vector3 >	m_Waypoints;		//!< The final set of waypoints.

		DynamicArray< float >	m_SegmentLengths;	//!< The length of each line segment in this path segment (m_Waypoints.Count() - 1 elements).

		//! The next pointer used by DoublyLinkedList objects.
		PathSegment			*next;

		//! The prev pointer used by DoublyLinkedList objects.
		PathSegment			*prev;
		//@}

	protected:

		/*!	Search for a path between our begin and end points.
			@return Returns the number of steps actually taken.  If the return value < iSteps, then no further PathSearch steps need be taken.
		*/
		virtual unsigned int	PathSearch( unsigned int iSteps );

		/*! Simplify the current path.
			@return Returns the number of steps actually taken.  If the return value < iSteps, then no further Simplify steps need be taken.
		*/
		virtual unsigned int	Simplify( unsigned int iSteps );

		/*! Smooth the (simplified) path
			@return Returns the number of steps actually taken.  If the return value < iSteps, then no further Smooth steps need be taken.
		*/
		virtual unsigned int	Smooth( unsigned int iSteps );

		/*! Analyze the finished path
			@return Returns the number of steps actually taken.  If the return value < iSteps, then no further Analyze steps need be taken.
		*/
		virtual unsigned int	Analyze( unsigned int iSteps );

		//! The Mesh that this PathSegment is associated with.
		Mesh					*m_pMesh;

		//! The Path that this PathSegment is part of.
		Path					*m_pPath;

		//! The current refinement state.
		RefiningPhase		m_Phase;

		//! The agent to use for path searching.
		AStarNodeAgent		*m_pAgent;

		/* The search state data. */
		struct _SearchData
		{
			AStarNodeAgent	*pAgent;
		};

		/* The simplification state data. */
		struct _SimplificationData
		{
			unsigned int	iFromIndex;
			LineEdgeQuery	*pQuery;
		};

		/* The smoothing state data. */
		struct _SmoothingData
		{
			unsigned int	iPadding;
		};

		/* The analyzing state data. */
		struct _AnalyzingData
		{
			unsigned int	iCurrWaypoint;
		};

		/* Current state data. */
		union
		{
			_SearchData				searching;
			_SimplificationData	simplifying;
			_SmoothingData			smoothing;
			_AnalyzingData			analyzing;
		} data;

	};

	/*!	A Path in the Mesh.

		This is the abstract base class for all Path objects.
	*/
	class Path : public DeferredWorker
	{
	friend class Manager;
	friend class PathSegment;
	friend class PathConnection;
	public:
		Path( Manager *pManager, Mesh *pMesh );

		//! @note The major waypoint data should be part of any Path sub-class constructors.

		virtual	~Path( void );

		//! Initialize internal data.
		virtual bool				Initialize( void ) = 0;

		/*!	@name DeferredWorker Methods */
		/*! Determines whether there is more work to do on this path.
			@return <b>true</b> if there is more work to do, <b>false</b> otherwise.
		*/
		virtual bool				HasWorkToDo( void ) = 0;
		/*! Instructs the Path to do some more work.
			@param iSteps The number of '<i>work units</i>' to perform at this time.
			@return The number of '<i>work units</i>' used by the Path during this call.
		*/
		virtual unsigned int		DoWork( unsigned int iSteps ) = 0;

		/*! Create a PathConnection object associated with this path. */
		/*! The PathConnection is where the path follower's state is maintained, while hiding the refinement process from the end-user.
			@return A pointer to a new PathConnection.  Call ReleaseConnection to properly dispose of the memory allocated during this call.
		*/
		virtual PathConnection	*GetConnection( void ) = 0;

		/*! Uninitializes and deallocates the given PathConnection.
			@param pConnection A pointer to a PathConnection object obtained by calling GetConnection.
		*/
		virtual void				ReleaseConnection( PathConnection *pConnection ) = 0;

		/*! Return the number of current connections.
			@return The number of currently active connections.
		*/
		virtual unsigned int		GetConnectionCount( void ) = 0;

		/*! Get the number of PathSegment objects in this Path.
			@return The number of PathSegment object in this Path.
		*/
		virtual unsigned int		GetPathSegmentCount( void ) = 0;

		/*! Get a pointer to a specific PathSegment object.
			@param iPathSegment The index of the PathSegment of interest.
			@return A pointer to the requested PathSegment.
		*/
		virtual PathSegment		*GetPathSegment( unsigned int iPathSegment ) = 0;

		/*! Reverse the path.
		*/
		virtual void				Reverse( void ) = 0;

		/*! length of path
		*/
		float							Length( void );

#if defined( NAVIGATION_DEBUGGING )
		/*! Draw the path as a series of lines.
		*/
		virtual void				DrawPath( const ts_fRGB &fStartColor, const ts_fRGB &fStopColor ) = 0;
#endif
		//! @todo: need to add event notification when a waypoint is crossed

		//! @todo: need event notification when the end of the path is reached

	protected:
		/*! The manager that owns this path.
		*/
		Manager						*m_pManager;

		/*! The mesh for this path.
		*/
		Mesh							*m_pMesh;

		/*! Defer the creation of the path segments to the manager, to avoid reallocating memory as much as possible.
		*/
		virtual PathSegment*		AquireSegment( void );
		virtual void				ReleaseSegment( PathSegment *pSegment );
	};
}

#endif