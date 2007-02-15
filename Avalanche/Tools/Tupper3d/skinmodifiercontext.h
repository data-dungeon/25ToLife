////////////////////////////////////////////////////////////////////////////
//
// SkinModifierContext
//
// Class for handling a context for a skin modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifiercontext.h $
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

#ifndef TUPPER3D_SKIN_MODIFIER_CONTEXT_H
#define TUPPER3D_SKIN_MODIFIER_CONTEXT_H


class TupSkinModifierContext
{
	// Private Default constructor so no one can use it
	TupSkinModifierContext(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		VERTEX_POOL,
		BASE_TM,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	TupSkinModifierContext(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareAggregate *AddVertexPool(void);
	TupperwareList *AddBaseTM(const float *pValue);

// get methods
	TupperwareAggregate *GetVertexPool(void);
	TupperwareList *GetBaseTM(float **pValue);

// private methods
private:
	TupperwareAggregate *AddAggregate(TupSkinModifierContext::ChildrenAtoms index);
	TupperwareAggregate *GetAggregate(TupSkinModifierContext::ChildrenAtoms index);
	TupperwareList *AddFloatList(const float *pfloats,int numFloats,TupSkinModifierContext::ChildrenAtoms index);
	TupperwareList *GetFloatList(float **pFloats,int &numFloats,TupSkinModifierContext::ChildrenAtoms index);
};

#endif
