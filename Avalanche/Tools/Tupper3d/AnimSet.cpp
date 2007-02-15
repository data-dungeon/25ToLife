////////////////////////////////////////////////////////////////////////////
//
// AnimSet
//
// Class to handle an animation set
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSet.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimSet.h"

char *TupAnimSet::m_typeNames[TupAnimSet::LAST_ENTRY] = 
{
	"Global", // GLOBAL
	"Character", // CHARACTER
	"Unknown", // UNKNOWN
};

char *TupAnimSet::m_keyNames[TupAnimSet::NUM_CHILDREN_ATOMS] = 
{
	"Type", // TYPE
	"Name", // NAME
	"Channels", // CHANNELS
	"InstancedClips", // INSTANCED_CLIPS
	"SourceClips", // SOURCE_CLIPS
	"ClipBlends", // CLIP_BLENDS
	"Flags", // FLAGS
   "BoneDirection", // BONEDIRECTION
   "RollDirection", // ROLLDIRECTION
   "ParentBoneDirection", // PARENTBONEDIRECTION
   "ParentRollDirection", // PARENTROLLDIRECTION
   "LocalToParentBindMatrix", // LOCALTOPARENTBINDMATRIX
   "JointOrientationQuaternion",// JOINTORIENTATIONQUATERNION
};

TupAnimSet::TupAnimSet(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAnimSet::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAnimSet::GetBaseAggregate( void )
{
	return( m_pObject );
}

void TupAnimSet::SetAnimSetType(TupAnimSet::SetTypes type)
{
	AddStringScalar(m_typeNames[type],TYPE);
}

TupperwareScalar *TupAnimSet::AddName(const char *name)
{
	return AddStringScalar(name,NAME);
}

TupperwareAggregate *TupAnimSet::AddChannels(void)
{
	return AddAggregate(CHANNELS);
}

TupperwareAggregate *TupAnimSet::AddInstancedClips(void)
{
	return AddAggregate(INSTANCED_CLIPS);
}

TupperwareAggregate *TupAnimSet::AddSourceClips(void)
{
	return AddAggregate(SOURCE_CLIPS);
}

TupperwareAggregate *TupAnimSet::AddClipBlends(void)
{
	return AddAggregate(CLIP_BLENDS);
}

TupperwareScalar *TupAnimSet::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupperwareList *TupAnimSet::AddBoneDirection(const float *pValues)
{
   return AddFloatList(pValues,3,BONEDIRECTION);
}

TupperwareList *TupAnimSet::AddRollDirection(const float *pValues)
{
   return AddFloatList(pValues,3,ROLLDIRECTION);
}

TupperwareList *TupAnimSet::AddParentBoneDirection(const float *pValues)
{
   return AddFloatList(pValues,3,PARENTBONEDIRECTION);
}

TupperwareList *TupAnimSet::AddParentRollDirection(const float *pValues)
{
   return AddFloatList(pValues,3,PARENTROLLDIRECTION);
}

TupperwareAggregate *TupAnimSet::AddLocalToParentBindMatrix(void)
{
   TupperwareAtom *pAtom = m_cachedAtoms[LOCALTOPARENTBINDMATRIX];
   // delete if there
   if (pAtom)
      m_pObject->DeleteAtom(pAtom);

   TupperwareAggregate *pTransformationAgg = m_pObject->AddAggregate("LocalToParentBindMatrix");
   m_cachedAtoms[LOCALTOPARENTBINDMATRIX] = (TupperwareAtom *)pTransformationAgg;
   return pTransformationAgg;
}

TupperwareList *TupAnimSet::AddJointOrientationQuaternion(const float *pValues)
{
   return AddFloatList(pValues,4,JOINTORIENTATIONQUATERNION);
}

TupAnimSet::SetTypes TupAnimSet::GetAnimSetType(void)
{
	char *pTypeName;
	GetStringScalar(&pTypeName,TYPE);
	if (pTypeName)
	{
		for (int i=0;i<TupAnimSet::UNKNOWN;i++)
		{
			if (!strcmp(m_typeNames[i],pTypeName))
			{
				return (TupAnimSet::SetTypes)i;
			}
		}
	}
	return SetTypes::UNKNOWN;
}

TupperwareScalar *TupAnimSet::GetName(char **name)
{
	return GetStringScalar(name,NAME);
}

TupperwareAggregate *TupAnimSet::GetChannels(void)
{
	return GetAggregate(CHANNELS);
}

TupperwareAggregate *TupAnimSet::GetInstancedClips(void)
{
	return GetAggregate(INSTANCED_CLIPS);
}

TupperwareAggregate *TupAnimSet::GetSourceClips(void)
{
	return GetAggregate(SOURCE_CLIPS);
}

TupperwareAggregate *TupAnimSet::GetClipBlends(void)
{
	return GetAggregate(CLIP_BLENDS);
}

TupperwareScalar *TupAnimSet::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}

TupperwareList *TupAnimSet::GetBoneDirection(float **pValues)
{
   int numFloats;
   return GetFloatList(pValues,numFloats,BONEDIRECTION);
}

TupperwareList *TupAnimSet::GetRollDirection(float **pValues)
{
   int numFloats;
   return GetFloatList(pValues,numFloats,ROLLDIRECTION);
}

TupperwareList *TupAnimSet::GetParentBoneDirection(float **pValues)
{
   int numFloats;
   return GetFloatList(pValues,numFloats,PARENTBONEDIRECTION);
}

TupperwareList *TupAnimSet::GetParentRollDirection(float **pValues)
{
   int numFloats;
   return GetFloatList(pValues,numFloats,PARENTROLLDIRECTION);
}

TupperwareAggregate *TupAnimSet::GetLocalToParentBindMatrix(void)
{
   TupperwareAtom *pAtom = m_cachedAtoms[LOCALTOPARENTBINDMATRIX];
   if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
   {
      return (TupperwareAggregate *)pAtom;
   }
   return NULL;
}

TupperwareList *TupAnimSet::GetJointOrientationQuaternion(float **pValues)
{
   int numFloats;
   return GetFloatList(pValues,numFloats,JOINTORIENTATIONQUATERNION);
}

// private methods
TupperwareAggregate *TupAnimSet::AddAggregate(TupAnimSet::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareScalar *TupAnimSet::AddStringScalar(const char *pString,TupAnimSet::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *pScalar = m_pObject->AddScalarString(m_keyNames[index],NULL,pString);
	m_cachedAtoms[index] = (TupperwareAtom *)pScalar;
	return pScalar;
}

TupperwareAggregate *TupAnimSet::GetAggregate(TupAnimSet::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

TupperwareScalar *TupAnimSet::GetStringScalar(char **pString,TupAnimSet::ChildrenAtoms index)
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

TupperwareScalar *TupAnimSet::AddIntScalar(int value,TupAnimSet::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAnimSet::GetIntScalar(int &value,TupAnimSet::ChildrenAtoms index)
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

TupperwareList *TupAnimSet::AddFloatList(const float *pFloats,int numFloats,TupAnimSet::ChildrenAtoms index)
{
   TupperwareAtom *pAtom = m_cachedAtoms[index];
   // delete if there
   if (pAtom)
      m_pObject->DeleteAtom(pAtom);
   TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
   m_cachedAtoms[index] = (TupperwareAtom *)list;
   return list;
}

TupperwareList *TupAnimSet::GetFloatList(float **pFloats,int &numFloats,TupAnimSet::ChildrenAtoms index)
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