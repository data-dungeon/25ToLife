/********************************************************************
	created:	2004/01/26
	created:	26:1:2004   12:37
	filename: 	c:\dev\avalanche\gamelib\Navigate\OneWayPath.h
	file path:	c:\dev\avalanche\gamelib\Navigate
	file base:	OneWayPath
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Implement a one-way path.
*********************************************************************/

#if !defined( _ONEWAYPATH_H )
#define _ONEWAYPATH_H

#include "Navigate/NavPath.h"

namespace Navigate
{
	class OneWayPath : public Path
	{
		friend class Manager;
		friend class PathSegment;
		friend class PathConnection;
		friend class OneWayPathConnection;

	public:
		OneWayPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints );
		virtual ~OneWayPath( void );

		virtual bool			Initialize( void );

		virtual bool			HasWorkToDo( void );
		virtual unsigned int	DoWork( unsigned int iSteps = 10 );

		virtual PathConnection	*GetConnection( void );
		virtual unsigned int	GetConnectionCount( void );
		virtual void			ReleaseConnection( PathConnection *pConnection );

		virtual unsigned int	GetPathSegmentCount( void );
		virtual PathSegment		*GetPathSegment( unsigned int iPathSegment );

		virtual void			Reverse( void );

#if defined( NAVIGATION_DEBUGGING )
		virtual void			DrawPath( const ts_fRGB &fStartColor, const ts_fRGB &fStopColor );
#endif

	protected:
		virtual PathConnection	*CreateNewConnection( void );

		DynamicArray< PathSegment* >		m_Segments;
		DynamicArray< PathConnection* >	m_Connections;
	};




	// the kind of connection the onewaypath presents
	class OneWayPathConnection : public PathConnection
	{
		friend class PathSegment;
		friend class Path;
		friend class OneWayPath;
	public:

		virtual PathConnection::Status	GetStatus( void );
		virtual bool					MoveAlongPath( float fDistance, float &fDistanceMoved );
		virtual bool					MoveToNextWaypoint( void );
		virtual Vector3					GetPosition( void );
		virtual Vector3					GetDirection( void );
		virtual Vector3					GetDirectionNormal( void );
		virtual Vector3					GetNextWaypoint( void );
		virtual Vector3					ConnectNearest( const Vector3 vPoint );
		virtual void					Reverse( void );
		virtual void					Release( bool bReleaseUnusedPath = true );


	protected:
		// only the Path may create PathConnections
		OneWayPathConnection( void );
		OneWayPathConnection( PathConnection &copy );
		OneWayPathConnection( OneWayPath *pPath );

		//! A way to alert child classes that the end of the path has been reached
		//! @return Wether path traversal should continue
		virtual bool	OnPathEndReached( void );

		bool			m_bAtEndOfPath;
		bool			m_bPositionDirty;
		bool			m_bDirectionDirty;
		bool			m_bDirectionNormalDirty;

		Vector3			m_vPosition;
		Vector3			m_vDirection;
		Vector3			m_vDirectionNormal;

		OneWayPath		*m_pPath;
		unsigned int	m_iCurrentSegment;	// [ 0 .. ( m_pPath->m_Segments.Count() - 1 ) ]
		unsigned int	m_iCurrentWaypoint;	// [ 0 .. ( m_pPath->m_Segments[ iCurrentSegment ].m_Waypoints.Count() - 1 ) ]
		float			m_fCurrentT;		// current distance along the current waypoint segment ( 0.0f .. 1.0f )
	};
}

#endif	// _ONEWAYPATH_H
