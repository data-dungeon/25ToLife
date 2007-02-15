////////////////////////////////////////////////////////////////////////////
//
// MaterialPool
//
// Class to handle the objects
//
////////////////////////////////////////////////////////////////////////////

/*
 * $History: materialpool.cpp $
 * 
 * *****************  Version 5  *****************
 * User: Adam Clayton Date: 9/05/02    Time: 4:25p
 * Updated in $/Avalanche/tools/Tupper3d
 * added delete methods
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
 * User: Adam Clayton Date: 12/10/01   Time: 4:20p
 * Updated in $/Avalanche/tools/Tupper3d
 * Materials - Textures Part 1
 */

////////////////////////////////////////////////////////////////////////////

#include "Tupper3dPch.h"
#include "materialpool.h"

TupMaterialPool::TupMaterialPool(TupperwareAggregate *pObject)
{
	m_pObject = pObject;
	m_pMaterialPool = new TupperwarePoolValueString(pObject,"Material");
}

TupMaterialPool::~TupMaterialPool()
{
	delete m_pMaterialPool;
}

int TupMaterialPool::AddMaterial(void)
{
	TupperwareAggregate *pAgg = m_pMaterialPool->AddAggregate();
	assert(pAgg);
	return atoi(pAgg->GetName());
}

TupperwareAggregate *TupMaterialPool::AddMaterial(int index)
{
	return m_pMaterialPool->AddAggregate(index);
}

TupperwareAggregate *TupMaterialPool::GetMaterial(int index)
{
	return m_pMaterialPool->FindAggregate(index);
}

int TupMaterialPool::GetNumMaterials(void)
{
	return m_pMaterialPool->GetPoolSize();
}

void TupMaterialPool::DeleteMaterial(int index)
{
	m_pMaterialPool->DeleteEntry(index);
}



