////////////////////////////////////////////////////////////////////////////
//
// CompositeMaterial
//
// Class to handle a composite materials
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: compositematerial.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 1/16/02    Time: 3:44p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added constructors that use the parent as a parameter
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/13/01   Time: 10:56a
 * Updated in $/Avalanche/tools/Tupper3d
 * More Materials
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_COMPOSITE_MATERIAL_H
#define TUPPER3D_COMPOSITE_MATERIAL_H

class TupMaterial;

class TupCompositeMaterial
{
	// Private Default constructor so no one can use it
	TupCompositeMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		BASE_MATERIAL_REFERENCE,
		SUB_MATERIALS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupCompositeMaterial(TupperwareAggregate *pObject);
	TupCompositeMaterial(TupMaterial &materialBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddBaseMaterialReference(int value);
	TupperwareAggregate *AddSubMaterial(int index);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetBaseMaterialReference(int &value);
	int GetNumSubMaterials(void);
	TupperwareAggregate *GetSubMaterial(int index);
};

#endif
