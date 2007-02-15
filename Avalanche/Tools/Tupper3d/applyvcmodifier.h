////////////////////////////////////////////////////////////////////////////
//
// ApplyVCModifier
//
// Class to handle the apply vc modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: applyvcmodifier.h $
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 1/28/03    Time: 12:08p
 * Updated in $/Avalanche/tools/Tupper3d
 * vertex alpha added to apply vc modifier
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 1:08p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply Vertex Color Modifiers to Derived objects
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 2/27/02    Time: 2:24p
 * Updated in $/Avalanche/tools/Tupper3d
 * Made all Add list functions pass const pointers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/03/02    Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * Vertex Paint Modifier (pass 1)
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/02/02    Time: 4:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply VC Modifier complete
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifiers
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_APPLY_VC_MODIFIER_H
#define TUPPER3D_APPLY_VC_MODIFIER_H

class TupModifier;

class TupApplyVCModifier
{
	// Private Default constructor so no one can use it
	TupApplyVCModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		COLOR_VERTS,
		COLOR_INDICES,
		ALPHA_VERTS,
		ALPHA_INDICES,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum ApplyVCModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
		FLAGS_MIXED_COLORS	= 1<<1,
		FLAGS_HAS_ALPHA		= 1<<2,
	};

	TupApplyVCModifier(TupperwareAggregate *pObject);
	TupApplyVCModifier(TupModifier &modifierBase);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareList *AddColorVerts(const float *pVerts,int numVerts);
	TupperwareList *AddColorIndices(const int *pIndices,int numIndices);
	TupperwareList *AddAlphaVerts(const float *pVerts,int numVerts);
	TupperwareList *AddAlphaIndices(const int *pIndices,int numIndices);
	TupperwareScalar *AddFlags(int value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareList *GetColorVerts(float **pVerts,int &numVerts);
	TupperwareList *GetColorIndices(int **pIndices,int &numIndices);
	TupperwareList *GetAlphaVerts(float **pVerts,int &numVerts);
	TupperwareList *GetAlphaIndices(int **pIndices,int &numIndices);
	TupperwareScalar *GetFlags(int &value);

private:
// add methods
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupApplyVCModifier::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupApplyVCModifier::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupApplyVCModifier::ChildrenAtoms index);

// get methods
	TupperwareList *GetFloatList(float **pfloats,int &numFloats,TupApplyVCModifier::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupApplyVCModifier::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupApplyVCModifier::ChildrenAtoms index);
};

#endif
