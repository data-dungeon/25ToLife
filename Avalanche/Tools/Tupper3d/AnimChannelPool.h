////////////////////////////////////////////////////////////////////////////
//
// AnimationSetPool
//
// Class to deal with the animation sets
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimChannelPool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCHANNELPOOL_H
#define TUPPER3D_ANIMCHANNELPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimChannelPool
{
	// Private Default constructor so no one can use it
	TupAnimChannelPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAnimChannelPool; // the pool that knows about all the child atoms

public:
	TupAnimChannelPool(TupperwareAggregate *pObject); // use this one
	~TupAnimChannelPool();

// add methods
	int AddAnimChannel(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAnimChannel(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAnimChannel(int index); // return the aggregate using a integer index
	int GetNumAnimChannels(void);

// delete methods
	void DeleteAnimChannel(int index); // remove and delete the aggregate from the pool
};


#endif
