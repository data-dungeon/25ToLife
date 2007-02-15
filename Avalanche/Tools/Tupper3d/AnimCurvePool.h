////////////////////////////////////////////////////////////////////////////
//
// AnimationCurvePool
//
// Class to deal with a bunch of animation curves
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimCurvePool.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMCURVEPOOL_H
#define TUPPER3D_ANIMCURVEPOOL_H

class TupperwarePoolValueString;

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimCurvePool
{
	// Private Default constructor so no one can use it
	TupAnimCurvePool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pAnimCurvePool; // the pool that knows about all the child atoms

public:
	TupAnimCurvePool(TupperwareAggregate *pObject); // use this one
	~TupAnimCurvePool();

// add methods
	int AddAnimCurve(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddAnimCurve(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetAnimCurve(int index); // return the aggregate using a integer index
	int GetNumAnimCurves(void);

// delete methods
	void DeleteAnimCurve(int index); // remove and delete the aggregate from the pool
};


#endif
