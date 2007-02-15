////////////////////////////////////////////////////////////////////////////
//
// Lookat
//
// Class for Lookats
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: lookat.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 6/09/03    Time: 2:55p
 * Updated in $/Avalanche/tools/Tupper3d
 * camera lookat node references
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 6/06/03    Time: 4:06p
 * Updated in $/Avalanche/tools/Tupper3d
 * instanced clip curves
 * camera lookat
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_LOOKAT_H
#define TUPPER3D_LOOKAT_H

class TupObject;

class TupLookAt
{
	// Private Default constructor so no one can use it
	TupLookAt(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		DISTANCE_BETWEEN,
		TWIST,
		WORLD_UP_VECTOR,
		AIM_VECTOR,
		UP_VECTOR,
		CAMERA_NODE_REFERENCE,
		AIM_NODE_REFERENCE,
		UP_NODE_REFERENCE,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum Flags
	{
		FLAGS_WORLD_UP_TYPE_MASK						= 7<<0,
		FLAGS_WORLD_UP_TYPE_SCENE_UP					= 0<<0,
		FLAGS_WORLD_UP_TYPE_OBJECT_UP					= 1<<0,
		FLAGS_WORLD_UP_TYPE_OBJECT_ROTATION_UP		= 2<<0,
		FLAGS_WORLD_UP_TYPE_VECTOR						= 3<<0,
		FLAGS_WORLD_UP_TYPE_NONE						= 4<<0,
		FLAGS_DISPLAY_CONNECTOR							= 1<<3,
	};

	TupLookAt(TupperwareAggregate *pObject);
	TupLookAt(TupObject &objectBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddDistanceBetween(float value);
	TupperwareScalar *AddTwist(float value);
	TupperwareList *AddWorldUpVector(float *pValues);
	TupperwareList *AddAimVector(float *pValues);
	TupperwareList *AddUpVector(float *pValues);
	TupperwareScalar *AddCameraNodeReference(int ref);
	TupperwareScalar *AddUpNodeReference(int ref);
	TupperwareScalar *AddAimNodeReference(int ref);
	TupperwareScalar *AddFlags(int value);
	
	// get methods
	TupperwareScalar *GetDistanceBetween(float &value);
	TupperwareScalar *GetTwist(float &value);
	TupperwareList *GetWorldUpVector(float **pValues);
	TupperwareList *GetAimVector(float **pValues);
	TupperwareList *GetUpVector(float **pValues);
	TupperwareScalar *GetCameraNodeReference(int &ref);
	TupperwareScalar *GetUpNodeReference(int &ref);
	TupperwareScalar *GetAimNodeReference(int &ref);
	TupperwareScalar *GetFlags(int &value);
	
private:
	// add methods
	TupperwareScalar *AddFloatScalar(float value,TupLookAt::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupLookAt::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupLookAt::ChildrenAtoms index);
	
	// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupLookAt::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupLookAt::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pfloats,int &numFloats,TupLookAt::ChildrenAtoms index);
};

#endif