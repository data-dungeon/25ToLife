/********************************************************************
	created:	2004/02/18
	created:	18:2:2004   13:50
	filename: 	C:\dev\avalanche\gamelib\Navigate\BackAndForthPath.h
	file path:	C:\dev\avalanche\gamelib\Navigate
	file base:	BackAndForthPath
	file ext:	h
	author:		Shon C. Love
	
	purpose:	Interface for a back-and-forth path object
*********************************************************************/
#if !defined( _BACKANDFORTHPATH_H )
#define _BACKANDFORTHPATH_H

#include "Navigate/NavPath.h"
#include "Navigate/OneWayPath.h"

namespace Navigate
{
	class BackAndForthPath : public OneWayPath
	{
		friend class Manager;
		friend class PathSegment;
		friend class PathConnection;
		friend class BackAndForthPathConnection;

	public:
		BackAndForthPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints );
		virtual ~BackAndForthPath( void );

	protected:
		virtual PathConnection	*CreateNewConnection( void );
	};




	// the kind of connection the onewaypath presents
	class BackAndForthPathConnection : public OneWayPathConnection
	{
		friend class PathSegment;
		friend class Path;
		friend class BackAndForthPath;
	public:

	protected:
		// only the Path may create PathConnections
		BackAndForthPathConnection( void );
		BackAndForthPathConnection( PathConnection &copy );
		BackAndForthPathConnection( BackAndForthPath *pPath );

		virtual bool	OnPathEndReached( void );

		bool			m_bReversed;
	};
}

#endif	// _BACKANDFORTHPATH_H
