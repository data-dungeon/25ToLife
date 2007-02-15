////////////////////////////////////////////////////////////////////////////
//
// AnimSetPool
//
// Class to handle the animation sets
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimSetPool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimSetPool.h"

TupAnimSetPool::TupAnimSetPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAnimSetPool = new TupperwarePoolValueString(pObject,"Set");
}

TupAnimSetPool::~TupAnimSetPool()
{
	delete m_pAnimSetPool;
}

int TupAnimSetPool::AddAnimSet( void )
{
	TupperwareAggregate *pAgg = m_pAnimSetPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAnimSetPool::AddAnimSet(int index)
{
	return m_pAnimSetPool->AddAggregate(index);
}

TupperwareAggregate *TupAnimSetPool::GetAnimSet(int index)
{
	return m_pAnimSetPool->FindAggregate(index);
}

int TupAnimSetPool::GetNumAnimSets(void)
{
	return m_pAnimSetPool->GetPoolSize();
}

void TupAnimSetPool::DeleteAnimSet(int index)
{
	m_pAnimSetPool->DeleteEntry(index);
}




