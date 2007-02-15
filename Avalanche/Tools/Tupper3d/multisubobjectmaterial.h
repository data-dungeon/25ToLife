////////////////////////////////////////////////////////////////////////////
//
// MultiSubObjectMaterial
//
// Class to handle a multi material (has sub materials)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: multisubobjectmaterial.h $
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:18p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MULTI_SUB_OBJECT_MATERIAL_H
#define TUPPER3D_MULTI_SUB_OBJECT_MATERIAL_H

class TupMaterial;

class TupMultiSubObjectMaterial
{
	// Private Default constructor so no one can use it
	TupMultiSubObjectMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		SUB_MATERIALS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupMultiSubObjectMaterial(TupperwareAggregate *pObject);
	TupMultiSubObjectMaterial(TupMaterial &materialBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareAggregate *AddSubMaterial(int index);

// get methods
	TupperwareScalar *GetName(char **name);
	int GetNumSubMaterials(void);
	TupperwareAggregate *GetSubMaterial(int index);
};

#endif
