////////////////////////////////////////////////////////////////////////////
//
// AnimationClipPool
//
// Class to deal with a bunch of animation clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipPool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCLIPPOOL_H
#define TUPPER3D_ANIMCLIPPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimClipPool
{
	// Private Default constructor so no one can use it
	TupAnimClipPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAnimClipPool; // the pool that knows about all the child atoms

public:
	TupAnimClipPool(TupperwareAggregate *pObject); // use this one
	~TupAnimClipPool();

// add methods
	int AddAnimClip(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAnimClip(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAnimClip(int index); // return the aggregate using a integer index
	int GetNumAnimClips(void);

// delete methods
	void DeleteAnimClip(int index); // remove and delete the aggregate from the pool
};


#endif
