////////////////////////////////////////////////////////////////////////////
//
// AnimSet
//
// Handle a animation set
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSet.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_ANIMSET_H
#define TUPPER3D_ANIMSET_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupAnimSet
{
	// Private Default constructor so no one can use it
	TupAnimSet(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:

	enum ChildrenAtoms 
	{
		TYPE,
		NAME,
		CHANNELS,
		INSTANCED_CLIPS,
		SOURCE_CLIPS,
		CLIP_BLENDS,
		FLAGS,
      BONEDIRECTION,
      ROLLDIRECTION,
      PARENTBONEDIRECTION,
      PARENTROLLDIRECTION,
      LOCALTOPARENTBINDMATRIX,
      JOINTORIENTATIONQUATERNION,
		NUM_CHILDREN_ATOMS, // last entry
	};
	// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

	enum SetTypes
	{
		GLOBAL,
		CHARACTER,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

   enum AnimSetFlags
   {
      FLAGS_EULER_ROTATION				= 0<<0,
      FLAGS_QUATERNION_ROTATION		= 1<<0,
   };

	TupAnimSet(TupperwareAggregate *pObject);
	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	void SetAnimSetType(TupAnimSet::SetTypes type);
	TupperwareScalar *AddName(const char *name);
	TupperwareAggregate *AddChannels(void);
	TupperwareAggregate *AddInstancedClips(void);
	TupperwareAggregate *AddSourceClips(void);
	TupperwareAggregate *AddClipBlends(void);
	TupperwareScalar *AddFlags(int flags);

   TupperwareList *AddBoneDirection(const float *pValues);
   TupperwareList *AddRollDirection(const float *pValues);
   TupperwareList *AddParentBoneDirection(const float *pValues);
   TupperwareList *AddParentRollDirection(const float *pValues);
	TupperwareAggregate *AddLocalToParentBindMatrix(void);
   TupperwareList *AddJointOrientationQuaternion(const float *pValues);

// get methods
	TupAnimSet::SetTypes GetAnimSetType(void);
	TupperwareScalar *GetName(char **name);
	TupperwareAggregate *GetChannels(void);
	TupperwareAggregate *GetInstancedClips(void);
	TupperwareAggregate *GetSourceClips(void);
	TupperwareAggregate *GetClipBlends(void);
	TupperwareScalar *GetFlags(int &flags);

   TupperwareList *GetBoneDirection(float **pValues);
   TupperwareList *GetRollDirection(float **pValues);
   TupperwareList *GetParentBoneDirection(float **pValues);
   TupperwareList *GetParentRollDirection(float **pValues);
   TupperwareAggregate *GetLocalToParentBindMatrix(void);
   TupperwareList *GetJointOrientationQuaternion(float **pValues);

private:
	// add methods
	TupperwareAggregate *AddAggregate(TupAnimSet::ChildrenAtoms index);
	TupperwareScalar *AddStringScalar(const char *pString,TupAnimSet::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupAnimSet::ChildrenAtoms index);
   TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupAnimSet::ChildrenAtoms index);
	
	// get methods
	TupperwareAggregate *GetAggregate(TupAnimSet::ChildrenAtoms index);
	TupperwareScalar *GetStringScalar(char **pString,TupAnimSet::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupAnimSet::ChildrenAtoms index);
   TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupAnimSet::ChildrenAtoms index);
};


#endif
