////////////////////////////////////////////////////////////////////////////
//
// AccessPath
//
// Class to deal accessing the path attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessPath.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperAttribPch.h"
#include "AccessPath.h"

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

// TupAccessPathPool Methods
TupAccessPathPool::TupAccessPathPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pPathPool = new TupperwarePoolValueString(pObject,"Path");
}

TupAccessPathPool::~TupAccessPathPool()
{
	delete m_pPathPool;
}

int TupAccessPathPool::AddPath( void )
{
	TupperwareAggregate *pAgg = m_pPathPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAccessPathPool::AddPath(int index)
{
	return m_pPathPool->AddAggregate(index);
}

TupperwareAggregate *TupAccessPathPool::GetPath(int index)
{
	return m_pPathPool->FindAggregate(index);
}

int TupAccessPathPool::GetNumPaths(void)
{
	return m_pPathPool->GetPoolSize();
}


// TupAccessPath Methods
char *TupAccessPath::m_keyNames[TupAccessPath::NUM_CHILDREN_ATOMS] = 
{
	"Name", // NAME
	"Attributes", // ATTRIBUTES
};

TupAccessPath::TupAccessPath(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAccessPath::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAccessPath::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareScalar *TupAccessPath::AddName(const char *name)
{
	TupperwareAtom *pAtom = m_cachedAtoms[NAME];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[NAME],NULL,name);
	m_cachedAtoms[NAME] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAccessPath::AddAttributes(const char *attributesString)
{
	TupperwareAtom *pAtom = m_cachedAtoms[ATTRIBUTES];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareScalar *scalar = m_pObject->AddScalarString(m_keyNames[ATTRIBUTES],NULL,attributesString);
	m_cachedAtoms[ATTRIBUTES] = (TupperwareAtom *)scalar;
	return scalar;
}

TupperwareScalar *TupAccessPath::GetName(char **name)
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

TupperwareScalar *TupAccessPath::GetAttributes(char **attributesString)
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

