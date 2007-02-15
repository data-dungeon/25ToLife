////////////////////////////////////////////////////////////////////////////
//
// TexturePool
//
// Class to handle texture pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: mappool.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete methods
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:05a
 * Updated in $/Avalanche/tools/Tupper3d
 * change to correct name
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "mappool.h"

TupMapPool::TupMapPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pMapPool = new TupperwarePoolValueString(pObject,"Map");
}

TupMapPool::~TupMapPool()
{
	delete m_pMapPool;
}

int TupMapPool::AddMap(void)
{
	TupperwareAggregate *pAgg = m_pMapPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupMapPool::AddMap(int index)
{
	return m_pMapPool->AddAggregate(index);
}

TupperwareAggregate *TupMapPool::GetMap(int index)
{
	return m_pMapPool->FindAggregate(index);
}

int TupMapPool::GetNumMaps(void)
{
	return m_pMapPool->GetPoolSize();
}

void TupMapPool::DeleteMap(int index)
{
	m_pMapPool->DeleteEntry(index);
}


