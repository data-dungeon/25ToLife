////////////////////////////////////////////////////////////////////////////
//
// SplineShape
//
// Class for handling spline shapes
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SPLINE_SHAPE_H
#define TUPPER3D_SPLINE_SHAPE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupObject;

class TupSplineShape
{
	// Private Default constructor so no one can use it
	TupSplineShape(void);

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
	TupSplineShape(TupperwareAggregate *pObject);
	TupSplineShape(TupObject &objectBase);

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
