////////////////////////////////////////////////////////////////////////////
//
// Bone
//
// Class to handle the bone objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: Bone.h $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/18/02    Time: 2:11p
 * Updated in $/Avalanche/tools/Tupper3d
 * Bone Export Complete
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_BONE_H
#define TUPPER3D_BONE_H

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

class TupObject;

class TupBone
{
	// Private Default constructor so no one can use it
	TupBone(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		WIDTH,
		HEIGHT,
		TAPER,
		LENGTH,
		FLAGS,
		SIDE_FINS_SIZE,
		SIDE_FINS_START_TAPER,
		SIDE_FINS_END_TAPER,
		FRONT_FIN_SIZE,
		FRONT_FIN_START_TAPER,
		FRONT_FIN_END_TAPER,
		BACK_FIN_SIZE,
		BACK_FIN_START_TAPER,
		BACK_FIN_END_TAPER,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum Flags
	{
		SIDE_FINS,
		FRONT_FIN,
		BACK_FIN,
		GEN_MAP,
	};

	TupBone(TupperwareAggregate *pObject);
	TupBone(TupObject &objectBase);

	void BuildAtomCache(void);
  
  TupperwareAggregate *GetBaseAggregate( void );

// add methods
	TupperwareScalar *AddFlags(int flags);
	TupperwareScalar *AddWidth(float value);
	TupperwareScalar *AddHeight(float value);
	TupperwareScalar *AddTaper(float value);
	TupperwareScalar *AddLength(float value);
	TupperwareScalar *AddSideFinsSize(float value);
	TupperwareScalar *AddSideFinsStartTaper(float value);
	TupperwareScalar *AddSideFinsEndTaper(float value);
	TupperwareScalar *AddFrontFinSize(float value);
	TupperwareScalar *AddFrontFinStartTaper(float value);
	TupperwareScalar *AddFrontFinEndTaper(float value);
	TupperwareScalar *AddBackFinSize(float value);
	TupperwareScalar *AddBackFinStartTaper(float value);
	TupperwareScalar *AddBackFinEndTaper(float value);

// get methods
	TupperwareScalar *GetFlags(int &flags);
	TupperwareScalar *GetWidth(float &value);
	TupperwareScalar *GetHeight(float &value);
	TupperwareScalar *GetTaper(float &value);
	TupperwareScalar *GetLength(float &value);
	TupperwareScalar *GetSideFinsSize(float &value);
	TupperwareScalar *GetSideFinsStartTaper(float &value);
	TupperwareScalar *GetSideFinsEndTaper(float &value);
	TupperwareScalar *GetFrontFinSize(float &value);
	TupperwareScalar *GetFrontFinStartTaper(float &value);
	TupperwareScalar *GetFrontFinEndTaper(float &value);
	TupperwareScalar *GetBackFinSize(float &value);
	TupperwareScalar *GetBackFinStartTaper(float &value);
	TupperwareScalar *GetBackFinEndTaper(float &value);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupBone::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupBone::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupBone::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupBone::ChildrenAtoms index);
};

#endif
