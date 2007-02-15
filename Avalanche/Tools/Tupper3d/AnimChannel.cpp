////////////////////////////////////////////////////////////////////////////
//
// AnimChannel
//
// Class to handle an animation channel
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimChannel.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 10:35a
 * Updated in $/Avalanche/tools/Tupper3d
 * Add character Index
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 4:19p
 * Updated in $/Avalanche/tools/Tupper3d
 * first phase of animation export
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimChannel.h"

char *TupAnimChannel::m_typeNames[TupAnimChannel::LAST_ENTRY] = 
{
	"Node", // NODE
	"Unknown", // UNKNOWN
};

char *TupAnimChannel::m_keyNames[TupAnimChannel::NUM_CHILDREN_ATOMS] = 
{
	"Type", // TYPE
	"Name", // NAME
	"Attribute", // ATTRIBUTE
	"Reference", // REFERENCE
	"CharacterIndex", // CHARACTER_INDEX
	"Curves", // CURVES
	"Flags", // FLAGS
};

TupAnimChannel::TupAnimChannel(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimChannel::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimChannel::GetBaseAggregate( void )
{
	return( m_pObject );
}

void TupAnimChannel::SetAnimChannelType(TupAnimChannel::ChannelTypes type)
{
	AddStringScalar(m_typeNames[type],TYPE);
}

TupperwareScalar *TupAnimChannel::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareScalar *TupAnimChannel::AddAttribute(const char *attribute)
{
	return AddStringScalar(attribute,ATTRIBUTE);
}

TupperwareScalar *TupAnimChannel::AddReference(int ref)
{
	return AddIntScalar(ref,REFERENCE);
}

TupperwareScalar *TupAnimChannel::AddCharacterIndex(int index)
{
	return AddIntScalar(index,CHARACTER_INDEX);
}

TupperwareAggregate *TupAnimChannel::AddCurves(void)
{
	return AddAggregate(CURVES);
}

TupperwareScalar *TupAnimChannel::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupAnimChannel::ChannelTypes TupAnimChannel::GetAnimChannelType(void)
{
	char *pTypeName;
	GetStringScalar(&pTypeName,TYPE);
	if (pTypeName)
	{
		for (int i=0;i<TupAnimChannel::UNKNOWN;i++)
		{
			if (!strcmp(m_typeNames[i],pTypeName))
			{
				return (TupAnimChannel::ChannelTypes)i;
			}
		}
	}
	return ChannelTypes::UNKNOWN;
}

TupperwareScalar *TupAnimChannel::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareScalar *TupAnimChannel::GetAttribute(char **attribute)
{
	return GetStringScalar(attribute,ATTRIBUTE);
}

TupperwareScalar *TupAnimChannel::GetReference(int &ref)
{
	return GetIntScalar(ref,REFERENCE);
}

TupperwareScalar *TupAnimChannel::GetCharacterIndex(int &index)
{
	return GetIntScalar(index,CHARACTER_INDEX);
}

TupperwareAggregate *TupAnimChannel::GetCurves(void)
{
	return GetAggregate(CURVES);
}

TupperwareScalar *TupAnimChannel::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

// private methods
TupperwareAggregate *TupAnimChannel::AddAggregate(TupAnimChannel::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareScalar *TupAnimChannel::AddStringScalar(const char *pString,TupAnimChannel::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareAggregate *TupAnimChannel::GetAggregate(TupAnimChannel::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareScalar *TupAnimChannel::GetStringScalar(char **pString,TupAnimChannel::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *) pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*pString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*pString = NULL;
	return NULL;
}

TupperwareScalar *TupAnimChannel::AddIntScalar(int value,TupAnimChannel::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimChannel::GetIntScalar(int &value,TupAnimChannel::ChildrenAtoms index)
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

TupperwareList *TupAnimChannel::AddFloatList(const float *pFloats,int numFloats,TupAnimChannel::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   // delete if there
   if (pAtom)
      m_pObject->DeleteAtom(pAtom);
   TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
   m_cachedAtoms[index] = (TupperwareAtom *)list;
   return list;
}

TupperwareList *TupAnimChannel::GetFloatList(float **pFloats,int &numFloats,TupAnimChannel::ChildrenAtoms index)
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