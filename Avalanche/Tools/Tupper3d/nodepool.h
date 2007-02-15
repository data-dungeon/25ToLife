
////////////////////////////////////////////////////////////////////////////
//
// NodePool
//
// Class to deal with the node pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: nodepool.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * added remove function
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_NODEPOOL_H
#define TUPPER3D_NODEPOOL_H

class TupperwarePoolValueString;

class TupNodePool
{
	// Private Default constructor so no one can use it
	TupNodePool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pNodePool; // the pool that knows about all the child atoms

public:
	TupNodePool(TupperwareAggregate *pObject); // use this one
	~TupNodePool();

// add methods
	int AddNode(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddNode(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetNode(int index); // return the aggregate using a integer index
	int GetNumNodes(void);

// delete methods
	void DeleteNode(int index); // remove and delete the aggregate from the pool

};


#endif
