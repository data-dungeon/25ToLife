////////////////////////////////////////////////////////////////////////////
//
// SkinModifier
//
// Class to handle the Skin Modifier
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: skinmodifier.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 8/07/02    Time: 2:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * added modifier constructor
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/19/02    Time: 12:56p
 * Updated in $/Avalanche/tools/Tupper3d
 * Add Skin Modifier
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "skinmodifier.h"
#include "modifier.h"

char *TupSkinModifier::m_keyNames[TupSkinModifier::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Flags", // FLAGS
	"Bones", // BONES
};

TupSkinModifier::TupSkinModifier(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

TupSkinModifier::TupSkinModifier(TupModifier &modifierBase)
{
	m_pObject = modifierBase.GetBaseAggregate();
	BuildAtomCache();
}

void TupSkinModifier::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupSkinModifier::GetBaseAggregate(void)
{
	return m_pObject;
}

TupperwareScalar *TupSkinModifier::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupSkinModifier::AddFlags(int value)
{
	return AddIntScalar(value,TupSkinModifier::FLAGS);
}

TupperwareAggregate *TupSkinModifier::AddBone(int index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[BONES];
	// delete if wrong type
	if (pAtom && pAtom->GetAtomType() != TupperwareAtom::ATOM_AGGREGATE)
	{
		m_pObject->DeleteAtom(pAtom);
		pAtom = NULL;
	}
	// Add if not there
	if (!pAtom)
	{
		pAtom = m_cachedAtoms[BONES] = (TupperwareAtom *)m_pObject->AddAggregate(m_keyNames[BONES]);
	}
	TupperwareAggregate *pBonesAgg = (TupperwareAggregate *)pAtom;

	char indexString[20];
	itoa(index,indexString,10);
	TupperwareAtom *pBoneAtom = pBonesAgg->FindAtomByKeyAndName("Bone",indexString);
	if (pBoneAtom)
	{
		pBonesAgg->DeleteAtom(pBoneAtom);
	}
	return pBonesAgg->AddAggregate("Bone",indexString);
}

TupperwareScalar *TupSkinModifier::GetName(char **name)
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

TupperwareScalar *TupSkinModifier::GetFlags(int &value)
{
	return GetIntScalar(value,TupSkinModifier::FLAGS);
}

int TupSkinModifier::GetNumBones(void)
{
	int total = 0;
	TupperwareAtom *pBonesAtom = m_cachedAtoms[BONES];
	// if right type
	if (pBonesAtom && pBonesAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pBonesAgg = (TupperwareAggregate *)pBonesAtom;
		// make sure it has children
		if (pBonesAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pBonesAgg->FindByKey("Bone"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pBoneAtom = i.GetAsAtom();
				if (pBoneAtom && pBoneAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					total++;
				}
			}
		}
	}
	return total;
}

TupperwareAggregate *TupSkinModifier::GetBone(int index)
{
	int count = 0;
	TupperwareAtom *pBonesAtom = m_cachedAtoms[BONES];
	// if right type
	if (pBonesAtom && pBonesAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
	{
		TupperwareAggregate *pBonesAgg = (TupperwareAggregate *)pBonesAtom;
		// make sure it has children
		if (pBonesAgg->GetFirstChildAtom())
		{
			for (TupperwareIterator i = pBonesAgg->FindByKey("Bone"); ! i.Finished(); i.Next() ) 
			{
				// make sure the child is a aggregate
				TupperwareAtom *pBoneAtom = i.GetAsAtom();
				if (pBoneAtom && pBoneAtom->GetAtomType() == TupperwareAtom::ATOM_AGGREGATE)
				{
					if (index == count)
						return (TupperwareAggregate *)pBoneAtom;
					count++;
				}
			}
		}
	}
	return NULL;
}


TupperwareScalar *TupSkinModifier::AddIntScalar(int value,TupSkinModifier::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupSkinModifier::GetIntScalar(int &value,TupSkinModifier::ChildrenAtoms index)
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
