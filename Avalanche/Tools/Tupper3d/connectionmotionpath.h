////////////////////////////////////////////////////////////////////////////
//
// ConnectionMotionPath
//
// Class deal with a motion path connection
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connectionmotionpath.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CONNECTIONMOTIONPATH_H
#define TUPPER3D_CONNECTIONMOTIONPATH_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupConnectionMotionPath
{
	// Private Default constructor so no one can use it
	TupConnectionMotionPath(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		ANIMATED_NODE_REFERENCES,
		PATH_NODE_REFERENCE,
		WORLD_UP_VECTOR,
		WORLD_UP_NODE_REFERENCE,
		FRONT_TWIST,
		UP_TWIST,
		SIDE_TWIST,
		BANK_SCALE,
		BANK_LIMIT,
		U_VALUE,
		ANIMATED_CHANNELS,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum NodeFlags
	{
		FLAGS_PARAMETRIC_LENGTH							= 1<<0,
		FLAGS_FOLLOW										= 1<<1,
		FLAGS_UP_AXIS_MASK								= 3<<2,
		FLAGS_UP_AXIS_X									= 0<<2,
		FLAGS_UP_AXIS_Y									= 1<<2,
		FLAGS_UP_AXIS_Z									= 2<<2,
		FLAGS_FRONT_AXIS_MASK							= 3<<4,
		FLAGS_FRONT_AXIS_X								= 0<<4,
		FLAGS_FRONT_AXIS_Y								= 1<<4,
		FLAGS_FRONT_AXIS_Z								= 2<<4,
		FLAGS_WORLD_UP_TYPE_MASK						= 7<<6,
		FLAGS_WORLD_UP_TYPE_SCENE_UP					= 0<<6,
		FLAGS_WORLD_UP_TYPE_OBJECT_UP					= 1<<6,
		FLAGS_WORLD_UP_TYPE_OBJECT_ROTATION_UP		= 2<<6,
		FLAGS_WORLD_UP_TYPE_VECTOR						= 3<<6,
		FLAGS_WORLD_UP_TYPE_NORMAL						= 4<<6,
		FLAGS_INVERSE_UP									= 1<<9,
		FLAGS_INVERSE_FRONT								= 1<<10,
		FLAGS_BANK											= 1<<11,
	};

	TupConnectionMotionPath(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareList *AddAnimatedNodeReferences(const int *pRefs,int numRefs);
	TupperwareScalar *AddPathNodeReference(int pathNodeReference);
	TupperwareList *AddWorldUpVector(const float *pVector);
	TupperwareScalar *AddWorldUpNodeReference(int nodeReference);
	TupperwareScalar *AddFrontTwist(float frontTwist);
	TupperwareScalar *AddUpTwist(float upTwist);
	TupperwareScalar *AddSideTwist(float sideTwist);
	TupperwareScalar *AddBankScale(float bankScale);
	TupperwareScalar *AddBankLimit(float bankLimit);
	TupperwareScalar *AddUValue(float uValue);
	TupperwareAggregate *AddAnimatedChannels(void);
	TupperwareScalar *AddFlags(int flags);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareList *GetAnimatedNodeReferences(int **pRefs,int &numRefs);
	TupperwareScalar *GetPathNodeReference(int &pathNodeReference);
	TupperwareList *GetWorldUpVector(float **pRefs);
	TupperwareScalar *GetWorldUpNodeReference(int &nodeReference);
	TupperwareScalar *GetFrontTwist(float &frontTwist);
	TupperwareScalar *GetUpTwist(float &upTwist);
	TupperwareScalar *GetSideTwist(float &sideTwist);
	TupperwareScalar *GetBankScale(float &bankScale);
	TupperwareScalar *GetBankLimit(float &bankLimit);
	TupperwareScalar *GetUValue(float &uValue);
	TupperwareAggregate *GetAnimatedChannels(void);
	TupperwareScalar *GetFlags(int &flags);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareScalar *AddFloatScalar(float value,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareAggregate *AddAggregate(TupConnectionMotionPath::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareScalar *GetFloatScalar(float &value,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupConnectionMotionPath::ChildrenAtoms index);
	TupperwareAggregate *GetAggregate(TupConnectionMotionPath::ChildrenAtoms index);
};


#endif
