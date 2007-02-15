////////////////////////////////////////////////////////////////////////////
//
// Modifier
//
// Handle the types of modifiers
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: modifier.h $
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 7/02/02    Time: 3:10p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Cloth Modifier
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 3/06/02    Time: 3:03p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added support for the HoldVC Modifier
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/05/02    Time: 1:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get BaseAggregate
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 1/02/02    Time: 4:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Apply VC Modifier complete
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/31/01   Time: 3:37p
 * Updated in $/Avalanche/tools/Tupper3d
 * modifiers pass 1
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:33p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifiers
 */

////////////////////////////////////////////////////////////////////////////

#ifndef TUPPER3D_MODIFIER_H
#define TUPPER3D_MODIFIER_H

class TupModifier
{
	// Private Default constructor so no one can use it
	TupModifier(void);

	TupperwareAggregate *m_pObject; // aggregate that holds all the mesh atoms

public:
	enum ModifierTypes
	{
		APPLY_VC_MODIFIER,
		VERTEX_PAINT_MODIFIER,
		HOLD_VC_MODIFIER,
		FACE_ATTRIBUTE_MODIFIER,
		SKIN_MODIFIER,
		CLOTH_MODIFIER,
		UNKNOWN,
		LAST_ENTRY,
	};
	static char *m_typeNames[LAST_ENTRY]; // names for the children atoms

	TupModifier(TupperwareAggregate *pObject);

// add methods
	void SetModifierType(TupModifier::ModifierTypes type);

// get methods
	TupModifier::ModifierTypes GetModifierType(void);
	TupperwareAggregate *GetBaseAggregate(void);
};


#endif
