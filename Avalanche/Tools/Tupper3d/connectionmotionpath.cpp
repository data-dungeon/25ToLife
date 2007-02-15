////////////////////////////////////////////////////////////////////////////
//
// Node
//
// Class to handle nodes
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: connectionmotionpath.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/05/03    Time: 12:02p
 * Updated in $/Avalanche/tools/Tupper3d
 * motion path exporting
 * 
 * *****************  Version 10  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 9  *****************
 * User: Adam Clayton Date: 2/28/02    Time: 11:21a
 * Updated in $/Avalanche/tools/Tupper3d
 * Node Reference Stuff.  Moved code to use private functions (less space)
 * 
 * *****************  Version 8  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 * 
 * *****************  Version 7  *****************
 * User: Adam Clayton Date: 12/28/01   Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * user properties and flags
 * 
 * *****************  Version 6  *****************
 * User: Shon         Date: 12/18/01   Time: 3:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/07/01   Time: 9:20a
 * Updated in $/Avalanche/tools/Tupper3d
 * GetTransform fix
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/06/01   Time: 4:38p
 * Updated in $/Avalanche/tools/Tupper3d
 * Get Methods
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/05/01   Time: 4:26p
 * Updated in $/Avalanche/tools/Tupper3d
 * Level A
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "connectionmotionpath.h"

char *TupConnectionMotionPath::m_keyNames[TupConnectionMotionPath::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"AnimatedNodeReferences", // ANIMATED_NODE_REFERENCES
	"PathNodeReference", // PATH_NODE_REFERENCE
	"WorldUpVector", // WORLD_UP_VECTOR
	"WorldUpNodeReference", // WORLD_UP_NODE_REFERENCE
	"FrontTwist", // FRONT_TWIST
	"UpTwist", // UP_TWIST
	"SideTwist", // SIDE_TWIST
	"BankScale", // BANK_SCALE
	"BankLimit", // BANK_LIMIT
	"UValue", // U_VALUE
	"AnimatedChannels", // ANIMATED_CHANNELS
	"Flags", // FLAGS
};

TupConnectionMotionPath::TupConnectionMotionPath(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupConnectionMotionPath::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupConnectionMotionPath::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupConnectionMotionPath::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupConnectionMotionPath::AddAnimatedNodeReferences(const int *pRefs,int numRefs)
{
	return AddIntList(pRefs,numRefs,ANIMATED_NODE_REFERENCES);
}

TupperwareScalar *TupConnectionMotionPath::AddPathNodeReference(int pathNodeReference)
{
	return AddIntScalar(pathNodeReference,PATH_NODE_REFERENCE);
}

TupperwareList *TupConnectionMotionPath::AddWorldUpVector(const float *pVector)
{
	return AddFloatList(pVector,3,WORLD_UP_VECTOR);
}

TupperwareScalar *TupConnectionMotionPath::AddWorldUpNodeReference(int nodeReference)
{
	return AddIntScalar(nodeReference,WORLD_UP_NODE_REFERENCE);
}

TupperwareScalar *TupConnectionMotionPath::AddFrontTwist(float frontTwist)
{
	return AddFloatScalar(frontTwist,FRONT_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::AddUpTwist(float upTwist)
{
	return AddFloatScalar(upTwist,UP_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::AddSideTwist(float sideTwist)
{
	return AddFloatScalar(sideTwist,SIDE_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::AddBankScale(float bankScale)
{
	return AddFloatScalar(bankScale,BANK_SCALE);
}

TupperwareScalar *TupConnectionMotionPath::AddBankLimit(float bankLimit)
{
	return AddFloatScalar(bankLimit,BANK_LIMIT);
}

TupperwareScalar *TupConnectionMotionPath::AddUValue(float uValue)
{
	return AddFloatScalar(uValue,U_VALUE);
}

TupperwareAggregate *TupConnectionMotionPath::AddAnimatedChannels(void)
{
	return AddAggregate(ANIMATED_CHANNELS);
}

TupperwareScalar *TupConnectionMotionPath::AddFlags(int flags)
{
	return AddIntScalar(flags,FLAGS);
}

TupperwareScalar *TupConnectionMotionPath::GetName(char **name)
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

TupperwareList *TupConnectionMotionPath::GetAnimatedNodeReferences(int **pRefs,int &numRefs)
{
	return GetIntList(pRefs,numRefs,ANIMATED_NODE_REFERENCES);
}

TupperwareScalar *TupConnectionMotionPath::GetPathNodeReference(int &pathNodeReference)
{
	return GetIntScalar(pathNodeReference,PATH_NODE_REFERENCE);
}

TupperwareList *TupConnectionMotionPath::GetWorldUpVector(float **pVector)
{
	int numFloats;
	return GetFloatList(pVector,numFloats,WORLD_UP_VECTOR);
}

TupperwareScalar *TupConnectionMotionPath::GetWorldUpNodeReference(int &nodeReference)
{
	return GetIntScalar(nodeReference,WORLD_UP_NODE_REFERENCE);
}

TupperwareScalar *TupConnectionMotionPath::GetFrontTwist(float &frontTwist)
{
	return GetFloatScalar(frontTwist,FRONT_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::GetUpTwist(float &upTwist)
{
	return GetFloatScalar(upTwist,UP_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::GetSideTwist(float &sideTwist)
{
	return GetFloatScalar(sideTwist,SIDE_TWIST);
}

TupperwareScalar *TupConnectionMotionPath::GetBankScale(float &bankScale)
{
	return GetFloatScalar(bankScale,BANK_SCALE);
}

TupperwareScalar *TupConnectionMotionPath::GetBankLimit(float &bankLimit)
{
	return GetFloatScalar(bankLimit,BANK_LIMIT);
}

TupperwareScalar *TupConnectionMotionPath::GetUValue(float &uValue)
{
	return GetFloatScalar(uValue,U_VALUE);
}

TupperwareAggregate *TupConnectionMotionPath::GetAnimatedChannels(void)
{
	return GetAggregate(ANIMATED_CHANNELS);
}

TupperwareScalar *TupConnectionMotionPath::GetFlags(int &flags)
{
	return GetIntScalar(flags,FLAGS);
}


// Private Methods
TupperwareScalar *TupConnectionMotionPath::AddIntScalar(int value,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarInt(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupConnectionMotionPath::AddFloatScalar(float value,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarFloat(m_keyNames[index],NULL,value);
	m_cachedAtoms[index] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareList *TupConnectionMotionPath::AddFloatList(const float *pFloats,int numFloats,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListFloat(m_keyNames[index],NULL,pFloats,numFloats);
	m_cachedAtoms[index] = (TupperwareAtom *)list;
	return list;
}

TupperwareList *TupConnectionMotionPath::AddIntList(const int *pInts,int numInts,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareList *list = m_pObject->AddListInt(m_keyNames[index],NULL,pInts,numInts);
	m_cachedAtoms[index] = list;
	return list;
}

TupperwareAggregate *TupConnectionMotionPath::AddAggregate(TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareScalar *TupConnectionMotionPath::GetIntScalar(int &value,TupConnectionMotionPath::ChildrenAtoms index)
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

TupperwareScalar *TupConnectionMotionPath::GetFloatScalar(float &value,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType()==TupperwareScalar::DATA_FLOAT)
		{
			value = pScalar->GetAsFloat();
			return pScalar;
		}
	}
	value = 0;
	return NULL;
}

TupperwareList *TupConnectionMotionPath::GetFloatList(float **pFloats,int &numFloats,TupConnectionMotionPath::ChildrenAtoms index)
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

TupperwareList *TupConnectionMotionPath::GetIntList(int **pInts,int &numInts,TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_LIST)
	{
		TupperwareList *pList = (TupperwareList *)pAtom;
		if (pList->GetDataType()==TupperwareList::DATA_INT)
		{
			*pInts = pList->GetAsInt();
			numInts = pList->GetLength();
			return pList;
		}
	}
	*pInts = NULL;
	numInts = 0;
	return NULL;
}

TupperwareAggregate *TupConnectionMotionPath::GetAggregate(TupConnectionMotionPath::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}
