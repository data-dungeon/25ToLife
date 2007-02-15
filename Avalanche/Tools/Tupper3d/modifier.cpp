////////////////////////////////////////////////////////////////////////////
//
// Modifier
//
// Class to handle the modifier types
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: modifier.cpp $
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

#include "Tupper3dPch.h"
#include "modifier.h"

char *TupModifier::m_typeNames[TupModifier::LAST_ENTRY] = 
{
	"ApplyVC", // APPLY_VC_MODIFIER
	"VertexPaint", // VERTEX_PAINT_MODIFIER
	"HoldVC", // HOLD_VC_MODIFIER
	"FaceAttribute", // FACE_ATTRIBUTE_MODIFIER
	"Skin", // SKIN_MODIFIER
	"Cloth", // CLOTH_MODIFIER
	"Unknown", // UNKNOWN
};

TupModifier::TupModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
}

void TupModifier::SetModifierType(TupModifier::ModifierTypes type)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ModifierType");
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_pObject->AddScalarString("ModifierType",NULL,m_typeNames[type]);
}

TupModifier::ModifierTypes TupModifier::GetModifierType(void)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("ModifierType");
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupModifier::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupModifier::ModifierTypes)i;
				}
			}
		}
	}
	return TupModifier::UNKNOWN;
}

TupperwareAggregate *TupModifier::GetBaseAggregate(void)
{
	return m_pObject;
}
