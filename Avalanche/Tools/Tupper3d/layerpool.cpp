////////////////////////////////////////////////////////////////////////////
//
// LayerPool
//
// Class to handle the layer pool
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: layerpool.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Adam Clayton Date: 9/05/03    Time: 4:35p
 * Updated in $/Avalanche/tools/Tupper3d
 * exporting layers
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "layerpool.h"

TupLayerPool::TupLayerPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pLayerPool = new TupperwarePoolValueString(pObject,"Layer");
}

TupLayerPool::~TupLayerPool()
{
	delete m_pLayerPool;
}

int TupLayerPool::AddLayer(void)
{
	TupperwareAggregate *pAgg = m_pLayerPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupLayerPool::AddLayer(int index)
{
	return m_pLayerPool->AddAggregate(index);
}

TupperwareAggregate *TupLayerPool::GetLayer(int index)
{
	return m_pLayerPool->FindAggregate(index);
}

int TupLayerPool::GetNumLayers(void)
{
	return m_pLayerPool->GetPoolSize();
}

void TupLayerPool::DeleteLayer(int index)
{
	m_pLayerPool->DeleteEntry(index);
}



