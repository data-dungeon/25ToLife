////////////////////////////////////////////////////////////////////////////
//
// AccessRoot
//
// Class to deal accessing the attributes from the game engine side
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AccessRoot.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 4/08/02    Time: 4:27p
 * Updated in $/Avalanche/tools/TupperAttrib
 * Added access classes
 */

////////////////////////////////////////////////////////////////////////////

#include "TupperAttribPch.h"
#include "AccessRoot.h"

class TupperwareAtom;
class TupperwareAggregate;
class TupperwareScalar;

char *TupAccessRoot::m_keyNames[TupAccessRoot::NUM_CHILDREN_ATOMS] = 
{
	"InstanceAttributes", // INSTANCE_ATTRIBUTES
	"PathAttributes", // PATH_ATTRIBUTES
	"NodeAttributeTree", // NODE_ATTRIBUTE_TREE
	"PathPointAttributeTree", // PATH_POINT_ATTRIBUTE_TREE
};

TupAccessRoot::TupAccessRoot(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	BuildAtomCache();
}

void TupAccessRoot::BuildAtomCache(void)
{
	for (int i=0;i<NUM_CHILDREN_ATOMS;i++)
	{
		m_cachedAtoms[i] = m_pObject->FindAtomByKey(m_keyNames[i]);
	}
}

TupperwareAggregate *TupAccessRoot::GetBaseAggregate( void )
{
  return( m_pObject );
}

TupperwareAggregate *TupAccessRoot::AddInstanceAttributes(void)
{
	return AddAggregate(TupAccessRoot::INSTANCE_ATTRIBUTES);
}

TupperwareAggregate *TupAccessRoot::AddPathAttributes(void)
{
	return AddAggregate(TupAccessRoot::PATH_ATTRIBUTES);
}

TupperwareAggregate *TupAccessRoot::AddNodeAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pNodeAttributeTreeAgg = AddAggregate(TupAccessRoot::NODE_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pNodeAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pNodeAttributeTreeAgg;
}

TupperwareAggregate *TupAccessRoot::AddPathPointAttributeTree(TupperwareAggregate *pTreeAgg)
{
	TupperwareAggregate *pPathPointAttributeTreeAgg = AddAggregate(TupAccessRoot::PATH_POINT_ATTRIBUTE_TREE);
	for (TupperwareIterator i = pTreeAgg->FindAll(); !i.Finished(); i.Next()) 
	{
		pPathPointAttributeTreeAgg->Copy(i.GetAsAtom());
	}
	return pPathPointAttributeTreeAgg;
}

TupperwareAggregate *TupAccessRoot::GetInstanceAttributes(void)
{
	return GetAggregate(TupAccessRoot::INSTANCE_ATTRIBUTES);
}

TupperwareAggregate *TupAccessRoot::GetPathAttributes(void)
{
	return GetAggregate(TupAccessRoot::PATH_ATTRIBUTES);
}

TupperwareAggregate *TupAccessRoot::GetNodeAttributeTree(void)
{
	return GetAggregate(TupAccessRoot::NODE_ATTRIBUTE_TREE);
}

TupperwareAggregate *TupAccessRoot::GetPathPointAttributeTree(void)
{
	return GetAggregate(TupAccessRoot::PATH_POINT_ATTRIBUTE_TREE);
}

// private methods

TupperwareAggregate *TupAccessRoot::AddAggregate(TupAccessRoot::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	// delete if there
	if (pAtom)
		m_pObject->DeleteAtom(pAtom);
	TupperwareAggregate *pAgg = m_pObject->AddAggregate(m_keyNames[index],NULL);
	m_cachedAtoms[index] = (TupperwareAtom *)pAgg;
	return pAgg;
}

TupperwareAggregate *TupAccessRoot::GetAggregate(TupAccessRoot::ChildrenAtoms index)
{
	TupperwareAtom *pAtom = m_cachedAtoms[index];
	if (pAtom && pAtom->GetAtomType()==TupperwareAtom::ATOM_AGGREGATE)
	{
		return (TupperwareAggregate *)pAtom;
	}
	return NULL;
}

