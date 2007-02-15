////////////////////////////////////////////////////////////////////////////
//
// AnimClipPool
//
// Class to handle a bunch of animation clips
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: AnimClipPool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 5/15/03    Time: 2:22p
 * Updated in $/Avalanche/tools/Tupper3d
 * Added Animation Wrapper Classes
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "AnimClipPool.h"

TupAnimClipPool::TupAnimClipPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAnimClipPool = new TupperwarePoolValueString(pObject,"Clip");
}

TupAnimClipPool::~TupAnimClipPool()
{
	delete m_pAnimClipPool;
}

int TupAnimClipPool::AddAnimClip( void )
{
	TupperwareAggregate *pAgg = m_pAnimClipPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAnimClipPool::AddAnimClip(int index)
{
	return m_pAnimClipPool->AddAggregate(index);
}

TupperwareAggregate *TupAnimClipPool::GetAnimClip(int index)
{
	return m_pAnimClipPool->FindAggregate(index);
}

int TupAnimClipPool::GetNumAnimClips(void)
{
	return m_pAnimClipPool->GetPoolSize();
}

void TupAnimClipPool::DeleteAnimClip(int index)
{
	m_pAnimClipPool->DeleteEntry(index);
}




