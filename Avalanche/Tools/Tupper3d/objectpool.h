////////////////////////////////////////////////////////////////////////////
//
// ObjectPool
//
// Class to deal with the object pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: objectpool.h $
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 5/24/02    Time: 3:16p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added a delete function
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/22/02    Time: 12:54p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added class references for import export
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 5  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 4:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * GetNumObjects
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_OBJECTPOOL_H
#define TUPPER3D_OBJECTPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupObjectPool
{
	// Private Default constructor so no one can use it
	TupObjectPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pObjectPool; // the pool that knows about all the child atoms

public:
	TupObjectPool(TupperwareAggregate *pObject); // use this one
	~TupObjectPool();

// add methods
	int AddObjectAggregate(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddObjectAggregate(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetObjectAggregate(int index); // return the aggregate using a integer index
	int GetNumObjects(void);

// delete methods
	void DeleteObjectAggregate(int index); // remove and delete the aggregate from the pool
};


#endif
