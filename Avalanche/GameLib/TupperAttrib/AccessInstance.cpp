////////////////////////////////////////////////////////////////////////////
//
// AccessInstance
//
// Class to deal accessing the instance attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessInstance.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperAttribPch.h"
#include "AccessInstance.h"

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

// TupAccessInstancePool Methods
TupAccessInstancePool::TupAccessInstancePool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pInstancePool = new TupperwarePoolValueString(pObject,"Instance");
}

TupAccessInstancePool::~TupAccessInstancePool()
{
	delete m_pInstancePool;
}

int TupAccessInstancePool::AddInstance( void )
{
	TupperwareAggregate *pAgg = m_pInstancePool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAccessInstancePool::AddInstance(int index)
{
	return m_pInstancePool->AddAggregate(index);
}

TupperwareAggregate *TupAccessInstancePool::GetInstance(int index)
{
	return m_pInstancePool->FindAggregate(index);
}

int TupAccessInstancePool::GetNumInstances(void)
{
	return m_pInstancePool->GetPoolSize();
}


// TupAccessInstance Methods
char *TupAccessInstance::m_keyNames[TupAccessInstance::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Attributes", // ATTRIBUTES
};

TupAccessInstance::TupAccessInstance(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAccessInstance::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAccessInstance::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupAccessInstance::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAccessInstance::AddAttributes(const char *attributesString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[ATTRIBUTES];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[ATTRIBUTES],NULL,attributesString);
	m_cachedAtoms[ATTRIBUTES] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAccessInstance::GetName(char **name)
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

TupperwareScalar *TupAccessInstance::GetAttributes(char **attributesString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[ATTRIBUTES];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_SCALAR)
	{
		TupperwareScalar *pScalar = (TupperwareScalar *)pAtom;
		if (pScalar->GetDataType() == TupperwareScalar::DATA_STRING)
		{
			*attributesString = pScalar->GetAsString();
			return pScalar;
		}
	}
	*attributesString = NULL;
	return NULL;
}

