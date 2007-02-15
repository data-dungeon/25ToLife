////////////////////////////////////////////////////////////////////////////
//
// AnimClipBlendPool
//
// Class to handle a bunch of animation clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipBlendPool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/20/03    Time: 2:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * add clip blends
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimClipBlendPool.h"

TupAnimClipBlendPool::TupAnimClipBlendPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAnimClipBlendPool = new TupperwarePoolValueString(pObject,"ClipBlend");
}

TupAnimClipBlendPool::~TupAnimClipBlendPool()
{
	delete m_pAnimClipBlendPool;
}

int TupAnimClipBlendPool::AddAnimClipBlend( void )
{
	TupperwareAggregate *pAgg = m_pAnimClipBlendPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAnimClipBlendPool::AddAnimClipBlend(int index)
{
	return m_pAnimClipBlendPool->AddAggregate(index);
}

TupperwareAggregate *TupAnimClipBlendPool::GetAnimClipBlend(int index)
{
	return m_pAnimClipBlendPool->FindAggregate(index);
}

int TupAnimClipBlendPool::GetNumAnimClipBlends(void)
{
	return m_pAnimClipBlendPool->GetPoolSize();
}

void TupAnimClipBlendPool::DeleteAnimClipBlend(int index)
{
	m_pAnimClipBlendPool->DeleteEntry(index);
}




