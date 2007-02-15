////////////////////////////////////////////////////////////////////////////
//
// Layer
//
// Class to handle layers
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: layer.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 9/11/03    Time: 4:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * import and export of layers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "layer.h"

char *TupLayer::m_keyNames[TupLayer::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
	"ColorIndex", // COLOR_INDEX
	"DisplayOrder", // DISPLAY_ORDER
};

TupLayer::TupLayer(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupLayer::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupLayer::GetBaseAggregate( void )
{
	return( m_pObject );
}

TupperwareScalar *TupLayer::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupLayer::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupperwareScalar *TupLayer::AddColorIndex(int index)
{
	return AddIntScalar(index,COLOR_INDEX);
}

TupperwareScalar *TupLayer::AddDisplayOrder(int displayOrder)
{
	return AddIntScalar(displayOrder,DISPLAY_ORDER);
}


TupperwareScalar *TupLayer::GetName(char **name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*name = pScalar->GetAsString();
			return pScalar;
		}
	}
	*name = NULL;
	return NULL;
}

TupperwareScalar *TupLayer::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

TupperwareScalar *TupLayer::GetColorIndex(int &index)
{
	return GetIntScalar(index,COLOR_INDEX);
}

TupperwareScalar *TupLayer::GetDisplayOrder(int &displayOrder)
{
	return GetIntScalar(displayOrder,DISPLAY_ORDER);
}

// Private Methods
TupperwareScalar *TupLayer::AddIntScalar(int value,TupLayer::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupLayer::GetIntScalar(int &value,TupLayer::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = pScalar->GetAsInt();
			return pScalar;
		}
	}
	value = 0;
	return NULL;
}
