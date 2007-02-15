////////////////////////////////////////////////////////////////////////////
//
// SkinModifierBone
//
// Class for handling a single bone within the skin modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifierbone.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added correct matrix lists
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SKIN_MODIFIER_BONE_H
#define TUPPER3D_SKIN_MODIFIER_BONE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;
class TupperwareList;

class TupObject;

class TupSkinModifierBone
{
	// Private Default constructor so no one can use it
	TupSkinModifierBone(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FLAGS,
		NODE_REFERENCE,
		BONE_INIT_OBJECT_TM,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum BoneFlags
	{
		ABSOLUTE_WEIGHTS			= 1<<0,
		IS_SPLINE					= 1<<1,
		IS_CLOSED_SPLINE			= 1<<2,
		DRAW_ENVELOPE				= 1<<3,
		BONE_FLAG					= 1<<4,
		BONE_DEAD					= 1<<5,
	};

	TupSkinModifierBone(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
  
	TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddName(const char *pName);
	TupperwareScalar *AddFlags(int flags);
	TupperwareScalar *AddNodeReference(int nodeReference);
	TupperwareList *AddBoneInitObjectTM(const float *pValue);

// get methods
	TupperwareScalar *GetName(char **pName);
	TupperwareScalar *GetFlags(int &flags);
	TupperwareScalar *GetNodeReference(int &nodeReference);
	TupperwareList *GetBoneInitObjectTM(float **pValue);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupSkinModifierBone::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupSkinModifierBone::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupSkinModifierBone::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupSkinModifierBone::ChildrenAtoms index);
};

#endif
