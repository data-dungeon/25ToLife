////////////////////////////////////////////////////////////////////////////
//
// AnimationSetPool
//
// Class to deal with the animation sets
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSetPool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMSETPOOL_H
#define TUPPER3D_ANIMSETPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimSetPool
{
	// Private Default constructor so no one can use it
	TupAnimSetPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAnimSetPool; // the pool that knows about all the child atoms

public:
	TupAnimSetPool(TupperwareAggregate *pObject); // use this one
	~TupAnimSetPool();

// add methods
	int AddAnimSet(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAnimSet(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAnimSet(int index); // return the aggregate using a integer index
	int GetNumAnimSets(void);

// delete methods
	void DeleteAnimSet(int index); // remove and delete the aggregate from the pool
};


#endif
