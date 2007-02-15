////////////////////////////////////////////////////////////////////////////
//
// ImagePool
//
// Class to handle image pools
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: imagepool.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:23p
 * Updated in $/Avalanche/tools/Tupper3d
 * added remove function
 * 
 * *****************  Version 4  *****************
 * User: Adam Clayton Date: 3/21/02    Time: 1:13p
 * Updated in $/Avalanche/tools/Tupper3d
 * Precompiled Headers
 * 
 * *****************  Version 3  *****************
 * User: Adam Clayton Date: 12/17/01   Time: 11:05a
 * Updated in $/Avalanche/tools/Tupper3d
 * change to correct name
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 12/12/01   Time: 4:34p
 * Updated in $/Avalanche/tools/Tupper3d
 * Material - Map stuff
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "imagepool.h"

TupImagePool::TupImagePool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pImagePool = new TupperwarePoolValueString(pObject,"Image");
}

TupImagePool::~TupImagePool()
{
	delete m_pImagePool;
}

int TupImagePool::AddImage(void)
{
	TupperwareAggregate *pAgg = m_pImagePool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupImagePool::AddImage(int index)
{
	return m_pImagePool->AddAggregate(index);
}

TupperwareAggregate *TupImagePool::GetImage(int index)
{
	return m_pImagePool->FindAggregate(index);
}

int TupImagePool::GetNumImages(void)
{
	return m_pImagePool->GetPoolSize();
}

void TupImagePool::DeleteImage(int index)
{
	m_pImagePool->DeleteEntry(index);
}


