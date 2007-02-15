////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterialSubMaterial
//
// Class to handle materials that a multi sub object material points to
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: multisubobjectmaterialsubmaterial.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:18p
 * Updated in $/Avalanche/tools/Tupper3d
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MULTI_SUB_OBJECT_SUB_MATERIAL_H
#define TUPPER3D_MULTI_SUB_OBJECT_SUB_MATERIAL_H

class TupMultiSubObjectMaterialSubMaterial
{
	// Private Default constructor so no one can use it
	TupMultiSubObjectMaterialSubMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		SLOT_NAME,
		FLAGS,
		MATERIAL_REFERENCE,
		NUM_CHILDREN_ATOMS, // last entry
	};
// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		ENABLED				= 1<<0,
	};

	TupMultiSubObjectMaterialSubMaterial(TupperwareAggregate *pObject);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddMaterialReference(int materialIndex);
	TupperwareScalar *AddSlotName(const char *name);
	TupperwareScalar *AddFlags(int value);


// get methods
	int GetSubMaterialIndex(void);
	TupperwareScalar *GetMaterialReference(int &materialIndex);
	TupperwareScalar *GetSlotName(char **name);
	TupperwareScalar *GetFlags(int &value);
};

#endif
