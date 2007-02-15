////////////////////////////////////////////////////////////////////////////
//
// StaticMatrix
//
// Class that deals with a transform using a single matrix
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: staticmatrix.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/16/03    Time: 1:50p
 * Updated in $/Avalanche/tools/Tupper3d
 * added more data for matrix components
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_STATICMATRIX_H
#define TUPPER3D_STATICMATRIX_H

class TupStaticMatrix
{
	// Private Default constructor so no one can use it
	TupStaticMatrix(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the transformation

	enum ChildrenAtoms 
	{
		MATRIX,
		TRANSLATION,
		ROTATION,
		SCALE,
		SHEAR,
		SCALE_PIVOT,
		ROTATION_PIVOT,
		SCALE_PIVOT_TRANSLATION,
		ROTATION_PIVOT_TRANSLATION,
		ROTATION_ORIENTATION,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		FLAGS_ROTATION_ORDER_MASK				= 7<<0,

		FLAGS_ROTATION_ORDER_XYZ				= 0<<0,
		FLAGS_ROTATION_ORDER_YZX				= 1<<0,
		FLAGS_ROTATION_ORDER_ZXY				= 2<<0,
		FLAGS_ROTATION_ORDER_XZY				= 3<<0,
		FLAGS_ROTATION_ORDER_YXZ				= 4<<0,
		FLAGS_ROTATION_ORDER_ZYX				= 5<<0,
		FLAGS_ROTATION_ORDER_UNKNOWN			= 6<<0,

		FLAGS_INHERIT_TRANSFORM					= 1<<3,
	};
	TupStaticMatrix(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	
	TupperwareAggregate *GetBaseAggregate( void );


// add methods
	TupperwareList *AddMatrix(const float *pMatrix);

	TupperwareList *AddTranslation(const float *pValues);
	TupperwareList *AddRotation(const float *pValues);
	TupperwareList *AddScale(const float *pValues);
	TupperwareList *AddShear(const float *pValues);
	TupperwareList *AddScalePivot(const float *pValues);
	TupperwareList *AddRotationPivot(const float *pValues);
	TupperwareList *AddScalePivotTranslation(const float *pValues);
	TupperwareList *AddRotationPivotTranslation(const float *pValues);
	TupperwareList *AddRotationOrientation(const float *pValues);

	TupperwareScalar *AddFlags(int flags);

// get methods
	TupperwareList *GetMatrix(float **pMatrix);

	TupperwareList *GetTranslation(float **pValues);
	TupperwareList *GetRotation(float **pValues);
	TupperwareList *GetScale(float **pValues);
	TupperwareList *GetShear(float **pValues);
	TupperwareList *GetScalePivot(float **pValues);
	TupperwareList *GetRotationPivot(float **pValues);
	TupperwareList *GetScalePivotTranslation(float **pValues);
	TupperwareList *GetRotationPivotTranslation(float **pValues);
	TupperwareList *GetRotationOrientation(float **pValues);
	
	TupperwareScalar *GetFlags(int &flags);

private:
	// add methods
	TupperwareScalar *AddFloatScalar(float value,TupStaticMatrix::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupStaticMatrix::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupStaticMatrix::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupStaticMatrix::ChildrenAtoms index);
	
	// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupStaticMatrix::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupStaticMatrix::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupStaticMatrix::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupStaticMatrix::ChildrenAtoms index);


};

#endif
