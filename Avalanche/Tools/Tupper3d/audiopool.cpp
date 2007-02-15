////////////////////////////////////////////////////////////////////////////
//
// AudioPool
//
// Class to handle audio pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: audiopool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 6/17/03    Time: 2:57p
 * Updated in $/Avalanche/tools/Tupper3d
 * add audio clips
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "audiopool.h"

TupAudioPool::TupAudioPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pAudioPool = new TupperwarePoolValueString(pObject,"Audio");
}

TupAudioPool::~TupAudioPool()
{
	delete m_pAudioPool;
}

int TupAudioPool::AddAudio( void )
{
	TupperwareAggregate *pAgg = m_pAudioPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupAudioPool::AddAudio(int index)
{
	return m_pAudioPool->AddAggregate(index);
}

TupperwareAggregate *TupAudioPool::GetAudio(int index)
{
	return m_pAudioPool->FindAggregate(index);
}

int TupAudioPool::GetNumAudios(void)
{
	return m_pAudioPool->GetPoolSize();
}

void TupAudioPool::DeleteAudio(int index)
{
	m_pAudioPool->DeleteEntry(index);
}




