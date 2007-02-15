////////////////////////////////////////////////////////////////////////////
//
// Derived
//
// Class for derived objects (objects that hold modifiers)
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: derived.h $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 8/08/02    Time: 9:36a
 * Updated in $/Avalanche/tools/Tupper3d
 * Modifier access using slots
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
 * User: Adam Clayton Date: 12/27/01   Time: 4:31p
 * Updated in $/Avalanche/tools/Tupper3d
 * xrefs and derived objects
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_DERIVED_H
#define TUPPER3D_DERIVED_H

class TupObject;

class TupDerived
{
	// Private Default constructor so no one can use it
	TupDerived(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

	enum ChildrenAtoms 
	{
		MODIFIERS,
		OBJECT_REFERENCE,
		NUM_CHILDREN_ATOMS, // last entry
	};

// cached atom pointers
	TupperwareAtom *m_cachedAtoms[NUM_CHILDREN_ATOMS];
	static char *m_keyNames[NUM_CHILDREN_ATOMS]; // names for the children atoms

public:

	TupDerived(TupperwareAggregate *pObject);
	TupDerived(TupObject &objectBase);

	TupperwareAggregate *GetBaseAggregate( void );

	void BuildAtomCache(void);

// add methods
	TupperwareAggregate *AddModifier(int index);
	TupperwareScalar *AddObjectReference(int objectReference);

// get methods
	int GetNumModifierSlots(void);
	TupperwareAggregate *GetModifierBySlot(int slotIndex);
	TupperwareScalar *GetObjectReference(int &objectReference);

};

#endif
