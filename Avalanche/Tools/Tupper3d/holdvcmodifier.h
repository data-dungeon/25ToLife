////////////////////////////////////////////////////////////////////////////
//
// HoldVCModifier
//
// Class to handle the hold vc modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: holdvcmodifier.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 3:03p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added support for the HoldVC Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_HOLD_VC_MODIFIER_H
#define TUPPER3D_HOLD_VC_MODIFIER_H

class TupModifier;

class TupHoldVCModifier
{
	// Private Default constructor so no one can use it
	TupHoldVCModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		COLOR_VERTS,
		COLOR_INDICES,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum HoldVCModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
	};

	TupHoldVCModifier(TupperwareAggregate *pObject);
	TupHoldVCModifier(TupModifier &modifierBase);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareList *AddColorVerts(const float *pVerts,int numVerts);
	TupperwareList *AddColorIndices(const int *pVerts,int numFaces);
	TupperwareScalar *AddFlags(int value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareList *GetColorVerts(float **pVerts,int &numVerts);
	TupperwareList *GetColorIndices(int **pIndices,int &numFaces);
	TupperwareScalar *GetFlags(int &value);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupHoldVCModifier::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupHoldVCModifier::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupHoldVCModifier::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pfloats,int &numFloats,TupHoldVCModifier::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupHoldVCModifier::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupHoldVCModifier::ChildrenAtoms index);
};

#endif
