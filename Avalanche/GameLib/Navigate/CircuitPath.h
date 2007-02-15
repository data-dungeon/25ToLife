/********************************************************************
created:	2004/02/18
created:	18:2:2004   13:50
filename: 	C:\dev\avalanche\gamelib\Navigate\CircuitPath.h
file path:	C:\dev\avalanche\gamelib\Navigate
file base:	CircuitPath
file ext:	h
author:		Shon C. Love

purpose:	Interface for a circuit path object
*********************************************************************/
#if !defined( _CIRCUITPATH_H )
#define _CIRCUITPATH_H

#include "Navigate/NavPath.h"
#include "Navigate/OneWayPath.h"

namespace Navigate
{
	class CircuitPath : public OneWayPath
	{
		friend class Manager;
		friend class PathSegment;
		friend class PathConnection;
		friend class CircuitPathConnection;

	public:
		CircuitPath( Manager *pManager, Mesh *pMesh, unsigned int iWaypointCount, const Vector3 **pPoints );
		virtual ~CircuitPath( void );

		PathConnection* CreateNewConnection( void );
	};



	// the kind of connection the onewaypath presents
	class CircuitPathConnection : public OneWayPathConnection
	{
		friend class PathSegment;
		friend class Path;
		friend class CircuitPath;
	public:

	protected:
		// only the Path may create PathConnections
		CircuitPathConnection( void );
		CircuitPathConnection( PathConnection &copy );
		CircuitPathConnection( CircuitPath *pPath );

		virtual bool OnPathEndReached( void );
	};
}

#endif	// _CIRCUITPATH_H
