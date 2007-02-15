////////////////////////////////////////////////////////////////////////////
//
// Node
//
// Class to handle nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: transform.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 1:51p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add and Get functions complete
 * Added new files for handling various parts of the graphics format
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "transform.h"

char *TupTransform::m_typeNames[TupTransform::UNKNOWN] = 
{
	"StaticMatrix", // STATIC_MATRIX
};

TupTransform::TupTransform(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
}

void TupTransform::SetTransformType(TupTransform::TransformTypes type)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("TransformType");
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	m_pObject->AddScalarString("TransformType",NULL,m_typeNames[type]);
}

TupTransform::TransformTypes TupTransform::GetTransformType(void)
{
	TupperwareAtom *pAtom = m_pObject->FindAtomByKey("TransformType");
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_STRING)
		{
			for (int i=0;i<TupTransform::UNKNOWN;i++)
			{
				if (!strcmp(m_typeNames[i],pScalar->GetAsString()))
				{
					return (TupTransform::TransformTypes)i;
				}
			}
		}
	}
	return TupTransform::UNKNOWN;
}
