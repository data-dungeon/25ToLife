////////////////////////////////////////////////////////////////////////////
//
// MapPool
//
// Class to deal with the map pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: mappool.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete methods
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MAPPOOL_H
#define TUPPER3D_MAPPOOL_H

class TupperwarePoolValueString;

class TupMapPool
{
	// Private Default constructor so no one can use it
	TupMapPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pMapPool; // the pool that knows about all the child atoms

public:
	TupMapPool(TupperwareAggregate *pObject); // use this one
	~TupMapPool();

// add methods
	int AddMap(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddMap(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetMap(int index); // return the aggregate using a integer index
	int GetNumMaps(void);

// delete methods
	void DeleteMap(int index); // remove and delete the aggregate from the pool

};


#endif
