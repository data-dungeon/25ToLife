////////////////////////////////////////////////////////////////////////////
//
// FaceAttributesModifierContext
//
// Class to handle the Face Attributes modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: faceattributesmodifiercontext.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 4/24/02    Time: 11:06a
 * Updated in $/Avalanche/tools/Tupper3d
 * Removed STL usage from Project
 * Added TupArray and TupString classes to replace std::vector and
 * std::string
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

#ifndef TUPPER3D_FACE_ATTRIBUTES_MODIFIER_CONTEXT_H
#define TUPPER3D_FACE_ATTRIBUTES_MODIFIER_CONTEXT_H

class TupFaceAttributesModifierContext
{
	// Private Default constructor so no one can use it
	TupFaceAttributesModifierContext(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		FACE_ATTRIBUTE_VERTS,
		FACE_ATTRIBUTE_INDICES,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupFaceAttributesModifierContext(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareList *AddFaceAttributeVerts(const char **pValues,int numVerts);
	TupperwareList *AddFaceAttributeVerts(const TupArray<TupString> &strings);
	TupperwareList *AddFaceAttributeIndices(const int *pValues,int numIndices);

// get methods
	TupperwareList *GetFaceAttributeVerts(char ***pValues,int &numVerts);
	TupperwareList *GetFaceAttributeVerts(TupArray<TupString> &strings);
	TupperwareList *GetFaceAttributeIndices(int **pValues,int &numIndices);

private:
// add methods
	TupperwareList *AddStringList(const char **pStrings,int numStrings,TupFaceAttributesModifierContext::ChildrenAtoms index);
	TupperwareList *AddStringList(const TupArray<TupString> &strings,TupFaceAttributesModifierContext::ChildrenAtoms index);
	TupperwareList *AddIntList(const int *pInts,int numInts,TupFaceAttributesModifierContext::ChildrenAtoms index);

// get methods
	TupperwareList *GetStringList(char ***pStrings,int &numStrings,TupFaceAttributesModifierContext::ChildrenAtoms index);
	TupperwareList *GetStringList(TupArray<TupString> &strings,TupFaceAttributesModifierContext::ChildrenAtoms index);
	TupperwareList *GetIntList(int **pInts,int &numInts,TupFaceAttributesModifierContext::ChildrenAtoms index);
};

#endif
