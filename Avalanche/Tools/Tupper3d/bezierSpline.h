////////////////////////////////////////////////////////////////////////////
//
// BezierSpline
//
// Class for handling a single bezier spline
//
////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_BEZIER_SPLINE_H
#define TUPPER3D_BEZIER_SPLINE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupObject;

class TupBezierSpline
{
	// Private Default constructor so no one can use it
	TupBezierSpline(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		POINT_LOCATIONS,
		IN_VECTORS,
		OUT_VECTORS,
		POINT_FLAGS,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum SplineFlags
	{
		FLAGS_CLOSED										= 1<<0
	};

	enum SplinePointFlags
	{
		FLAGS_KNOT_TYPE_MASK								= 3<<0,
		FLAGS_KNOT_TYPE_AUTO								= 0<<0,
		FLAGS_KNOT_TYPE_CORNER								= 1<<0,
		FLAGS_KNOT_TYPE_BEZIER								= 2<<0,
		FLAGS_KNOT_TYPE_BEZIER_CORNER						= 3<<0,
		FLAGS_LINE_TYPE_MASK								= 1<<2,
		FLAGS_LINE_TYPE_CURVE								= 0<<2,
		FLAGS_LINE_TYPE_LINE								= 1<<2,
	};

	TupBezierSpline(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

	TupperwareAggregate *GetBaseAggregate( void );

	// add methods
	TupperwareList *AddPointLocations(const float *pPointLocations,int numPoints);
	TupperwareList *AddInVectors(const float *pInVectors,int numPoints);
	TupperwareList *AddOutVectors(const float *pOutVectors,int numPoints);
	TupperwareList *AddPointFlags(const int *pPointFlags,int numPoints);
	TupperwareScalar *AddFlags(int flags);

	// get methods
	TupperwareList *GetPointLocations(float **pPointLocations,int &numPoints);
	TupperwareList *GetInVectors(float **pInVectors,int &numPoints);
	TupperwareList *GetOutVectors(float **pOutVectors,int &numPoints);
	TupperwareList *GetPointFlags(int **pPointFlags,int &numPoints);
	TupperwareScalar *GetFlags(int &flags);

private:
	// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupBezierSpline::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupBezierSpline::ChildrenAtoms index);

	// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupBezierSpline::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupBezierSpline::ChildrenAtoms index);
};

#endif
