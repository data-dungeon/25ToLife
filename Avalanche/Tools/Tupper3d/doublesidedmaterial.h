////////////////////////////////////////////////////////////////////////////
//
// DoubleSidedMaterial
//
// Class to handle a double sided material
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: doublesidedmaterial.h $
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

#ifndef TUPPER3D_DOUBLE_SIDED_MATERIAL_H
#define TUPPER3D_DOUBLE_SIDED_MATERIAL_H

class TupMaterial;

class TupDoubleSidedMaterial
{
	// Private Default constructor so no one can use it
	TupDoubleSidedMaterial(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FRONT_MATERIAL_REFERENCE,
		BACK_MATERIAL_REFERENCE,
		FLAGS,
		TRANSLUCENCY,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum Flags
	{
		FRONT_MATERIAL_ENABLE	= 1<<0,
		BACK_MATERIAL_ENABLE	= 1<<1,
	};

	TupDoubleSidedMaterial(TupperwareAggregate *pObject);
	TupDoubleSidedMaterial(TupMaterial &materialBase);

	void BuildAtomCache(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddFrontMaterialReference(int value);
	TupperwareScalar *AddBackMaterialReference(int value);
	TupperwareScalar *AddFlags(int value);
	TupperwareScalar *AddTranslucency(float value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetFrontMaterialReference(int &value);
	TupperwareScalar *GetBackMaterialReference(int &value);
	TupperwareScalar *GetFlags(int &value);
	TupperwareScalar *GetTranslucency(float &value);

private:
// add methods
	TupperwareScalar *AddFloatScalar(float value,TupDoubleSidedMaterial::ChildrenAtoms index);
	TupperwareScalar *AddIntScalar(int value,TupDoubleSidedMaterial::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetFloatScalar(float &value,TupDoubleSidedMaterial::ChildrenAtoms index);
	TupperwareScalar *GetIntScalar(int &value,TupDoubleSidedMaterial::ChildrenAtoms index);
};

#endif
