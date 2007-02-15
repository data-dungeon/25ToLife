////////////////////////////////////////////////////////////////////////////
//
// AnimCurve
//
// Handle the animation curves
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimCurve.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/18/03    Time: 9:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * added rotation interpolation flags
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/14/03    Time: 10:53a
 * Updated in $/Avalanche/tools/Tupper3d
 * start of anim curve data
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIM_CURVE_H
#define TUPPER3D_ANIM_CURVE_H

class TupAnimCurve
{
	// Private Default constructor so no one can use it
	TupAnimCurve(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		START_TIME,
		END_TIME,
		INPUT_VALUES,
		OUTPUT_VALUES,
		FLAGS,
		KEY_FLAGS,
		IN_TANGENT_ANGLES,
		OUT_TANGENT_ANGLES,
		IN_TANGENT_WEIGHTS,
		OUT_TANGENT_WEIGHTS,
		IN_TANGENT_X,
		OUT_TANGENT_X,
		IN_TANGENT_Y,
		OUT_TANGENT_Y,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum CurveFlags
	{
		FLAGS_PRE_INFINITY_MASK										= 7<<0,

		FLAGS_PRE_INFINITY_CONSTANT								= 0<<0,
		FLAGS_PRE_INFINITY_LINEAR									= 1<<0,
		FLAGS_PRE_INFINITY_CYCLE									= 2<<0,
		FLAGS_PRE_INFINITY_CYCLE_RELATIVE						= 3<<0,
		FLAGS_PRE_INFINITY_OSCILLATE								= 4<<0,

		FLAGS_POST_INFINITY_MASK									= 7<<3,
		
		FLAGS_POST_INFINITY_CONSTANT								= 0<<3,
		FLAGS_POST_INFINITY_LINEAR									= 1<<3,
		FLAGS_POST_INFINITY_CYCLE									= 2<<3,
		FLAGS_POST_INFINITY_CYCLE_RELATIVE						= 3<<3,
		FLAGS_POST_INFINITY_OSCILLATE								= 4<<3,

		FLAGS_WEIGHTED_TANGENTS										= 1<<6,

		FLAGS_INPUT_TYPE_MASK										= 3<<7,

		FLAGS_INPUT_TYPE_TIME										= 0<<7,
		FLAGS_INPUT_TYPE_UNITLESS									= 1<<7,
		FLAGS_INPUT_TYPE_UNKNOWN									= 2<<7,

		FLAGS_OUTPUT_TYPE_MASK										= 7<<9,

		FLAGS_OUTPUT_TYPE_LINEAR									= 0<<9,
		FLAGS_OUTPUT_TYPE_ANGULAR									= 1<<9,
		FLAGS_OUTPUT_TYPE_TIME										= 2<<9,
		FLAGS_OUTPUT_TYPE_UNITLESS									= 3<<9,
      FLAGS_OUTPUT_TYPE_STRING                           = 4<<9,
		FLAGS_OUTPUT_TYPE_UNKNOWN									= 5<<9,

		FLAGS_ROTATION_INTERPOLATION_MASK						= 7<<12,
		FLAGS_ROTATION_INDEPENDENT_EULER							= 0<<12,
		FLAGS_ROTATION_SYNCRONIZED_EULER							= 1<<12,
		FLAGS_ROTATION_SYNCRONIZED_QUATERNION					= 2<<12,
		FLAGS_ROTATION_INDEPENDENT_QUATERNION					= 3<<12,
	};

	enum CurveKeyFlags
	{
		FLAGS_IN_TANGENT_TYPE_MASK									= 0xf<<0,

		FLAGS_IN_TANGENT_TYPE_GLOBAL								= 0<<0,
		FLAGS_IN_TANGENT_TYPE_FIXED								= 1<<0,
		FLAGS_IN_TANGENT_TYPE_LINEAR								= 2<<0,
		FLAGS_IN_TANGENT_TYPE_FLAT									= 3<<0,
		FLAGS_IN_TANGENT_TYPE_SMOOTH								= 4<<0,
		FLAGS_IN_TANGENT_TYPE_STEP									= 5<<0,
		FLAGS_IN_TANGENT_TYPE_SLOW									= 6<<0,
		FLAGS_IN_TANGENT_TYPE_FAST									= 7<<0,
		FLAGS_IN_TANGENT_TYPE_CLAMPED								= 8<<0,

		FLAGS_OUT_TANGENT_TYPE_MASK								= 0xf<<4,
		
		FLAGS_OUT_TANGENT_TYPE_GLOBAL								= 0<<4,
		FLAGS_OUT_TANGENT_TYPE_FIXED								= 1<<4,
		FLAGS_OUT_TANGENT_TYPE_LINEAR								= 2<<4,
		FLAGS_OUT_TANGENT_TYPE_FLAT								= 3<<4,
		FLAGS_OUT_TANGENT_TYPE_SMOOTH								= 4<<4,
		FLAGS_OUT_TANGENT_TYPE_STEP								= 5<<4,
		FLAGS_OUT_TANGENT_TYPE_SLOW								= 6<<4,
		FLAGS_OUT_TANGENT_TYPE_FAST								= 7<<4,
		FLAGS_OUT_TANGENT_TYPE_CLAMPED							= 8<<4,

		FLAGS_TANGENTS_LOCKED										= 1<<8,
		FLAGS_WEIGHTS_LOCKED											= 1<<9,
		FLAGS_IS_BREAKDOWN											= 1<<10,
	};


	TupAnimCurve(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	
	TupperwareAggregate *GetBaseAggregate( void );

// add methods

	TupperwareScalar *AddStartTime(float value);
	TupperwareScalar *AddEndTime(float value);
	TupperwareScalar *AddFlags(int value);
	TupperwareList *AddInputValues(const float *pValues,int numValues);
	TupperwareList *AddOutputValues(const float *pValues,int numValues);
	TupperwareList *AddKeyFlags(const int *pValues,int numValues);
	TupperwareList *AddInTangentAngles(const float *pValues,int numValues);
	TupperwareList *AddInTangentWeights(const float *pValues,int numValues);
	TupperwareList *AddOutTangentAngles(const float *pValues,int numValues);
	TupperwareList *AddOutTangentWeights(const float *pValues,int numValues);
	TupperwareList *AddInTangentX(const float *pValues,int numValues);
	TupperwareList *AddInTangentY(const float *pValues,int numValues);
	TupperwareList *AddOutTangentX(const float *pValues,int numValues);
	TupperwareList *AddOutTangentY(const float *pValues,int numValues);
	TupperwareList *AddOutputValues(const char **pStrings,int numValues);
   TupperwareList *AddOutputValues(const TupArray<TupString> &strings);

// get methods

	TupperwareScalar *GetStartTime(float &value);
	TupperwareScalar *GetEndTime(float &value);
	TupperwareScalar *GetFlags(int &value);
	TupperwareList *GetInputValues(float **pValues,int &numValues);
	TupperwareList *GetOutputValues(float **pValues,int &numValues);
	TupperwareList *GetKeyFlags(int **pValues,int &numValues);
	TupperwareList *GetInTangentAngles(float **pValues,int &numValues);
	TupperwareList *GetInTangentWeights(float **pValues,int &numValues);
	TupperwareList *GetOutTangentAngles(float **pValues,int &numValues);
	TupperwareList *GetOutTangentWeights(float **pValues,int &numValues);
	TupperwareList *GetInTangentX(float **pValues,int &numValues);
	TupperwareList *GetInTangentY(float **pValues,int &numValues);
	TupperwareList *GetOutTangentX(float **pValues,int &numValues);
	TupperwareList *GetOutTangentY(float **pValues,int &numValues);
   TupperwareList *GetOutputValues(char ***pStrings,int &numValues);
   TupperwareList *GetOutputValues(TupArray<TupString> &strings);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupAnimCurve::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimCurve::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupAnimCurve::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupAnimCurve::ChildrenAtoms index);
   TupperwareList *AddStringList(const char **pStrings,int numStrings,TupAnimCurve::ChildrenAtoms index);
   TupperwareList *AddStringList(const TupArray<TupString> &strings,TupAnimCurve::ChildrenAtoms index);
	
// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupAnimCurve::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimCurve::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupAnimCurve::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupAnimCurve::ChildrenAtoms index);
   TupperwareList *GetStringList(char ***pStrings,int &numStrings,TupAnimCurve::ChildrenAtoms index);
   TupperwareList *GetStringList(TupArray<TupString> &strings,TupAnimCurve::ChildrenAtoms index);
};

#endif
