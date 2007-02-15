////////////////////////////////////////////////////////////////////////////
//
// FaceAttributesModifier
//
// Class to handle the Face Attributes modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: faceattributesmodifier.h $
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
 * User: Adam Clayton Date: 3/19/02    Time: 3:09p
 * Updated in $/Avalanche/tools/Tupper3d
 * Face Attributes
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_FACE_ATTRIBUTES_MODIFIER_H
#define TUPPER3D_FACE_ATTRIBUTES_MODIFIER_H

class TupModifier;

class TupFaceAttributesModifier
{
	// Private Default constructor so no one can use it
	TupFaceAttributesModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		NAME,
		FLAGS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:
	enum FaceAttributesModifierFlags
	{
		FLAGS_ENABLED			= 1<<0,
	};

	TupFaceAttributesModifier(TupperwareAggregate *pObject);
	TupFaceAttributesModifier(TupModifier &modifierBase);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareScalar *AddName(const char *name);
	TupperwareScalar *AddFlags(int value);

// get methods
	TupperwareScalar *GetName(char **name);
	TupperwareScalar *GetFlags(int &value);

private:
// add methods
	TupperwareScalar *AddIntScalar(int value,TupFaceAttributesModifier::ChildrenAtoms index);

// get methods
	TupperwareScalar *GetIntScalar(int &value,TupFaceAttributesModifier::ChildrenAtoms index);
};

#endif
