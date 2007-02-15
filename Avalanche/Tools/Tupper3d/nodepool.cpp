////////////////////////////////////////////////////////////////////////////
//
// NodePool
//
// Class to handle the node pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: nodepool.cpp $
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * added remove function
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 2/26/02    Time: 11:43a
 * Updated in $/Avalanche/tools/Tupper3d
 * Added the Node Pool and Node Referencing
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "nodepool.h"

TupNodePool::TupNodePool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pNodePool = new TupperwarePoolValueString(pObject,"Node");
}

TupNodePool::~TupNodePool()
{
	delete m_pNodePool;
}

int TupNodePool::AddNode( void )
{
	TupperwareAggregate *pAgg = m_pNodePool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupNodePool::AddNode(int index)
{
	return m_pNodePool->AddAggregate(index);
}

TupperwareAggregate *TupNodePool::GetNode(int index)
{
	return m_pNodePool->FindAggregate(index);
}

int TupNodePool::GetNumNodes(void)
{
	return m_pNodePool->GetPoolSize();
}

void TupNodePool::DeleteNode(int index)
{
	m_pNodePool->DeleteEntry(index);
}


