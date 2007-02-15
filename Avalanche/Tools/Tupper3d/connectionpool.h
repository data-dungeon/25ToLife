////////////////////////////////////////////////////////////////////////////
//
// ConnectionPool
//
// Class to deal with the connectionPools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connectionpool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CONNECTIONPOOL_H
#define TUPPER3D_CONNECTIONPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupConnectionPool
{
	// Private Default constructor so no one can use it
	TupConnectionPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pConnectionPool; // the pool that knows about all the child atoms

public:
	TupConnectionPool(TupperwareAggregate *pObject); // use this one
	~TupConnectionPool();

// add methods
	int AddConnection(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddConnection(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetConnection(int index); // return the aggregate using a integer index
	int GetNumConnections(void);

// delete methods
	void DeleteConnection(int index); // remove and delete the aggregate from the pool
};


#endif
