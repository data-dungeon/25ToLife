////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContextVertex
//
// Class to handle a vertex within the skin modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontextvertex.h $
 * 
 * *****************  Version 1  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Created in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_SKIN_MODIFIER_CONTEXT_VERTEX_H
#define TUPPER3D_SKIN_MODIFIER_CONTEXT_VERTEX_H

class TupSkinModifierContextVertex
{
	// Private Default constructor so no one can use it
	TupSkinModifierContextVertex(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		BONES,
		WEIGHTS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupSkinModifierContextVertex(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareList *AddBones(const int *pValues,int numBones);
	TupperwareList *AddWeights(const float *pValues,int numBones);

// get methods
	TupperwareList *GetBones(int **pValues,int &numBones);
	TupperwareList *GetWeights(float **pValues,int &numBones);

private:
// add methods
	TupperwareList *AddIntList(const int *pInts,int numInts,TupSkinModifierContextVertex::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pFloats,int numFloats,TupSkinModifierContextVertex::ChildrenAtoms index);

// get methods
	TupperwareList *GetIntList(int **pInts,int &numInts,TupSkinModifierContextVertex::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupSkinModifierContextVertex::ChildrenAtoms index);
};

#endif
