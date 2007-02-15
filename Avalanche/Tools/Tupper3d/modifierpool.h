////////////////////////////////////////////////////////////////////////////
//
// ModifierPool
//
// Class to deal with the modifier pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: modifierpool.h $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete method
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifiers
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MODIFIERPOOL_H
#define TUPPER3D_MODIFIERPOOL_H

class TupperwarePoolValueString;

class TupModifierPool
{
	// Private Default constructor so no one can use it
	TupModifierPool(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the object atoms
	TupperwarePoolValueString *m_pModifierPool; // the pool that knows about all the child atoms

public:
	TupModifierPool(TupperwareAggregate *pObject); // use this one
	~TupModifierPool();

// add methods
	int AddModifier(void); // create a new object in the pool and return the index to this object
	TupperwareAggregate *AddModifier(int index); // create a new object in the pool using the index

// get methods
	TupperwareAggregate *GetModifier(int index); // return the aggregate using a integer index
	int GetNumModifiers(void);

// delete methods
	void DeleteModifier(int index);
};

#endif
