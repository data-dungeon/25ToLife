////////////////////////////////////////////////////////////////////////////
//
// AnimChannelPool
//
// Class to handle the animation channels
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimChannelPool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimChannelPool.h"

TupAnimChannelPool::TupAnimChannelPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAnimChannelPool = new TupperwarePoolValueString(pObject,"Channel");
}

TupAnimChannelPool::~TupAnimChannelPool()
{
	delete m_pAnimChannelPool;
}

int TupAnimChannelPool::AddAnimChannel( void )
{
	TupperwareAggregate *pAgg = m_pAnimChannelPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAnimChannelPool::AddAnimChannel(int index)
{
	return m_pAnimChannelPool->AddAggregate(index);
}

TupperwareAggregate *TupAnimChannelPool::GetAnimChannel(int index)
{
	return m_pAnimChannelPool->FindAggregate(index);
}

int TupAnimChannelPool::GetNumAnimChannels(void)
{
	return m_pAnimChannelPool->GetPoolSize();
}

void TupAnimChannelPool::DeleteAnimChannel(int index)
{
	m_pAnimChannelPool->DeleteEntry(index);
}




