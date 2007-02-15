////////////////////////////////////////////////////////////////////////////
//
// Connection
//
// Handle the types of connections
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connection.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CONNECTION_H
#define TUPPER3D_CONNECTION_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupConnection
{
	// Private Default constructor so no one can use it
	TupConnection(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:
	enum ConnectionTypes
	{
		MOTION_PATH,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

	TupConnection(TupperwareAggregate *pObject);

// add methods
	void SetConnectionType(TupConnection::ConnectionTypes type);

// get methods
	TupperwareAggregate *GetBaseAggregate(void);
	TupConnection::ConnectionTypes GetConnectionType(void);
};


#endif
