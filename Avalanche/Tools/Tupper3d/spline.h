////////////////////////////////////////////////////////////////////////////
//
// Spline
//
// Class for handling a single spline
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: spline.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/29/02    Time: 4:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bezier shapes and splines
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SPLINE_H
#define TUPPER3D_SPLINE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupObject;

class TupSpline
{
	// Private Default constructor so no one can use it
	TupSpline(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		CONTROL_POINTS,
		KNOTS,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum SplineFlags
	{
		FLAGS_DEGREE_MASK									= 7<<0,
		FLAGS_DEGREE_ONE									= 1<<0,
		FLAGS_DEGREE_TWO									= 2<<0,
		FLAGS_DEGREE_THREE								= 3<<0,
		FLAGS_DEGREE_FOUR 								= 4<<0,
		FLAGS_DEGREE_FIVE									= 5<<0,
		FLAGS_DEGREE_SIX									= 6<<0,
		FLAGS_DEGREE_SEVEN								= 7<<0,

		FLAGS_RATIONAL										= 1<<3,

		FLAGS_FORM_MASK									= 3<<4,
		FLAGS_FORM_OPEN									= 0<<4,
		FLAGS_FORM_CLOSED									= 1<<4,
		FLAGS_FORM_PERIODIC								= 2<<4,

		FLAGS_TYPE_MASK									= 0xF<<8,
		FLAGS_TYPE_B_SPLINE								= 0<<8,
	};

	TupSpline(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
  
	TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareList *AddControlPoints(const float *pControlPoints,int numPoints);
	TupperwareList *AddKnots(const float *pKnots,int numKnots);
	TupperwareScalar *AddFlags(int flags);

// get methods
	TupperwareList *GetControlPoints(float **pControlPoints,int &numPoints);
	TupperwareList *GetKnots(float **pKnots,int &numKnots);
	TupperwareScalar *GetFlags(int &flags);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupSpline::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupSpline::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupSpline::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupSpline::ChildrenAtoms index);
};

#endif
