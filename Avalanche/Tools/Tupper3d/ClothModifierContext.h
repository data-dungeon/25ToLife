////////////////////////////////////////////////////////////////////////////
//
// ClothModifierContext
//
// Class to handle the Cloth modifier context
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: ClothModifierContext.h $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 7/22/02    Time: 4:21p
 * Updated in $/Avalanche/tools/Tupper3d
 * Changed connections to 1 list instead of 2
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 2:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * Cloth Modifier
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_CLOTH_MODIFIER_CONTEXT_H
#define TUPPER3D_CLOTH_MODIFIER_CONTEXT_H

class TupClothModifierContext
{
	// Private Default constructor so no one can use it
	TupClothModifierContext(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		POINT_FLAGS,
		CONNECTION_POINTS,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	enum PointFlags
	{
		FLAGS_POINT_ACTIVE		= 0x0001,
		FLAGS_POINT_ATTACHED	= 0x0002,
		FLAGS_POINT_BACK		= 0x0004,
	};

	TupClothModifierContext(TupperwareAggregate *pObject);

	void BuildAtomCache(void);
	TupperwareAggregate *GetBaseAggregate(void);

// add methods
	TupperwareList *AddPointFlags(const int *pValues,int numPoints);
	TupperwareList *AddConnectionPoints(const int *pValues,int numConnections); // two ints per connections

// get methods

	TupperwareList *GetPointFlags(int **pValues,int &numPoints);
	TupperwareList *GetConnectionPoints(int **pValues,int &numConnections); // two ints per connections

private:
// add methods
	TupperwareList *AddIntList(const int *pInts,int numInts,TupClothModifierContext::ChildrenAtoms index);

// get methods
	TupperwareList *GetIntList(int **pInts,int &numInts,TupClothModifierContext::ChildrenAtoms index);
};

#endif
