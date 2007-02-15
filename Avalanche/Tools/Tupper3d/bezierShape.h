////////////////////////////////////////////////////////////////////////////
//
// BezierShape
//
// Class for handling bezier shapes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: bezierShape.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bezier shapes and splines
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_BEZIER_SHAPE_H
#define TUPPER3D_BEZIER_SHAPE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupObject;

class TupBezierShape
{
	// Private Default constructor so no one can use it
	TupBezierShape(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FLAGS,
		SPLINES,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupBezierShape(TupperwareAggregate *pObject);
	TupBezierShape(TupObject &objectBase);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddFlags(int flags);
	TupperwareAggregate *AddSpline(int index);

// get methods
	TupperwareScalar *GetFlags(int &flags);
	int GetNumSplines(void);
	TupperwareAggregate *GetSpline(int index);
};

#endif
