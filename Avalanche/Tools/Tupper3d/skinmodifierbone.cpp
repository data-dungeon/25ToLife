////////////////////////////////////////////////////////////////////////////
//
// SkinModifierBone
//
// Class for handling a single bone within the skin modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifierbone.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/29/02    Time: 2:59p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added correct matrix lists
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "skinmodifierbone.h"

char *TupSkinModifierBone::m_keyNames[TupSkinModifierBone::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
	"NodeReference", //	NODE_REFERENCE
	"BoneInitObjectTM", // BONE_INIT_OBJECT_TM,
};

TupSkinModifierBone::TupSkinModifierBone(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupSkinModifierBone::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSkinModifierBone::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupSkinModifierBone::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupSkinModifierBone::AddFlags(int value)
{
	return AddIntScalar(value,TupSkinModifierBone::FLAGS);
}

TupperwareScalar *TupSkinModifierBone::AddNodeReference(int nodeReference)
{
	return AddIntScalar(nodeReference,TupSkinModifierBone::NODE_REFERENCE);
}

TupperwareList *TupSkinModifierBone::AddBoneInitObjectTM(const float *pValue)
{
	return AddFloatList(pValue,16,TupSkinModifierBone::BONE_INIT_OBJECT_TM);
}

TupperwareScalar *TupSkinModifierBone::GetName(char **name)
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

TupperwareScalar *TupSkinModifierBone::GetFlags(int &value)
{
	return GetIntScalar(value,TupSkinModifierBone::FLAGS);
}

TupperwareScalar *TupSkinModifierBone::GetNodeReference(int &nodeReference)
{
	TupperwareScalar *pScalar = GetIntScalar(nodeReference,TupSkinModifierBone::NODE_REFERENCE);
	if (!pScalar)
	{
		nodeReference = -1;
	}
	return pScalar;
}

TupperwareList *TupSkinModifierBone::GetBoneInitObjectTM(float **pValue)
{
	int numValues;
	TupperwareList *pList = GetFloatList(pValue,numValues,TupSkinModifierBone::BONE_INIT_OBJECT_TM);
	return pList;
}

// Private Methods
TupperwareList *TupSkinModifierBone::AddFloatList(const float *pFloats,int numFloats,TupSkinModifierBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareScalar *TupSkinModifierBone::AddIntScalar(int value,TupSkinModifierBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupSkinModifierBone::GetFloatList(float **pFloats,int &numFloats,TupSkinModifierBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_FLOAT)
		{
			*pFloats = pList->GetAsFloat();
			numFloats = pList->GetLength();
			return pList;
		}
	}
	*pFloats = NULL;
	numFloats = 0;
	return NULL;
}

TupperwareScalar *TupSkinModifierBone::GetIntScalar(int &value,TupSkinModifierBone::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *scalar = (TupperwareScalar *)pAtom;
		if (scalar->GetDataType()==TupperwareScalar::DATA_INT)
		{
			value = scalar->GetAsInt();
			return scalar;
		}
	}
	value = 0;
	return NULL;
}
