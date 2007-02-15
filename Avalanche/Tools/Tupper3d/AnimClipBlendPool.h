////////////////////////////////////////////////////////////////////////////
//
// AnimationClipBlendPool
//
// Class to deal with a bunch of animation clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipBlendPool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCLIPBLENDPOOL_H
#define TUPPER3D_ANIMCLIPBLENDPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimClipBlendPool
{
	// Private Default constructor so no one can use it
	TupAnimClipBlendPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAnimClipBlendPool; // the pool that knows about all the child atoms

public:
	TupAnimClipBlendPool(TupperwareAggregate *pObject); // use this one
	~TupAnimClipBlendPool();

// add methods
	int AddAnimClipBlend(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAnimClipBlend(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAnimClipBlend(int index); // return the aggregate using a integer index
	int GetNumAnimClipBlends(void);

// delete methods
	void DeleteAnimClipBlend(int index); // remove and delete the aggregate from the pool
};


#endif
